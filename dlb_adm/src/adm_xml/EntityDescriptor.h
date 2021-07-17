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

#ifndef DLB_ADM_ENTITY_DESCRIPTOR_H
#define DLB_ADM_ENTITY_DESCRIPTOR_H

#include "dlb_adm/include/dlb_adm_api_types.h"

#include <string>

namespace DlbAdm
{

    struct EntityDescriptor
    {
        std::string          name;              // Must be unique
        DLB_ADM_ENTITY_TYPE  entityType;
        bool                 xmlTypeComposite;
        bool                 hasADMIdOrRef;
        bool                 isReference;
        DLB_ADM_TAG          distinguishedTag;  // ID or value
    };

    extern EntityDescriptor nullEntityDescriptor;

    void InitializeEntityIndex();

    int GetEntityDescriptor(EntityDescriptor &d, const std::string &name);

    int GetEntityDescriptor(EntityDescriptor &d, DLB_ADM_ENTITY_TYPE eType);    // Returns "not unique" for multiple results

    int GetEntityDescriptor(EntityDescriptor &d, DLB_ADM_ENTITY_TYPE eType, bool isReference);

}

#endif
