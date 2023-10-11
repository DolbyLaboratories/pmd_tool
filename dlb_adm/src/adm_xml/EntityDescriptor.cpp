/************************************************************************
 * dlb_adm
 * Copyright (c) 2023, Dolby Laboratories Inc.
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
            // Name index -- not unique because the same name may be used to label different entities
            ordered_non_unique<tag<EntityIndex_Name>, member<EntityDescriptor, std::string, &EntityDescriptor::name> >,
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

    int GetEntityDescriptor(EntityDescriptor &d, const std::string &name, EntityNameDisambiguationFn disambiguator/*= nullptr*/)
    {
        if (theADMEntityIndex.size() == 0)
        {
            InitializeEntityIndex();
        }

        EntityIndex_NameIndex &index = theADMEntityIndex.get<EntityIndex_Name>();
        auto range = index.equal_range(name);
        int status = DLB_ADM_STATUS_NOT_FOUND;

        if (range.first != range.second)
        {
            if (disambiguator != nullptr)
            {
                while (range.first != range.second)
                {
                    if (disambiguator(*range.first))
                    {
                        d = *range.first;
                        status = DLB_ADM_STATUS_OK;
                        break;
                    }
                    ++range.first;
                }
            } 
            else
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
