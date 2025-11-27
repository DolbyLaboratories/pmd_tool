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
#include "DolbyEToSADMReferenceFiles.h"
#include "TestUtilities.h"

TEST_F(DlbXMLToXMLCommon, ingestGenerate_51_20)
{
    int status;

    status = ::dlb_adm_container_read_xml_buffer
        (originalContainer
        ,dolbyE_51_20.c_str()
        ,dolbyE_51_20.length()
        ,true
        );
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    static const char dolbyEIngestedReferenceFileName[] = "dolbye51_20.ingested.ref.xml";
    status = dlb_adm_container_write_xml_file(originalContainer, dolbyEIngestedReferenceFileName);

    status = dlb_adm_core_model_ingest_xml_container(coreModel, originalContainer);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // generate from core model
    dlb_adm_xml_container *outputContainer = nullptr;
    status = dlb_adm_container_open_from_core_model(&outputContainer, coreModel);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    static const char dolbyEIngestedOutFileName[] = "dolbye51_20.ingested.out.xml";
    status = dlb_adm_container_write_xml_file(outputContainer, dolbyEIngestedOutFileName);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_TRUE(DlbAdmTest::CompareFiles(dolbyEIngestedReferenceFileName, dolbyEIngestedOutFileName));

    dlb_adm_container_close(&outputContainer);
}

TEST_F(DlbXMLToXMLCommon, ingestGenerate_51_20_cartesian)
{
    int status;

    status = ::dlb_adm_container_read_xml_buffer
        (originalContainer
        ,dolbyE_51_20_cartesian.c_str()
        ,dolbyE_51_20_cartesian.length()
        ,true
        );
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    static const char dolbyEIngestedReferenceFileName[] = "dolbye51_20_cart.ingested.ref.xml";
    status = dlb_adm_container_write_xml_file(originalContainer, dolbyEIngestedReferenceFileName);

    status = dlb_adm_core_model_ingest_xml_container(coreModel, originalContainer);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // generate from core model
    dlb_adm_xml_container *outputContainer = nullptr;
    status = dlb_adm_container_open_from_core_model(&outputContainer, coreModel);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    static const char dolbyEIngestedOutFileName[] = "dolbye51_20_cart.ingested.out.xml";
    status = dlb_adm_container_write_xml_file(outputContainer, dolbyEIngestedOutFileName);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_TRUE(DlbAdmTest::CompareFiles(dolbyEIngestedReferenceFileName, dolbyEIngestedOutFileName));

    dlb_adm_container_close(&outputContainer);
}


TEST_F(DlbXMLToXMLCommon, ingestGenerate_4x_20)
{
    int status;

    status = ::dlb_adm_container_read_xml_buffer
        (originalContainer
        ,dolbyE_4x_20_1.c_str()
        ,dolbyE_4x_20_1.length()
        ,true
        );
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    static const char dolbyEIngestedReferenceFileName[] = "dolbye4x_20.ingested.ref.xml";
    status = dlb_adm_container_write_xml_file(originalContainer, dolbyEIngestedReferenceFileName);

    status = dlb_adm_core_model_ingest_xml_container(coreModel, originalContainer);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // generate from core model
    dlb_adm_xml_container *outputContainer = nullptr;
    status = dlb_adm_container_open_from_core_model(&outputContainer, coreModel);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    static const char dolbyEIngestedOutFileName[] = "dolbye4x_20.ingested.out.xml";
    status = dlb_adm_container_write_xml_file(outputContainer, dolbyEIngestedOutFileName);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_TRUE(DlbAdmTest::CompareFiles(dolbyEIngestedReferenceFileName, dolbyEIngestedOutFileName));
    dlb_adm_container_close(&outputContainer);
}

TEST_F(DlbXMLToXMLCommon, ingestGenerate_51)
{
    int status;

    status = ::dlb_adm_container_read_xml_buffer
        (originalContainer
        ,dolbyE_51_1.c_str()
        ,dolbyE_51_1.length()
        ,true
        );
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    static const char dolbyEIngestedReferenceFileName[] = "dolbye51.ingested.ref.xml";
    status = dlb_adm_container_write_xml_file(originalContainer, dolbyEIngestedReferenceFileName);

    status = dlb_adm_core_model_ingest_xml_container(coreModel, originalContainer);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // generate from core model
    dlb_adm_xml_container *outputContainer = nullptr;
    status = dlb_adm_container_open_from_core_model(&outputContainer, coreModel);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    static const char dolbyEIngestedOutFileName[] = "dolbye51.ingested.out.xml";
    status = dlb_adm_container_write_xml_file(outputContainer, dolbyEIngestedOutFileName);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_TRUE(DlbAdmTest::CompareFiles(dolbyEIngestedReferenceFileName, dolbyEIngestedOutFileName));
    dlb_adm_container_close(&outputContainer);
}
