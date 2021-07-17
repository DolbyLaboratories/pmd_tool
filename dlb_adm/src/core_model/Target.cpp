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

#include "Target.h"

namespace DlbAdm
{

    static const size_t NAME_LIMIT = 1;

    Target::Target()
        : ModelEntity()
        , mAudioType(DLB_ADM_AUDIO_TYPE_NONE)
        , mSpeakerLabel()
    {
        mNameLimit = NAME_LIMIT;
    }

    Target::Target(dlb_adm_entity_id entityID, DLB_ADM_AUDIO_TYPE audioType, const std::string &speakerLabel)
        : ModelEntity(entityID, NAME_LIMIT)
        , mAudioType(audioType)
        , mSpeakerLabel(speakerLabel)
    {
        // Empty
    }

    Target::Target(const Target &x)
        : ModelEntity(x)
        , mAudioType(x.mAudioType)
        , mSpeakerLabel(x.mSpeakerLabel)
    {
        // Empty
    }

    Target::~Target()
    {
        mAudioType = DLB_ADM_AUDIO_TYPE_NONE;
    }

    Target & Target::operator=(const Target & x)
    {
        (void)ModelEntity::operator=(x);
        mAudioType = x.mAudioType;
        mSpeakerLabel = x.mSpeakerLabel;
        return *this;
    }

}
