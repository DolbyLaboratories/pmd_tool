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

#ifndef DLB_ADM_PRESENTATION_RECORD_H
#define DLB_ADM_PRESENTATION_RECORD_H

#include "dlb_adm/include/dlb_adm_api_types.h"

namespace DlbAdm
{

    struct PresentationRecord
    {
        dlb_adm_entity_id   presentationID;
        dlb_adm_entity_id   contentGroupID;
        dlb_adm_entity_id   elementGroupID;
        dlb_adm_entity_id   audioElementID;

        PresentationRecord();
        PresentationRecord(dlb_adm_entity_id presID,
                           dlb_adm_entity_id contentGrpID,
                           dlb_adm_entity_id elementID,
                           dlb_adm_entity_id elementGrpID = DLB_ADM_NULL_ENTITY_ID);
        PresentationRecord(const PresentationRecord &x);
        ~PresentationRecord();

        PresentationRecord &operator=(const PresentationRecord &x);

        bool operator<(const PresentationRecord &x) const;

        PresentationRecord &Clear();

        bool IsNull() const;
        bool Validate(bool nullOK = false) const;
    };

}

#endif  // DLB_ADM_PRESENTATION_RECORD_H
