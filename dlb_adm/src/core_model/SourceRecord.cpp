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

#include "SourceRecord.h"
#include "dlb_adm/src/adm_identity/AdmIdTranslator.h"

#include <tuple>

namespace DlbAdm
{

    SourceRecord::SourceRecord()
    {
        Clear();
    }

    SourceRecord::SourceRecord(dlb_adm_entity_id groupID, dlb_adm_entity_id srcID, dlb_adm_entity_id trackID)
        : sourceGroupID(groupID)
        , sourceID(srcID)
        , audioTrackID(trackID)
    {
        // Empty
    }

    SourceRecord::SourceRecord(const SourceRecord &x)
        : sourceGroupID(x.sourceGroupID)
        , sourceID(x.sourceID)
        , audioTrackID(x.audioTrackID)
    {
        // Empty
    }

    SourceRecord::~SourceRecord()
    {
        Clear();
    }

    SourceRecord &SourceRecord::operator=(const SourceRecord &x)
    {
        sourceGroupID = x.sourceGroupID;
        sourceID = x.sourceID;
        audioTrackID = x.audioTrackID;

        return *this;
    }

    bool SourceRecord::operator<(const SourceRecord &x) const
    {
        return
            std::tie(  sourceGroupID,   sourceID,   audioTrackID) <
            std::tie(x.sourceGroupID, x.sourceID, x.audioTrackID);
    }

    SourceRecord &SourceRecord::Clear()
    {
        sourceGroupID = DLB_ADM_NULL_ENTITY_ID;
        sourceID      = DLB_ADM_NULL_ENTITY_ID;
        audioTrackID  = DLB_ADM_NULL_ENTITY_ID;

        return *this;
    }

    bool SourceRecord::IsNull() const
    {
        return
            sourceGroupID == DLB_ADM_NULL_ENTITY_ID &&
            sourceID      == DLB_ADM_NULL_ENTITY_ID &&
            audioTrackID  == DLB_ADM_NULL_ENTITY_ID;
    }

    bool SourceRecord::Validate(bool nullOK /* = false */) const
    {
        AdmIdTranslator translator;

        return
            (translator.GetEntityType(sourceGroupID) == DLB_ADM_ENTITY_TYPE_TRANSPORT_TRACK_FORMAT &&
             translator.GetEntityType(sourceID)      == DLB_ADM_ENTITY_TYPE_AUDIO_TRACK &&
             translator.GetEntityType(audioTrackID)  == DLB_ADM_ENTITY_TYPE_TRACK_UID)
            ||
            (nullOK && IsNull());
    }

}

