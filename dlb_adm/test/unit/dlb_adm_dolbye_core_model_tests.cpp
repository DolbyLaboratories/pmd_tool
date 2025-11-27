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

#include "gtest/gtest.h"
#include "dlb_adm/include/dlb_adm_api_types.h"

#include "DolbyeEncoderParameters.h"
#include "DolbyeInfo.h"
#include "DolbyeProgram.h"

using namespace DlbAdm;

TEST(dlb_adm_dolbye_core_model_test, DolbyeInfoTest)
{
    // default constructor
    DolbyeInfo d1;

    dlb_adm_entity_id dolbyeInfoId = 1;
    // constructor
    DolbyeInfo d2(dolbyeInfoId, DLB_ADM_DOLBYE_FRAME_RATE_25, DLB_ADM_DOLBYE_PROGRAM_CONFIG_51_2, 2);

    EXPECT_EQ(d2.GetEntityID(), dolbyeInfoId);

    d2.SetSmpteTimeCode(0, 1, 2, 3);

    dlb_adm_data_SMPTE_timecode_dolbye timecode = d2.GetSmpteTimeCode();
    EXPECT_EQ(timecode.tc1, 0);
    EXPECT_EQ(timecode.tc2, 1);
    EXPECT_EQ(timecode.tc3, 2);
    EXPECT_EQ(timecode.tc4, 3);

    EXPECT_EQ(d2.GetFrameRate(), DLB_ADM_DOLBYE_FRAME_RATE_25);

    EXPECT_EQ(d2.GetProgramConfig(), DLB_ADM_DOLBYE_PROGRAM_CONFIG_51_2);
    
    EXPECT_EQ(d2.GetProgramCount(), 2);

}

TEST(dlb_adm_dolbye_core_model_test, DolbyeEncoderParametersTest)
{
    // default constructor
    DolbyeEncoderParameters d1;

    dlb_adm_entity_id deEncParamsId = 2;
    DolbyeEncoderParameters d2(deEncParamsId, 2, 0, 1, 0, 1, 1, 0);

    EXPECT_EQ(d2.GetEntityID(), deEncParamsId);

    EXPECT_EQ(d2.GetProgramId(), 2);
    EXPECT_EQ(d2.GetHpfon(), 0);
    EXPECT_EQ(d2.GetBwlpfon(), 1);
    EXPECT_EQ(d2.GetLfelpfon(), 0);
    EXPECT_EQ(d2.GetSur90on(), 1);
    EXPECT_EQ(d2.GetSuratton(), 1);
    EXPECT_EQ(d2.GetRfpremphon(), 0);

}

TEST(dlb_adm_dolbye_core_model_test, DolbyeProgramTest)
{
    DolbyeProgram d1;
    dlb_adm_entity_id deProgramId = 3;
    dlb_adm_uint ac3ProgramId = 0;

    DolbyeProgram d2(deProgramId, ac3ProgramId, DLB_ADM_DOLBYE_ACMOD_3_2, DLB_ADM_DOLBYE_BSMOD_COMPLETE_MAIN, 1);
    EXPECT_EQ(d2.GetEntityID(), deProgramId);

    EXPECT_EQ(d2.GetProgramId(), 0);
    EXPECT_EQ(d2.GetAcmod(), DLB_ADM_DOLBYE_ACMOD_3_2);
    EXPECT_EQ(d2.GetBsmod(), DLB_ADM_DOLBYE_BSMOD_COMPLETE_MAIN);
    EXPECT_EQ(d2.GetLfeon(), 1);

    d2.SetDialnorm(31);
    EXPECT_EQ(d2.GetDialnorm(), 31);

    d2.SetCmixlev(DLB_ADM_DOLBYE_CMIXLEV_M30_DB);
    EXPECT_EQ(d2.GetCmixlev(), DLB_ADM_DOLBYE_CMIXLEV_M30_DB);

    d2.SetSurmixlev(DLB_ADM_DOLBYE_SURMIXLEV_M30_DB);
    EXPECT_EQ(d2.GetSurmixlev(), DLB_ADM_DOLBYE_SURMIXLEV_M30_DB);

    d2.SetDsurmod(DLB_ADM_DOLBYE_DSURMOD_NOT_DOLBY_SURROUND);
    EXPECT_EQ(d2.GetDsurmod(), DLB_ADM_DOLBYE_DSURMOD_NOT_DOLBY_SURROUND);

    d2.SetCopyrightb(1);
    EXPECT_EQ(d2.GetCopyrightb(), 1);

    d2.SetOrigbs(1);
    EXPECT_EQ(d2.GetOrigbs(), 1);

    d2.SetLangcode(1, 255);
    EXPECT_EQ(d2.GetLangcodExists(), 1);
    EXPECT_EQ(d2.GetLangcod(), 255);

    d2.SetAudprodi(1, 25, DLB_ADM_DOLBYE_ROOM_TYPE_SMALL);
    EXPECT_EQ(d2.GetAudprodie(), 1);
    EXPECT_EQ(d2.GetMixlevel(), 25);
    EXPECT_EQ(d2.GetRoomtyp(), DLB_ADM_DOLBYE_ROOM_TYPE_SMALL);

    d2.SetXbsi1Md(1, 
                  DLB_ADM_DOLBYE_DMIXMODE_LT_RT, 
                  DLB_ADM_DOLBYE_LX_RX_MIXLEV_0_DB, 
                  DLB_ADM_DOLBYE_LX_RX_MIXLEV_15_DB, 
                  DLB_ADM_DOLBYE_LX_RX_MIXLEV_M15_DB,
                  DLB_ADM_DOLBYE_LX_RX_MIXLEV_M30_DB);

    EXPECT_EQ(d2.GetXbsi1Exists(), 1);
    EXPECT_EQ(d2.GetXbsi1Dmixmod(), DLB_ADM_DOLBYE_DMIXMODE_LT_RT);
    EXPECT_EQ(d2.GetXbsi1Ltrtcmixlev(), DLB_ADM_DOLBYE_LX_RX_MIXLEV_0_DB);
    EXPECT_EQ(d2.GetXbsi1Ltrtsurmixlev(), DLB_ADM_DOLBYE_LX_RX_MIXLEV_15_DB);
    EXPECT_EQ(d2.GetXbsi1Lorocmixlev(), DLB_ADM_DOLBYE_LX_RX_MIXLEV_M15_DB);
    EXPECT_EQ(d2.GetXbsi1Lorosurmixlev(), DLB_ADM_DOLBYE_LX_RX_MIXLEV_M30_DB);

    d2.SetXbsi2Md(1,
                  DLB_ADM_DOLBYE_DSUREXMOD_DOLBY_SURROUND_EX,
                  DLB_ADM_DOLBYE_DHEADPHONEMOD_DOLBY_HEADHONE,
                  1);
    
    EXPECT_EQ(d2.GetXbsi2Exists(), 1);
    EXPECT_EQ(d2.GetXbsi2Dsurexmod(), DLB_ADM_DOLBYE_DSUREXMOD_DOLBY_SURROUND_EX);
    EXPECT_EQ(d2.GetXbsi2Dheadphonmod(), DLB_ADM_DOLBYE_DHEADPHONEMOD_DOLBY_HEADHONE);
    EXPECT_EQ(d2.GetXbsi2Adconvtyp(), 1);

    d2.SetDynrng1(1, 127);
    dlb_adm_data_dolbye_drc drc1 = d2.GetDynrng1();
    EXPECT_EQ(drc1.is_profile, 0);
    EXPECT_EQ(drc1.drc.gain_word, 127);

    d2.SetDynrng1(0, DLB_ADM_DOLBYE_DRC_PROFILE_FILM_LIGHT);
    dlb_adm_data_dolbye_drc drc2 = d2.GetDynrng1();
    EXPECT_EQ(drc2.is_profile, 1);
    EXPECT_EQ(drc2.drc.profile, DLB_ADM_DOLBYE_DRC_PROFILE_FILM_LIGHT);

    d2.SetCompr1(1, 127);
    dlb_adm_data_dolbye_drc drc3 = d2.GetCompr1();
    EXPECT_EQ(drc3.is_profile, 0);
    EXPECT_EQ(drc3.drc.gain_word, 127);

    d2.SetCompr1(0, DLB_ADM_DOLBYE_DRC_PROFILE_FILM_LIGHT);
    dlb_adm_data_dolbye_drc drc4 = d2.GetCompr1();
    EXPECT_EQ(drc4.is_profile, 1);
    EXPECT_EQ(drc4.drc.profile, DLB_ADM_DOLBYE_DRC_PROFILE_FILM_LIGHT);
    
}
