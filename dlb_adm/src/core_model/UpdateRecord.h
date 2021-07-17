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

#ifndef DLB_ADM_UPDATE_RECORD_H
#define DLB_ADM_UPDATE_RECORD_H

#include "dlb_adm/include/dlb_adm_entity_id.h"

namespace DlbAdm
{

    struct UpdateRecord
    {
        dlb_adm_entity_id   updateID;

        UpdateRecord();
        explicit UpdateRecord(dlb_adm_entity_id updateID);
        UpdateRecord(const UpdateRecord &x);
        ~UpdateRecord();

        UpdateRecord &operator=(const UpdateRecord &x);

        bool operator<(const UpdateRecord &x) const;

        dlb_adm_entity_id GetUpdateID() const { return updateID; }
        dlb_adm_entity_id GetTargetID() const;

        bool IsNull() const;
        bool Validate(bool nullOK = false) const;
    };

}

#endif  // DLB_ADM_UPDATE_RECORD_H
