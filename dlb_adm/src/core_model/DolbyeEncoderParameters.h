/************************************************************************
 * dlb_adm
 * Copyright (c) 2025, Dolby Laboratories Inc.
 * Copyright (c) 2025, Dolby International AB.
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

#ifndef DLB_ADM_DOLBYE_ENCODER_PARAMETERS_H
#define DLB_ADM_DOLBYE_ENCODER_PARAMETERS_H

#include "ModelEntity.h"
#include "dlb_adm/include/dlb_adm_data_types.h"

namespace DlbAdm
{

    class DolbyeEncoderParameters : public ModelEntity
    {
    public:
        DolbyeEncoderParameters();
        DolbyeEncoderParameters(dlb_adm_entity_id id,
                                dlb_adm_uint programId,
                                dlb_adm_bool hpfon, 
                                dlb_adm_bool bwlpfon, 
                                dlb_adm_bool lfelpfon, 
                                dlb_adm_bool sur90on, 
                                dlb_adm_bool suratton, 
                                dlb_adm_bool rfpremphon);
        dlb_adm_uint GetProgramId() const { return mProgramId; }
        dlb_adm_bool GetHpfon() const { return mHpfon; }
        dlb_adm_bool GetBwlpfon() const { return mBwlpfon; }
        dlb_adm_bool GetLfelpfon() const { return mLfelpfon; }
        dlb_adm_bool GetSur90on() const { return mSur90on; }
        dlb_adm_bool GetSuratton() const { return mSuratton; }
        dlb_adm_bool GetRfpremphon() const { return mRfpremphon; }
    private:
        dlb_adm_uint mProgramId;
        dlb_adm_bool mHpfon;
        dlb_adm_bool mBwlpfon;
        dlb_adm_bool mLfelpfon;
        dlb_adm_bool mSur90on;
        dlb_adm_bool mSuratton;
        dlb_adm_bool mRfpremphon;
    };

}

#endif // DLB_ADM_DOLBYE_ENCODER_PARAMETERS_H
