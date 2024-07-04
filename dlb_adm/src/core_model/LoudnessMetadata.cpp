/************************************************************************
 * dlb_adm
 * Copyright (c) 2020, Dolby Laboratories Inc.
 * Copyright (c) 2020, Dolby International AB.
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

#include "LoudnessMetadata.h"

namespace DlbAdm
{
        LoudnessMetadata::LoudnessMetadata()
        : mLoudnessType(DLB_ADM_LOUDNESS_TYPE_NOT_INITIALIZED)
        , mLoudnessValue(0.0)
        {
            // empty
        };

        LoudnessMetadata::LoudnessMetadata(dlb_adm_gain_value value, DLB_ADM_LOUDNESS_TYPE type)
        {
            mLoudnessType = IsValidType(type) ? type : DLB_ADM_LOUDNESS_TYPE_NOT_INITIALIZED;

            if(mLoudnessType != DLB_ADM_LOUDNESS_TYPE_NOT_INITIALIZED)
            {
                mLoudnessValue = value;
            }
            else
            {
                mLoudnessValue = 0.0;
            }
            
        };

        LoudnessMetadata::LoudnessMetadata(const LoudnessMetadata &x)
        {
            mLoudnessType = x.GetLoudnessType();
            mLoudnessValue = x.GetLoudnessValue();
        };

        LoudnessMetadata::~LoudnessMetadata()
        {
            // empty
        };

        bool LoudnessMetadata::IsInitialized() const 
        {
            return mLoudnessType != DLB_ADM_LOUDNESS_TYPE_NOT_INITIALIZED;
        }

        DLB_ADM_LOUDNESS_TYPE LoudnessMetadata::GetLoudnessType() const
        {
            return mLoudnessType;
        };

        dlb_adm_gain_value LoudnessMetadata::GetLoudnessValue() const
        {
            return mLoudnessValue;
        };

        LoudnessMetadata & LoudnessMetadata::operator=(const LoudnessMetadata &x)
        {
            this->mLoudnessType = x.GetLoudnessType();
            this->mLoudnessValue = x.GetLoudnessValue();
            return *this;
        };

        bool LoudnessMetadata::IsValidType(DLB_ADM_LOUDNESS_TYPE type) const
        {
            return (type >= DLB_ADM_LOUDNESS_TYPE_FIRST && type <= DLB_ADM_LOUDNESS_TYPE_LAST);
        };

}
