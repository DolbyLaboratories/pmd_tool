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
#ifndef DLB_ADM_DOLBYE_PROGRAM_H
#define DLB_ADM_DOLBYE_PROGRAM_H

#include "ModelEntity.h"
#include "dlb_adm/include/dlb_adm_data_types.h"

namespace DlbAdm
{
    constexpr unsigned int MAX_DOLBYE_DIALNORM_VALUE = 31;
    constexpr unsigned int MAX_DOLBYE_MIXLEVEL_VALUE = 31;
    constexpr unsigned int MAX_DOLBYE_DRC_GAIN_WORD_VALUE = 255;
    constexpr unsigned int MAX_DOLBYE_LANG_COD = 255;

    class DolbyeProgram : public ModelEntity
    {
    public:
        DolbyeProgram();
        DolbyeProgram(dlb_adm_entity_id id,
                      dlb_adm_uint programId,
                      DLB_ADM_DOLBYE_ACMOD acmod,
                      DLB_ADM_DOLBYE_BSMOD bsmod,
                      dlb_adm_bool lfeon);

        dlb_adm_uint GetProgramId() const { return mAC3ProgramId; }
        DLB_ADM_DOLBYE_ACMOD GetAcmod() const { return mAcmod; }
        DLB_ADM_DOLBYE_BSMOD GetBsmod() const { return mBsmod; }
        dlb_adm_bool GetLfeon() const { return mLfeon; }
        DLB_ADM_DOLBYE_CMIXLEV GetCmixlev() const { return mCmixlev; }
        DLB_ADM_DOLBYE_SURMIXLEV GetSurmixlev() const { return mSurmixlev; }
        DLB_ADM_DOLBYE_DSURMOD GetDsurmod() const { return mDsurmod; }
        int32_t GetDialnorm() const { return mDialnorm; }
        dlb_adm_bool GetCopyrightb() const { return mCopyrightb; }
        dlb_adm_bool GetOrigbs() const { return mOrigbs; }
        dlb_adm_bool GetLangcodExists() const { return mLangcodExists; }
        int32_t GetLangcod() const { return mLangcod; }
        dlb_adm_bool GetAudprodie() const { return mAudprodie; }
        int32_t GetMixlevel() const { return mMixlevel; }
        DLB_ADM_DOLBYE_ROOM_TYPE GetRoomtyp() const { return mRoomtyp; }
        dlb_adm_bool GetXbsi1Exists() const { return mXbsi1Exists; }
        DLB_ADM_DOLBYE_DMIXMODE GetXbsi1Dmixmod() const { return mXbsi1Dmixmod; }
        DLB_ADM_DOLBYE_LX_RX_MIXLEV GetXbsi1Ltrtcmixlev() const { return mXbsi1Ltrtcmixlev; }
        DLB_ADM_DOLBYE_LX_RX_MIXLEV GetXbsi1Ltrtsurmixlev() const { return mXbsi1Ltrtsurmixlev; }
        DLB_ADM_DOLBYE_LX_RX_MIXLEV GetXbsi1Lorocmixlev() const { return mXbsi1Lorocmixlev; }
        DLB_ADM_DOLBYE_LX_RX_MIXLEV GetXbsi1Lorosurmixlev() const { return mXbsi1Lorosurmixlev; }
        dlb_adm_bool GetXbsi2Exists() const { return mXbsi2Exists; }
        DLB_ADM_DOLBYE_DSUREXMOD GetXbsi2Dsurexmod() const { return mXbsi2Dsurexmod; }
        DLB_ADM_DOLBYE_DHEADPHONEMOD GetXbsi2Dheadphonmod() const { return mXbsi2Dheadphonmod; }
        dlb_adm_bool GetXbsi2Adconvtyp() const { return mXbsi2Adconvtyp; }
        dlb_adm_data_dolbye_drc GetDynrng1() const { return mDynrng1; }
        dlb_adm_data_dolbye_drc GetCompr1() const { return mCompr1; }

        void SetProgramId(dlb_adm_uint programId) { mAC3ProgramId = programId; }
        void SetCmixlev(DLB_ADM_DOLBYE_CMIXLEV cmixlev) { mCmixlev = cmixlev; }
        void SetSurmixlev(DLB_ADM_DOLBYE_SURMIXLEV surmixlev) { mSurmixlev = surmixlev; }
        void SetDsurmod(DLB_ADM_DOLBYE_DSURMOD dsurmod) { mDsurmod = dsurmod; }
        void SetDialnorm(int32_t dialnorm) { mDialnorm = dialnorm; }
        void SetCopyrightb(dlb_adm_bool copyrightb) { mCopyrightb = copyrightb; }
        void SetOrigbs(dlb_adm_bool origbs) { mOrigbs = origbs; }
        void SetLangcode(dlb_adm_bool langcodExists, int32_t langcod) { mLangcodExists = langcodExists; mLangcod = langcod; }
        void SetAudprodi(dlb_adm_bool audprodie, int32_t mixlevel, DLB_ADM_DOLBYE_ROOM_TYPE roomtyp) { mAudprodie = audprodie; mMixlevel = mixlevel; mRoomtyp = roomtyp;}
        void SetXbsi1Md(dlb_adm_bool xbsi1Exists,
                        DLB_ADM_DOLBYE_DMIXMODE xbsi1Dmixmod,
                        DLB_ADM_DOLBYE_LX_RX_MIXLEV xbsi1Ltrtcmixlev,
                        DLB_ADM_DOLBYE_LX_RX_MIXLEV xbsi1Ltrtsurmixlev,
                        DLB_ADM_DOLBYE_LX_RX_MIXLEV xbsi1Lorocmixlev,
                        DLB_ADM_DOLBYE_LX_RX_MIXLEV xbsi1Lorosurmixlev);
        void SetXbsi2Md(dlb_adm_bool xbsi2Exists,
                        DLB_ADM_DOLBYE_DSUREXMOD xbsi2Dsurexmod,
                        DLB_ADM_DOLBYE_DHEADPHONEMOD xbsi2Dheadphonmod,
                        dlb_adm_bool xbsi2Adconvtyp);

        void SetDynrng1(dlb_adm_bool is_exist, int32_t value);
        void SetCompr1(dlb_adm_bool is_exist, int32_t value);

    private:
        dlb_adm_uint         mAC3ProgramId;
        DLB_ADM_DOLBYE_ACMOD mAcmod;
        DLB_ADM_DOLBYE_BSMOD mBsmod;
        dlb_adm_bool mLfeon;
        DLB_ADM_DOLBYE_CMIXLEV mCmixlev;
        DLB_ADM_DOLBYE_SURMIXLEV mSurmixlev;
        DLB_ADM_DOLBYE_DSURMOD mDsurmod;
        int32_t mDialnorm;
        dlb_adm_bool mCopyrightb;
        dlb_adm_bool mOrigbs;
        dlb_adm_bool mLangcodExists;
        int32_t mLangcod;
        dlb_adm_bool mAudprodie;
        int32_t mMixlevel;
        DLB_ADM_DOLBYE_ROOM_TYPE mRoomtyp;
        dlb_adm_bool mXbsi1Exists;
        DLB_ADM_DOLBYE_DMIXMODE mXbsi1Dmixmod;
        DLB_ADM_DOLBYE_LX_RX_MIXLEV mXbsi1Ltrtcmixlev;
        DLB_ADM_DOLBYE_LX_RX_MIXLEV mXbsi1Ltrtsurmixlev;
        DLB_ADM_DOLBYE_LX_RX_MIXLEV mXbsi1Lorocmixlev;
        DLB_ADM_DOLBYE_LX_RX_MIXLEV mXbsi1Lorosurmixlev;
        dlb_adm_bool mXbsi2Exists;
        DLB_ADM_DOLBYE_DSUREXMOD mXbsi2Dsurexmod;
        DLB_ADM_DOLBYE_DHEADPHONEMOD mXbsi2Dheadphonmod;
        dlb_adm_bool mXbsi2Adconvtyp;
        dlb_adm_data_dolbye_drc mDynrng1;
        dlb_adm_data_dolbye_drc mCompr1;
    };

}

#endif // DLB_ADM_DOLBYE_PROGRAM_H
