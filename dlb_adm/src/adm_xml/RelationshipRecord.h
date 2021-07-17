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

#ifndef DLB_ADM_RELATIONSHIP_RECORD_H
#define DLB_ADM_RELATIONSHIP_RECORD_H

#include "dlb_adm/include/dlb_adm_api_types.h"
#include "EntityRelationship.h"

namespace DlbAdm
{

    struct RelationshipRecord
    {
        dlb_adm_entity_id    fromId;
        dlb_adm_entity_id    toId;
        ENTITY_RELATIONSHIP  relationship;

        dlb_adm_entity_id   GetFromId()       const;
        dlb_adm_entity_id   GetToId()         const;
        ENTITY_RELATIONSHIP GetRelationship() const;
        DLB_ADM_ENTITY_TYPE GetToEntityType() const;
    };

}

#endif
