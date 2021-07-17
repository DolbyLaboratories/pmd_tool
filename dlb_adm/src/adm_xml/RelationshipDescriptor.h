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

#ifndef DLB_ADM_RELATIONSHIP_DESCRIPTOR_H
#define DLB_ADM_RELATIONSHIP_DESCRIPTOR_H

#include "dlb_adm/include/dlb_adm_api_types.h"
#include "EntityRelationship.h"

namespace DlbAdm
{

    struct RelationshipArity
    {
        int     minArity;
        int     maxArity;

        static const int ANY;
    };

    struct RelationshipDescriptor
    {
        DLB_ADM_ENTITY_TYPE  fromType;
        DLB_ADM_ENTITY_TYPE  toType;
        ENTITY_RELATIONSHIP  relationship;
        RelationshipArity    arity;

        bool operator<(const RelationshipDescriptor &x) const;
    };

    extern RelationshipDescriptor nullRelationshipDescriptor;

    void InitializeRelationshipIndex();

    int GetRelationshipDescriptor(RelationshipDescriptor &rd, DLB_ADM_ENTITY_TYPE f, DLB_ADM_ENTITY_TYPE t);

}

#endif /* DLB_ADM_RELATIONSHIPS_H */
