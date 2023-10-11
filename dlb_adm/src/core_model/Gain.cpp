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

#include "Gain.h"

#include <cmath>

namespace DlbAdm
{

    // Assumption: float can represent these two values exactly
    static const float UNITY_LINEAR_GAIN = 1.0f;
    static const float UNITY_DB_GAIN = 0.0f;

    Gain::Gain()
    {
        Clear();
    }

    Gain::Gain(const dlb_adm_data_gain &gain)
    {
        Clear();
        if ((gain.gain_unit >= DLB_ADM_GAIN_UNIT_FIRST) && (gain.gain_unit <= DLB_ADM_GAIN_UNIT_LAST))
        {
            mGainValue = gain.gain_value;
            mGainUnit = static_cast<GAIN_UNIT>(gain.gain_unit);
        }
    }

    Gain::Gain(float value, GAIN_UNIT unit /*= GAIN_UNIT::LINEAR*/)
        : mGainValue(value)
        , mGainUnit(unit)
    {
        // Empty
    }

    Gain::Gain(const Gain &x)
        : mGainValue(x.mGainValue)
        , mGainUnit(x.mGainUnit)
    {
        // Empty
    }

    Gain::~Gain()
    {
        Clear(true);
    }

    Gain & Gain::operator=(const Gain &x)
    {
        mGainValue = x.mGainValue;
        mGainUnit = x.mGainUnit;
        return *this;
    }

    bool Gain::operator<(const Gain &x) const
    {
        bool lessp = false;

        if (mGainUnit == x.mGainUnit)
        {
            lessp = (mGainValue < x.mGainValue);
        }
        else if (mGainUnit == GAIN_UNIT::LINEAR)
        {
            lessp = (mGainValue < DecibelsToLinear(x.mGainValue));
        } 
        else
        {
            lessp = (mGainValue < LinearToDecibels(x.mGainValue));
        }

        return lessp;
    }

    bool Gain::operator==(const Gain &x) const
    {
        return (mGainUnit == x.mGainUnit)
            && (mGainValue == x.mGainValue);
    }

    bool Gain::operator!=(const Gain &x) const
    {
        return !operator==(x);
    }

    bool Gain::Convert(GAIN_UNIT gainUnit)
    {
        bool converted = false;

        if (mGainUnit != gainUnit)
        {
            if (mGainUnit == GAIN_UNIT::LINEAR)
            {
                mGainValue = LinearToDecibels(mGainValue);
            } 
            else
            {
                mGainValue = DecibelsToLinear(mGainValue);
            }
            mGainUnit = gainUnit;
            converted = true;
        }

        return converted;
    }

    bool Gain::IsUnity() const
    {
        bool isUnity = false;

        // Use exact match for gain value
        if (((mGainUnit == GAIN_UNIT::DECIBELS) && (mGainValue == UNITY_DB_GAIN)) ||
            ((mGainUnit == GAIN_UNIT::LINEAR)   && (mGainValue == UNITY_LINEAR_GAIN)))
        {
            isUnity = true;
        }

        return isUnity;
    }

    void Gain::Clear(bool toZero)
    {
        mGainValue = toZero ? 0.0f : UNITY_LINEAR_GAIN;
        mGainUnit = GAIN_UNIT::LINEAR;
    }

    float Gain::LinearToDecibels(float gainValue)
    {
        float dBGain = UNITY_DB_GAIN;

        if (gainValue != UNITY_LINEAR_GAIN) // Use exact match for unity gain
        {
            dBGain = 20.0f * ::log10f(gainValue);
        }

        return dBGain;
    }

    float Gain::DecibelsToLinear(float gainValue)
    {
        float linearGain = UNITY_LINEAR_GAIN;

        if (gainValue != UNITY_DB_GAIN)     // Use exact match for unity gain
        {
            linearGain = ::powf(10.0f, gainValue / 20.0f);
        }

        return linearGain;
    }

}
