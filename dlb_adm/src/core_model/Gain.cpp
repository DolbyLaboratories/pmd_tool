/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2020-2021 by Dolby Laboratories,
 *                Copyright (C) 2020-2021 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

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
