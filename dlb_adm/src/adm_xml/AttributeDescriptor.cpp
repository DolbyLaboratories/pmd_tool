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

#include "AttributeDescriptor.h"

#include <tuple>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>

namespace DlbAdm
{
    // AttributeIndex

    using namespace boost::multi_index;

    struct AttributeIndex_Tag {};
    struct AttributeIndex_Name {};

    typedef multi_index_container<
        AttributeDescriptor,
        indexed_by<
            // Index by attribute tag
            ordered_unique<tag<AttributeIndex_Tag>, member<AttributeDescriptor, DLB_ADM_TAG, &AttributeDescriptor::attributeTag>  >,
            // Index by entity type plus name
            ordered_unique<tag<AttributeIndex_Name>, identity<AttributeDescriptor> >
        >
    > AttributeIndex;

    typedef AttributeIndex::index<AttributeIndex_Tag> ::type AttributeIndex_TagIndex;
    typedef AttributeIndex::index<AttributeIndex_Name>::type AttributeIndex_NameIndex;


    // AttributeDescriptor

    AttributeDescriptor nullAttributeDescriptor =
    {
        DLB_ADM_ENTITY_TYPE_ILLEGAL,
        "",
        DLB_ADM_TAG_UNKNOWN,
        DLB_ADM_VALUE_TYPE_BOOL
    };

    bool AttributeDescriptor::operator<(const AttributeDescriptor &x) const
    {
        return
            std::tie(  entityType,   attributeName) <
            std::tie(x.entityType, x.attributeName);
    }

    static AttributeIndex theAdmAttributeIndex;

    struct AttributeInitializer
    {
        DLB_ADM_ENTITY_TYPE      entityType;
        const char *             attributeName;
        DLB_ADM_TAG              attributeTag;
        DLB_ADM_VALUE_TYPE       attributeValueType;
    };

#include "AttributeInitializers.h"

    static const size_t ATTRIBUTE_COUNT = sizeof(initializers) / sizeof(AttributeInitializer);

    void InitializeAttributeIndex()
    {
        if (theAdmAttributeIndex.size() == 0)
        {
            for (size_t i = 0; i < ATTRIBUTE_COUNT; i++)
            {
                const AttributeInitializer *initializer = &initializers[i];
                AttributeDescriptor descriptor;

                descriptor.entityType = initializer->entityType;
                descriptor.attributeName = initializer->attributeName;
                descriptor.attributeTag = initializer->attributeTag;
                descriptor.attributeValueType = initializer->attributeValueType;
                theAdmAttributeIndex.insert(descriptor);
            }
        }
    }

    int GetAttributeDescriptor(AttributeDescriptor &d, DLB_ADM_TAG tag)
    {
        if (theAdmAttributeIndex.size() == 0)
        {
            InitializeAttributeIndex();
        }

        int status = DLB_ADM_STATUS_NOT_FOUND;
        AttributeIndex_TagIndex &index = theAdmAttributeIndex.get<AttributeIndex_Tag>();
        AttributeIndex_TagIndex::iterator it = index.find(tag);

        if (it != index.end())
        {
            d = *it;
            status = DLB_ADM_STATUS_OK;
        }

        return status;
    }

    int GetAttributeDescriptor(AttributeDescriptor &d, DLB_ADM_ENTITY_TYPE entityType, const std::string &name)
    {
        if (theAdmAttributeIndex.size() == 0)
        {
            InitializeAttributeIndex();
        }

        int status = DLB_ADM_STATUS_NOT_FOUND;

        AttributeDescriptor q;
        q.entityType = entityType;
        q.attributeName = name;

        AttributeIndex_NameIndex &index = theAdmAttributeIndex.get<AttributeIndex_Name>();
        AttributeIndex_NameIndex::iterator it = index.find(q);

        if (it != index.end())
        {
            d = *it;
            status = DLB_ADM_STATUS_OK;
        }

        return status;
    }

}
