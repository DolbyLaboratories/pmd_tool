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
#include "DolbyEProfileXMLBuffers.h"
#include "AnalyzeContentXMLBuffers.h"

class DlbDolbyEXMLTests : public DlbXMLToAPICommon
{
};

TEST_F(DlbDolbyEXMLTests, config_51_20)
{
    int status;

    status = ::dlb_adm_container_read_xml_buffer
        (originalContainer
        ,dolbyE_51_20.c_str()
        ,dolbyE_51_20.length()
        ,true
        );
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
}

TEST_F(DlbDolbyEXMLTests, profileCheck)
{
    int status;

    status = ::dlb_adm_container_read_xml_buffer
        (originalContainer
        ,dolbyE_51_20.c_str()
        ,dolbyE_51_20.length()
        ,true
        );
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = dlb_adm_core_model_ingest_xml_container(coreModel, originalContainer);
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

    EXPECT_STREQ("Super Good Profile", profiles.profiles[2].name);
    EXPECT_STREQ("Unknown Publisher: Document 4682", profiles.profiles[2].value);
    EXPECT_STREQ("1234.56", profiles.profiles[2].version);
    EXPECT_STREQ("over 9000", profiles.profiles[2].level);
    EXPECT_EQ(DLB_ADM_PROFILE_NOT_INITIALIZED, profiles.profiles[2].type);
}

TEST_F(DlbDolbyEXMLTests, profileCheckClearAll)
{
    int status;

    status = ::dlb_adm_container_read_xml_buffer
        (originalContainer
        ,dolbyE_51_20.c_str()
        ,dolbyE_51_20.length()
        ,true
        );
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = dlb_adm_core_model_ingest_xml_container(coreModel, originalContainer);
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

    EXPECT_STREQ("Super Good Profile", profiles.profiles[2].name);
    EXPECT_STREQ("Unknown Publisher: Document 4682", profiles.profiles[2].value);
    EXPECT_STREQ("1234.56", profiles.profiles[2].version);
    EXPECT_STREQ("over 9000", profiles.profiles[2].level);
    EXPECT_EQ(DLB_ADM_PROFILE_NOT_INITIALIZED, profiles.profiles[2].type);

    status = dlb_adm_container_clear_all(originalContainer);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_clear(coreModel);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_container_read_xml_buffer
        (originalContainer
        ,xml_20.c_str()
        ,xml_20.length()
        ,true
        );
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = dlb_adm_core_model_ingest_xml_container(coreModel, originalContainer);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_core_model_has_profile(coreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE, &isEmissionProfile);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_TRUE, isEmissionProfile);

    //memset(&profiles, 0, sizeof(dlb_adm_data_profile_list));
    status = ::dlb_adm_core_model_get_profile_list(&profiles, coreModel);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(1, profiles.profiles_count);
}

TEST_F(DlbDolbyEXMLTests, getDolbyEData_51_20)
{
    int status;

    status = ::dlb_adm_container_read_xml_buffer
        (originalContainer
        ,dolbyE_51_20.c_str()
        ,dolbyE_51_20.length()
        ,true
        );
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = dlb_adm_core_model_ingest_xml_container(coreModel, originalContainer);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    dlb_adm_data_dolbye_data dolbye_data;
    status = ::dlb_adm_core_model_get_dolbye_data(&dolbye_data, coreModel);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    EXPECT_EQ(2, dolbye_data.program_count);
    EXPECT_EQ(DLB_ADM_DOLBYE_PROGRAM_CONFIG_51_2, dolbye_data.info.program_config_id);
    EXPECT_EQ(DLB_ADM_DOLBYE_FRAME_RATE_25, dolbye_data.info.frame_rate);

    EXPECT_EQ(DLB_ADM_DOLBYE_ACMOD_3_2, dolbye_data.program_ac3_params[0].acmod);
    EXPECT_EQ(DLB_ADM_DOLBYE_BSMOD_COMPLETE_MAIN, dolbye_data.program_ac3_params[0].bsmod);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[0].lfeon);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[0].cmixlev);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[0].surmixlev);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[0].dsurmod);
    EXPECT_EQ(20, dolbye_data.program_ac3_params[0].dialnorm);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[0].copyrightb);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[0].origbs);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[0].langcod_exists);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[0].audprodie);
    EXPECT_EQ(21, dolbye_data.program_ac3_params[0].mixlevel);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[0].roomtyp);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[0].xbsi1_exists);
    EXPECT_EQ(5, dolbye_data.program_ac3_params[0].xbsi1_lorocmixlev);
    EXPECT_EQ(4, dolbye_data.program_ac3_params[0].xbsi1_lorosurmixlev);
    EXPECT_EQ(4, dolbye_data.program_ac3_params[0].xbsi1_ltrtcmixlev);
    EXPECT_EQ(4, dolbye_data.program_ac3_params[0].xbsi1_ltrtsurmixlev);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[0].xbsi1_dmixmod);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[0].xbsi2_exists);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[0].xbsi2_dsurexmod);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[0].xbsi2_dheadphonmod);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[0].xbsi2_adconvtyp);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[0].dynrng1.is_profile);
    EXPECT_EQ(2, dolbye_data.program_ac3_params[0].dynrng1.drc.profile);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[0].compr1.is_profile);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[0].compr1.drc.profile); 
    EXPECT_STREQ("Program 1", dolbye_data.program_ac3_params[0].programDescriptionText);
    EXPECT_EQ(1, dolbye_data.program_encode_params[0].hpfon);
    EXPECT_EQ(0, dolbye_data.program_encode_params[0].bwlpfon);
    EXPECT_EQ(1, dolbye_data.program_encode_params[0].lfelpfon);
    EXPECT_EQ(0, dolbye_data.program_encode_params[0].sur90on);
    EXPECT_EQ(1, dolbye_data.program_encode_params[0].suratton);
    EXPECT_EQ(0, dolbye_data.program_encode_params[0].rfpremphon);               


    EXPECT_EQ(DLB_ADM_DOLBYE_ACMOD_2_0, dolbye_data.program_ac3_params[1].acmod);
    EXPECT_EQ(DLB_ADM_DOLBYE_BSMOD_VISUALLY_IMPAIRED, dolbye_data.program_ac3_params[1].bsmod);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[1].lfeon);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[1].cmixlev);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[1].surmixlev);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[1].dsurmod);
    EXPECT_EQ(29, dolbye_data.program_ac3_params[1].dialnorm);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[1].copyrightb);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[1].origbs);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[1].langcod_exists);
    EXPECT_EQ(15, dolbye_data.program_ac3_params[1].langcod);    
    EXPECT_EQ(0, dolbye_data.program_ac3_params[1].audprodie);
    EXPECT_EQ(25, dolbye_data.program_ac3_params[1].mixlevel);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[1].roomtyp);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[1].xbsi1_exists);
    EXPECT_EQ(4, dolbye_data.program_ac3_params[1].xbsi1_lorocmixlev);
    EXPECT_EQ(5, dolbye_data.program_ac3_params[1].xbsi1_lorosurmixlev);
    EXPECT_EQ(3, dolbye_data.program_ac3_params[1].xbsi1_ltrtcmixlev);
    EXPECT_EQ(5, dolbye_data.program_ac3_params[1].xbsi1_ltrtsurmixlev);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[1].xbsi1_dmixmod);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[1].xbsi2_exists);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[1].xbsi2_dsurexmod);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[1].xbsi2_dheadphonmod);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[1].xbsi2_adconvtyp);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[1].dynrng1.is_profile);
    EXPECT_EQ(44, dolbye_data.program_ac3_params[1].dynrng1.drc.gain_word);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[1].compr1.is_profile);
    EXPECT_EQ(126, dolbye_data.program_ac3_params[1].compr1.drc.gain_word);
    EXPECT_STREQ("", dolbye_data.program_ac3_params[1].programDescriptionText);
    EXPECT_EQ(0, dolbye_data.program_encode_params[1].hpfon);
    EXPECT_EQ(1, dolbye_data.program_encode_params[1].bwlpfon);
    EXPECT_EQ(0, dolbye_data.program_encode_params[1].lfelpfon);
    EXPECT_EQ(1, dolbye_data.program_encode_params[1].sur90on);
    EXPECT_EQ(0, dolbye_data.program_encode_params[1].suratton);
    EXPECT_EQ(0, dolbye_data.program_encode_params[1].rfpremphon);             
}

TEST_F(DlbDolbyEXMLTests, getDolbyEData_51)
{
    int status;

    status = ::dlb_adm_container_read_xml_buffer
        (originalContainer
        ,dolbyE_51_selected.c_str()
        ,dolbyE_51_selected.length()
        ,true
        );
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = dlb_adm_core_model_ingest_xml_container(coreModel, originalContainer);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    dlb_adm_data_dolbye_data dolbye_data;
    status = ::dlb_adm_core_model_get_dolbye_data(&dolbye_data, coreModel);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    EXPECT_EQ(1, dolbye_data.program_count);
    EXPECT_EQ(DLB_ADM_DOLBYE_PROGRAM_CONFIG_51, dolbye_data.info.program_config_id);
    EXPECT_EQ(DLB_ADM_DOLBYE_FRAME_RATE_2997, dolbye_data.info.frame_rate);
    EXPECT_EQ(21, dolbye_data.info.time_code.tc1);
    EXPECT_EQ(20, dolbye_data.info.time_code.tc2);
    EXPECT_EQ(19, dolbye_data.info.time_code.tc3);
    EXPECT_EQ(18, dolbye_data.info.time_code.tc4);    

    EXPECT_EQ(DLB_ADM_DOLBYE_ACMOD_2_0, dolbye_data.program_ac3_params[0].acmod);
    EXPECT_EQ(DLB_ADM_DOLBYE_BSMOD_VISUALLY_IMPAIRED, dolbye_data.program_ac3_params[0].bsmod);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[0].lfeon);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[0].cmixlev);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[0].surmixlev);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[0].dsurmod);
    EXPECT_EQ(23, dolbye_data.program_ac3_params[0].dialnorm);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[0].copyrightb);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[0].origbs);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[0].langcod_exists);
    EXPECT_EQ(128, dolbye_data.program_ac3_params[0].langcod);    
    EXPECT_EQ(1, dolbye_data.program_ac3_params[0].audprodie);
    EXPECT_EQ(21, dolbye_data.program_ac3_params[0].mixlevel);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[0].roomtyp);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[0].xbsi1_exists);
    EXPECT_EQ(5, dolbye_data.program_ac3_params[0].xbsi1_lorocmixlev);
    EXPECT_EQ(4, dolbye_data.program_ac3_params[0].xbsi1_lorosurmixlev);
    EXPECT_EQ(4, dolbye_data.program_ac3_params[0].xbsi1_ltrtcmixlev);
    EXPECT_EQ(4, dolbye_data.program_ac3_params[0].xbsi1_ltrtsurmixlev);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[0].xbsi1_dmixmod);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[0].xbsi2_exists);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[0].xbsi2_dsurexmod);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[0].xbsi2_dheadphonmod);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[0].xbsi2_adconvtyp);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[0].dynrng1.is_profile);
    EXPECT_EQ(2, dolbye_data.program_ac3_params[0].dynrng1.drc.profile);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[0].compr1.is_profile);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[0].compr1.drc.profile); 
    EXPECT_STREQ("Program 1", dolbye_data.program_ac3_params[0].programDescriptionText);
    EXPECT_EQ(1, dolbye_data.program_encode_params[0].hpfon);
    EXPECT_EQ(1, dolbye_data.program_encode_params[0].bwlpfon);
    EXPECT_EQ(0, dolbye_data.program_encode_params[0].lfelpfon);
    EXPECT_EQ(0, dolbye_data.program_encode_params[0].sur90on);
    EXPECT_EQ(1, dolbye_data.program_encode_params[0].suratton);
    EXPECT_EQ(1, dolbye_data.program_encode_params[0].rfpremphon);
}

TEST_F(DlbDolbyEXMLTests, getDolbyEData_8x10)
{
    int status;

    status = ::dlb_adm_container_read_xml_buffer
        (originalContainer
        ,dolbyE_8x_10.c_str()
        ,dolbyE_8x_10.length()
        ,true
        );
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = dlb_adm_core_model_ingest_xml_container(coreModel, originalContainer);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    dlb_adm_data_dolbye_data dolbye_data;
    status = ::dlb_adm_core_model_get_dolbye_data(&dolbye_data, coreModel);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    EXPECT_EQ(8, dolbye_data.program_count);
    EXPECT_EQ(DLB_ADM_DOLBYE_PROGRAM_CONFIG_1_1_1_1_1_1_1_1, dolbye_data.info.program_config_id);
    EXPECT_EQ(DLB_ADM_DOLBYE_FRAME_RATE_30, dolbye_data.info.frame_rate);

    EXPECT_EQ(DLB_ADM_DOLBYE_ACMOD_1_0, dolbye_data.program_ac3_params[0].acmod);
    EXPECT_EQ(DLB_ADM_DOLBYE_BSMOD_COMPLETE_MAIN, dolbye_data.program_ac3_params[0].bsmod);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[0].lfeon);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[0].cmixlev);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[0].surmixlev);
    EXPECT_EQ(2, dolbye_data.program_ac3_params[0].dsurmod);
    EXPECT_EQ(22, dolbye_data.program_ac3_params[0].dialnorm);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[0].copyrightb);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[0].origbs);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[0].langcod_exists);   
    EXPECT_EQ(0, dolbye_data.program_ac3_params[0].audprodie);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[0].xbsi1_exists);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[0].xbsi1_lorocmixlev);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[0].xbsi1_lorosurmixlev);
    EXPECT_EQ(2, dolbye_data.program_ac3_params[0].xbsi1_ltrtcmixlev);
    EXPECT_EQ(3, dolbye_data.program_ac3_params[0].xbsi1_ltrtsurmixlev);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[0].xbsi1_dmixmod);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[0].xbsi2_exists);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[0].xbsi2_dsurexmod);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[0].xbsi2_dheadphonmod);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[0].xbsi2_adconvtyp);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[0].compr1.is_profile);
    EXPECT_EQ(128, dolbye_data.program_ac3_params[0].compr1.drc.profile); 
    EXPECT_EQ(1, dolbye_data.program_ac3_params[0].dynrng1.is_profile);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[0].dynrng1.drc.profile);
    EXPECT_STREQ("Program 1 (Ice Hockey)", dolbye_data.program_ac3_params[0].programDescriptionText);
    EXPECT_EQ(0, dolbye_data.program_encode_params[0].hpfon);
    EXPECT_EQ(0, dolbye_data.program_encode_params[0].bwlpfon);
    EXPECT_EQ(0, dolbye_data.program_encode_params[0].lfelpfon);
    EXPECT_EQ(0, dolbye_data.program_encode_params[0].sur90on);
    EXPECT_EQ(0, dolbye_data.program_encode_params[0].suratton);
    EXPECT_EQ(0, dolbye_data.program_encode_params[0].rfpremphon);

    EXPECT_EQ(DLB_ADM_DOLBYE_ACMOD_1_0, dolbye_data.program_ac3_params[1].acmod);
    EXPECT_EQ(DLB_ADM_DOLBYE_BSMOD_MUSIC_AND_EFFECT, dolbye_data.program_ac3_params[1].bsmod);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[1].lfeon);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[1].cmixlev);
    EXPECT_EQ(2, dolbye_data.program_ac3_params[1].surmixlev);
    EXPECT_EQ(3, dolbye_data.program_ac3_params[1].dsurmod);
    EXPECT_EQ(23, dolbye_data.program_ac3_params[1].dialnorm);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[1].copyrightb);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[1].origbs);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[1].langcod_exists);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[1].langcod);    
    EXPECT_EQ(0, dolbye_data.program_ac3_params[1].audprodie);
    EXPECT_EQ(21, dolbye_data.program_ac3_params[1].mixlevel);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[1].roomtyp);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[1].xbsi1_exists);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[1].xbsi1_lorocmixlev);
    EXPECT_EQ(2, dolbye_data.program_ac3_params[1].xbsi1_lorosurmixlev);
    EXPECT_EQ(3, dolbye_data.program_ac3_params[1].xbsi1_ltrtcmixlev);
    EXPECT_EQ(4, dolbye_data.program_ac3_params[1].xbsi1_ltrtsurmixlev);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[1].xbsi1_dmixmod);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[1].xbsi2_exists);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[1].xbsi2_dsurexmod);
    EXPECT_EQ(2, dolbye_data.program_ac3_params[1].xbsi2_dheadphonmod);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[1].xbsi2_adconvtyp);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[1].compr1.is_profile);
    EXPECT_EQ(255, dolbye_data.program_ac3_params[1].compr1.drc.profile); 
    EXPECT_EQ(1, dolbye_data.program_ac3_params[1].dynrng1.is_profile);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[1].dynrng1.drc.profile);
    EXPECT_STREQ("Program 2 (Hockey sobre hielo)", dolbye_data.program_ac3_params[1].programDescriptionText);
    EXPECT_EQ(1, dolbye_data.program_encode_params[1].hpfon);
    EXPECT_EQ(0, dolbye_data.program_encode_params[1].bwlpfon);
    EXPECT_EQ(0, dolbye_data.program_encode_params[1].lfelpfon);
    EXPECT_EQ(0, dolbye_data.program_encode_params[1].sur90on);
    EXPECT_EQ(0, dolbye_data.program_encode_params[1].suratton);
    EXPECT_EQ(0, dolbye_data.program_encode_params[1].rfpremphon);

    EXPECT_EQ(DLB_ADM_DOLBYE_ACMOD_1_0, dolbye_data.program_ac3_params[2].acmod);
    EXPECT_EQ(DLB_ADM_DOLBYE_BSMOD_VISUALLY_IMPAIRED, dolbye_data.program_ac3_params[2].bsmod);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[2].lfeon);
    EXPECT_EQ(2, dolbye_data.program_ac3_params[2].cmixlev);
    EXPECT_EQ(3, dolbye_data.program_ac3_params[2].surmixlev);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[2].dsurmod);
    EXPECT_EQ(26, dolbye_data.program_ac3_params[2].dialnorm);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[2].copyrightb);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[2].origbs);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[2].langcod_exists);
    EXPECT_EQ(20, dolbye_data.program_ac3_params[2].langcod);    
    EXPECT_EQ(1, dolbye_data.program_ac3_params[2].audprodie);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[2].mixlevel);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[2].roomtyp);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[2].xbsi1_exists);
    EXPECT_EQ(3, dolbye_data.program_ac3_params[2].xbsi1_lorocmixlev);
    EXPECT_EQ(4, dolbye_data.program_ac3_params[2].xbsi1_lorosurmixlev);
    EXPECT_EQ(5, dolbye_data.program_ac3_params[2].xbsi1_ltrtcmixlev);
    EXPECT_EQ(6, dolbye_data.program_ac3_params[2].xbsi1_ltrtsurmixlev);
    EXPECT_EQ(2, dolbye_data.program_ac3_params[2].xbsi1_dmixmod);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[2].xbsi2_exists);
    EXPECT_EQ(2, dolbye_data.program_ac3_params[2].xbsi2_dsurexmod);
    EXPECT_EQ(3, dolbye_data.program_ac3_params[2].xbsi2_dheadphonmod);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[2].xbsi2_adconvtyp);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[2].compr1.is_profile);
    EXPECT_EQ(5, dolbye_data.program_ac3_params[2].compr1.drc.profile); 
    EXPECT_EQ(1, dolbye_data.program_ac3_params[2].dynrng1.is_profile);
    EXPECT_EQ(2, dolbye_data.program_ac3_params[2].dynrng1.drc.profile);
    EXPECT_STREQ("Program 3 (Hockey sur glace)", dolbye_data.program_ac3_params[2].programDescriptionText);
    EXPECT_EQ(1, dolbye_data.program_encode_params[2].hpfon);
    EXPECT_EQ(1, dolbye_data.program_encode_params[2].bwlpfon);
    EXPECT_EQ(0, dolbye_data.program_encode_params[2].lfelpfon);
    EXPECT_EQ(0, dolbye_data.program_encode_params[2].sur90on);
    EXPECT_EQ(0, dolbye_data.program_encode_params[2].suratton);
    EXPECT_EQ(0, dolbye_data.program_encode_params[2].rfpremphon);

    EXPECT_EQ(DLB_ADM_DOLBYE_ACMOD_1_0, dolbye_data.program_ac3_params[3].acmod);
    EXPECT_EQ(DLB_ADM_DOLBYE_BSMOD_HEARING_IMPAIRED, dolbye_data.program_ac3_params[3].bsmod);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[3].lfeon);
    EXPECT_EQ(3, dolbye_data.program_ac3_params[3].cmixlev);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[3].surmixlev);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[3].dsurmod);
    EXPECT_EQ(28, dolbye_data.program_ac3_params[3].dialnorm);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[3].copyrightb);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[3].origbs);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[3].langcod_exists);
    EXPECT_EQ(100, dolbye_data.program_ac3_params[3].langcod);    
    EXPECT_EQ(1, dolbye_data.program_ac3_params[3].audprodie);
    EXPECT_EQ(10, dolbye_data.program_ac3_params[3].mixlevel);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[3].roomtyp);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[3].xbsi1_exists);
    EXPECT_EQ(4, dolbye_data.program_ac3_params[3].xbsi1_lorocmixlev);
    EXPECT_EQ(5, dolbye_data.program_ac3_params[3].xbsi1_lorosurmixlev);
    EXPECT_EQ(6, dolbye_data.program_ac3_params[3].xbsi1_ltrtcmixlev);
    EXPECT_EQ(7, dolbye_data.program_ac3_params[3].xbsi1_ltrtsurmixlev);
    EXPECT_EQ(3, dolbye_data.program_ac3_params[3].xbsi1_dmixmod);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[3].xbsi2_exists);
    EXPECT_EQ(3, dolbye_data.program_ac3_params[3].xbsi2_dsurexmod);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[3].xbsi2_dheadphonmod);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[3].xbsi2_adconvtyp);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[3].compr1.is_profile);
    EXPECT_EQ(4, dolbye_data.program_ac3_params[3].compr1.drc.profile); 
    EXPECT_EQ(1, dolbye_data.program_ac3_params[3].dynrng1.is_profile);
    EXPECT_EQ(3, dolbye_data.program_ac3_params[3].dynrng1.drc.profile);
    EXPECT_STREQ("Program 4 (Calcio)", dolbye_data.program_ac3_params[3].programDescriptionText);
    EXPECT_EQ(1, dolbye_data.program_encode_params[3].hpfon);
    EXPECT_EQ(1, dolbye_data.program_encode_params[3].bwlpfon);
    EXPECT_EQ(1, dolbye_data.program_encode_params[3].lfelpfon);
    EXPECT_EQ(0, dolbye_data.program_encode_params[3].sur90on);
    EXPECT_EQ(0, dolbye_data.program_encode_params[3].suratton);
    EXPECT_EQ(0, dolbye_data.program_encode_params[3].rfpremphon);

    EXPECT_EQ(DLB_ADM_DOLBYE_ACMOD_1_0, dolbye_data.program_ac3_params[4].acmod);
    EXPECT_EQ(DLB_ADM_DOLBYE_BSMOD_DIALOGUE, dolbye_data.program_ac3_params[4].bsmod);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[4].lfeon);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[4].cmixlev);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[4].surmixlev);
    EXPECT_EQ(2, dolbye_data.program_ac3_params[4].dsurmod);
    EXPECT_EQ(31, dolbye_data.program_ac3_params[4].dialnorm);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[4].copyrightb);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[4].origbs);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[4].langcod_exists);
    EXPECT_EQ(150, dolbye_data.program_ac3_params[4].langcod);    
    EXPECT_EQ(1, dolbye_data.program_ac3_params[4].audprodie);
    EXPECT_EQ(23, dolbye_data.program_ac3_params[4].mixlevel);
    EXPECT_EQ(2, dolbye_data.program_ac3_params[4].roomtyp);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[4].xbsi1_exists);
    EXPECT_EQ(5, dolbye_data.program_ac3_params[4].xbsi1_lorocmixlev);
    EXPECT_EQ(6, dolbye_data.program_ac3_params[4].xbsi1_lorosurmixlev);
    EXPECT_EQ(7, dolbye_data.program_ac3_params[4].xbsi1_ltrtcmixlev);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[4].xbsi1_ltrtsurmixlev);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[4].xbsi1_dmixmod);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[4].xbsi2_exists);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[4].compr1.is_profile);
    EXPECT_EQ(3, dolbye_data.program_ac3_params[4].compr1.drc.profile); 
    EXPECT_EQ(1, dolbye_data.program_ac3_params[4].dynrng1.is_profile);
    EXPECT_EQ(4, dolbye_data.program_ac3_params[4].dynrng1.drc.profile);
    EXPECT_STREQ("Program 5 (Gimnasia rítmica)", dolbye_data.program_ac3_params[4].programDescriptionText);
    EXPECT_EQ(1, dolbye_data.program_encode_params[4].hpfon);
    EXPECT_EQ(1, dolbye_data.program_encode_params[4].bwlpfon);
    EXPECT_EQ(1, dolbye_data.program_encode_params[4].lfelpfon);
    EXPECT_EQ(1, dolbye_data.program_encode_params[4].sur90on);
    EXPECT_EQ(0, dolbye_data.program_encode_params[4].suratton);
    EXPECT_EQ(0, dolbye_data.program_encode_params[4].rfpremphon);

    EXPECT_EQ(DLB_ADM_DOLBYE_ACMOD_1_0, dolbye_data.program_ac3_params[5].acmod);
    EXPECT_EQ(DLB_ADM_DOLBYE_BSMOD_COMMENTARY, dolbye_data.program_ac3_params[5].bsmod);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[5].lfeon);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[5].cmixlev);
    EXPECT_EQ(2, dolbye_data.program_ac3_params[5].surmixlev);
    EXPECT_EQ(3, dolbye_data.program_ac3_params[5].dsurmod);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[5].dialnorm);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[5].copyrightb);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[5].origbs);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[5].langcod_exists);
    EXPECT_EQ(255, dolbye_data.program_ac3_params[5].langcod);    
    EXPECT_EQ(1, dolbye_data.program_ac3_params[5].audprodie);
    EXPECT_EQ(31, dolbye_data.program_ac3_params[5].mixlevel);
    EXPECT_EQ(3, dolbye_data.program_ac3_params[5].roomtyp);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[5].xbsi1_exists);
    EXPECT_EQ(6, dolbye_data.program_ac3_params[5].xbsi1_lorocmixlev);
    EXPECT_EQ(7, dolbye_data.program_ac3_params[5].xbsi1_lorosurmixlev);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[5].xbsi1_ltrtcmixlev);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[5].xbsi1_ltrtsurmixlev);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[5].xbsi1_dmixmod);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[5].xbsi2_exists);
    EXPECT_EQ(2, dolbye_data.program_ac3_params[5].xbsi2_dsurexmod);
    EXPECT_EQ(2, dolbye_data.program_ac3_params[5].xbsi2_dheadphonmod);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[5].xbsi2_adconvtyp);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[5].compr1.is_profile);
    EXPECT_EQ(2, dolbye_data.program_ac3_params[5].compr1.drc.profile); 
    EXPECT_EQ(1, dolbye_data.program_ac3_params[5].dynrng1.is_profile);
    EXPECT_EQ(5, dolbye_data.program_ac3_params[5].dynrng1.drc.profile);
    EXPECT_STREQ("Program 6 (Gymnastique rythmique)", dolbye_data.program_ac3_params[5].programDescriptionText);
    EXPECT_EQ(1, dolbye_data.program_encode_params[5].hpfon);
    EXPECT_EQ(1, dolbye_data.program_encode_params[5].bwlpfon);
    EXPECT_EQ(1, dolbye_data.program_encode_params[5].lfelpfon);
    EXPECT_EQ(1, dolbye_data.program_encode_params[5].sur90on);
    EXPECT_EQ(1, dolbye_data.program_encode_params[5].suratton);
    EXPECT_EQ(0, dolbye_data.program_encode_params[5].rfpremphon);

    EXPECT_EQ(DLB_ADM_DOLBYE_ACMOD_1_0, dolbye_data.program_ac3_params[6].acmod);
    EXPECT_EQ(DLB_ADM_DOLBYE_BSMOD_EMERGENCY, dolbye_data.program_ac3_params[6].bsmod);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[6].lfeon);
    EXPECT_EQ(2, dolbye_data.program_ac3_params[6].cmixlev);
    EXPECT_EQ(3, dolbye_data.program_ac3_params[6].surmixlev);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[6].dsurmod);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[6].dialnorm);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[6].copyrightb);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[6].origbs);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[6].langcod_exists);
    EXPECT_EQ(200, dolbye_data.program_ac3_params[6].langcod);    
    EXPECT_EQ(1, dolbye_data.program_ac3_params[6].audprodie);
    EXPECT_EQ(15, dolbye_data.program_ac3_params[6].mixlevel);
    EXPECT_EQ(2, dolbye_data.program_ac3_params[6].roomtyp);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[6].xbsi1_exists);
    EXPECT_EQ(7, dolbye_data.program_ac3_params[6].xbsi1_lorocmixlev);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[6].xbsi1_lorosurmixlev);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[6].xbsi1_ltrtcmixlev);
    EXPECT_EQ(2, dolbye_data.program_ac3_params[6].xbsi1_ltrtsurmixlev);
    EXPECT_EQ(3, dolbye_data.program_ac3_params[6].xbsi1_dmixmod);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[6].xbsi2_exists);
    EXPECT_EQ(3, dolbye_data.program_ac3_params[6].xbsi2_dsurexmod);
    EXPECT_EQ(3, dolbye_data.program_ac3_params[6].xbsi2_dheadphonmod);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[6].xbsi2_adconvtyp);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[6].compr1.is_profile);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[6].compr1.drc.profile); 
    EXPECT_EQ(0, dolbye_data.program_ac3_params[6].dynrng1.is_profile);
    EXPECT_EQ(255, dolbye_data.program_ac3_params[6].dynrng1.drc.profile);
    EXPECT_STREQ("Program 7 (Piłka nożna)", dolbye_data.program_ac3_params[6].programDescriptionText);
    EXPECT_EQ(1, dolbye_data.program_encode_params[6].hpfon);
    EXPECT_EQ(1, dolbye_data.program_encode_params[6].bwlpfon);
    EXPECT_EQ(1, dolbye_data.program_encode_params[6].lfelpfon);
    EXPECT_EQ(1, dolbye_data.program_encode_params[6].sur90on);
    EXPECT_EQ(1, dolbye_data.program_encode_params[6].suratton);
    EXPECT_EQ(1, dolbye_data.program_encode_params[6].rfpremphon);

    EXPECT_EQ(DLB_ADM_DOLBYE_ACMOD_1_0, dolbye_data.program_ac3_params[7].acmod);
    EXPECT_EQ(DLB_ADM_DOLBYE_BSMOD_VOICE_OVER_KARAOKE, dolbye_data.program_ac3_params[7].bsmod);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[7].lfeon);
    EXPECT_EQ(3, dolbye_data.program_ac3_params[7].cmixlev);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[7].surmixlev);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[7].dsurmod);
    EXPECT_EQ(10, dolbye_data.program_ac3_params[7].dialnorm);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[7].copyrightb);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[7].origbs);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[7].langcod_exists);
    EXPECT_EQ(44, dolbye_data.program_ac3_params[7].langcod);    
    EXPECT_EQ(1, dolbye_data.program_ac3_params[7].audprodie);
    EXPECT_EQ(28, dolbye_data.program_ac3_params[7].mixlevel);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[7].roomtyp);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[7].xbsi1_exists);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[7].xbsi2_exists);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[7].xbsi2_dsurexmod);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[7].xbsi2_dheadphonmod);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[7].xbsi2_adconvtyp);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[7].compr1.is_profile);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[7].compr1.drc.profile); 
    EXPECT_EQ(0, dolbye_data.program_ac3_params[7].dynrng1.is_profile);
    EXPECT_EQ(128, dolbye_data.program_ac3_params[7].dynrng1.drc.profile);
    EXPECT_STREQ("Program 8 (Football)", dolbye_data.program_ac3_params[7].programDescriptionText);
    EXPECT_EQ(0, dolbye_data.program_encode_params[7].hpfon);
    EXPECT_EQ(0, dolbye_data.program_encode_params[7].bwlpfon);
    EXPECT_EQ(0, dolbye_data.program_encode_params[7].lfelpfon);
    EXPECT_EQ(0, dolbye_data.program_encode_params[7].sur90on);
    EXPECT_EQ(0, dolbye_data.program_encode_params[7].suratton);
    EXPECT_EQ(1, dolbye_data.program_encode_params[7].rfpremphon);
}
