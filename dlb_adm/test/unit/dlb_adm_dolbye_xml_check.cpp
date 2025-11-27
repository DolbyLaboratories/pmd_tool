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
#include <regex>

#include "DolbyEProfileXMLBuffers.h"
#include "IncorrectDolbyEXMLBuffers.h"

using std::get;
using ::testing::TestWithParam;
using ::testing::tuple;
using ::testing::get;
using ::testing::Values;
using ::testing::make_tuple;

class DlbDolbyEXMLCheckBased : public ::testing::Test
{
    protected:

        dlb_adm_container_counts     containerCounts;
        dlb_adm_core_model_counts    coreModelCounts;
        dlb_adm_xml_container       *originalContainer; 
        dlb_adm_core_model          *coreModel;

        void SetUp() override
        {
            int status;
            ::memset(&containerCounts, 0, sizeof(containerCounts));
            ::memset(&coreModelCounts, 0, sizeof(coreModelCounts));
            originalContainer = nullptr;  
            coreModel = nullptr;

            status = ::dlb_adm_container_open(&originalContainer, &containerCounts);
            ASSERT_EQ(DLB_ADM_STATUS_OK, status);      
            status = ::dlb_adm_core_model_open(&coreModel, &coreModelCounts);
            ASSERT_EQ(DLB_ADM_STATUS_OK, status);
        }

        void TearDown() override
        {
            if (originalContainer != nullptr)
            {
                if (::dlb_adm_container_close(&originalContainer))
                {
                    originalContainer = nullptr;
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

        std::string testedXML;
};

class DlbDolbyEValueRangeCheckTests : public DlbDolbyEXMLCheckBased, public testing::WithParamInterface<tuple<std::string, std::string> >
{
    protected:
        void SetUp() override
        {
            DlbDolbyEXMLCheckBased::SetUp();

            std::string tag = get<0>(GetParam());
            std::string value = get<1>(GetParam());
            std::regex tagRegex("<" + tag + R"(((\s[^>]*)?)>(.*?)</)" + tag + ">");
            testedXML = std::regex_replace(dolbyE_51_selected, tagRegex, "<" + tag + "$1>" + value + "</" + tag + ">");
        }
};

INSTANTIATE_TEST_CASE_P(CheckRangeTests,
                         DlbDolbyEValueRangeCheckTests,
                         ::testing::Values
                         (make_tuple("acMod", "8")
                         ,make_tuple("acMod", "-1")
                         ,make_tuple("bsMod", "8")
                         ,make_tuple("lfeOn", "2")
                         ,make_tuple("cMixLev", "4")
                         ,make_tuple("surMixLev", "4")
                         ,make_tuple("dSurMod", "4")
                         ,make_tuple("dialNorm", "32")
                         ,make_tuple("copyRightB", "2")
                         ,make_tuple("origBs", "2")
                         ,make_tuple("hpFOn", "2")
                         ,make_tuple("bwLpFOn", "7")
                         ,make_tuple("lfeLpFOn","2")
                         ,make_tuple("sur90On", "2")
                         ,make_tuple("surAttOn", "3")
                         ,make_tuple("rfPremphOn", "2")
                         ,make_tuple("langCod", "256")
                         ,make_tuple("mixLevel", "32")
                         ,make_tuple("roomTyp", "4")
                         ,make_tuple("loRoCMixLev", "8")
                         ,make_tuple("loRoSurMixLev", "8")
                         ,make_tuple("ltRtCMixLev", "8")
                         ,make_tuple("ltRtSurMixLev", "8")
                         ,make_tuple("dMixMod", "4")
                         ,make_tuple("dSurExMod", "4")
                         ,make_tuple("dHeadPhonMod", "4")
                         ,make_tuple("compr1", "256")
                         ,make_tuple("dynRng1", "256")
                         )
);

TEST_P(DlbDolbyEValueRangeCheckTests, CheckValueOutOfRange)
{
    int status;

    status = ::dlb_adm_container_read_xml_buffer
        (originalContainer
        ,testedXML.c_str()
        ,testedXML.length()
        ,true
        );
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = dlb_adm_core_model_ingest_xml_container(coreModel, originalContainer);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_DOLBYE_MD, status);
}

class DlbDolbyEValueOptionalTests : public DlbDolbyEXMLCheckBased, public testing::WithParamInterface<std::string >
{
    protected:
        void SetUp() override
        {
            DlbDolbyEXMLCheckBased::SetUp();
            std::string tag = GetParam();
            std::string pattern = "<" + tag + R"(\b[^>]*?>[\s\S]*?</)" + tag + ">";
            std::regex tagRegex(pattern, std::regex::icase);

            testedXML = std::regex_replace(dolbyE_51_selected, tagRegex, "");
        }
};

INSTANTIATE_TEST_CASE_P(CheckXMLTagsOptional,
                         DlbDolbyEValueOptionalTests,
                         ::testing::Values
                         ("programDescriptionText"
                         )
);

TEST_P(DlbDolbyEValueOptionalTests, CheckXMLTagsOptional)
{
    int status;

    status = ::dlb_adm_container_read_xml_buffer
        (originalContainer
        ,testedXML.c_str()
        ,testedXML.length()
        ,true
        );
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = dlb_adm_core_model_ingest_xml_container(coreModel, originalContainer);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
}


class DlbDolbyEValueAbsenceTests : public DlbDolbyEXMLCheckBased, public testing::WithParamInterface<std::string >
{
    protected:
        void SetUp() override
        {
            DlbDolbyEXMLCheckBased::SetUp();
            std::string tag = GetParam();
            std::string pattern = "<" + tag + R"(\b[^>]*?>[\s\S]*?</)" + tag + ">";
            std::regex tagRegex(pattern, std::regex::icase);

            testedXML = std::regex_replace(dolbyE_51_selected, tagRegex, "");
        }
};

INSTANTIATE_TEST_CASE_P(CheckXMLTagsNotPresent,
                         DlbDolbyEValueAbsenceTests,
                         ::testing::Values
                         ("acMod"
                         ,"bsMod"
                         ,"lfeOn"
                         ,"cMixLev"
                         ,"surMixLev"
                         ,"dSurMod"
                         ,"dialNorm"
                         ,"copyRightB"
                         ,"origBs"
                         ,"hpFOn"
                         ,"bwLpFOn"
                         ,"lfeLpFOn"
                         ,"sur90On"
                         ,"surAttOn"
                         ,"rfPremphOn"
                         ,"langCod"
                         ,"mixLevel"
                         ,"roomTyp"
                         ,"loRoCMixLev"
                         ,"loRoSurMixLev"
                         ,"ltRtCMixLev"
                         ,"ltRtSurMixLev"
                         ,"dMixMod"
                         ,"dSurExMod"
                         ,"dHeadPhonMod"
                         ,"compr1"
                         ,"dynRng1"
                         ,"audioProdInfo"
                         ,"langCode"
                         ,"extBsi1e"
                         ,"extBsi2e"
                         )
);

TEST_P(DlbDolbyEValueAbsenceTests, CheckXMLTagsNotPresent)
{
    int status;

    status = ::dlb_adm_container_read_xml_buffer
        (originalContainer
        ,testedXML.c_str()
        ,testedXML.length()
        ,true
        );
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = dlb_adm_core_model_ingest_xml_container(coreModel, originalContainer);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_DOLBYE_MD, status);
}

class DlbAMDValueAbsenceTests : public DlbDolbyEValueAbsenceTests
{
};

INSTANTIATE_TEST_CASE_P(CheckXMLAdmTagsNotPresent,
                         DlbAMDValueAbsenceTests,
                         ::testing::Values
                         ("audioProgramme"
                         )
);

TEST_P(DlbAMDValueAbsenceTests, CheckXMLAdmTagsNotPresent)
{
    int status;

    status = ::dlb_adm_container_read_xml_buffer
        (originalContainer
        ,testedXML.c_str()
        ,testedXML.length()
        ,true
        );
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = dlb_adm_core_model_ingest_xml_container(coreModel, originalContainer);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ADM_FOR_DBMD, status);
}

class DlbDolbyEVNotExistTests : public DlbDolbyEXMLCheckBased, public testing::WithParamInterface<tuple<std::string, std::string> >
{
    protected:
        void SetUp() override
        {
            DlbDolbyEXMLCheckBased::SetUp();

            std::string tag = get<0>(GetParam());
            std::string removeTag = get<1>(GetParam());

            std::regex extBsi1eTagRegex("<" + tag + "([^>]*)>([\\s\\S]*?)</" + tag + ">", std::regex::icase);
            std::smatch match;

            if (std::regex_search(dolbyE_51_selected, match, extBsi1eTagRegex))
            {
                std::string attributes = match[1].str();
                std::string innerContent = match[2].str();

                // Replace exist="1" with exist="0"
                std::regex existAttrRegex("exists\\s*=\\s*[\"']?1[\"']?", std::regex::icase);
                attributes = std::regex_replace(attributes, existAttrRegex, "exists=\"0\"");

                // Remove only the specified tag
                std::regex childRegex("<" + removeTag + "\\b[^>]*?>[\\s\\S]*?</" + removeTag + ">", std::regex::icase);
                innerContent = std::regex_replace(innerContent, childRegex, "");

                std::string updatedTag = "<" + tag + attributes + ">" + innerContent + "</" + tag + ">";
                testedXML = match.prefix().str() + updatedTag + match.suffix().str();;
            }
        }
};

INSTANTIATE_TEST_CASE_P(CheckXMLTagsNotPresentInNotExist,
                         DlbDolbyEVNotExistTests,
                         ::testing::Values
                         (make_tuple("extBsi1e", "loRoCMixLev")
                         ,make_tuple("extBsi1e", "loRoSurMixLev")
                         ,make_tuple("extBsi1e", "ltRtCMixLev")
                         ,make_tuple("extBsi1e", "ltRtSurMixLev")
                         ,make_tuple("extBsi1e", "dMixMod")
                         ,make_tuple("extBsi2e", "dSurExMod")
                         ,make_tuple("extBsi2e", "dHeadPhonMod")
                         ,make_tuple("extBsi2e", "adConvTyp")
                         ,make_tuple("audioProdInfo", "mixLevel")
                         ,make_tuple("audioProdInfo", "roomTyp")
                         ,make_tuple("langCode", "langCod")
                         )
);

TEST_P(DlbDolbyEVNotExistTests, CheckXMLTagsNotPresentInNotExist)
{
    int status;

    status = ::dlb_adm_container_read_xml_buffer
        (originalContainer
        ,testedXML.c_str()
        ,testedXML.length()
        ,true
        );
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = dlb_adm_core_model_ingest_xml_container(coreModel, originalContainer);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
}

class DlbDolbyEDuplicateTagTests : public DlbDolbyEXMLCheckBased, public testing::WithParamInterface<std::string>
{
    protected:
        void SetUp() override
        {
            DlbDolbyEXMLCheckBased::SetUp();
            std::string tag = GetParam();

            std::regex repeatTagRegex("<" + tag + "[^>]*>[\\s\\S]*?</" + tag + ">", std::regex::icase);
            std::smatch match;

            if (std::regex_search(dolbyE_51_selected, match, repeatTagRegex))
            {
                std::string originalTag = match[0].str();
                // Insert the duplicate right after the original
                testedXML = match.prefix().str() + originalTag + originalTag + match.suffix().str();
            }
        }
};

INSTANTIATE_TEST_CASE_P(CheckXMLTagsRepeated,
                         DlbDolbyEDuplicateTagTests,
                         ::testing::Values
                         ("acMod"
                         ,"bsMod"
                         ,"lfeOn"
                         ,"cMixLev"
                         ,"surMixLev"
                         ,"dSurMod"
                         ,"dialNorm"
                         ,"copyRightB"
                         ,"origBs"
                         ,"hpFOn"
                         ,"bwLpFOn"
                         ,"lfeLpFOn"
                         ,"sur90On"
                         ,"surAttOn"
                         ,"rfPremphOn"
                         ,"mixLevel"
                         ,"roomTyp"
                         ,"loRoCMixLev"
                         ,"loRoSurMixLev"
                         ,"ltRtCMixLev"
                         ,"ltRtSurMixLev"
                         ,"dMixMod"
                         ,"dSurExMod"
                         ,"dHeadPhonMod"
                         ,"compr1"
                         ,"dynRng1"
                         ,"audioProdInfo"
                         ,"langCode"
                         ,"extBsi1e"
                         ,"extBsi2e"
                         ,"encodeParameters"
                         ,"ac3Program"
                         )
);

TEST_P(DlbDolbyEDuplicateTagTests, CheckXMLTagsRepeated)
{
    int status;

    status = ::dlb_adm_container_read_xml_buffer
        (originalContainer
        ,testedXML.c_str()
        ,testedXML.length()
        ,true
        );
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = dlb_adm_core_model_ingest_xml_container(coreModel, originalContainer);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_DOLBYE_MD, status);
}

class DlbDolbyEIncorectAttributesTests : public DlbDolbyEXMLCheckBased, public testing::WithParamInterface<tuple<std::string, std::string, int> >
{
    protected:
        void SetUp() override
        {
            DlbDolbyEXMLCheckBased::SetUp();
            std::string value = get<0>(GetParam());
            testedXML = dolbyE_51_selected;
            size_t pos = testedXML.find(value);
            if (pos != std::string::npos)
            {
                testedXML.replace(pos, value.length(), get<1>(GetParam()));
            }
        }
};

INSTANTIATE_TEST_CASE_P(CheckXMLIncorrectAttributeValue,
                         DlbDolbyEIncorectAttributesTests,
                         ::testing::Values
                         (make_tuple(R"(ac3Program ID="2")", R"(ac3Program ID="3")", DLB_ADM_STATUS_INVALID_DOLBYE_MD)
                         ,make_tuple(R"(ac3Program ID="2")", R"(ac3Program ID="9")", DLB_ADM_STATUS_INVALID_DOLBYE_MD)
                         ,make_tuple(R"(encodeParameters ID="2")", R"(encodeParameters ID="3")", DLB_ADM_STATUS_INVALID_DOLBYE_MD)
                         ,make_tuple(R"(metadataSegment ID="3")", R"(metadataSegment ID="0")", DLB_ADM_STATUS_INVALID_DBMD_SEGMENT_ID)
                         ,make_tuple(R"(metadataSegment ID="3")", R"(metadataSegment ID="4")", DLB_ADM_STATUS_INVALID_DBMD_SEGMENT_ID)
                         )
);

TEST_P(DlbDolbyEIncorectAttributesTests, CheckXMLIncorrectAttributeValue)
{
    int status;

    status = ::dlb_adm_container_read_xml_buffer
        (originalContainer
        ,testedXML.c_str()
        ,testedXML.length()
        ,true
        );
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = dlb_adm_core_model_ingest_xml_container(coreModel, originalContainer);
    EXPECT_EQ(get<2>(GetParam()), status);
}

TEST_F(DlbDolbyEXMLCheckBased, CheckDolbyEProfileOnly)
{
    int status;
    testedXML = dolbyE_51_selected;
    std::string toRemove("ITU-R BS.2168");

    size_t pos = 0;
    while (pos != std::string::npos)
    {
        pos = testedXML.find(toRemove);
        if (pos != std::string::npos)
        {
            testedXML.replace(pos, toRemove.length(), "");
        }
    }

    status = ::dlb_adm_container_read_xml_buffer
        (originalContainer
        ,testedXML.c_str()
        ,testedXML.length()
        ,true
        );
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = dlb_adm_core_model_ingest_xml_container(coreModel, originalContainer);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ADM_FOR_DBMD, status);
}

TEST_F(DlbDolbyEXMLCheckBased, CheckMoreThan8Tracks)
{
    int status;
    testedXML = dolbyE_8x_10;
    static std::string toInsert = R"(<audioTrackUID UID="ATU_00000009">
      <audioChannelFormatIDRef>AC_00010003</audioChannelFormatIDRef>
      <audioPackFormatIDRef>AP_00010001</audioPackFormatIDRef>
    </audioTrackUID>)";
    size_t pos = testedXML.find("</audioFormatExtended>");
    testedXML.insert(pos, toInsert);

    status = ::dlb_adm_container_read_xml_buffer
        (originalContainer
        ,testedXML.c_str()
        ,testedXML.length()
        ,true
        );
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = dlb_adm_core_model_ingest_xml_container(coreModel, originalContainer);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ADM_FOR_DBMD, status);
}

TEST_F(DlbDolbyEXMLCheckBased, CheckMoreThan8Programs)
{
    int status;
    testedXML = dolbyE_8x_10;
    static std::string toInsert = R"(<audioProgramme audioProgrammeID="APR_1009" audioProgrammeLanguage="und" audioProgrammeName=\"Programme 9 (Program 9)\">
      <audioContentIDRef>ACO_1008</audioContentIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-10</dialogueLoudness>
      </loudnessMetadata>
    </audioProgramme>)";
    size_t pos = testedXML.find("<audioTrackUID UID=");
    testedXML.insert(pos, toInsert);

    status = ::dlb_adm_container_read_xml_buffer
        (originalContainer
        ,testedXML.c_str()
        ,testedXML.length()
        ,true
        );
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = dlb_adm_core_model_ingest_xml_container(coreModel, originalContainer);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ADM_FOR_DBMD, status);
}

TEST_F(DlbDolbyEXMLCheckBased, CheckMoreThan8Contents)
{
    int status;
    testedXML = dolbyE_8x_10;
    static std::string toInsert = R"(<audioContent audioContentID="ACO_1009" audioContentLanguage="und" audioContentName="Content 9">
      <audioObjectIDRef>AO_1009</audioObjectIDRef>
      <loudnessMetadata>
        <dialogueLoudness>-10</dialogueLoudness>
      </loudnessMetadata>
      <dialogue dialogueContentKind="2">1</dialogue>
    </audioContent>
    <audioObject audioObjectID="AO_1009" audioObjectName="Object 8" interact="0">
      <audioPackFormatIDRef>AP_00010001</audioPackFormatIDRef>
      <audioTrackUIDRef>ATU_00000008</audioTrackUIDRef>
    </audioObject>)";
    size_t pos = testedXML.find("<audioTrackUID UID=");
    testedXML.insert(pos, toInsert);

    status = ::dlb_adm_container_read_xml_buffer
        (originalContainer
        ,testedXML.c_str()
        ,testedXML.length()
        ,true
        );
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = dlb_adm_core_model_ingest_xml_container(coreModel, originalContainer);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ADM_FOR_DBMD, status);
}

TEST_F(DlbDolbyEXMLCheckBased, CheckObjectPackFormat)
{
    int status;
    testedXML = dolbyE_51_selected;
    static std::string toReplace = R"(<audioPackFormatIDRef>AP_00031002</audioPackFormatIDRef>)";
    static std::string toFind = R"(<audioPackFormatIDRef>AP_00010002</audioPackFormatIDRef>)";
    size_t pos = testedXML.find(toFind);
    testedXML.replace(pos, toFind.length(), toReplace);
    static std::string toInsert = R"(<audioPackFormat audioPackFormatID="AP_00031002" audioPackFormatName="dialogue 1" typeLabel="0003" typeDefinition="Objects">
    <audioChannelFormatIDRef>AC_00031002</audioChannelFormatIDRef>
    </audioPackFormat>
    <audioChannelFormat audioChannelFormatID="AC_00031002" audioChannelFormatName="dialogue 1" typeLabel="0003" typeDefinition="Objects">
    <audioBlockFormat audioBlockFormatID="AB_00031002_00000001" lstart="00:00:00.00000" lduration="00:00:00.01920S48000">
    <cartesian>1</cartesian>
    <position coordinate="X">0.00</position>
    <position coordinate="Y">1.00</position>
    <position coordinate="Z">0.00</position>
    </audioBlockFormat>
    </audioChannelFormat>)";
    pos = testedXML.find("<audioTrackUID UID=");
    testedXML.insert(pos, toInsert);

    status = ::dlb_adm_container_read_xml_buffer
        (originalContainer
        ,testedXML.c_str()
        ,testedXML.length()
        ,true
        );
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = dlb_adm_core_model_ingest_xml_container(coreModel, originalContainer);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ADM_FOR_DBMD, status);
}

TEST_F(DlbDolbyEXMLCheckBased, CheckAltValueSet)
{
    int status;

    status = ::dlb_adm_container_read_xml_buffer
        (originalContainer
        ,dolbyE_51_altvalset.c_str()
        ,dolbyE_51_altvalset.length()
        ,true
        );
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = dlb_adm_core_model_ingest_xml_container(coreModel, originalContainer);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ADM_FOR_DBMD, status);
}

TEST_F(DlbDolbyEXMLCheckBased, CheckComplementaryObjects)
{
    int status;

    status = ::dlb_adm_container_read_xml_buffer
        (originalContainer
        ,dolbyE_2x20_complementary.c_str()
        ,dolbyE_2x20_complementary.length()
        ,true
        );
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = dlb_adm_core_model_ingest_xml_container(coreModel, originalContainer);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ADM_FOR_DBMD, status);
}


