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

#include "ElementRecord.h"
#include "dlb_adm/src/adm_identity/AdmIdTranslator.h"

#include <tuple>

namespace DlbAdm
{

    ElementRecord::ElementRecord()
    {
        Clear();
    }

    ElementRecord::ElementRecord(dlb_adm_entity_id elementID,
                                 dlb_adm_entity_id groupID,
                                 dlb_adm_entity_id tgtID,
                                 dlb_adm_entity_id trackID)
        : audioElementID(elementID)
        , targetGroupID(groupID)
        , targetID(tgtID)
        , audioTrackID(trackID)
    {
        // Empty
    }

    ElementRecord::ElementRecord(const ElementRecord &x)
        : audioElementID(x.audioElementID)
        , targetGroupID(x.targetGroupID)
        , targetID(x.targetID)
        , audioTrackID(x.audioTrackID)
    {
        // Empty
    }

    ElementRecord::~ElementRecord()
    {
        Clear();
    }

    ElementRecord &ElementRecord::operator=(const ElementRecord &x)
    {
        audioElementID = x.audioElementID;
        targetGroupID = x.targetGroupID;
        targetID = x.targetID;
        audioTrackID = x.audioTrackID;

        return *this;
    }

    bool ElementRecord::operator<(const ElementRecord &x) const
    {
        return
            std::tie(  audioElementID,   targetGroupID,   targetID,   audioTrackID) <
            std::tie(x.audioElementID, x.targetGroupID, x.targetID, x.audioTrackID);
    }

    ElementRecord &ElementRecord::Clear()
    {
        audioElementID = DLB_ADM_NULL_ENTITY_ID;
        targetGroupID  = DLB_ADM_NULL_ENTITY_ID;
        targetID       = DLB_ADM_NULL_ENTITY_ID;
        audioTrackID   = DLB_ADM_NULL_ENTITY_ID;

        return *this;
    }

    bool ElementRecord::IsNull() const
    {
        return
            audioElementID == DLB_ADM_NULL_ENTITY_ID &&
            targetGroupID  == DLB_ADM_NULL_ENTITY_ID &&
            targetID       == DLB_ADM_NULL_ENTITY_ID &&
            audioTrackID   == DLB_ADM_NULL_ENTITY_ID;
    }

    bool ElementRecord::Validate(bool nullOK /* = false */) const
    {
        AdmIdTranslator translator;
        bool good =
            (translator.GetEntityType(audioElementID) == DLB_ADM_ENTITY_TYPE_OBJECT &&
             translator.GetEntityType(targetGroupID)  == DLB_ADM_ENTITY_TYPE_PACK_FORMAT &&
             translator.GetEntityType(targetID)       == DLB_ADM_ENTITY_TYPE_CHANNEL_FORMAT &&
             translator.GetEntityType(audioTrackID)   == DLB_ADM_ENTITY_TYPE_TRACK_UID)
            ||
            (nullOK && IsNull());

        return good;
    }

}
