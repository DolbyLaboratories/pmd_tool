/************************************************************************
 * dlb_adm
 * Copyright (c) 2020 - 2022, Dolby Laboratories Inc.
 * Copyright (c) 2022, Dolby International AB.
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
#include "string.h"

#include <fstream>
#include <string>

#include "dlb_adm/include/dlb_adm_api.h"
#include "dlb_adm/src/core_model/dlb_adm_core_model.h"
#include "XMLBuffer.h"
#include "XMLIngester.h"
#include "XMLGenerator.h"
#include "CoreModel.h"

#include "dlb_adm_data.h"

static const char stereoXMLFileName[] = "stereo_test_05.xml";
static const char stereoOut1XMLFileName[] = "stereo_test_05.out1.xml";
static const char stereoOut2XMLFileName[] = "stereo_test_05.out2.xml";
static const char apiStereoOut1XMLFileName[] = "api_stereo_test_05.out1.xml";
static const char apiStereoOut2XMLFileName[] = "api_stereo_test_05.out2.xml";
static const char stereoXMLGenOutFileName[] = "stereo_test_05.gen.out.xml";
static const char stereoXMLBufferOutFileName[] = "stereo_test_05.XMLBuffer.out.xml";

class DlbAdm05 : public testing::Test
{
protected:

    void SetUpTestInput()
    {
        std::ifstream ifs(stereoXMLFileName);

        if (!ifs.good())
        {
            std::ofstream ofs(stereoXMLFileName);

            ofs << stereoXML;
        }
    }

    bool CompareFiles(const char *fname1, const char *fname2)
    {
        std::ifstream ifs1(fname1);
        std::ifstream ifs2(fname2);
        bool eq = ifs1.good() && ifs2.good();

        if (eq)
        {
            std::string line1;
            std::string line2;
            bool got1 = !std::getline(ifs1, line1).eof();
            bool got2 = !std::getline(ifs2, line2).eof();

            while (got1 && got2)
            {
                if (!(line1 == line2))
                {
                    eq = false;
                    break;
                }
                got1 = !std::getline(ifs1, line1).eof();
                got2 = !std::getline(ifs2, line2).eof();
            }
            if (eq && (got1 || got2))
            {
                eq = false; // they should end at the same time
            }
        }

        return eq;
    }

    virtual void SetUp()
    {
        SetUpTestInput();
    }

    virtual void TearDown()
    {
    }

};

TEST_F(DlbAdm05, ReadAndWriteStereoXMLFile)
{
    dlb_adm_container_counts counts;
    dlb_adm_xml_container *c1 = nullptr;
    dlb_adm_xml_container *c2 = nullptr;
    int status;

    // open the container
    ::memset(&counts, 0, sizeof(counts));
    status = ::dlb_adm_container_open(&c1, &counts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_container_open(&c2, &counts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // read the XML input file
    status = dlb_adm_container_read_xml_file(c1, stereoXMLFileName, DLB_ADM_FALSE);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // write the first XML output file
    status = dlb_adm_container_write_xml_file(c1, stereoOut1XMLFileName);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // read the first XML output file
    status = dlb_adm_container_read_xml_file(c2, stereoOut1XMLFileName, DLB_ADM_FALSE);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // write the second XML output file
    status = dlb_adm_container_write_xml_file(c2, stereoOut2XMLFileName);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // compare the output files
    EXPECT_TRUE(CompareFiles(stereoOut1XMLFileName, stereoOut2XMLFileName));

    // close the container
    status = ::dlb_adm_container_close(&c2);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_container_close(&c1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
}

TEST_F(DlbAdm05, ReadIngestGenerateAndWriteStereoXMLFile)
{
    using namespace DlbAdm;

    dlb_adm_container_counts counts;
    dlb_adm_xml_container *c = nullptr;
    int status;

    // open the container
    ::memset(&counts, 0, sizeof(counts));
    status = ::dlb_adm_container_open(&c, &counts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // read the XML input file
    status = dlb_adm_container_read_xml_file(c, stereoXMLFileName, DLB_ADM_FALSE);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // ingest the XML into a CoreModel instance
    CoreModel model;
    model.AddProfile(DLB_ADM_PROFILE_SADM_EMISSION_PROFILE);

    XMLIngester ingester(model, *c);

    status = ingester.Ingest();
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // close the "ingest" container
    status = ::dlb_adm_container_close(&c);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // open the "generate" container
    status = ::dlb_adm_container_open(&c, &counts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // generate the XML model
    XMLGenerator generator(*c, model);

    status = generator.GenerateFrame();
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // write the output XML file
    status = dlb_adm_container_write_xml_file(c, stereoXMLGenOutFileName);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // compare the output files
    EXPECT_TRUE(CompareFiles(stereoXMLFileName, stereoXMLGenOutFileName));

    // close the "generate" container
    status = ::dlb_adm_container_close(&c);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
}

TEST_F(DlbAdm05, OpenFromCoreModelBasic)
{
    dlb_adm_core_model_counts counts;
    dlb_adm_core_model *cm = nullptr;
    dlb_adm_xml_container *c = nullptr;
    int status;

    ::memset(&counts, 0, sizeof(counts));
    status = ::dlb_adm_core_model_open(&cm, &counts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = dlb_adm_container_open_from_core_model(nullptr, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = dlb_adm_container_open_from_core_model(nullptr, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);

    status = dlb_adm_container_open_from_core_model(&c, cm);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_container_close(&c);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_close(&cm);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
}

TEST_F(DlbAdm05, ReadAndWriteStereoXMLFileViaAPI)
{
    dlb_adm_container_counts counts;
    dlb_adm_xml_container *c1 = nullptr;
    dlb_adm_xml_container *c2 = nullptr;
    dlb_adm_core_model *cm = nullptr;
    int status;

    // open the first container
    ::memset(&counts, 0, sizeof(counts));
    status = ::dlb_adm_container_open(&c1, &counts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // read the XML input file
    status = dlb_adm_container_read_xml_file(c1, stereoXMLFileName, DLB_ADM_FALSE);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // write the first XML output file
    status = dlb_adm_container_write_xml_file(c1, apiStereoOut1XMLFileName);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // ingest into the core model
    status = dlb_adm_core_model_open_from_xml_container(&cm, c1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // generate into the second container (begin actual test!)
    status = dlb_adm_container_open_from_core_model(&c2, cm);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // write the second XML output file
    status = dlb_adm_container_write_xml_file(c2, apiStereoOut2XMLFileName);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // compare the output files
    EXPECT_TRUE(CompareFiles(apiStereoOut1XMLFileName, apiStereoOut2XMLFileName));

    // close everything
    status = ::dlb_adm_core_model_close(&cm);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_container_close(&c2);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_container_close(&c1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
}

TEST(dlb_adm_test, ModelCount)
{
    using namespace DlbAdm;

    dlb_adm_data_audio_track audioTrack;
    dlb_adm_core_model_counts counts;
    dlb_adm_core_model *cm = nullptr;
    int status;

    ::memset(&audioTrack, 0, sizeof(audioTrack));
    ::memset(&counts, 0, sizeof(counts));
    status = ::dlb_adm_core_model_open(&cm, &counts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    CoreModel &model = cm->GetCoreModel();
    size_t count;

    count = model.Count(DLB_ADM_ENTITY_TYPE_TRACK_UID);
    EXPECT_EQ(0u, count);
    status = ::dlb_adm_core_model_add_audio_track(cm, &audioTrack);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    count = model.Count(DLB_ADM_ENTITY_TYPE_TRACK_UID);
    EXPECT_EQ(1u, count);
    count = model.Count(DLB_ADM_ENTITY_TYPE_PROGRAMME);
    EXPECT_EQ(0u, count);

    audioTrack.id = DLB_ADM_NULL_ENTITY_ID;
    status = ::dlb_adm_core_model_add_audio_track(cm, &audioTrack);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    count = model.Count(DLB_ADM_ENTITY_TYPE_TRACK_UID);
    EXPECT_EQ(2u, count);
    count = model.Count(DLB_ADM_ENTITY_TYPE_PROGRAMME);
    EXPECT_EQ(0u, count);

    audioTrack.id = DLB_ADM_NULL_ENTITY_ID;
    status = ::dlb_adm_core_model_add_audio_track(cm, &audioTrack);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    count = model.Count(DLB_ADM_ENTITY_TYPE_TRACK_UID);
    EXPECT_EQ(3u, count);
    count = model.Count(DLB_ADM_ENTITY_TYPE_PROGRAMME);
    EXPECT_EQ(0u, count);

    status = ::dlb_adm_core_model_close(&cm);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
}

static void WriteChars(std::ostream &os, const char *chars, size_t count)
{
    for (size_t i = 0; i < count; ++i)
    {
        os << chars[i];
    }
}

TEST_F(DlbAdm05, TestXMLBuffer)
{
    using namespace DlbAdm;

    static const size_t LINE_BUFFER_SZ = 1024;
    static const size_t LINE_BUFFER_MAX = LINE_BUFFER_SZ - 1;
    char lineBuffer[LINE_BUFFER_SZ];

    size_t stereoXMLSize = ::strlen(stereoXML);
    XMLBuffer emptyBuffer;
    XMLBuffer stereoXmlBuffer(stereoXML, stereoXMLSize);
    size_t gotTotal = 0;
    size_t gotCount;

    ::memset(lineBuffer, 0, sizeof(lineBuffer));
    SetUpTestInput();

    gotCount = emptyBuffer.GetLine(lineBuffer, LINE_BUFFER_MAX);
    EXPECT_EQ(0u, gotCount);

    {
        std::ofstream outputFile(stereoXMLBufferOutFileName);
        ASSERT_TRUE(outputFile.good());

        gotCount = stereoXmlBuffer.GetLine(lineBuffer, LINE_BUFFER_MAX);
        while (gotCount > 0)
        {
            gotTotal += gotCount;
            WriteChars(outputFile, lineBuffer, gotCount);
            gotCount = stereoXmlBuffer.GetLine(lineBuffer, LINE_BUFFER_MAX);
        }
    }
    EXPECT_EQ(stereoXMLSize, gotTotal);
    EXPECT_TRUE(CompareFiles(stereoXMLFileName, stereoXMLBufferOutFileName));
}
