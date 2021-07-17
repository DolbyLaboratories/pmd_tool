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

#include "UpdateRecord.h"
#include "BlockUpdate.h"
#include "dlb_adm/src/adm_identity/AdmIdTranslator.h"

namespace DlbAdm
{

    UpdateRecord::UpdateRecord()
        : updateID(DLB_ADM_NULL_ENTITY_ID)
    {
        // Empty
    }

    UpdateRecord::UpdateRecord(dlb_adm_entity_id updateID)
        : updateID(updateID)
    {
        // Empty
    }

    UpdateRecord::UpdateRecord(const UpdateRecord &x)
        : updateID(x.updateID)
    {
        // Empty
    }

    UpdateRecord::~UpdateRecord()
    {
        updateID = DLB_ADM_NULL_ENTITY_ID;
    }

    UpdateRecord &UpdateRecord::operator=(const UpdateRecord &x)
    {
        updateID = x.updateID;
        return *this;
    }

    bool UpdateRecord::operator<(const UpdateRecord &x) const
    {
        return updateID < x.updateID;
    }

    dlb_adm_entity_id UpdateRecord::GetTargetID() const
    {
        return BlockUpdate::GetTargetID(updateID);
    }

    bool UpdateRecord::IsNull() const
    {
        return (updateID == DLB_ADM_NULL_ENTITY_ID);
    }

    bool UpdateRecord::Validate(bool nullOK) const
    {
        return
            (AdmIdTranslator().GetEntityType(updateID) == DLB_ADM_ENTITY_TYPE_BLOCK_FORMAT) ||
            (nullOK && updateID == DLB_ADM_NULL_ENTITY_ID);
    }

}
