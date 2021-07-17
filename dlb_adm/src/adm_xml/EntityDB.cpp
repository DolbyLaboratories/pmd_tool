/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2020 by Dolby Laboratories,
 *                Copyright (C) 2020 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#include "EntityDB.h"
#include "AttributeDescriptor.h"
#include "EntityContainer.h"
#include "dlb_adm/src/adm_identity/AdmId.h"

#include <vector>
#include <map>

#define CHECK_STATUS(s) if ((s) != DLB_ADM_STATUS_OK) return (s)

namespace DlbAdm
{

    // EntityData

    typedef std::map<DLB_ADM_TAG, AttributeValue> EntityAttributes;
    typedef std::vector<DlbAdm::EntityAttributes> AttributesVector;

    class EntityData
    {
    public:
        EntityContainer  &GetEntities()   { return mEntities; }
        AttributesVector &GetAttributes() { return mAttributes; }

        TableIndex AddAttributes();
        TableIndex AddAttributes(const DlbAdm::EntityAttributes &a);

    private:
        EntityContainer      mEntities;
        AttributesVector     mAttributes;
    };

    TableIndex EntityData::AddAttributes(const DlbAdm::EntityAttributes &a)
    {
        size_t n = mAttributes.size();

        mAttributes.push_back(a);
        return static_cast<TableIndex>(n);
    }

    TableIndex EntityData::AddAttributes()
    {
        return AddAttributes(EntityAttributes());
    }


    // EntityDB

    EntityDB::EntityDB()
        : mEntityData(new EntityData())
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
        if (d.entityType != DLB_ADM_ID_GET_ENTITY_TYPE(id))
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
        if (d.entityType != DLB_ADM_ID_GET_ENTITY_TYPE(id))
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

            const EntityAttributes &attr = mEntityData->GetAttributes()[it->attributesIndex];
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

        EntityAttributes &attributes = mEntityData->GetAttributes()[entityIt->attributesIndex];
        EntityAttributes::const_iterator attrIt = attributes.begin();

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

}
