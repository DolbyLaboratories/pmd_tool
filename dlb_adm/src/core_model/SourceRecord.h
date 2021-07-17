/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2021 by Dolby Laboratories,
 *                Copyright (C) 2021 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#ifndef DLB_ADM_SOURCE_RECORD_H
#define DLB_ADM_SOURCE_RECORD_H

#include "dlb_adm/include/dlb_adm_entity_id.h"

namespace DlbAdm
{

    struct SourceRecord
    {
        dlb_adm_entity_id   sourceGroupID;
        dlb_adm_entity_id   sourceID;
        dlb_adm_entity_id   audioTrackID;

        SourceRecord();
        SourceRecord(dlb_adm_entity_id groupID, dlb_adm_entity_id srcID, dlb_adm_entity_id trackID);
        SourceRecord(const SourceRecord &x);
        ~SourceRecord();

        SourceRecord &operator=(const SourceRecord &x);

        bool operator<(const SourceRecord &x) const;

        SourceRecord &Clear();

        bool IsNull() const;
        bool Validate(bool nullOK = false) const;
    };

}

#endif  // DLB_ADM_SOURCE_RECORD_H
