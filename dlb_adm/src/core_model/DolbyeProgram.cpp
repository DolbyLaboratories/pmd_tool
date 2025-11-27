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

#include "DolbyeProgram.h"

namespace DlbAdm
{
    DolbyeProgram::DolbyeProgram()
        : ModelEntity()
    {
        mNameLimit = 1;
    }

    DolbyeProgram::DolbyeProgram(dlb_adm_entity_id id,
                                 dlb_adm_uint programId,
                                 DLB_ADM_DOLBYE_ACMOD acmod,
                                 DLB_ADM_DOLBYE_BSMOD bsmod,
                                 dlb_adm_bool lfeon)
        : ModelEntity(id, 1)
        , mAC3ProgramId(programId)
        , mAcmod(acmod)
        , mBsmod(bsmod)
        , mLfeon(lfeon)
    {
    }

    void DolbyeProgram::SetXbsi1Md(dlb_adm_bool xbsi1Exists,
                                   DLB_ADM_DOLBYE_DMIXMODE xbsi1Dmixmod,
                                   DLB_ADM_DOLBYE_LX_RX_MIXLEV xbsi1Ltrtcmixlev,
                                   DLB_ADM_DOLBYE_LX_RX_MIXLEV xbsi1Ltrtsurmixlev,
                                   DLB_ADM_DOLBYE_LX_RX_MIXLEV xbsi1Lorocmixlev,
                                   DLB_ADM_DOLBYE_LX_RX_MIXLEV xbsi1Lorosurmixlev)
    { 
        mXbsi1Exists = xbsi1Exists;
        mXbsi1Dmixmod = xbsi1Dmixmod;
        mXbsi1Ltrtcmixlev = xbsi1Ltrtcmixlev;
        mXbsi1Ltrtsurmixlev = xbsi1Ltrtsurmixlev;
        mXbsi1Lorocmixlev = xbsi1Lorocmixlev;
        mXbsi1Lorosurmixlev = xbsi1Lorosurmixlev;
    }

    void DolbyeProgram::SetXbsi2Md(dlb_adm_bool xbsi2Exists,
                                   DLB_ADM_DOLBYE_DSUREXMOD xbsi2Dsurexmod,
                                   DLB_ADM_DOLBYE_DHEADPHONEMOD xbsi2Dheadphonmod,
                                   dlb_adm_bool xbsi2Adconvtyp)
    { 
        mXbsi2Exists = xbsi2Exists;
        mXbsi2Dsurexmod = xbsi2Dsurexmod;
        mXbsi2Dheadphonmod = xbsi2Dheadphonmod;
        mXbsi2Adconvtyp = xbsi2Adconvtyp;
    }

    void DolbyeProgram::SetDynrng1(dlb_adm_bool is_exist, int32_t value)
    {
        if (is_exist)
        {
            mDynrng1.drc.gain_word = value;
            mDynrng1.is_profile = DLB_ADM_FALSE;
        }
        else
        {
            mDynrng1.drc.profile = static_cast<DLB_ADM_DOLBYE_DRC_PROFILE>(value);
            mDynrng1.is_profile = DLB_ADM_TRUE;
        }
    }

    void DolbyeProgram::SetCompr1(dlb_adm_bool is_exist, int32_t value)
    {
        if (is_exist)
        {
            mCompr1.drc.gain_word = value;
            mCompr1.is_profile = DLB_ADM_FALSE;
        }
        else
        {
            mCompr1.drc.profile = static_cast<DLB_ADM_DOLBYE_DRC_PROFILE>(value);
            mCompr1.is_profile = DLB_ADM_TRUE;
        }
    }

}
