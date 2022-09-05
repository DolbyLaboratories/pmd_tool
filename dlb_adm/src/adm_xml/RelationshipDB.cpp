/************************************************************************
 * dlb_adm
 * Copyright (c) 2020 - 2022, Dolby Laboratories Inc.
 * Copyright (c) 2022, Dolby International AB.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 **********************************************************************/

#include "RelationshipDB.h"
#include "RelationshipDescriptor.h"
#include "RelationshipContainer.h"
#include "dlb_adm/src/adm_identity/AdmId.h"

namespace DlbAdm
{

    class RelationshipData : public boost::noncopyable
    {
    public:
        RelationshipContainer &Get() { return mRelationshipContainer; }

    private:
        RelationshipContainer mRelationshipContainer;
    };

    RelationshipDB::RelationshipDB()
        : mRelationshipData(new RelationshipData())
    {
        InitializeRelationshipIndex();
    }

    RelationshipDB::~RelationshipDB()
    {
        // Empty
    }

    int RelationshipDB::GetDescriptor(RelationshipDescriptor &rd, DLB_ADM_ENTITY_TYPE f, DLB_ADM_ENTITY_TYPE t)
    {
        return GetRelationshipDescriptor(rd, f, t);
    }

    int RelationshipDB::Add(dlb_adm_entity_id fromID, dlb_adm_entity_id toID)
    {
        RelationshipDescriptor rd;
        int status;

        // TODO: add arity checking

        status = GetRelationshipDescriptor
        (
            rd,
            static_cast<DLB_ADM_ENTITY_TYPE>(DLB_ADM_ID_GET_ENTITY_TYPE(fromID)),
            static_cast<DLB_ADM_ENTITY_TYPE>(DLB_ADM_ID_GET_ENTITY_TYPE(toID))
        );
        if (status == DLB_ADM_STATUS_OK)
        {
            bool valid = true;
            bool found = false;
            bool duplicate = false;
            bool containment = (rd.relationship == ENTITY_RELATIONSHIP::CONTAINS);
            RelationshipContainer &rc = mRelationshipData->Get();
            RelationshipContainer_PKIndex &index = rc.get<RelationshipContainer_PK>();
            RelationshipContainer_PKIndex::iterator it;

            if (containment)
            {
                // We allow the "to" entity to be contained only once, check to see if there's already a
                // containing entity that is not the "from" entity.

                it = index.find(std::make_tuple(toID, ENTITY_RELATIONSHIP::CONTAINED_BY));
                found = (it != index.end());
                valid = (!found || it->toId == fromID);
                if (valid)
                {
                    duplicate = found;
                }
            }
            else
            {
                it = index.find(std::make_tuple(fromID, ENTITY_RELATIONSHIP::REFERENCES, rd.toType, toID));
                found = (it != index.end());
                if (found)
                {
                    duplicate = true;
                }
            }

            if (valid)
            {
                if (!duplicate)
                {
                    RelationshipRecord r1;  // relationship
                    RelationshipRecord r2;  // inverse

                    r1.fromId = fromID;
                    r1.toId = toID;
                    r1.relationship = rd.relationship;
                    auto insertResult = rc.insert(r1);
                    if (!insertResult.second)
                    {
                        return DLB_ADM_STATUS_ERROR;
                    }

                    r2.fromId = toID;
                    r2.relationship = (containment ? ENTITY_RELATIONSHIP::CONTAINED_BY : ENTITY_RELATIONSHIP::REFERENCED_BY);
                    r2.toId = fromID;
                    insertResult = rc.insert(r2);
                    if (!insertResult.second)
                    {
                        // If we were unable to insert the second one, erase the first one
                        it = index.find(RelationshipTuple(r1.GetFromId(), r1.GetRelationship(), r1.GetToEntityType(), r1.GetToId()));
                        index.erase(it);
                        return DLB_ADM_STATUS_ERROR;
                    }
                }
            }
            else
            {
                status = DLB_ADM_STATUS_INVALID_RELATIONSHIP;
            }
        }
        else if (status == DLB_ADM_STATUS_NOT_FOUND)
        {
            status = DLB_ADM_STATUS_INVALID_RELATIONSHIP;
        }

        return status;
    }

    int RelationshipDB::Get(RelationshipRecord &r, dlb_adm_entity_id fromID, dlb_adm_entity_id toID)
    {
        RelationshipDescriptor d;
        DLB_ADM_ENTITY_TYPE fromType = static_cast<DLB_ADM_ENTITY_TYPE>(DLB_ADM_ID_GET_ENTITY_TYPE(fromID));
        DLB_ADM_ENTITY_TYPE toType   = static_cast<DLB_ADM_ENTITY_TYPE>(DLB_ADM_ID_GET_ENTITY_TYPE(toID));
        int status = GetRelationshipDescriptor(d, fromType, toType);

        if (status == DLB_ADM_STATUS_OK)
        {
            RelationshipContainer &rc = mRelationshipData->Get();
            RelationshipContainer_PKIndex &index = rc.get<RelationshipContainer_PK>();
            auto key = std::make_tuple(fromID, d.relationship, toType, toID);
            RelationshipContainer_PKIndex::iterator it = index.find(key);

            if (it == index.end())
            {
                status = DLB_ADM_STATUS_NOT_FOUND;
            }
            else
            {
                r = *it;
            }
        }

        return status;
    }
    
    int RelationshipDB::ForEach(RelationshipCallbackFn callbackFn)  // TODO: add a filter function parameter
    {
        RelationshipContainer &rc = mRelationshipData->Get();
        RelationshipContainer_PKIndex &index = rc.get<RelationshipContainer_PK>();
        RelationshipContainer::const_iterator it = index.begin();
        int status = DLB_ADM_STATUS_OK;

        while (it != index.end())
        {
            status = callbackFn(*it++);
            if (status != DLB_ADM_STATUS_OK)
            {
                break;
            }
        }

        return status;
    }

    int RelationshipDB::ForEach(dlb_adm_entity_id id, ENTITY_RELATIONSHIP r, RelationshipCallbackFn callbackFn, RelationshipFilterFn filterFn)
    {
        RelationshipContainer &rc = mRelationshipData->Get();
        RelationshipContainer_PKIndex &index = rc.get<RelationshipContainer_PK>();
        auto key = std::make_tuple(id, r);
        RelationshipContainer_PKIndex::iterator lowerIt = index.lower_bound(key);
        RelationshipContainer_PKIndex::iterator upperIt = index.upper_bound(key);
        int status = DLB_ADM_STATUS_OK;

        if (filterFn != nullptr)
        {
            while (lowerIt != upperIt)
            {
                if (!filterFn(*lowerIt))
                {
                    ++lowerIt;
                    continue;
                }
                status = callbackFn(*lowerIt++);
                if (status != DLB_ADM_STATUS_OK)
                {
                    break;
                }
            }
        } 
        else
        {
            while (lowerIt != upperIt)
            {
                status = callbackFn(*lowerIt++);
                if (status != DLB_ADM_STATUS_OK)
                {
                    break;
                }
            }
        }

        return status;
    }

    int RelationshipDB::ForEach(dlb_adm_entity_id id, DLB_ADM_ENTITY_TYPE entityType, RelationshipCallbackFn callbackFn, RelationshipFilterFn filterFn)
    {
        RelationshipDescriptor d;
        int status = GetRelationshipDescriptor(d, static_cast<DLB_ADM_ENTITY_TYPE>(DLB_ADM_ID_GET_ENTITY_TYPE(id)), entityType);
        if (status != DLB_ADM_STATUS_OK)
        {
            return status;
        }

        RelationshipContainer &rc = mRelationshipData->Get();
        RelationshipContainer_PKIndex &index = rc.get<RelationshipContainer_PK>();
        auto key = std::make_tuple(id, d.relationship, entityType);
        RelationshipContainer_PKIndex::iterator lowerIt = index.lower_bound(key);
        RelationshipContainer_PKIndex::iterator upperIt = index.upper_bound(key);

        if (filterFn != nullptr)
        {
            while (lowerIt != upperIt)
            {
                if (!filterFn(*lowerIt))
                {
                    ++lowerIt;
                    continue;
                }
                status = callbackFn(*lowerIt++);
                if (status != DLB_ADM_STATUS_OK)
                {
                    break;
                }
            }
        } 
        else
        {
            while (lowerIt != upperIt)
            {
                status = callbackFn(*lowerIt++);
                if (status != DLB_ADM_STATUS_OK)
                {
                    break;
                }
            }
        }

        return status;
    }

    bool RelationshipDB::Exists(dlb_adm_entity_id fromID, dlb_adm_entity_id toID)
    {
        bool exists = false;
        RelationshipDescriptor d;
        DLB_ADM_ENTITY_TYPE fromType = static_cast<DLB_ADM_ENTITY_TYPE>(DLB_ADM_ID_GET_ENTITY_TYPE(fromID));
        DLB_ADM_ENTITY_TYPE toType   = static_cast<DLB_ADM_ENTITY_TYPE>(DLB_ADM_ID_GET_ENTITY_TYPE(toID));
        int status = GetRelationshipDescriptor(d, fromType, toType);

        if (status == DLB_ADM_STATUS_OK)
        {
            RelationshipContainer &rc = mRelationshipData->Get();
            RelationshipContainer_PKIndex &index = rc.get<RelationshipContainer_PK>();
            auto key = std::make_tuple(fromID, d.relationship, toType, toID);
            RelationshipContainer_PKIndex::iterator it = index.find(key);

            exists = (it != index.end());
        }

        return exists;
    }
    
    bool RelationshipDB::Exists(dlb_adm_entity_id id, DLB_ADM_ENTITY_TYPE entityType)
    {
        bool exists = false;
        RelationshipDescriptor d;
        int status = GetRelationshipDescriptor(d, static_cast<DLB_ADM_ENTITY_TYPE>(DLB_ADM_ID_GET_ENTITY_TYPE(id)), entityType);

        if (status == DLB_ADM_STATUS_OK)
        {
            RelationshipContainer &rc = mRelationshipData->Get();
            RelationshipContainer_PKIndex &index = rc.get<RelationshipContainer_PK>();
            auto key = std::make_tuple(id, d.relationship, entityType);
            RelationshipContainer_PKIndex::iterator lowerIt = index.lower_bound(key);
            RelationshipContainer_PKIndex::iterator upperIt = index.upper_bound(key);

            exists = (lowerIt != upperIt);
        }

        return exists;
    }

    size_t RelationshipDB::Count(dlb_adm_entity_id id, DLB_ADM_ENTITY_TYPE entityType)
    {
        size_t count = 0;
        RelationshipDescriptor d;
        int status = GetRelationshipDescriptor(d, static_cast<DLB_ADM_ENTITY_TYPE>(DLB_ADM_ID_GET_ENTITY_TYPE(id)), entityType);

        if (status == DLB_ADM_STATUS_OK)
        {
            RelationshipContainer &rc = mRelationshipData->Get();
            RelationshipContainer_PKIndex &index = rc.get<RelationshipContainer_PK>();
            auto key = std::make_tuple(id, d.relationship, entityType);
            RelationshipContainer_PKIndex::iterator lowerIt = index.lower_bound(key);
            RelationshipContainer_PKIndex::iterator upperIt = index.upper_bound(key);

            while (lowerIt != upperIt)
            {
                ++count;
                ++lowerIt;
            }
        }

        return count;
    }

}
