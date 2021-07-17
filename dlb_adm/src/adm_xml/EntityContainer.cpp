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

#include "EntityContainer.h"
#include "dlb_adm/src/adm_identity/AdmId.h"

namespace DlbAdm
{

    bool EntityTypeCompare::operator()(const EntityRecord &lhs, DLB_ADM_ENTITY_TYPE rhs) const
    {
        return DLB_ADM_ID_GET_ENTITY_TYPE(lhs.id) < rhs;
    }

    bool EntityTypeCompare::operator()(DLB_ADM_ENTITY_TYPE lhs, const EntityRecord &rhs) const
    {
        return lhs < DLB_ADM_ID_GET_ENTITY_TYPE(rhs.id);
    }

    bool EntityIdCompare::operator()(const EntityRecord &lhs, dlb_adm_entity_id rhs) const
    {
        return lhs.id < rhs;
    }

    bool EntityIdCompare::operator()(dlb_adm_entity_id lhs, const EntityRecord &rhs) const
    {
        return lhs < rhs.id;
    }

}
