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

#ifndef DLB_ADM_ATTRIBUTE_DESCRIPTOR_H
#define DLB_ADM_ATTRIBUTE_DESCRIPTOR_H

#include "dlb_adm/include/dlb_adm_api_types.h"

#include <string>

namespace DlbAdm
{

    struct AttributeDescriptor
    {
        DLB_ADM_ENTITY_TYPE      entityType;
        std::string              attributeName;
        DLB_ADM_TAG              attributeTag;
        DLB_ADM_VALUE_TYPE       attributeValueType;

        bool operator<(const AttributeDescriptor &x) const;
    };

    extern AttributeDescriptor nullAttributeDescriptor;

    void InitializeAttributeIndex();

    int GetAttributeDescriptor(AttributeDescriptor &d, DLB_ADM_TAG tag);
    int GetAttributeDescriptor(AttributeDescriptor &d, DLB_ADM_ENTITY_TYPE entityType, const std::string &name);

}

#endif /* DLB_ADM_ATTRIBUTES_H */
