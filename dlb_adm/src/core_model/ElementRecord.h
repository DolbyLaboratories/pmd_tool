/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2020-2021 by Dolby Laboratories,
 *                Copyright (C) 2020-2021 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#ifndef DLB_ADM_ELEMENT_RECORD_H
#define DLB_ADM_ELEMENT_RECORD_H

#include "dlb_adm/include/dlb_adm_entity_id.h"

namespace DlbAdm
{

    struct ElementRecord
    {
        dlb_adm_entity_id   audioElementID;
        dlb_adm_entity_id   targetGroupID;
        dlb_adm_entity_id   targetID;
        dlb_adm_entity_id   audioTrackID;

        ElementRecord();
        ElementRecord
        (
            dlb_adm_entity_id elementID,
            dlb_adm_entity_id groupID,
            dlb_adm_entity_id tgtID,
            dlb_adm_entity_id trackID
        );
        ElementRecord(const ElementRecord &x);
        ~ElementRecord();

        ElementRecord &operator=(const ElementRecord &x);

        bool operator<(const ElementRecord &x) const;

        ElementRecord &Clear();

        bool IsNull() const;
        bool Validate(bool nullOK = false) const;
    };

}

#endif  // DLB_ADM_ELEMENT_RECORD_H
