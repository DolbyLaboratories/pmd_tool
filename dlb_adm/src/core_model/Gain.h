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

#ifndef DLB_ADM_GAIN_H
#define DLB_ADM_GAIN_H

#include "dlb_adm/include/dlb_adm_data_types.h"

namespace DlbAdm
{

    class Gain
    {
    public:
        enum class GAIN_UNIT
        {
            LINEAR,
            DECIBELS
        };

        Gain();
        explicit Gain(const dlb_adm_data_gain &gain);
        explicit Gain(float value, GAIN_UNIT unit = GAIN_UNIT::LINEAR);
        Gain(const Gain &x);
        ~Gain();

        Gain &operator=(const Gain &x);

        float GetGainValue() const { return mGainValue; }

        GAIN_UNIT GetGainUnit() const { return mGainUnit; }

        bool operator<(const Gain &x) const;

        bool Convert(GAIN_UNIT gainUnit);

        bool IsUnity() const;

        void Clear(bool toZero = false);

        static float LinearToDecibels(float gainValue);

        static float DecibelsToLinear(float gainValue);

    private:
        float mGainValue;
        GAIN_UNIT mGainUnit;
    };

}

#endif // DLB_ADM_GAIN_H
