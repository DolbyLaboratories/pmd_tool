/************************************************************************
 * dlb_adm
 * Copyright (c) 2023-2024, Dolby Laboratories Inc.
 * Copyright (c) 2023-2024, Dolby International AB.
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

#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

#include "gtest/gtest.h"

#include "dlb_adm/include/dlb_adm_api.h"
#include "dlb_adm_api_pvt.h"

#include "dlb_adm_xml_container.h"
#include "core_model/core_model_defs.h"
#include "AdmIdTranslator.h"
#include "AdmId.h"

#include <stdio.h>
#include <fstream>
#include <string>

#include "AnalyzeContentXMLBuffers.h"

class AnalyzeContentTest : public testing::Test
{
protected:

    dlb_adm_container_counts     containerCounts;
    dlb_adm_core_model_counts    coreModelCounts;
    dlb_adm_xml_container       *oryginalContainer;
    dlb_adm_xml_container       *flattenedContainer;    
    dlb_adm_core_model          *coreModel;

    virtual void SetUp()
    {
        int status;
        ::memset(&containerCounts, 0, sizeof(containerCounts));
        ::memset(&coreModelCounts, 0, sizeof(coreModelCounts));
        oryginalContainer = nullptr;
        flattenedContainer = nullptr;        
        coreModel = nullptr;

        status = ::dlb_adm_container_open(&oryginalContainer, &containerCounts);
        ASSERT_EQ(DLB_ADM_STATUS_OK, status);
        status = ::dlb_adm_container_open(&flattenedContainer, &containerCounts);
        ASSERT_EQ(DLB_ADM_STATUS_OK, status);        
        status = ::dlb_adm_core_model_open(&coreModel, &coreModelCounts);
        ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    }

    virtual void TearDown()
    {
        if (oryginalContainer != nullptr)
        {
            if (::dlb_adm_container_close(&oryginalContainer))
            {
                oryginalContainer = nullptr;
            }
        }

        if (flattenedContainer != nullptr)
        {
            if (::dlb_adm_container_close(&flattenedContainer))
            {
                flattenedContainer = nullptr;
            }
        }        

        if (coreModel != nullptr)
        {
            if (::dlb_adm_core_model_close(&coreModel))
            {
                coreModel = nullptr;
            }
        }
    }

};

TEST_F(AnalyzeContentTest, OneBed_Valid)
{
    int status = ::dlb_adm_container_clear_all(oryginalContainer);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_clear(coreModel);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_container_read_xml_buffer
        (oryginalContainer
        ,xml_20.c_str()
        ,xml_20.length()
        ,true
        );
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    //ingest into the core model
    status = dlb_adm_core_model_add_profile(coreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = dlb_adm_core_model_ingest_xml_container(coreModel, oryginalContainer);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
}

TEST_F(AnalyzeContentTest, ObjectRef_Valid)
{
    int status = ::dlb_adm_container_clear_all(oryginalContainer);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_clear(coreModel);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_container_read_xml_buffer
        (oryginalContainer
        ,object_ref.c_str()
        ,object_ref.length()
        ,true
        );
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    //ingest into the core model
    status = dlb_adm_core_model_add_profile(coreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = dlb_adm_core_model_ingest_xml_container(coreModel, oryginalContainer);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
}

TEST_F(AnalyzeContentTest, ObjectRef_PackFormat_InValid)
{
    int status = ::dlb_adm_container_clear_all(oryginalContainer);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_clear(coreModel);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_container_read_xml_buffer
        (oryginalContainer
        ,object_ref_pack_format_wrong.c_str()
        ,object_ref_pack_format_wrong.length()
        ,true
        );
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    //ingest into the core model
    status = dlb_adm_core_model_add_profile(coreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = dlb_adm_core_model_ingest_xml_container(coreModel, oryginalContainer);
    ASSERT_EQ(DLB_ADM_STATUS_ERROR, status);
}

TEST_F(AnalyzeContentTest, ObjectRef_Invalid)
{
    int status = ::dlb_adm_container_clear_all(oryginalContainer);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_clear(coreModel);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_container_read_xml_buffer(oryginalContainer, object_ref_wrong.c_str(), object_ref_wrong.length(), true);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    //ingest into the core model
    status = dlb_adm_core_model_add_profile(coreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = dlb_adm_core_model_ingest_xml_container(coreModel, oryginalContainer);
    ASSERT_EQ(DLB_ADM_STATUS_ERROR, status);
}

TEST_F(AnalyzeContentTest, Content_Invalid)
{
    int status = ::dlb_adm_container_clear_all(oryginalContainer);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_clear(coreModel);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_container_read_xml_buffer(oryginalContainer, content_wrong.c_str(), content_wrong.length(), true);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    //ingest into the core model
    status = dlb_adm_core_model_add_profile(coreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = dlb_adm_core_model_ingest_xml_container(coreModel, oryginalContainer);
    ASSERT_EQ(DLB_ADM_STATUS_ERROR, status);
}

TEST_F(AnalyzeContentTest, PackFormat_Invalid)
{
    int status = ::dlb_adm_container_clear_all(oryginalContainer);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_clear(coreModel);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_container_read_xml_buffer(oryginalContainer, packformat_wrong.c_str(), packformat_wrong.length(), true);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    //ingest into the core model
    status = dlb_adm_core_model_add_profile(coreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = dlb_adm_core_model_ingest_xml_container(coreModel, oryginalContainer);
    ASSERT_EQ(DLB_ADM_STATUS_ERROR, status);
}

TEST_F(AnalyzeContentTest, IngestAlternativeValueSetFromPMDStudio)
{
    int status = ::dlb_adm_container_clear_all(oryginalContainer);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_container_clear_all(flattenedContainer);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);    
    status = ::dlb_adm_core_model_clear(coreModel);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_container_read_xml_buffer(oryginalContainer, altValSet.c_str(), altValSet.length(), true);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    //ingest into the core model
    status = dlb_adm_core_model_add_profile(coreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = dlb_adm_container_flatten(oryginalContainer, flattenedContainer);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    
    status = dlb_adm_core_model_ingest_xml_container(coreModel, flattenedContainer);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
}
