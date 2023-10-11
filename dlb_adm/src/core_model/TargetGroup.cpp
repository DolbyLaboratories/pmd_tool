/************************************************************************
 * dlb_adm
 * Copyright (c) 2023, Dolby Laboratories Inc.
 * Copyright (c) 2023, Dolby International AB.
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

#include "TargetGroup.h"

namespace DlbAdm
{

    static const size_t NAME_LIMIT = 1;

    TargetGroup::TargetGroup()
        : ModelEntity()
        , mSpeakerConfig(DLB_ADM_SPEAKER_CONFIG_NONE)
        , mAudioType(DLB_ADM_AUDIO_TYPE_NONE)
        , mIsDynamic(false)
    {
        mNameLimit = NAME_LIMIT;
    }

    TargetGroup::TargetGroup(dlb_adm_entity_id id, DLB_ADM_SPEAKER_CONFIG speakerConfig, bool isCommon /*= false*/)
        : ModelEntity(id, NAME_LIMIT, isCommon)
        , mSpeakerConfig(speakerConfig)
        , mAudioType(DLB_ADM_AUDIO_TYPE_DIRECT_SPEAKERS)
        , mIsDynamic(false)
    {
        // Empty
    }

    TargetGroup::TargetGroup(dlb_adm_entity_id id, DLB_ADM_AUDIO_TYPE audioType, bool isDynamic)
        : ModelEntity(id, NAME_LIMIT)
        , mSpeakerConfig(DLB_ADM_SPEAKER_CONFIG_NONE)
        , mAudioType(audioType)
        , mIsDynamic(isDynamic)
    {
        // Empty
    }

    TargetGroup::TargetGroup(const TargetGroup &x)
        : ModelEntity(x)
        , mSpeakerConfig(x.mSpeakerConfig)
        , mAudioType(x.mAudioType)
        , mIsDynamic(x.mIsDynamic)
    {
        // Empty
    }

    TargetGroup::~TargetGroup()
    {
        mSpeakerConfig = DLB_ADM_SPEAKER_CONFIG_NONE;
        mAudioType = DLB_ADM_AUDIO_TYPE_NONE;
        mIsDynamic = false;
    }

    TargetGroup &TargetGroup::operator=(const TargetGroup &x)
    {
        (void)ModelEntity::operator=(x);
        mSpeakerConfig = x.mSpeakerConfig;
        mAudioType = x.mAudioType;
        mIsDynamic = x.mIsDynamic;
        return *this;
    }

}
