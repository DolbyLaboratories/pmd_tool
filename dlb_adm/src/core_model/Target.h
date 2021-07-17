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

#ifndef DLB_ADM_TARGET_H
#define DLB_ADM_TARGET_H

#include "ModelEntity.h"

namespace DlbAdm
{

    class Target : public ModelEntity
    {
    public:
        Target();
        Target(dlb_adm_entity_id entityID, DLB_ADM_AUDIO_TYPE audioType, const std::string &speakerLabel);
        Target(const Target &x);
        virtual ~Target();

        Target &operator=(const Target &x);

        DLB_ADM_AUDIO_TYPE GetAudioType() const { return mAudioType; }

        std::string GetAudioTypeString() const { return ModelEntity::TranslateAudioType(mAudioType); }

        std::string GetSpeakerLabel() const { return mSpeakerLabel; }

    private:
        DLB_ADM_AUDIO_TYPE mAudioType;
        std::string mSpeakerLabel;
    };

}

#endif  // DLB_ADM_TARGET_H
