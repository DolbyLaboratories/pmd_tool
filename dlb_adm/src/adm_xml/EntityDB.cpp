/************************************************************************
 * dlb_adm
 * Copyright (c) 2020-2025, Dolby Laboratories Inc.
 * Copyright (c) 2020-2025, Dolby International AB.
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

#include "EntityDB.h"
#include "AttributeDescriptor.h"
#include "EntityContainer.h"
#include "dlb_adm/src/adm_identity/AdmId.h"
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/allocators/node_allocator.hpp>

#include <vector>
#include <map>
#include <iostream>

#define CHECK_STATUS(s) if ((s) != DLB_ADM_STATUS_OK) return (s)

namespace DlbAdm
{
    using namespace boost::interprocess;
    // EntityData

    typedef std::pair<const DLB_ADM_TAG, AttributeValue> EntityAttributeValue;
    typedef node_allocator<EntityAttributeValue, managed_heap_memory::segment_manager> EntityAttributesAllocator;
    typedef map<DLB_ADM_TAG, AttributeValue, std::less<DLB_ADM_TAG>, EntityAttributesAllocator> EntityAttributesMap;
    typedef allocator<EntityAttributesMap, managed_heap_memory::segment_manager>  AttributeVectorAlloc;
    typedef vector<EntityAttributesMap, AttributeVectorAlloc> AttributesVector;
    typedef allocator<void, managed_heap_memory::segment_manager>  void_allocator;

    class EntityData
    {
    public:
        EntityData(boost::interprocess::managed_heap_memory &memory)
        :mMemory(memory)
        {
            mEntities = memory.construct<EntityContainer>("EntityContainer")(EntityContainer::ctor_args_list(), memory.get_allocator<EntityRecord>());
            mAttributes = memory.construct<AttributesVector>("AttributesVector")(memory.get_segment_manager());
        }
        EntityContainer  &GetEntities()   { return *mEntities; }
        AttributesVector &GetAttributes() { return *mAttributes; }

        TableIndex AddAttributes();
        TableIndex AddAttributes(const DlbAdm::EntityAttributesMap &a);

    private:
        EntityContainer     *mEntities;
        AttributesVector    *mAttributes;
        boost::interprocess::managed_heap_memory &mMemory;
    };

    TableIndex EntityData::AddAttributes(const DlbAdm::EntityAttributesMap &a)
    {
        size_t n = mAttributes->size();

        mAttributes->push_back(a);
        return static_cast<TableIndex>(n);
    }

    TableIndex EntityData::AddAttributes()
    {
      EntityAttributesMap e(mMemory.get_segment_manager());
      return AddAttributes(e);
    }


    // EntityDB

    EntityDB::EntityDB(boost::interprocess::managed_heap_memory &memory)
        : mEntityData(new EntityData(memory))
    {
        InitializeAttributeIndex();
    }


    EntityDB::~EntityDB()
    {
        // Empty
    }

    int EntityDB::Get(EntityRecord &e, dlb_adm_entity_id id)
    {
        EntityContainer &ec = mEntityData->GetEntities();
        EntityContainer_PKIndex &index = ec.get<EntityContainer_PK>();
        EntityContainer_PKIndex::iterator it = index.find(id, EntityIdCompare());
        int status = DLB_ADM_STATUS_NOT_FOUND;

        if (it != index.end())
        {
            e = *it;
            status = DLB_ADM_STATUS_OK;
        }

        return status;
    }

    int EntityDB::GetDescriptor(AttributeDescriptor &d, DLB_ADM_TAG tag)
    {
        return GetAttributeDescriptor(d, tag);
    }

    int EntityDB::GetDescriptor(AttributeDescriptor &d, DLB_ADM_ENTITY_TYPE entityType, const std::string &name)
    {
        return GetAttributeDescriptor(d, entityType, name);
    }

    int EntityDB::Add(dlb_adm_entity_id id)
    {
        EntityContainer &ec = mEntityData->GetEntities();
        EntityContainer_PKIndex &index = ec.get<EntityContainer_PK>();
        EntityContainer_PKIndex::iterator it = index.find(id, EntityIdCompare());

        if (it == index.end())
        {
            EntityRecord e;

            e.id = id;
            e.attributesIndex = mEntityData->AddAttributes();
            e.status = EntityRecord::STATUS::FORWARD_REFERENCE;
            auto insertResult = ec.insert(e);
            if (!insertResult.second)
            {
                return DLB_ADM_STATUS_ERROR;
            }
        }

        return DLB_ADM_STATUS_OK;
    }


    int EntityDB::SetValue(dlb_adm_entity_id id, DLB_ADM_TAG tag, const AttributeValue &value)
    {
        DLB_ADM_VALUE_TYPE valueType;
        AttributeDescriptor d;
        int status;

        status = GetAttributeDescriptor(d, tag);
        CHECK_STATUS(status);
        if (static_cast<long unsigned int>(d.entityType) != DLB_ADM_ID_GET_ENTITY_TYPE(id))
        {
            return DLB_ADM_STATUS_INVALID_ARGUMENT;
        }
        valueType = boost::apply_visitor(GetValueType(), value);
        if (d.attributeValueType != valueType)
        {
            return DLB_ADM_STATUS_VALUE_TYPE_MISMATCH;
        }

        EntityContainer &ec = mEntityData->GetEntities();
        EntityContainer_PKIndex &index = ec.get<EntityContainer_PK>();
        EntityContainer_PKIndex::iterator it = index.find(id, EntityIdCompare());

        if (it != index.end())
        {
            if (it->status == EntityRecord::STATUS::UNINITALIZED || it->status >= EntityRecord::STATUS::IMMUTABLE)
            {
                return DLB_ADM_STATUS_ERROR;
            }

            if (it->status < EntityRecord::STATUS::MUTABLE)
            {
                EntityRecord e = *it;
                e.status = EntityRecord::STATUS::MUTABLE;
                index.replace(it, e);
            }

            mEntityData->GetAttributes()[it->attributesIndex][tag] = value;
        }
        else
        {
            status = DLB_ADM_STATUS_NOT_FOUND;
        }

        return status;
    }

    int EntityDB::GetValue(AttributeValue &value, dlb_adm_entity_id id, DLB_ADM_TAG tag) const
    {
        AttributeDescriptor d;
        int status;

        status = GetAttributeDescriptor(d, tag);
        CHECK_STATUS(status);
        if (static_cast<long unsigned int>(d.entityType) != DLB_ADM_ID_GET_ENTITY_TYPE(id))
        {
            return DLB_ADM_STATUS_INVALID_ARGUMENT;
        }

        const EntityContainer &ec = mEntityData->GetEntities();
        const EntityContainer_PKIndex &index = ec.get<EntityContainer_PK>();
        EntityContainer_PKIndex::iterator it = index.find(id, EntityIdCompare());

        status = DLB_ADM_STATUS_NOT_FOUND;
        if (it != index.end() && it->attributesIndex != TableIndex_NIL)
        {
            if (it->status == EntityRecord::STATUS::UNINITALIZED || it->status >= EntityRecord::STATUS::DESTROYED)
            {
                return DLB_ADM_STATUS_ERROR;
            }

            const EntityAttributesMap &attr = mEntityData->GetAttributes()[it->attributesIndex];
            auto attrIt = attr.find(tag);

            if (attrIt != attr.end())
            {
                value = attrIt->second;
                status = DLB_ADM_STATUS_OK;
            }
        }

        return status;
    }

    int EntityDB::SetMutable(dlb_adm_entity_id id, dlb_adm_bool isMutable)
    {
        EntityContainer &ec = mEntityData->GetEntities();
        EntityContainer_PKIndex &index = ec.get<EntityContainer_PK>();
        EntityContainer_PKIndex::iterator it = index.find(id, EntityIdCompare());
        int status = DLB_ADM_STATUS_OK;

        if (it != index.end())
        {
            if (it->status == EntityRecord::STATUS::UNINITALIZED || it->status >= EntityRecord::STATUS::DESTROYED)
            {
                return DLB_ADM_STATUS_ERROR;
            }

            EntityRecord::STATUS newStatus = EntityRecord::STATUS::UNINITALIZED;
            bool setIt = false;

            if (isMutable)
            {
                if (it->status == EntityRecord::STATUS::IMMUTABLE)
                {
                    newStatus = EntityRecord::STATUS::MUTABLE;
                    setIt = true;
                }
                else if (it->status > EntityRecord::STATUS::IMMUTABLE)
                {
                    status = DLB_ADM_STATUS_ERROR;
                }
            }
            else
            {
                if (it->status <= EntityRecord::STATUS::MUTABLE)
                {
                    newStatus = EntityRecord::STATUS::IMMUTABLE;
                    setIt = true;
                }
            }

            if (setIt)
            {
                EntityRecord e = *it;
                e.status = newStatus;
                index.replace(it, e);
            }
        }
        else
        {
            status = DLB_ADM_STATUS_NOT_FOUND;
        }

        return status;
    }

    int EntityDB::SetIsCommon(dlb_adm_entity_id id)
    {
        EntityContainer &ec = mEntityData->GetEntities();
        EntityContainer_PKIndex &index = ec.get<EntityContainer_PK>();
        EntityContainer_PKIndex::iterator it = index.find(id, EntityIdCompare());
        int status = DLB_ADM_STATUS_OK;

        if (it != index.end())
        {
            EntityRecord::STATUS s = it->status;
            bool setIt = false;

            switch (s)
            {
            case EntityRecord::STATUS::COMMON_DEFINITION:
                break;

            case EntityRecord::STATUS::FORWARD_REFERENCE:
            case EntityRecord::STATUS::MUTABLE:
            case EntityRecord::STATUS::IMMUTABLE:
                setIt = true;
                break;

            default:
                return DLB_ADM_STATUS_ERROR;
            }

            if (setIt)
            {
                EntityRecord e = *it;
                e.status = EntityRecord::STATUS::COMMON_DEFINITION;
                index.replace(it, e);
            }
        }
        else
        {
            status = DLB_ADM_STATUS_NOT_FOUND;
        }

        return status;
    }

    int EntityDB::ForEach(DLB_ADM_ENTITY_TYPE entityType, EntityCallbackFn callbackFn, EntityFilterFn filterFn)
    {
        EntityContainer &ec = mEntityData->GetEntities();
        EntityContainer_PKIndex &index = ec.get<EntityContainer_PK>();
        auto itPair = index.equal_range(entityType, EntityTypeCompare());
        EntityContainer_PKIndex::iterator it = itPair.first;
        int status = DLB_ADM_STATUS_OK;

        if (filterFn != nullptr)
        {
            while (it != itPair.second)
            {
                if (!filterFn(*it))
                {
                    ++it;
                    continue;
                }
                status = callbackFn(*it);
                if (status != DLB_ADM_STATUS_OK)
                {
                    break;
                }
                ++it;
            }
        }
        else
        {
            while (it != itPair.second)
            {
                status = callbackFn(*it);
                if (status != DLB_ADM_STATUS_OK)
                {
                    break;
                }
                ++it;
            }
        }

        return status;
    }

    int EntityDB::ForEach(dlb_adm_entity_id id, AttributeCallbackFn callbackFn)
    {
        EntityContainer &ec = mEntityData->GetEntities();
        EntityContainer_PKIndex &index = ec.get<EntityContainer_PK>();
        EntityContainer_PKIndex::iterator entityIt = index.find(id, EntityIdCompare());
        int status = DLB_ADM_STATUS_OK;

        if (entityIt == index.end())
        {
            return DLB_ADM_STATUS_NOT_FOUND;
        }

        EntityAttributesMap &attributes = mEntityData->GetAttributes()[entityIt->attributesIndex];
        EntityAttributesMap::const_iterator attrIt = attributes.begin();

        while (attrIt != attributes.end())
        {
            status = callbackFn(id, attrIt->first, attrIt->second);
            if (status != DLB_ADM_STATUS_OK)
            {
                break;
            }
            ++attrIt;
        }

        return status;
    }

    void EntityDB::Clear()
    {
        mEntityData->GetEntities().clear();
        for (unsigned int i = 0; i <mEntityData->GetAttributes().size(); ++i)
        {
            mEntityData->GetAttributes()[i].clear();
        }
        mEntityData->GetAttributes().clear();
    }

}
