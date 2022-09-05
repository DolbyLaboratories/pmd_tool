/************************************************************************
 * dlb_adm
 * Copyright (c) 2020 - 2022, Dolby Laboratories Inc.
 * Copyright (c) 2022, Dolby International AB.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 **********************************************************************/

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

