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

#include "DolbyEToSADMReferenceFiles.h"

class DlbDolbyEXMLRefTests : public DlbXMLToAPICommon
{
};

TEST_F(DlbDolbyEXMLRefTests, getDolbyEData_4x_20_1_ref)
{
    int status;

    status = ::dlb_adm_container_read_xml_buffer
        (originalContainer
        ,dolbyE_4x_20_1.c_str()
        ,dolbyE_4x_20_1.length()
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

    dlb_adm_data_dolbye_data dolbye_data;
    status = ::dlb_adm_core_model_get_dolbye_data(&dolbye_data, coreModel);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    EXPECT_EQ(4, dolbye_data.program_count);
    EXPECT_EQ(DLB_ADM_DOLBYE_PROGRAM_CONFIG_2_2_2_2, dolbye_data.info.program_config_id);
    EXPECT_EQ(DLB_ADM_DOLBYE_FRAME_RATE_25, dolbye_data.info.frame_rate);
    EXPECT_EQ(01, dolbye_data.info.time_code.tc1);
    EXPECT_EQ(00, dolbye_data.info.time_code.tc2);
    EXPECT_EQ(00, dolbye_data.info.time_code.tc3);
    EXPECT_EQ(00, dolbye_data.info.time_code.tc4);

    EXPECT_EQ(DLB_ADM_DOLBYE_ACMOD_2_0, dolbye_data.program_ac3_params[0].acmod);
    EXPECT_EQ(DLB_ADM_DOLBYE_BSMOD_COMPLETE_MAIN, dolbye_data.program_ac3_params[0].bsmod);
    EXPECT_EQ(DLB_ADM_DOLBYE_ACMOD_2_0, dolbye_data.program_ac3_params[1].acmod);
    EXPECT_EQ(DLB_ADM_DOLBYE_BSMOD_MUSIC_AND_EFFECT, dolbye_data.program_ac3_params[1].bsmod);
    EXPECT_EQ(DLB_ADM_DOLBYE_ACMOD_2_0, dolbye_data.program_ac3_params[2].acmod);
    EXPECT_EQ(DLB_ADM_DOLBYE_BSMOD_VISUALLY_IMPAIRED, dolbye_data.program_ac3_params[2].bsmod);
    EXPECT_EQ(DLB_ADM_DOLBYE_ACMOD_2_0, dolbye_data.program_ac3_params[3].acmod);
    EXPECT_EQ(DLB_ADM_DOLBYE_BSMOD_HEARING_IMPAIRED, dolbye_data.program_ac3_params[3].bsmod);    
}

TEST_F(DlbDolbyEXMLRefTests, getDolbyEData_51_1_ref)
{
    int status;

    status = ::dlb_adm_container_read_xml_buffer
        (originalContainer
        ,dolbyE_51_1.c_str()
        ,dolbyE_51_1.length()
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

    dlb_adm_data_dolbye_data dolbye_data;
    status = ::dlb_adm_core_model_get_dolbye_data(&dolbye_data, coreModel);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    EXPECT_EQ(1, dolbye_data.program_count);
    EXPECT_EQ(DLB_ADM_DOLBYE_PROGRAM_CONFIG_51, dolbye_data.info.program_config_id);
    EXPECT_EQ(DLB_ADM_DOLBYE_FRAME_RATE_25, dolbye_data.info.frame_rate);

    EXPECT_EQ(DLB_ADM_DOLBYE_ACMOD_3_2, dolbye_data.program_ac3_params[0].acmod);
    EXPECT_EQ(DLB_ADM_DOLBYE_BSMOD_COMPLETE_MAIN, dolbye_data.program_ac3_params[0].bsmod);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[0].lfeon);
}

TEST_F(DlbDolbyEXMLRefTests, getDolbyEData_51_2_ref)
{
    int status;

    status = ::dlb_adm_container_read_xml_buffer
        (originalContainer
        ,dolbyE_51_2.c_str()
        ,dolbyE_51_2.length()
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

    dlb_adm_data_dolbye_data dolbye_data;
    status = ::dlb_adm_core_model_get_dolbye_data(&dolbye_data, coreModel);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    EXPECT_EQ(1, dolbye_data.program_count);
    EXPECT_EQ(DLB_ADM_DOLBYE_PROGRAM_CONFIG_51, dolbye_data.info.program_config_id);
    EXPECT_EQ(DLB_ADM_DOLBYE_FRAME_RATE_25, dolbye_data.info.frame_rate);

    EXPECT_EQ(DLB_ADM_DOLBYE_ACMOD_2_0, dolbye_data.program_ac3_params[0].acmod);
    EXPECT_EQ(DLB_ADM_DOLBYE_BSMOD_COMPLETE_MAIN, dolbye_data.program_ac3_params[0].bsmod);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[0].lfeon);
}

TEST_F(DlbDolbyEXMLRefTests, getDolbyEData_51_20_1_ref)
{
    int status;

    status = ::dlb_adm_container_read_xml_buffer
        (originalContainer
        ,dolbyE_51_20_1.c_str()
        ,dolbyE_51_20_1.length()
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

    dlb_adm_data_dolbye_data dolbye_data;
    status = ::dlb_adm_core_model_get_dolbye_data(&dolbye_data, coreModel);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    EXPECT_EQ(2, dolbye_data.program_count);
    EXPECT_EQ(DLB_ADM_DOLBYE_PROGRAM_CONFIG_51_2, dolbye_data.info.program_config_id);
    EXPECT_EQ(DLB_ADM_DOLBYE_FRAME_RATE_25, dolbye_data.info.frame_rate);

    EXPECT_EQ(DLB_ADM_DOLBYE_ACMOD_3_2, dolbye_data.program_ac3_params[0].acmod);
    EXPECT_EQ(DLB_ADM_DOLBYE_BSMOD_COMPLETE_MAIN, dolbye_data.program_ac3_params[0].bsmod);
    EXPECT_EQ(1, dolbye_data.program_ac3_params[0].lfeon);

    EXPECT_EQ(DLB_ADM_DOLBYE_ACMOD_2_0, dolbye_data.program_ac3_params[1].acmod);
    EXPECT_EQ(DLB_ADM_DOLBYE_BSMOD_COMPLETE_MAIN, dolbye_data.program_ac3_params[1].bsmod);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[1].lfeon);
}

TEST_F(DlbDolbyEXMLRefTests, getDolbyEData_51_20_2_ref)
{
    int status;

    status = ::dlb_adm_container_read_xml_buffer
        (originalContainer
        ,dolbyE_51_20_2.c_str()
        ,dolbyE_51_20_2.length()
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

    dlb_adm_data_dolbye_data dolbye_data;
    status = ::dlb_adm_core_model_get_dolbye_data(&dolbye_data, coreModel);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    EXPECT_EQ(2, dolbye_data.program_count);
    EXPECT_EQ(DLB_ADM_DOLBYE_PROGRAM_CONFIG_51_2, dolbye_data.info.program_config_id);
    EXPECT_EQ(DLB_ADM_DOLBYE_FRAME_RATE_25, dolbye_data.info.frame_rate);

    EXPECT_EQ(DLB_ADM_DOLBYE_ACMOD_2_0, dolbye_data.program_ac3_params[0].acmod);
    EXPECT_EQ(DLB_ADM_DOLBYE_BSMOD_COMPLETE_MAIN, dolbye_data.program_ac3_params[0].bsmod);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[0].lfeon);

    EXPECT_EQ(DLB_ADM_DOLBYE_ACMOD_2_0, dolbye_data.program_ac3_params[1].acmod);
    EXPECT_EQ(DLB_ADM_DOLBYE_BSMOD_COMPLETE_MAIN, dolbye_data.program_ac3_params[1].bsmod);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[1].lfeon);
}

TEST_F(DlbDolbyEXMLRefTests, getDolbyEData_20_20_1_ref)
{
    int status;

    status = ::dlb_adm_container_read_xml_buffer
        (originalContainer
        ,dolbyE_20_20_1.c_str()
        ,dolbyE_20_20_1.length()
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

    dlb_adm_data_dolbye_data dolbye_data;
    status = ::dlb_adm_core_model_get_dolbye_data(&dolbye_data, coreModel);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    EXPECT_EQ(2, dolbye_data.program_count);
    EXPECT_EQ(DLB_ADM_DOLBYE_PROGRAM_CONFIG_2_2, dolbye_data.info.program_config_id);
    EXPECT_EQ(DLB_ADM_DOLBYE_FRAME_RATE_25, dolbye_data.info.frame_rate);

    EXPECT_EQ(DLB_ADM_DOLBYE_ACMOD_2_0, dolbye_data.program_ac3_params[0].acmod);
    EXPECT_EQ(DLB_ADM_DOLBYE_BSMOD_COMPLETE_MAIN, dolbye_data.program_ac3_params[0].bsmod);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[0].lfeon);

    EXPECT_EQ(DLB_ADM_DOLBYE_ACMOD_2_0, dolbye_data.program_ac3_params[1].acmod);
    EXPECT_EQ(DLB_ADM_DOLBYE_BSMOD_MUSIC_AND_EFFECT, dolbye_data.program_ac3_params[1].bsmod);
    EXPECT_EQ(0, dolbye_data.program_ac3_params[1].lfeon);
}
