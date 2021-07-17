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

#include "EntityDescriptor.h"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

namespace DlbAdm
{
    using namespace boost::multi_index;

    struct EntityIndex_Name {};
    struct EntityIndex_Type {};

    typedef multi_index_container<
        EntityDescriptor,
        indexed_by<
            // Name index
            ordered_unique<tag<EntityIndex_Name>, member<EntityDescriptor, std::string, &EntityDescriptor::name> >,
            // Entity type index -- not unique because entity and reference to entity use the same type
            ordered_non_unique<tag<EntityIndex_Type>, member<EntityDescriptor, DLB_ADM_ENTITY_TYPE, &EntityDescriptor::entityType> >
        >
    > EntityIndex;

    typedef EntityIndex::index<EntityIndex_Name>::type EntityIndex_NameIndex;
    typedef EntityIndex::index<EntityIndex_Type>::type EntityIndex_TypeIndex;

    EntityDescriptor nullEntityDescriptor =
    {
        "",
        DLB_ADM_ENTITY_TYPE_ILLEGAL,
        false,
        false,
        DLB_ADM_TAG_UNKNOWN
    };

    static EntityIndex theADMEntityIndex;

#include "EntityInitializers.h"

    static const size_t INITIALIZER_COUNT = sizeof(initializers) / sizeof(EntityDescriptor);

    void InitializeEntityIndex()
    {
        size_t i;

        for (i = 0; i < INITIALIZER_COUNT; i++)
        {
            theADMEntityIndex.insert(initializers[i]);
        }
    }

    int GetEntityDescriptor(EntityDescriptor &d, const std::string &name)
    {
        if (theADMEntityIndex.size() == 0)
        {
            InitializeEntityIndex();
        }

        EntityIndex_NameIndex &index = theADMEntityIndex.get<EntityIndex_Name>();
        EntityIndex_NameIndex::iterator it = index.find(name);
        int status = DLB_ADM_STATUS_NOT_FOUND;

        if (it != index.end())
        {
            d = *it;
            status = DLB_ADM_STATUS_OK;
        }

        return status;
    }

    int GetEntityDescriptor(EntityDescriptor &d, DLB_ADM_ENTITY_TYPE eType)
    {
        if (theADMEntityIndex.size() == 0)
        {
            InitializeEntityIndex();
        }

        EntityIndex_TypeIndex &index = theADMEntityIndex.get<EntityIndex_Type>();
        auto range = index.equal_range(eType);
        int status = DLB_ADM_STATUS_NOT_FOUND;

        if (range.first != range.second)
        {
            d = *range.first;
            if (++range.first == range.second)
            {
                status = DLB_ADM_STATUS_OK;
            } 
            else
            {
                status = DLB_ADM_STATUS_NOT_UNIQUE;
            }
        }

        return status;
    }

    int GetEntityDescriptor(EntityDescriptor &d, DLB_ADM_ENTITY_TYPE eType, bool isReference)
    {
        if (theADMEntityIndex.size() == 0)
        {
            InitializeEntityIndex();
        }

        EntityIndex_TypeIndex &index = theADMEntityIndex.get<EntityIndex_Type>();
        auto range = index.equal_range(eType);
        int status = DLB_ADM_STATUS_NOT_FOUND;

        while (range.first != range.second)
        {
            if (isReference == range.first->isReference)
            {
                d = *range.first;
                status = DLB_ADM_STATUS_OK;
                break;
            }
            ++range.first;
        }

        return status;
    }

}
