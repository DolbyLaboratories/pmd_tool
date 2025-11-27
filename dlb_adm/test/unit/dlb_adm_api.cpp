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
#include "AnalyzeContentXMLBuffers.h"

TEST_F(DlbXMLToAPICommon, DolbyE_4x20_ContentKind)
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

    dlb_adm_entity_id firstElementId = 504420754746507264;
    dlb_adm_entity_id secondElementId = 504420759041474560;
    dlb_adm_entity_id thirdElementId = 504420763336441856;
    dlb_adm_entity_id forthElementId = 504420767631409152;

    DLB_ADM_CONTENT_KIND contentKind;
    status =  dlb_adm_core_model_get_audio_element_content_kind(coreModel, firstElementId, &contentKind);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_CONTENT_KIND_MK_COMPLETE_MAIN, contentKind);

    status = dlb_adm_core_model_get_audio_element_content_kind(coreModel, secondElementId, &contentKind);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_CONTENT_KIND_NK_MUSIC_AND_EFFECTS, contentKind);

    status = dlb_adm_core_model_get_audio_element_content_kind(coreModel, thirdElementId, &contentKind);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_CONTENT_KIND_MK_VISUALLY_IMPAIRED, contentKind);

    status = dlb_adm_core_model_get_audio_element_content_kind(coreModel, forthElementId, &contentKind);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_CONTENT_KIND_MK_UNDEFINED, contentKind);

}

TEST_F(DlbXMLToAPICommon, ME_D_ContentKind)
{
    int status;

    status = ::dlb_adm_container_read_xml_buffer
        (originalContainer
        ,xml_ME_D_AD.c_str()
        ,xml_ME_D_AD.length()
        ,true
        );
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = dlb_adm_core_model_ingest_xml_container(coreModel, originalContainer);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    dlb_adm_entity_id bedId = 504420754746507264;
    dlb_adm_entity_id dialogueId = 504420759041474560;
    dlb_adm_entity_id audioDescriptionId = 504420763336441856;

    DLB_ADM_CONTENT_KIND contentKind;
    status = dlb_adm_core_model_get_audio_element_content_kind(coreModel, bedId, &contentKind);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_CONTENT_KIND_NK_MUSIC_AND_EFFECTS, contentKind);

    status = dlb_adm_core_model_get_audio_element_content_kind(coreModel, dialogueId, &contentKind);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_CONTENT_KIND_DK_DIALOGUE, contentKind);

    status = dlb_adm_core_model_get_audio_element_content_kind(coreModel, audioDescriptionId, &contentKind);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_CONTENT_KIND_DK_DESCRIPTION, contentKind);

    dlb_adm_entity_id notExistId = 504420767631409152;
    status = dlb_adm_core_model_get_audio_element_content_kind(coreModel, notExistId, &contentKind);
    EXPECT_EQ(DLB_ADM_STATUS_NOT_FOUND, status);

}
