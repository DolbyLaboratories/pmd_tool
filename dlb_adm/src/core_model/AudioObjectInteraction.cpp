/************************************************************************
 * dlb_adm
 * Copyright (c) 2020-2021, Dolby Laboratories Inc.
 * Copyright (c) 2020-2021, Dolby International AB.
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

#include "AudioObjectInteraction.h"
#include "core_model_defs.h"

#include <limits>

namespace DlbAdm
{
    static const float MIN_GAIN_LOW_VALUE = -std::numeric_limits<float>::infinity();
    static const float MIN_GAIN_HIGH_VALUE = 0.0f;
    static const float MAX_GAIN_LOW_VALUE = 0.0f;
    static const float MAX_GAIN_HIGH_VALUE = 30.0f;

    static const int SPHERICAL_OFFSET = 3;


    AudioObjectInteraction::AudioObjectInteraction()
        : ModelEntity()
    {
        Clear();
        mNameLimit = DEFAULT_NAME_LIMIT;
    }

    AudioObjectInteraction::AudioObjectInteraction(const dlb_adm_data_audio_object_interaction &audioObjectInteraction)
        : ModelEntity()
    {
        Clear();
        mNameLimit = DEFAULT_NAME_LIMIT;

        mOnOffInteract = audioObjectInteraction.onOffInteract;
        mGainInteract = audioObjectInteraction.gainInteract;
        mPositionInteract = audioObjectInteraction.positionInteract;
        if (audioObjectInteraction.gainInteract)
        {
            Gain minGain(audioObjectInteraction.minGain);
            mMinGainRange = EnsureGainRange(minGain, MIN_GAIN_LOW_VALUE, MIN_GAIN_HIGH_VALUE);
            Gain maxGain(audioObjectInteraction.minGain);
            mMinGainRange = EnsureGainRange(maxGain, MIN_GAIN_LOW_VALUE, MIN_GAIN_HIGH_VALUE);
        }

        for (auto range : audioObjectInteraction.positionRanges)
        {
            Position::COORDINATE coordinate = range.cartesian ? static_cast<Position::COORDINATE>(range.coordinate) 
                                                                : static_cast<Position::COORDINATE>(range.coordinate + SPHERICAL_OFFSET);

            AssignPositionRange(coordinate, range.minValue, range.maxValue);
        }
    }

    AudioObjectInteraction::AudioObjectInteraction
        (dlb_adm_bool onOffInteract
        ,dlb_adm_bool gainInteract
        ,dlb_adm_bool positionInteract
        ,const Gain &minGain
        ,const Gain &maxGain
        ,const std::map<Position::COORDINATE, float> &minPositions /* = empty map */
        ,const std::map<Position::COORDINATE, float> &maxPositions /* = empty map */
        )
        : ModelEntity()
    {
        mNameLimit = DEFAULT_NAME_LIMIT;
        Clear();
        mOnOffInteract = onOffInteract;
        mGainInteract = gainInteract;
        mPositionInteract = positionInteract;

        if (gainInteract)
        {
            mMinGainRange = EnsureGainRange(minGain, MIN_GAIN_LOW_VALUE, MIN_GAIN_HIGH_VALUE);
            mMaxGainRange = EnsureGainRange(maxGain, MAX_GAIN_LOW_VALUE, MAX_GAIN_HIGH_VALUE);
        }

        /* Do not check positionInteract bacause in some cases it shoud be set to 0
         * and position range still should be present
         */
        if ((minPositions.size() >= 1) && (maxPositions.size() >= 1))
        {
            auto minIt = minPositions.begin();
            auto maxIt = maxPositions.begin();
            /* both positions rengers has to be set or both shoud be clear */
            while (minIt != minPositions.end())
            {
                if (minIt->first == maxIt->first)
                {
                    AssignPositionRange(minIt->first, minIt->second, maxIt->second);
                }
                ++minIt;
                ++maxIt;
            }
        }
    }

    AudioObjectInteraction::~AudioObjectInteraction()
    {
        Clear();
    }

    AudioObjectInteraction::AudioObjectInteraction(const AudioObjectInteraction &x)
        : ModelEntity(x)
        , mOnOffInteract(x.mOnOffInteract)
        , mGainInteract(x.mGainInteract)
        , mPositionInteract(x.mPositionInteract)
        , mMinGainRange(x.mMinGainRange)
        , mMaxGainRange(x.mMaxGainRange)
        , mMinPositionRange(x.mMinPositionRange)
        , mMaxPositionRange(x.mMaxPositionRange)
    {
        // empty
    }

    AudioObjectInteraction& AudioObjectInteraction::operator=(const AudioObjectInteraction &x)
    {
        (void)ModelEntity::operator=(x);
        mOnOffInteract = x.mOnOffInteract;
        mGainInteract = x.mGainInteract;
        mPositionInteract = x.mPositionInteract;
        mMinGainRange = x.mMinGainRange;
        mMaxGainRange = x.mMaxGainRange;
        mMinPositionRange = x.mMinPositionRange;
        mMaxPositionRange = x.mMaxPositionRange;
        return *this;
    }

    void AudioObjectInteraction::Clear()
    {
        mOnOffInteract = 0;
        mGainInteract = 0;
        mPositionInteract = 0;
        mMinGainRange = Gain();
        mMaxGainRange = Gain();
        mMinPositionRange.clear();
        mMaxPositionRange.clear();
    }

    Gain AudioObjectInteraction::EnsureGainRange(const Gain &gain, float min, float max) const
    {
        Gain gain_min = Gain(min, Gain::GAIN_UNIT::DECIBELS);
        Gain gain_max = Gain(max, Gain::GAIN_UNIT::DECIBELS);

        if (gain < gain_min)
        {
            /* TODO: Check gain units */
            return gain_min;
        }
        else if (gain_max < gain)
        {
            return gain_max;
        }
        else
        {
            return gain;
        }
    }

    float AudioObjectInteraction::EnsurePositionRange(const float value, float min, float max) const
    {
        if (value < min)
        {
            return min;
        }
        else if (value > max)
        {
            return max;
        }
        else
        {
            return value;
        }
    }

    void AudioObjectInteraction::AssignPositionRange(Position::COORDINATE coordinate, float min, float max)
    {
        switch (coordinate)
        {
            case Position::COORDINATE::X:
            case Position::COORDINATE::Y:
            case Position::COORDINATE::Z:
                mMinPositionRange[coordinate] = EnsurePositionRange(min, -1.0f, 0.0f);
                mMaxPositionRange[coordinate] = EnsurePositionRange(max, 0.0f, 1.0f);
                break;

            case Position::COORDINATE::AZIMUTH:
            case Position::COORDINATE::ELEVATION:
                mMinPositionRange[coordinate] = EnsurePositionRange(min, -30.0f, 0.0f);
                mMaxPositionRange[coordinate] = EnsurePositionRange(max, 0.0f, 30.0f);
                break;
            case Position::COORDINATE::DISTANCE:
                mMinPositionRange[coordinate] = EnsurePositionRange(min, 0.0f, 1.0f);
                mMaxPositionRange[coordinate] = EnsurePositionRange(max, 0.0f, 1.0f);
                break;
            default:
                break;
        }
    }
}
