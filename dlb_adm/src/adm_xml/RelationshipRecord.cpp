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

#include "RelationshipRecord.h"
#include "dlb_adm/src/adm_identity/AdmId.h"

namespace DlbAdm
{

    dlb_adm_entity_id RelationshipRecord::GetFromId() const
    {
        return fromId;
    }

    dlb_adm_entity_id RelationshipRecord::GetToId() const
    {
        return toId;
    }

    ENTITY_RELATIONSHIP RelationshipRecord::GetRelationship() const
    {
        return relationship;
    }

    DLB_ADM_ENTITY_TYPE RelationshipRecord::GetToEntityType() const
    {
        return static_cast<DLB_ADM_ENTITY_TYPE>(DLB_ADM_ID_GET_ENTITY_TYPE(toId));
    }

}
