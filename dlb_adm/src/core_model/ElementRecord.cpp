/************************************************************************
 * dlb_adm
 * Copyright (c) 2021, Dolby Laboratories Inc.
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
