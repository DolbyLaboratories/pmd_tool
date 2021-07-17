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

#include "AudioTrack.h"

namespace DlbAdm
{

    AudioTrack::AudioTrack()
        : ModelEntity()
        , mSampleRate(UNKNOWN_SAMPLE_RATE)
        , mBitDepth(UNKNOWN_BIT_DEPTH)
    {
        // Empty
    }

    AudioTrack::AudioTrack(dlb_adm_entity_id id, SampleRate sampleRate /*= UNKNOWN_SAMPLE_RATE*/, BitDepth bitDepth /*= UNKNOWN_BIT_DEPTH*/)
        : ModelEntity(id)
        , mSampleRate(sampleRate)
        , mBitDepth(bitDepth)
    {
        // Empty
    }

    AudioTrack::AudioTrack(const AudioTrack &x)
        : ModelEntity(x)
        , mSampleRate(x.mSampleRate)
        , mBitDepth(x.mBitDepth)
    {
        // Empty
    }

    AudioTrack::~AudioTrack()
    {
        mSampleRate = UNKNOWN_SAMPLE_RATE;
        mBitDepth = UNKNOWN_BIT_DEPTH;
    }

    AudioTrack &AudioTrack::operator=(const AudioTrack &x)
    {
        (void)ModelEntity::operator=(x);
        mSampleRate = x.mSampleRate;
        mBitDepth = x.mBitDepth;
        return *this;
    }

}
