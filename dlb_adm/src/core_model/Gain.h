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

        bool operator==(const Gain &x) const;

        bool operator!=(const Gain &x) const;

        bool operator<(const Gain &x) const;

        float GetGainValue() const { return mGainValue; }

        GAIN_UNIT GetGainUnit() const { return mGainUnit; }

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
