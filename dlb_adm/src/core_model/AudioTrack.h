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

#ifndef DLB_ADM_AUDIO_TRACK_H
#define DLB_ADM_AUDIO_TRACK_H

#include "ModelEntity.h"
#include "core_model_defs.h"

namespace DlbAdm
{

    class AudioTrack : public ModelEntity
    {
    public:
        AudioTrack();
        explicit AudioTrack(dlb_adm_entity_id id, SampleRate sampleRate = UNKNOWN_SAMPLE_RATE, BitDepth bitDepth = UNKNOWN_BIT_DEPTH);
        AudioTrack(const AudioTrack &x);
        virtual ~AudioTrack();

        AudioTrack &operator=(const AudioTrack &x);

        SampleRate GetSampleRate() const { return mSampleRate; }

        BitDepth GetBitDepth() const { return mBitDepth; }

    private:
        SampleRate mSampleRate;
        BitDepth mBitDepth;
    };

}

#endif // DLB_ADM_AUDIO_TRACK_H
