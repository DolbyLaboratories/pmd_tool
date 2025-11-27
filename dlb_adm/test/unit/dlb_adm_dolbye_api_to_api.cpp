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

#include "dlb_adm/include/dlb_adm_api.h"
#include "dlb_adm/include/dlb_adm_api_types.h"

#include "UnitTestCommon.h"

class DlbAPItoAPICommon : public testing::Test
{
    protected:

        dlb_adm_core_model_counts    coreModelCounts;
        dlb_adm_core_model          *coreModel;
        dlb_adm_data_dolbye_data     inputData;
        dlb_adm_data_dolbye_data     outputData;

        virtual void SetUp()
        {
            ::memset(&inputData, 0, sizeof(dlb_adm_data_dolbye_data));
            ::memset(&outputData, 0, sizeof(dlb_adm_data_dolbye_data));
            int status;
            ::memset(&coreModelCounts, 0, sizeof(coreModelCounts));
            coreModel = nullptr;
    
            status = ::dlb_adm_core_model_open(&coreModel, &coreModelCounts);
            ASSERT_EQ(DLB_ADM_STATUS_OK, status);

        }

        virtual void TearDown()
        {
            if (coreModel != nullptr)
            {
                if (::dlb_adm_core_model_close(&coreModel))
                {
                    coreModel = nullptr;
                }
            }
        }
};

TEST_F(DlbAPItoAPICommon, DolbyEData_51_20)
{
    inputData.info.program_config_id = DLB_ADM_DOLBYE_PROGRAM_CONFIG_51_2;
    inputData.info.frame_rate = DLB_ADM_DOLBYE_FRAME_RATE_2398;
    inputData.info.time_code.tc1 = 11;
    inputData.info.time_code.tc2 = 45;
    inputData.info.time_code.tc3 = 24;
    inputData.info.time_code.tc4 = 15;
    inputData.program_count = 2;

    inputData.program_ac3_params[0].acmod = DLB_ADM_DOLBYE_ACMOD_3_2;
    inputData.program_ac3_params[0].bsmod = DLB_ADM_DOLBYE_BSMOD_COMPLETE_MAIN;
    inputData.program_ac3_params[0].lfeon = 1;
    inputData.program_ac3_params[0].cmixlev = DLB_ADM_DOLBYE_CMIXLEV_M45_DB;
    inputData.program_ac3_params[0].surmixlev = DLB_ADM_DOLBYE_SURMIXLEV_0;
    inputData.program_ac3_params[0].dsurmod = DLB_ADM_DOLBYE_DSURMOD_NOT_INDICATED;
    inputData.program_ac3_params[0].dialnorm = 23;
    inputData.program_ac3_params[0].copyrightb = 1;
    inputData.program_ac3_params[0].origbs = 0;
    inputData.program_ac3_params[0].langcod_exists = 0;
    inputData.program_ac3_params[0].audprodie = 1;
    inputData.program_ac3_params[0].mixlevel = 21;
    inputData.program_ac3_params[0].roomtyp = DLB_ADM_DOLBYE_ROOM_TYPE_SMALL;
    inputData.program_ac3_params[0].xbsi1_exists = 1;
    inputData.program_ac3_params[0].xbsi1_dmixmod = DLB_ADM_DOLBYE_DMIXMODE_LO_RO;
    inputData.program_ac3_params[0].xbsi1_lorocmixlev = DLB_ADM_DOLBYE_LX_RX_MIXLEV_30_DB;
    inputData.program_ac3_params[0].xbsi1_lorosurmixlev = DLB_ADM_DOLBYE_LX_RX_MIXLEV_0_DB;
    inputData.program_ac3_params[0].xbsi1_ltrtcmixlev = DLB_ADM_DOLBYE_LX_RX_MIXLEV_M30_DB;
    inputData.program_ac3_params[0].xbsi1_ltrtsurmixlev = DLB_ADM_DOLBYE_LX_RX_MIXLEV_M60_DB;
    inputData.program_ac3_params[0].xbsi2_exists = 1;
    inputData.program_ac3_params[0].xbsi2_dsurexmod = DLB_ADM_DOLBYE_DSUREXMOD_DOLBY_SURROUND_EX;
    inputData.program_ac3_params[0].xbsi2_dheadphonmod = DLB_ADM_DOLBYE_DHEADPHONEMOD_DOLBY_HEADHONE;
    inputData.program_ac3_params[0].xbsi2_adconvtyp = 0;
    strncpy(inputData.program_ac3_params[0].programDescriptionText, "The best program", 17);
    inputData.program_ac3_params[0].dynrng1.is_profile = 1;
    inputData.program_ac3_params[0].dynrng1.drc.profile = DLB_ADM_DOLBYE_DRC_PROFILE_MUSIC_STANDARD;
    inputData.program_ac3_params[0].compr1.is_profile = 1;
    inputData.program_ac3_params[0].compr1.drc.profile = DLB_ADM_DOLBYE_DRC_PROFILE_SPEECH;
    inputData.program_encode_params[0].hpfon = 1;
    inputData.program_encode_params[0].bwlpfon = 0;
    inputData.program_encode_params[0].lfelpfon = 1;
    inputData.program_encode_params[0].sur90on = 0;
    inputData.program_encode_params[0].suratton = 1;
    inputData.program_encode_params[0].rfpremphon = 0;

    inputData.program_ac3_params[1].acmod = DLB_ADM_DOLBYE_ACMOD_1_0;
    inputData.program_ac3_params[1].bsmod = DLB_ADM_DOLBYE_BSMOD_VISUALLY_IMPAIRED;
    inputData.program_ac3_params[1].lfeon = 0;
    inputData.program_ac3_params[1].cmixlev = DLB_ADM_DOLBYE_CMIXLEV_M60_DB;
    inputData.program_ac3_params[1].surmixlev = DLB_ADM_DOLBYE_SURMIXLEV_M60_DB;
    inputData.program_ac3_params[1].dsurmod = DLB_ADM_DOLBYE_DSURMOD_DOLBY_SURROUND;
    inputData.program_ac3_params[1].dialnorm = 27;
    inputData.program_ac3_params[1].copyrightb = 0;
    inputData.program_ac3_params[1].origbs = 0;
    inputData.program_ac3_params[1].langcod_exists = 1;
    inputData.program_ac3_params[1].langcod = 15;
    inputData.program_ac3_params[1].audprodie = 0;
    inputData.program_ac3_params[1].xbsi1_exists = 1;
    inputData.program_ac3_params[1].xbsi1_dmixmod = DLB_ADM_DOLBYE_DMIXMODE_NOT_INDICATED;
    inputData.program_ac3_params[1].xbsi1_lorocmixlev = DLB_ADM_DOLBYE_LX_RX_MIXLEV_0_DB;
    inputData.program_ac3_params[1].xbsi1_lorosurmixlev = DLB_ADM_DOLBYE_LX_RX_MIXLEV_MINF_DB;
    inputData.program_ac3_params[1].xbsi1_ltrtcmixlev = DLB_ADM_DOLBYE_LX_RX_MIXLEV_30_DB;
    inputData.program_ac3_params[1].xbsi1_ltrtsurmixlev = DLB_ADM_DOLBYE_LX_RX_MIXLEV_M60_DB;
    inputData.program_ac3_params[1].xbsi2_exists = 0;
    strncpy(inputData.program_ac3_params[1].programDescriptionText, "Not even a program", 19);
    inputData.program_ac3_params[1].dynrng1.is_profile = 0;
    inputData.program_ac3_params[1].dynrng1.drc.gain_word = 236;
    inputData.program_ac3_params[1].compr1.is_profile = 1;
    inputData.program_ac3_params[1].compr1.drc.profile = DLB_ADM_DOLBYE_DRC_PROFILE_MUSIC_LIGHT;
    inputData.program_encode_params[1].hpfon = 0;
    inputData.program_encode_params[1].bwlpfon = 1;
    inputData.program_encode_params[1].lfelpfon = 0;
    inputData.program_encode_params[1].sur90on = 1;
    inputData.program_encode_params[1].suratton = 0;
    inputData.program_encode_params[1].rfpremphon = 1;

    int status = dlb_adm_core_model_add_dolbye_data(coreModel, &inputData);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_core_model_get_dolbye_data(&outputData, coreModel);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    EXPECT_EQ(2, outputData.program_count);
    EXPECT_EQ(DLB_ADM_DOLBYE_PROGRAM_CONFIG_51_2, outputData.info.program_config_id);
    EXPECT_EQ(DLB_ADM_DOLBYE_FRAME_RATE_2398, outputData.info.frame_rate);
    EXPECT_EQ(11, outputData.info.time_code.tc1);
    EXPECT_EQ(45, outputData.info.time_code.tc2);
    EXPECT_EQ(24, outputData.info.time_code.tc3);
    EXPECT_EQ(15, outputData.info.time_code.tc4);

    EXPECT_EQ(DLB_ADM_DOLBYE_ACMOD_3_2, outputData.program_ac3_params[0].acmod);
    EXPECT_EQ(DLB_ADM_DOLBYE_BSMOD_COMPLETE_MAIN, outputData.program_ac3_params[0].bsmod);
    EXPECT_EQ(1, outputData.program_ac3_params[0].lfeon);
    EXPECT_EQ(DLB_ADM_DOLBYE_CMIXLEV_M45_DB, outputData.program_ac3_params[0].cmixlev);
    EXPECT_EQ(DLB_ADM_DOLBYE_SURMIXLEV_0, outputData.program_ac3_params[0].surmixlev);
    EXPECT_EQ(DLB_ADM_DOLBYE_DSURMOD_NOT_INDICATED, outputData.program_ac3_params[0].dsurmod);
    EXPECT_EQ(23, outputData.program_ac3_params[0].dialnorm);
    EXPECT_EQ(1, outputData.program_ac3_params[0].copyrightb);
    EXPECT_EQ(0, outputData.program_ac3_params[0].origbs);
    EXPECT_EQ(0, outputData.program_ac3_params[0].langcod_exists);
    EXPECT_EQ(1, outputData.program_ac3_params[0].audprodie);
    EXPECT_EQ(21, outputData.program_ac3_params[0].mixlevel);
    EXPECT_EQ(DLB_ADM_DOLBYE_ROOM_TYPE_SMALL, outputData.program_ac3_params[0].roomtyp);
    EXPECT_EQ(1, outputData.program_ac3_params[0].xbsi1_exists);
    EXPECT_EQ(DLB_ADM_DOLBYE_LX_RX_MIXLEV_30_DB, outputData.program_ac3_params[0].xbsi1_lorocmixlev);
    EXPECT_EQ(DLB_ADM_DOLBYE_LX_RX_MIXLEV_0_DB, outputData.program_ac3_params[0].xbsi1_lorosurmixlev);
    EXPECT_EQ(DLB_ADM_DOLBYE_LX_RX_MIXLEV_M30_DB, outputData.program_ac3_params[0].xbsi1_ltrtcmixlev);
    EXPECT_EQ(DLB_ADM_DOLBYE_LX_RX_MIXLEV_M60_DB, outputData.program_ac3_params[0].xbsi1_ltrtsurmixlev);
    EXPECT_EQ(DLB_ADM_DOLBYE_DMIXMODE_LO_RO, outputData.program_ac3_params[0].xbsi1_dmixmod);
    EXPECT_EQ(1, outputData.program_ac3_params[0].xbsi2_exists);
    EXPECT_EQ(DLB_ADM_DOLBYE_DSUREXMOD_DOLBY_SURROUND_EX, outputData.program_ac3_params[0].xbsi2_dsurexmod);
    EXPECT_EQ(DLB_ADM_DOLBYE_DHEADPHONEMOD_DOLBY_HEADHONE, outputData.program_ac3_params[0].xbsi2_dheadphonmod);
    EXPECT_EQ(0, outputData.program_ac3_params[0].xbsi2_adconvtyp);
    EXPECT_EQ(1, outputData.program_ac3_params[0].dynrng1.is_profile);
    EXPECT_EQ(DLB_ADM_DOLBYE_DRC_PROFILE_MUSIC_STANDARD, outputData.program_ac3_params[0].dynrng1.drc.profile);
    EXPECT_EQ(1, outputData.program_ac3_params[0].compr1.is_profile);
    EXPECT_EQ(DLB_ADM_DOLBYE_DRC_PROFILE_SPEECH, outputData.program_ac3_params[0].compr1.drc.profile); 
    EXPECT_STREQ("The best program", outputData.program_ac3_params[0].programDescriptionText);
    EXPECT_EQ(1, outputData.program_encode_params[0].hpfon);
    EXPECT_EQ(0, outputData.program_encode_params[0].bwlpfon);
    EXPECT_EQ(1, outputData.program_encode_params[0].lfelpfon);
    EXPECT_EQ(0, outputData.program_encode_params[0].sur90on);
    EXPECT_EQ(1, outputData.program_encode_params[0].suratton);
    EXPECT_EQ(0, outputData.program_encode_params[0].rfpremphon);               


    EXPECT_EQ(DLB_ADM_DOLBYE_ACMOD_1_0, outputData.program_ac3_params[1].acmod);
    EXPECT_EQ(DLB_ADM_DOLBYE_BSMOD_VISUALLY_IMPAIRED, outputData.program_ac3_params[1].bsmod);
    EXPECT_EQ(0, outputData.program_ac3_params[1].lfeon);
    EXPECT_EQ(DLB_ADM_DOLBYE_CMIXLEV_M60_DB, outputData.program_ac3_params[1].cmixlev);
    EXPECT_EQ(DLB_ADM_DOLBYE_SURMIXLEV_M60_DB, outputData.program_ac3_params[1].surmixlev);
    EXPECT_EQ(DLB_ADM_DOLBYE_DSURMOD_DOLBY_SURROUND, outputData.program_ac3_params[1].dsurmod);
    EXPECT_EQ(27, outputData.program_ac3_params[1].dialnorm);
    EXPECT_EQ(0, outputData.program_ac3_params[1].copyrightb);
    EXPECT_EQ(0, outputData.program_ac3_params[1].origbs);
    EXPECT_EQ(1, outputData.program_ac3_params[1].langcod_exists);
    EXPECT_EQ(15, outputData.program_ac3_params[1].langcod);    
    EXPECT_EQ(0, outputData.program_ac3_params[1].audprodie);
    EXPECT_EQ(1, outputData.program_ac3_params[1].xbsi1_exists);
    EXPECT_EQ(DLB_ADM_DOLBYE_LX_RX_MIXLEV_0_DB, outputData.program_ac3_params[1].xbsi1_lorocmixlev);
    EXPECT_EQ(DLB_ADM_DOLBYE_LX_RX_MIXLEV_MINF_DB, outputData.program_ac3_params[1].xbsi1_lorosurmixlev);
    EXPECT_EQ(DLB_ADM_DOLBYE_LX_RX_MIXLEV_30_DB, outputData.program_ac3_params[1].xbsi1_ltrtcmixlev);
    EXPECT_EQ(DLB_ADM_DOLBYE_LX_RX_MIXLEV_M60_DB, outputData.program_ac3_params[1].xbsi1_ltrtsurmixlev);
    EXPECT_EQ(DLB_ADM_DOLBYE_DMIXMODE_NOT_INDICATED, outputData.program_ac3_params[1].xbsi1_dmixmod);
    EXPECT_EQ(0, outputData.program_ac3_params[1].xbsi2_exists);
    EXPECT_EQ(0, outputData.program_ac3_params[1].dynrng1.is_profile);
    EXPECT_EQ(236, outputData.program_ac3_params[1].dynrng1.drc.gain_word);
    EXPECT_EQ(1, outputData.program_ac3_params[1].compr1.is_profile);
    EXPECT_EQ(DLB_ADM_DOLBYE_DRC_PROFILE_MUSIC_LIGHT, outputData.program_ac3_params[1].compr1.drc.profile);
    EXPECT_STREQ("Not even a program", outputData.program_ac3_params[1].programDescriptionText);
    EXPECT_EQ(0, outputData.program_encode_params[1].hpfon);
    EXPECT_EQ(1, outputData.program_encode_params[1].bwlpfon);
    EXPECT_EQ(0, outputData.program_encode_params[1].lfelpfon);
    EXPECT_EQ(1, outputData.program_encode_params[1].sur90on);
    EXPECT_EQ(0, outputData.program_encode_params[1].suratton);
    EXPECT_EQ(1, outputData.program_encode_params[1].rfpremphon);
}

TEST_F(DlbAPItoAPICommon, ProfilesCheckPositive)
{
    dlb_adm_data_profile_list list;
    list.profiles_count =  3;
    strncpy(list.profiles[0].name, "Advanced sound system: ADM and S-ADM profile for emission", DLB_ADM_DATA_NAME_SZ);
    strncpy(list.profiles[0].value, "ITU-R BS.2168", DLB_ADM_DATA_NAME_SZ);
    strncpy(list.profiles[0].version, "1", DLB_ADM_DATA_PROFILE_VERSION_SIZE);
    strncpy(list.profiles[0].level, "1", DLB_ADM_DATA_PROFILE_VERSION_SIZE);
    list.profiles[0].type = DLB_ADM_PROFILE_SADM_EMISSION_PROFILE;

    strncpy(list.profiles[1].name, "Dolby E ADM and S-ADM Profile for emission", DLB_ADM_DATA_NAME_SZ);
    strncpy(list.profiles[1].value, "Dolby E ADM and S-ADM Profile for emission", DLB_ADM_DATA_NAME_SZ);
    strncpy(list.profiles[1].version, "1", DLB_ADM_DATA_PROFILE_VERSION_SIZE);
    strncpy(list.profiles[1].level, "1", DLB_ADM_DATA_PROFILE_VERSION_SIZE);
    list.profiles[1].type = DLB_ADM_PROFILE_SADM_DOLBY_E;

    strncpy(list.profiles[2].name, "Unknown profile", DLB_ADM_DATA_NAME_SZ);
    strncpy(list.profiles[2].value, "Unknown Publisher", DLB_ADM_DATA_NAME_SZ);
    strncpy(list.profiles[2].version, "1u", DLB_ADM_DATA_PROFILE_VERSION_SIZE);
    strncpy(list.profiles[2].level, "2u", DLB_ADM_DATA_PROFILE_VERSION_SIZE);
    list.profiles[2].type = DLB_ADM_PROFILE_NOT_INITIALIZED;

    int status = dlb_adm_core_model_add_profile_list(coreModel, &list);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    dlb_adm_bool isEmissionProfile, isDolbyEProfile;
    status = ::dlb_adm_core_model_has_profile(coreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE, &isEmissionProfile);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_TRUE, isEmissionProfile);

    status = ::dlb_adm_core_model_has_profile(coreModel, DLB_ADM_PROFILE_SADM_DOLBY_E, &isDolbyEProfile);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_TRUE, isDolbyEProfile);

    dlb_adm_data_profile_list profiles;
    status = ::dlb_adm_core_model_get_profile_list(&profiles, coreModel);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(3, profiles.profiles_count);

    EXPECT_STREQ("Advanced sound system: ADM and S-ADM profile for emission", profiles.profiles[0].name);
    EXPECT_STREQ("ITU-R BS.2168", profiles.profiles[0].value);
    EXPECT_STREQ("1", profiles.profiles[0].version);
    EXPECT_STREQ("1", profiles.profiles[0].level);
    EXPECT_EQ(DLB_ADM_PROFILE_SADM_EMISSION_PROFILE, profiles.profiles[0].type);

    EXPECT_STREQ("Dolby E ADM and S-ADM Profile for emission", profiles.profiles[1].name);
    EXPECT_STREQ("Dolby E ADM and S-ADM Profile for emission", profiles.profiles[1].value);
    EXPECT_STREQ("1", profiles.profiles[1].version);
    EXPECT_STREQ("1", profiles.profiles[1].level);
    EXPECT_EQ(DLB_ADM_PROFILE_SADM_DOLBY_E, profiles.profiles[1].type);

    EXPECT_STREQ("Unknown profile", profiles.profiles[2].name);
    EXPECT_STREQ("Unknown Publisher", profiles.profiles[2].value);
    EXPECT_STREQ("1u", profiles.profiles[2].version);
    EXPECT_STREQ("2u", profiles.profiles[2].level);
    EXPECT_EQ(DLB_ADM_PROFILE_NOT_INITIALIZED, profiles.profiles[2].type);

}


TEST_F(DlbAPItoAPICommon, addProfileCheck)
{
    int status = dlb_adm_core_model_add_profile(coreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    dlb_adm_bool isEmissionProfile, isDolbyEProfile;
    status = ::dlb_adm_core_model_has_profile(coreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE, &isEmissionProfile);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_TRUE, isEmissionProfile);

    status = ::dlb_adm_core_model_has_profile(coreModel, DLB_ADM_PROFILE_SADM_DOLBY_E, &isDolbyEProfile);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_FALSE, isDolbyEProfile);

    dlb_adm_data_profile_list profiles;
    status = ::dlb_adm_core_model_get_profile_list(&profiles, coreModel);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(1, profiles.profiles_count);

    EXPECT_STREQ("Advanced sound system: ADM and S-ADM profile for emission", profiles.profiles[0].name);
    EXPECT_STREQ("ITU-R BS.2168", profiles.profiles[0].value);
    EXPECT_STREQ("1", profiles.profiles[0].version);
    EXPECT_STREQ("1", profiles.profiles[0].level);
    EXPECT_EQ(DLB_ADM_PROFILE_SADM_EMISSION_PROFILE, profiles.profiles[0].type);
}

TEST_F(DlbAPItoAPICommon, addProfileUnrecognizeCheck)
{
    int status = dlb_adm_core_model_add_profile(coreModel, DLB_ADM_PROFILE_NOT_INITIALIZED);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);
}

TEST_F(DlbAPItoAPICommon, addTwoProfiles)
{
    int status = dlb_adm_core_model_add_profile(coreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = dlb_adm_core_model_add_profile(coreModel, DLB_ADM_PROFILE_SADM_DOLBY_E);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    dlb_adm_data_profile_list profiles;
    status = ::dlb_adm_core_model_get_profile_list(&profiles, coreModel);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(2, profiles.profiles_count);

    EXPECT_STREQ("Advanced sound system: ADM and S-ADM profile for emission", profiles.profiles[0].name);
    EXPECT_STREQ("ITU-R BS.2168", profiles.profiles[0].value);
    EXPECT_STREQ("1", profiles.profiles[0].version);
    EXPECT_STREQ("1", profiles.profiles[0].level);
    EXPECT_EQ(DLB_ADM_PROFILE_SADM_EMISSION_PROFILE, profiles.profiles[0].type);

    EXPECT_STREQ("Dolby E ADM and S-ADM Profile for emission", profiles.profiles[1].name);
    EXPECT_STREQ("Dolby E ADM and S-ADM Profile for emission", profiles.profiles[1].value);
    EXPECT_STREQ("1", profiles.profiles[1].version);
    EXPECT_STREQ("1", profiles.profiles[1].level);
    EXPECT_EQ(DLB_ADM_PROFILE_SADM_DOLBY_E, profiles.profiles[1].type);
}

TEST_F(DlbAPItoAPICommon, AddOldEmissionProfile)
{
    dlb_adm_data_profile_list list;
    list.profiles_count = 1;
    strncpy(list.profiles[0].name, "AdvSS Emission S-ADM Profile", DLB_ADM_DATA_NAME_SZ);
    strncpy(list.profiles[0].value, "ITU-R BS.[ADM-NGA-EMISSION]-X", DLB_ADM_DATA_NAME_SZ);
    strncpy(list.profiles[0].version, "1.0.0", DLB_ADM_DATA_PROFILE_VERSION_SIZE);
    strncpy(list.profiles[0].level, "1", DLB_ADM_DATA_PROFILE_VERSION_SIZE);
    list.profiles[0].type = DLB_ADM_PROFILE_NOT_INITIALIZED;

    int status = dlb_adm_core_model_add_profile_list(coreModel, &list);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    dlb_adm_bool isEmissionProfile;
    status = ::dlb_adm_core_model_has_profile(coreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE, &isEmissionProfile);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_TRUE, isEmissionProfile);

    dlb_adm_data_profile_list profiles;
    status = ::dlb_adm_core_model_get_profile_list(&profiles, coreModel);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(1, profiles.profiles_count);

    EXPECT_STREQ("AdvSS Emission S-ADM Profile", profiles.profiles[0].name);
    EXPECT_STREQ("ITU-R BS.[ADM-NGA-EMISSION]-X", profiles.profiles[0].value);
    EXPECT_STREQ("1.0.0", profiles.profiles[0].version);
    EXPECT_STREQ("1", profiles.profiles[0].level);
    EXPECT_EQ(DLB_ADM_PROFILE_SADM_EMISSION_PROFILE, profiles.profiles[0].type);
}

TEST_F(DlbAPItoAPICommon, AddOldDolbyEProfile)
{
    dlb_adm_data_profile_list list;
    list.profiles_count = 1;
    strncpy(list.profiles[0].name, "Dolby E ADM and S-ADM Profile for emission", DLB_ADM_DATA_NAME_SZ);
    strncpy(list.profiles[0].value, "Dolby E ADM and S-ADM Profile for emission v1.0", DLB_ADM_DATA_NAME_SZ);
    strncpy(list.profiles[0].version, "1", DLB_ADM_DATA_PROFILE_VERSION_SIZE);
    strncpy(list.profiles[0].level, "1", DLB_ADM_DATA_PROFILE_VERSION_SIZE);
    list.profiles[0].type = DLB_ADM_PROFILE_NOT_INITIALIZED;

    int status = dlb_adm_core_model_add_profile_list(coreModel, &list);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    dlb_adm_bool isDolbyEProfile;
    status = ::dlb_adm_core_model_has_profile(coreModel, DLB_ADM_PROFILE_SADM_DOLBY_E, &isDolbyEProfile);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_TRUE, isDolbyEProfile);

    dlb_adm_data_profile_list profiles;
    status = ::dlb_adm_core_model_get_profile_list(&profiles, coreModel);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(1, profiles.profiles_count);

    EXPECT_STREQ("Dolby E ADM and S-ADM Profile for emission", profiles.profiles[0].name);
    EXPECT_STREQ("Dolby E ADM and S-ADM Profile for emission v1.0", profiles.profiles[0].value);
    EXPECT_STREQ("1", profiles.profiles[0].version);
    EXPECT_STREQ("1", profiles.profiles[0].level);
    EXPECT_EQ(DLB_ADM_PROFILE_SADM_DOLBY_E, profiles.profiles[0].type);
}
