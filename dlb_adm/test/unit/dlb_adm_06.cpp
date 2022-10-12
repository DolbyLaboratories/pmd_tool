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

/**
 * @file dlb_adm_06.cpp
 * @brief Tests for the C-callable API functions for constructing and traversing
 * the core model; generally:
 *        int dlb_adm_core_model_xxx(dlb_adm_data_names *names, ...)
 *        int dlb_adm_core_model_xxx(dlb_adm_core_model *model, ...)
 */

#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

#include "gtest/gtest.h"

// As much as possible, limit include files to the public C interface of the system,
// keep the C++ out to avoid contaminating the tests.
//
#include "dlb_adm/include/dlb_adm_api.h"
#include "core_model/core_model_defs.h"
#include "CoreModelTest.h"
#include "dlb_adm_data.h"

#include <cstring>
#include <fstream>

#define CHECK_STATUS(s) if (s) return (s)

static const char NAME_1[] = "TheFirstName";
static const char NAME_2[] = "TheSecondName";

static const char LANG_1[] = "en";
static const char LANG_2[] = "eng";
static const char LANG_3[] = "fr";

static const char LABEL_1[] = "TheFirstLabel";
static const char LABEL_2[] = "LabelDeuxieme";

static const char stereoXMLInputFileName[] = "stereo_test_06.xml";
static const char stereoXMLOutputFileName[] = "stereo_test_06.out.xml";
static const char pmdXMLInputFileName[] = "pmd_test_06.xml";

static const dlb_adm_alt_val_count MAX_SADM_ALT_VALUE_SETS = 8;
static const dlb_adm_channel_count MAX_PMD_BED_CHANNELS = 16;
static const dlb_adm_element_count MAX_PMD_PRESENTATION_ELEMENTS = 128;

using namespace DlbAdmTest;

class DlbAdm06 : public testing::Test
{
protected:
    static const unsigned int mNameCount = DlbAdm::DEFAULT_NAME_LIMIT;
    char *mNamesMemory0;
    char *mNamesMemory1;
    char *mAltValSetLabelsMemory;
    dlb_adm_data_names mNames;
    dlb_adm_data_names mAltValSetLabels;

    uint8_t *mElementDataMemory;
    dlb_adm_data_audio_element_data mElementData;

    uint8_t *mPresentationDataMemory;
    dlb_adm_data_presentation_data mPresentationData;

    dlb_adm_xml_container *mXmlContainer;
    dlb_adm_core_model *mCoreModel;

    void SetUpTestInput(const char *fileName, const char *xmlString)
    {
        std::ifstream ifs(fileName);

        if (!ifs.good())
        {
            std::ofstream ofs(fileName);

            ofs << xmlString;
        }
    }

    bool CompareIds(const dlb_adm_entity_id sourceId, const char *idString)
    {
        int status;

        dlb_adm_entity_id idFromString = DLB_ADM_NULL_ENTITY_ID;
        status = ::dlb_adm_read_entity_id(&idFromString, idString, strlen(idString)+1);

        bool ok = status == DLB_ADM_STATUS_OK ? true : false;

        return ok && sourceId == idFromString;
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

    int SetUpNames()
    {
        size_t memorySize;
        int status;

        status = ::dlb_adm_core_model_query_names_memory_size(&memorySize, DLB_ADM_DATA_NAME_SZ, mNameCount);
        if (status == DLB_ADM_STATUS_OK)
        {
            mNamesMemory0 = new char[memorySize];
            status = ::dlb_adm_core_model_configure_names(&mNames, mNameCount, mNamesMemory0, memorySize);
        }

        if(status != DLB_ADM_STATUS_OK)
        {
            return status;
        }

        status = ::dlb_adm_core_model_query_names_memory_size(&memorySize, DLB_ADM_DATA_NAME_SZ, mNameCount);
        if (status == DLB_ADM_STATUS_OK)
        {
            mAltValSetLabelsMemory = new char[memorySize];
            status = ::dlb_adm_core_model_configure_names(&mAltValSetLabels, mNameCount, mAltValSetLabelsMemory, memorySize);
        }

        return status;
    }

    int SetUpTestModel()
    {
        dlb_adm_container_counts containerCounts;
        int status;

        // create the test input file
        SetUpTestInput(stereoXMLInputFileName, stereoXML);

        // open the XML container
        ::memset(&containerCounts, 0, sizeof(containerCounts));
        status = ::dlb_adm_container_open(&mXmlContainer, &containerCounts);
        CHECK_STATUS(status);

        // read the XML input file
        status = dlb_adm_container_read_xml_file(mXmlContainer, stereoXMLInputFileName, DLB_ADM_FALSE);
        CHECK_STATUS(status);

        // load the core model
        status = ::dlb_adm_core_model_open_from_xml_container(&mCoreModel, mXmlContainer);

        return status;
    }

    virtual void SetUp()
    {
        mNamesMemory0 = nullptr;
        mNamesMemory1 = nullptr;
        mAltValSetLabelsMemory = nullptr;
	    mElementDataMemory = nullptr;
        mPresentationDataMemory = nullptr;
        mXmlContainer = nullptr;
        mCoreModel = nullptr;
    }

    virtual void TearDown()
    {
        if (mCoreModel != nullptr)
        {
            (void)::dlb_adm_core_model_close(&mCoreModel);
        }
        if (mXmlContainer != nullptr)
        {
            (void)::dlb_adm_container_close(&mXmlContainer);
        }
        if (mPresentationDataMemory != nullptr)
        {
            delete[] mPresentationDataMemory;
            mPresentationDataMemory = nullptr;
        }
        if (mElementDataMemory != nullptr)
        {
            delete[] mElementDataMemory;
            mElementDataMemory = nullptr;
        }
        if (mNamesMemory1 != nullptr)
        {
            delete[] mNamesMemory1;
            mNamesMemory1 = nullptr;
        }
        if (mNamesMemory0 != nullptr)
        {
            delete[] mNamesMemory0;
            mNamesMemory0 = nullptr;
        }
        if (mAltValSetLabelsMemory != nullptr)
        {
            delete[] mAltValSetLabelsMemory;
            mAltValSetLabelsMemory = nullptr;
        }
    }

};

TEST(dlb_adm_test, NamesQueryMem)
{
    static const size_t nameCount = 10;
    static const size_t maxNameSize = DLB_ADM_DATA_NAME_SZ;
    size_t memorySize;
    int status;

    status = ::dlb_adm_core_model_query_names_memory_size(nullptr, 0, 0);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_query_names_memory_size(&memorySize, 0, 0);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);
    status = ::dlb_adm_core_model_query_names_memory_size(&memorySize, maxNameSize, 0);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);

    memorySize = 0;
    status = ::dlb_adm_core_model_query_names_memory_size(&memorySize, maxNameSize, nameCount);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_LT(0, memorySize);
}

TEST_F(DlbAdm06, Profiles)
{
    int status;
    dlb_adm_bool hasProfile;
    dlb_adm_bool isEmpty;

    status = SetUpTestModel();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_core_model_add_profile(nullptr, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);

    status = ::dlb_adm_core_model_add_profile(mCoreModel, DLB_ADM_PROFILE_NOT_INITIALIZED);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);

    status = ::dlb_adm_core_model_has_profile(nullptr, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_has_profile(mCoreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_has_profile(nullptr, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE, &hasProfile);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);

    status = ::dlb_adm_core_model_has_profile(mCoreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE, &hasProfile);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_TRUE, hasProfile);
    status = ::dlb_adm_core_model_is_empty(mCoreModel, &isEmpty);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_FALSE, isEmpty);

    status = ::dlb_adm_core_model_clear(mCoreModel);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_core_model_has_profile(mCoreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE, &hasProfile);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_FALSE, hasProfile);
    status = ::dlb_adm_core_model_is_empty(mCoreModel, &isEmpty);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_TRUE, isEmpty);


    status = ::dlb_adm_core_model_add_profile(mCoreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_core_model_has_profile(mCoreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE, &hasProfile);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_TRUE, hasProfile);
    status = ::dlb_adm_core_model_is_empty(mCoreModel, &isEmpty);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_FALSE, isEmpty);
}

TEST_F(DlbAdm06, ConfigureNames)
{
    static const size_t nameCount = 10;
    static const size_t maxNameSize = DLB_ADM_DATA_NAME_SZ;
    dlb_adm_data_names names;
    size_t memorySize;
    int status;

    status = ::dlb_adm_core_model_query_names_memory_size(&memorySize, maxNameSize, nameCount);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    mNamesMemory1 = new char[memorySize];
    ASSERT_NE(nullptr, mNamesMemory1);

    status = ::dlb_adm_core_model_configure_names(nullptr, 0, nullptr, 0);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_configure_names(&names, 0, nullptr, 0);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_configure_names(&names, 0, mNamesMemory1, 0);
    EXPECT_EQ(DLB_ADM_STATUS_OUT_OF_RANGE, status);
    status = ::dlb_adm_core_model_configure_names(&names, nameCount, mNamesMemory1, 0);
    EXPECT_EQ(DLB_ADM_STATUS_OUT_OF_RANGE, status);

    status = ::dlb_adm_core_model_configure_names(&names, nameCount, mNamesMemory1, memorySize);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(maxNameSize, names.max_name_size);
}

TEST_F(DlbAdm06, AddName)
{
    int status;

    status = SetUpNames();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_core_model_add_name(nullptr, nullptr, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_add_name(&mNames, nullptr, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);

    status = ::dlb_adm_core_model_add_name(&mNames, NAME_1, LANG_1);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(1, mNames.name_count);
    EXPECT_EQ(0, ::strcmp(NAME_1, mNames.names[0]));
    EXPECT_EQ(0, ::strcmp(LANG_1, mNames.langs[0]));

    status = ::dlb_adm_core_model_add_name(&mNames, NAME_2, LANG_2);
    EXPECT_EQ(DLB_ADM_STATUS_ERROR, status);    // Only one name allowed
    EXPECT_EQ(1, mNames.name_count);
    EXPECT_EQ(0, ::strcmp(NAME_1, mNames.names[0]));
    EXPECT_EQ(0, ::strcmp(LANG_1, mNames.langs[0]));
}

TEST_F(DlbAdm06, AddLabel)
{
    int status;

    status = SetUpNames();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_core_model_add_label(nullptr, nullptr, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_add_label(&mNames, nullptr, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);

    status = ::dlb_adm_core_model_add_name(&mNames, NAME_1, LANG_1);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(1, mNames.name_count);
    EXPECT_EQ(0, mNames.label_count);
    EXPECT_EQ(0, ::strcmp(NAME_1, mNames.names[0]));
    EXPECT_EQ(0, ::strcmp(LANG_1, mNames.langs[0]));

    status = ::dlb_adm_core_model_add_label(&mNames, LABEL_1, LANG_2);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(2, mNames.name_count);
    EXPECT_EQ(1, mNames.label_count);
    EXPECT_EQ(0, ::strcmp(LABEL_1, mNames.names[1]));
    EXPECT_EQ(0, ::strcmp(LANG_2, mNames.langs[1]));

    status = ::dlb_adm_core_model_add_label(&mNames, LABEL_2, LANG_3);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(3, mNames.name_count);
    EXPECT_EQ(2, mNames.label_count);
    EXPECT_EQ(0, ::strcmp(LABEL_2, mNames.names[2]));
    EXPECT_EQ(0, ::strcmp(LANG_3, mNames.langs[2]));
}

TEST_F(DlbAdm06, HasName)
{
    dlb_adm_bool hasName;
    int status;

    status = SetUpNames();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_core_model_has_name(nullptr, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_has_name(&hasName, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);

    hasName = DLB_ADM_TRUE;
    status = ::dlb_adm_core_model_has_name(&hasName, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_FALSE(hasName);

    status = ::dlb_adm_core_model_add_name(&mNames, NAME_1, LANG_1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    hasName = DLB_ADM_FALSE;
    status = ::dlb_adm_core_model_has_name(&hasName, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_TRUE(hasName);
}

TEST_F(DlbAdm06, ClearNames)
{
    int status;

    status = SetUpNames();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_core_model_add_name(&mNames, NAME_1, LANG_1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_core_model_add_label(&mNames, LABEL_1, LANG_2);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_add_label(&mNames, LABEL_2, LANG_3);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    ASSERT_EQ(3, mNames.name_count);
    ASSERT_EQ(2, mNames.label_count);

    status = ::dlb_adm_core_model_clear_names(nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);

    status = ::dlb_adm_core_model_clear_names(&mNames);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(0, mNames.name_count);
    EXPECT_EQ(0, mNames.label_count);

    status = ::dlb_adm_core_model_add_name(&mNames, NAME_1, LANG_1);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(0, ::strcmp(NAME_1, mNames.names[0]));
    EXPECT_EQ(0, ::strcmp(LANG_1, mNames.langs[0]));
}

TEST_F(DlbAdm06, GetNames)
{
    static const char badIdString[] = "APR_1FFF";
    static const char goodIdString[] = "APR_1001";
    dlb_adm_entity_id goodId;
    dlb_adm_entity_id badId;
    int status;

    status = SetUpNames();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = SetUpTestModel();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = dlb_adm_read_entity_id(&badId, badIdString, sizeof(badIdString));
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_core_model_get_names(nullptr, nullptr, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_get_names(mCoreModel, nullptr, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_get_names(mCoreModel, &mNames, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);
    status = ::dlb_adm_core_model_get_names(mCoreModel, &mNames, badId);
    EXPECT_EQ(DLB_ADM_STATUS_ERROR, status);    // APR_1FFF does not exist in the model

    status = dlb_adm_read_entity_id(&goodId, goodIdString, sizeof(goodIdString));
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_core_model_get_names(mCoreModel, &mNames, goodId);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(2, mNames.name_count);
    EXPECT_EQ(0, ::strcmp("English", mNames.names[0]));
    EXPECT_EQ(0, ::strcmp("en", mNames.langs[0]));
    EXPECT_EQ(0, ::strcmp("English", mNames.names[1]));
    EXPECT_EQ(0, ::strcmp("en", mNames.langs[1]));
}

TEST(dlb_adm_test, CoreModelQueryMemBasic)
{
    dlb_adm_core_model_counts counts;
    size_t memSize;
    int status;

    ::memset(&counts, 0, sizeof(counts));
    status = ::dlb_adm_core_model_query_memory_size(NULL, NULL);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_query_memory_size(&memSize, NULL);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);

    memSize = 99;
    status = ::dlb_adm_core_model_query_memory_size(&memSize, &counts);
    EXPECT_EQ(DLB_ADM_STATUS_OUT_OF_MEMORY, status);    // For now!
    EXPECT_EQ(0, memSize);                              // ditto!
}

TEST_F(DlbAdm06, CoreModelOpenBasic)
{
    dlb_adm_core_model_counts counts;
    int status;

    ::memset(&counts, 0, sizeof(counts));
    status = ::dlb_adm_core_model_open(NULL, NULL);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_open(&mCoreModel, NULL);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);

    status = ::dlb_adm_core_model_open(&mCoreModel, &counts);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
}

TEST_F(DlbAdm06, CoreModelOpenFromContainerBasic)
{
    dlb_adm_container_counts containerCounts;
    int status;

    SetUpTestInput(stereoXMLInputFileName, stereoXML);

    // open the XML container
    ::memset(&containerCounts, 0, sizeof(containerCounts));
    status = ::dlb_adm_container_open(&mXmlContainer, &containerCounts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // read the XML input file
    status = dlb_adm_container_read_xml_file(mXmlContainer, stereoXMLInputFileName, DLB_ADM_FALSE);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // null arguments
    status = ::dlb_adm_core_model_open_from_xml_container(nullptr, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_open_from_xml_container(&mCoreModel, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);

    // good to go
    status = ::dlb_adm_core_model_open_from_xml_container(&mCoreModel, mXmlContainer);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // check the results
    status = ::dlb_adm_container_close(&mXmlContainer);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_container_open_from_core_model(&mXmlContainer, mCoreModel);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_container_write_xml_file(mXmlContainer, stereoXMLOutputFileName);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_TRUE(CompareFiles(stereoXMLInputFileName, stereoXMLOutputFileName));
}

TEST_F(DlbAdm06, CoreModelIngestContainerBasic)
{
    dlb_adm_container_counts containerCounts;
    dlb_adm_core_model_counts modelCounts;
    int status;

    SetUpTestInput(stereoXMLInputFileName, stereoXML);

    // open the XML container
    ::memset(&containerCounts, 0, sizeof(containerCounts));
    status = ::dlb_adm_container_open(&mXmlContainer, &containerCounts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // read the XML input file
    status = ::dlb_adm_container_read_xml_file(mXmlContainer, stereoXMLInputFileName, DLB_ADM_FALSE);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // open the core model
    ::memset(&modelCounts, 0, sizeof(modelCounts));
    status = ::dlb_adm_core_model_open(&mCoreModel, &modelCounts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // null arguments
    status = ::dlb_adm_core_model_ingest_xml_container(nullptr, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_ingest_xml_container(mCoreModel, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);

    // good to go
    status = ::dlb_adm_core_model_ingest_xml_container(mCoreModel, mXmlContainer);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // check the results
    status = ::dlb_adm_container_close(&mXmlContainer);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_container_open_from_core_model(&mXmlContainer, mCoreModel);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_container_write_xml_file(mXmlContainer, stereoXMLOutputFileName);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_TRUE(CompareFiles(stereoXMLInputFileName, stereoXMLOutputFileName));
}

TEST(dlb_adm_test, CoreModelCloseBasic)
{
    dlb_adm_core_model_counts counts;
    dlb_adm_core_model *model = nullptr;
    int status;

    ::memset(&counts, 0, sizeof(counts));
    status = ::dlb_adm_core_model_close(NULL);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_close(&model);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);

    status = ::dlb_adm_core_model_open(&model, &counts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    ASSERT_NE(nullptr, model);

    status = ::dlb_adm_core_model_close(&model);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(nullptr, model);
}

TEST_F(DlbAdm06, CoreModelEntityExists)
{
    static const char *presentationIdString = "APR_1001";
    dlb_adm_data_presentation presentation;
    dlb_adm_core_model_counts counts;
    dlb_adm_data_source source;
    dlb_adm_entity_id id;
    dlb_adm_bool exists;
    int status;

    status = SetUpNames();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    ::memset(&counts, 0, sizeof(counts));
    ::memset(&source, 0, sizeof(source));
    ::memset(&presentation, 0, sizeof(presentation));
    status = ::dlb_adm_core_model_open(&mCoreModel, &counts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    ASSERT_NE(nullptr, mCoreModel);

    id = DLB_ADM_NULL_ENTITY_ID;
    status = ::dlb_adm_core_model_entity_exists(nullptr, id, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_entity_exists(mCoreModel, id, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);

    exists = DLB_ADM_TRUE;
    status = ::dlb_adm_core_model_entity_exists(mCoreModel, id, &exists);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);
    EXPECT_FALSE(exists);

    status = dlb_adm_read_entity_id(&id, presentationIdString, ::strlen(presentationIdString));
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    exists = DLB_ADM_TRUE;
    status = ::dlb_adm_core_model_entity_exists(mCoreModel, id, &exists);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_FALSE(exists);

    status = ::dlb_adm_core_model_add_name(&mNames, "Presentation 1", LANG_2);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    presentation.id = id;
    status = ::dlb_adm_core_model_add_presentation(mCoreModel, &presentation, &mNames);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_core_model_entity_exists(mCoreModel, id, &exists);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_TRUE(exists);
}

TEST_F(DlbAdm06, CoreModelAddSource)
{
    dlb_adm_core_model_counts counts;
    dlb_adm_data_source source;
    int status;

    ::memset(&counts, 0, sizeof(counts));
    ::memset(&source, 0, sizeof(source));
    status = ::dlb_adm_core_model_open(&mCoreModel, &counts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    ASSERT_NE(nullptr, mCoreModel);

    status = ::dlb_adm_core_model_add_source(nullptr, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_add_source(mCoreModel, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_add_source(mCoreModel, &source);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);
    source.group_id = 1;
    status = ::dlb_adm_core_model_add_source(mCoreModel, &source);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);

    source.channel = 1;
    status = ::dlb_adm_core_model_add_source(mCoreModel, &source);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_NE(DLB_ADM_NULL_ENTITY_ID, source.id);

    status = ::dlb_adm_core_model_add_source(mCoreModel, &source);
    EXPECT_EQ(DLB_ADM_STATUS_ERROR, status);
}

TEST_F(DlbAdm06, CoreModelAddSources)
{
    static const size_t channelCount = 3;

    dlb_adm_core_model_counts counts;
    dlb_adm_entity_id sourceIDs[channelCount];
    int status;
    size_t i;

    ::memset(&counts, 0, sizeof(counts));
    ::memset(&sourceIDs, 0, sizeof(sourceIDs));
    status = ::dlb_adm_core_model_open(&mCoreModel, &counts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    ASSERT_NE(nullptr, mCoreModel);

    status = ::dlb_adm_core_model_add_sources(nullptr, 0, 0, 0, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_add_sources(mCoreModel, 0, 0, 0, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_add_sources(mCoreModel, 0, 0, 0, sourceIDs);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);
    status = ::dlb_adm_core_model_add_sources(mCoreModel, 1, 0, 0, sourceIDs);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);
    status = ::dlb_adm_core_model_add_sources(mCoreModel, 1, 1, 0, sourceIDs);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);

    status = ::dlb_adm_core_model_add_sources(mCoreModel, 1, 1, channelCount, sourceIDs);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    for (i = 0; i < channelCount; i++)
    {
        EXPECT_NE(DLB_ADM_NULL_ENTITY_ID, sourceIDs[i]);
    }
    for (i = 1; i < channelCount; i++)
    {
        EXPECT_NE(sourceIDs[i - 1], sourceIDs[i]);
    }

    status = ::dlb_adm_core_model_add_sources(mCoreModel, 1, 1, channelCount, sourceIDs);
    EXPECT_EQ(DLB_ADM_STATUS_ERROR, status);
}

TEST_F(DlbAdm06, CoreModelAddSourceGroup)
{
    dlb_adm_core_model_counts counts;
    dlb_adm_data_source_group sourceGroup;
    int status;

    ::memset(&counts, 0, sizeof(counts));
    ::memset(&sourceGroup, 0, sizeof(sourceGroup));
    status = ::dlb_adm_core_model_open(&mCoreModel, &counts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    ASSERT_NE(nullptr, mCoreModel);

    status = ::dlb_adm_core_model_add_source_group(nullptr, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_add_source_group(mCoreModel, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_add_source_group(mCoreModel, &sourceGroup);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status); // Group ID is 0

    sourceGroup.group_id = 1;
    ::strncpy(sourceGroup.name, "Interface 1", sizeof(sourceGroup.name));
    status = ::dlb_adm_core_model_add_source_group(mCoreModel, &sourceGroup);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_core_model_add_source_group(mCoreModel, &sourceGroup);
    EXPECT_EQ(DLB_ADM_STATUS_ERROR, status);    // Duplicate entity id not allowed
    sourceGroup.id = DLB_ADM_NULL_ENTITY_ID;
    status = ::dlb_adm_core_model_add_source_group(mCoreModel, &sourceGroup);
    EXPECT_EQ(DLB_ADM_STATUS_ERROR, status);    // Duplicate group id not allowed

    ::memset(&sourceGroup, 0, sizeof(sourceGroup));
    sourceGroup.group_id = 2;
    status = ::dlb_adm_core_model_add_source_group(mCoreModel, &sourceGroup);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);       // Adding with no name is OK
}

TEST_F(DlbAdm06, CoreModelAddSourceRelation)
{
    static const size_t channelCount = 3;

    dlb_adm_core_model_counts counts;
    dlb_adm_data_audio_track audioTracks[channelCount];
    dlb_adm_entity_id sourceIDs[channelCount];
    dlb_adm_data_source_group sourceGroup;
    int status;
    size_t i;

    ::memset(&counts, 0, sizeof(counts));
    ::memset(&audioTracks, 0, sizeof(audioTracks));
    ::memset(&sourceIDs, 0, sizeof(sourceIDs));
    ::memset(&sourceGroup, 0, sizeof(sourceGroup));
    status = ::dlb_adm_core_model_open(&mCoreModel, &counts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    ASSERT_NE(nullptr, mCoreModel);
    status = ::dlb_adm_core_model_add_sources(mCoreModel, 1, 1, channelCount, sourceIDs);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    sourceGroup.group_id = 1;
    ::strncpy(sourceGroup.name, "Interface 1", sizeof(sourceGroup.name));
    status = ::dlb_adm_core_model_add_source_group(mCoreModel, &sourceGroup);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    for (i = 0; i < channelCount; ++i)
    {
        status = ::dlb_adm_core_model_add_audio_track(mCoreModel, &audioTracks[i]);
        ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    }

    status = ::dlb_adm_core_model_add_source_relation(
        nullptr, DLB_ADM_NULL_ENTITY_ID, DLB_ADM_NULL_ENTITY_ID, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_add_source_relation(mCoreModel,
        DLB_ADM_NULL_ENTITY_ID, DLB_ADM_NULL_ENTITY_ID, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);
    status = ::dlb_adm_core_model_add_source_relation(mCoreModel, sourceGroup.id,
        DLB_ADM_NULL_ENTITY_ID, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);
    status = ::dlb_adm_core_model_add_source_relation(mCoreModel, sourceGroup.id, sourceIDs[0],
        DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);

    for (i = 0; i < channelCount; ++i)
    {
        status = ::dlb_adm_core_model_add_source_relation(
            mCoreModel, sourceGroup.id, sourceIDs[i], audioTracks[i].id);
        EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    }

    status = ::dlb_adm_core_model_add_source_relation(
        mCoreModel, sourceGroup.id, sourceIDs[0], audioTracks[0].id);
    EXPECT_EQ(DLB_ADM_STATUS_ERROR, status);    // Duplicate entry
}

TEST_F(DlbAdm06, CoreModelAddTarget)
{
    dlb_adm_core_model_counts counts;
    dlb_adm_data_target target;
    dlb_adm_entity_id id1, id2;
    int status;

    status = SetUpNames();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    ::memset(&counts, 0, sizeof(counts));
    ::memset(&target, 0, sizeof(target));
    status = ::dlb_adm_core_model_open(&mCoreModel, &counts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    ASSERT_NE(nullptr, mCoreModel);

    status = ::dlb_adm_core_model_add_target(nullptr, nullptr, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_add_target(mCoreModel, &target, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status); // No audio type in target

    status = ::dlb_adm_core_model_add_name(&mNames, "Object_1", nullptr);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    target.audio_type = DLB_ADM_AUDIO_TYPE_MATRIX;
    status = ::dlb_adm_core_model_add_target(mCoreModel, &target, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);
    EXPECT_EQ(DLB_ADM_NULL_ENTITY_ID, target.id);

    target.audio_type = DLB_ADM_AUDIO_TYPE_OBJECTS;
    status = ::dlb_adm_core_model_add_target(mCoreModel, &target, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_NE(DLB_ADM_NULL_ENTITY_ID, target.id);
    id1 = target.id;

    status = ::dlb_adm_core_model_add_target(mCoreModel, &target, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_ERROR, status);    // Duplicate entry not allowed
    EXPECT_EQ(id1, target.id);

    target.id = DLB_ADM_NULL_ENTITY_ID;
    status = ::dlb_adm_core_model_add_target(mCoreModel, &target, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_NE(DLB_ADM_NULL_ENTITY_ID, target.id);
    id2 = target.id;
    EXPECT_NE(id1, id2);
    EXPECT_TRUE(CheckNames(*mCoreModel, id2, mNames));

    target.id = DLB_ADM_NULL_ENTITY_ID;
    status = ::dlb_adm_core_model_clear_names(&mNames);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_add_label(&mNames, LABEL_1, nullptr);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_add_target(mCoreModel, &target, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status); // No labels allowed

    target.audio_type = DLB_ADM_AUDIO_TYPE_DIRECT_SPEAKERS;
    target.speaker_label[0] = 'L';
    status = ::dlb_adm_core_model_clear_names(&mNames);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_add_name(&mNames, "RoomCentric_Left", LANG_1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_add_target(mCoreModel, &target, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_NE(DLB_ADM_NULL_ENTITY_ID, target.id);
    EXPECT_NE(id2, target.id);
    EXPECT_TRUE(CheckNames(*mCoreModel, target.id, mNames));
}

TEST_F(DlbAdm06, CoreModelAddTargetGroup)
{
    dlb_adm_core_model_counts counts;
    dlb_adm_data_target_group targetGroup;
    dlb_adm_entity_id id1, id2, id3;
    int status;

    status = SetUpNames();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    ::memset(&counts, 0, sizeof(counts));
    ::memset(&targetGroup, 0, sizeof(targetGroup));
    status = ::dlb_adm_core_model_open(&mCoreModel, &counts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    ASSERT_NE(nullptr, mCoreModel);

    status = ::dlb_adm_core_model_add_target_group(nullptr, nullptr, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_add_target_group(mCoreModel, nullptr, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_add_target_group(mCoreModel, &targetGroup, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);

    status = ::dlb_adm_core_model_add_target_group(mCoreModel, &targetGroup, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status); // No speaker config or audio type in target group

    targetGroup.speaker_config = DLB_ADM_SPEAKER_CONFIG_2_0;
    targetGroup.audio_type = DLB_ADM_AUDIO_TYPE_OBJECTS;
    status = ::dlb_adm_core_model_add_target_group(mCoreModel, &targetGroup, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status); // Can't set both speaker config or audio type

    targetGroup.audio_type = DLB_ADM_AUDIO_TYPE_NONE;
    status = ::dlb_adm_core_model_add_target_group(mCoreModel, &targetGroup, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status); // Name is required

    status = ::dlb_adm_core_model_add_name(&mNames, "Stereo_Bed", nullptr);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    targetGroup.speaker_config = DLB_ADM_SPEAKER_CONFIG_COUNT;
    status = ::dlb_adm_core_model_add_target_group(mCoreModel, &targetGroup, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status); // Invalid speaker config

    targetGroup.speaker_config = DLB_ADM_SPEAKER_CONFIG_2_0;
    status = ::dlb_adm_core_model_add_target_group(mCoreModel, &targetGroup, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_NE(DLB_ADM_NULL_ENTITY_ID, targetGroup.id);
    id1 = targetGroup.id;
    EXPECT_TRUE(CheckNames(*mCoreModel, id1, mNames));

    status = ::dlb_adm_core_model_add_target_group(mCoreModel, &targetGroup, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_ERROR, status);    // Duplicate entry not allowed
    EXPECT_EQ(id1, targetGroup.id);

    targetGroup.id = DLB_ADM_NULL_ENTITY_ID;
    status = ::dlb_adm_core_model_add_target_group(mCoreModel, &targetGroup, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_NE(DLB_ADM_NULL_ENTITY_ID, targetGroup.id);
    id2 = targetGroup.id;
    EXPECT_NE(id1, id2);
    EXPECT_TRUE(CheckNames(*mCoreModel, id2, mNames));

    targetGroup.id = DLB_ADM_NULL_ENTITY_ID;
    status = ::dlb_adm_core_model_clear_names(&mNames);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_add_name(&mNames, "Stereo_Bed", nullptr);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_add_label(&mNames, LABEL_1, nullptr);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_add_target_group(mCoreModel, &targetGroup, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status); // No labels allowed

    ::memset(&targetGroup, 0, sizeof(targetGroup));
    status = ::dlb_adm_core_model_clear_names(&mNames);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_add_name(&mNames, "Object_1", LANG_1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    targetGroup.audio_type = DLB_ADM_AUDIO_TYPE_LAST_CUSTOM;
    status = ::dlb_adm_core_model_add_target_group(mCoreModel, &targetGroup, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status); // Invalid object class

    targetGroup.audio_type = DLB_ADM_AUDIO_TYPE_OBJECTS;
    status = ::dlb_adm_core_model_add_target_group(mCoreModel, &targetGroup, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    id3 = targetGroup.id;
    EXPECT_NE(id2, id3);
    EXPECT_TRUE(CheckNames(*mCoreModel, id3, mNames));

    status = ::dlb_adm_core_model_add_target_group(mCoreModel, &targetGroup, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_ERROR, status);    // Duplicate entry not allowed
    EXPECT_EQ(id3, targetGroup.id);
}

TEST_F(DlbAdm06, CoreModelAddAudioTrack)
{
    dlb_adm_core_model_counts counts;
    dlb_adm_data_audio_track audioTrack;
    dlb_adm_entity_id id1, id2;
    int status;

    ::memset(&counts, 0, sizeof(counts));
    ::memset(&audioTrack, 0, sizeof(audioTrack));
    status = ::dlb_adm_core_model_open(&mCoreModel, &counts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    ASSERT_NE(nullptr, mCoreModel);

    status = ::dlb_adm_core_model_add_audio_track(nullptr, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_add_audio_track(mCoreModel, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);

    status = ::dlb_adm_core_model_add_audio_track(mCoreModel, &audioTrack);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_NE(DLB_ADM_NULL_ENTITY_ID, audioTrack.id);
    id1 = audioTrack.id;

    status = ::dlb_adm_core_model_add_audio_track(mCoreModel, &audioTrack);
    EXPECT_EQ(DLB_ADM_STATUS_ERROR, status);    // Duplicate entry not allowed
    EXPECT_EQ(id1, audioTrack.id);

    audioTrack.id = DLB_ADM_NULL_ENTITY_ID;
    status = ::dlb_adm_core_model_add_audio_track(mCoreModel, &audioTrack);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_NE(DLB_ADM_NULL_ENTITY_ID, audioTrack.id);
    id2 = audioTrack.id;
    EXPECT_NE(id1, id2);
}

TEST_F(DlbAdm06, CoreModelAddAudioElement)
{
    dlb_adm_core_model_counts counts;
    dlb_adm_data_audio_element audioElement;
    dlb_adm_entity_id id1, id2, id3;
    int status;

    status = SetUpNames();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    ::memset(&counts, 0, sizeof(counts));
    ::memset(&audioElement, 0, sizeof(audioElement));
    status = ::dlb_adm_core_model_open(&mCoreModel, &counts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    ASSERT_NE(nullptr, mCoreModel);

    status = ::dlb_adm_core_model_add_audio_element(nullptr, nullptr, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_add_audio_element(mCoreModel, nullptr, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_add_audio_element(mCoreModel, &audioElement, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);

    audioElement.gain.gain_unit = DLB_ADM_GAIN_UNIT_COUNT;
    status = ::dlb_adm_core_model_add_audio_element(mCoreModel, &audioElement, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);

    audioElement.gain.gain_unit = DLB_ADM_GAIN_UNIT_DB;
    status = ::dlb_adm_core_model_add_audio_element(mCoreModel, &audioElement, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);     // Name is required

    status = ::dlb_adm_core_model_add_name(&mNames, "Audio Element 1", nullptr);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_core_model_add_audio_element(mCoreModel, &audioElement, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_NE(DLB_ADM_NULL_ENTITY_ID, audioElement.id);
    EXPECT_TRUE(CompareIds(audioElement.id, "AO_1001"));
    id1 = audioElement.id;
    EXPECT_TRUE(CheckNames(*mCoreModel, id1, mNames));

    status = ::dlb_adm_core_model_add_audio_element(mCoreModel, &audioElement, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_ERROR, status);    // Duplicate entry not allowed
    EXPECT_EQ(id1, audioElement.id);

    audioElement.id = DLB_ADM_NULL_ENTITY_ID;
    status = ::dlb_adm_core_model_add_audio_element(mCoreModel, &audioElement, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_NE(DLB_ADM_NULL_ENTITY_ID, audioElement.id);
    EXPECT_TRUE(CompareIds(audioElement.id, "AO_1002"));
    id2 = audioElement.id;
    EXPECT_NE(id1, id2);
    EXPECT_TRUE(CheckNames(*mCoreModel, id2, mNames));

    status = ::dlb_adm_core_model_add_label(&mNames, LABEL_1, LANG_1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_add_label(&mNames, LABEL_2, LANG_3);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    audioElement.id = DLB_ADM_NULL_ENTITY_ID;
    status = ::dlb_adm_core_model_add_audio_element(mCoreModel, &audioElement, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_NE(DLB_ADM_NULL_ENTITY_ID, audioElement.id);
    EXPECT_TRUE(CompareIds(audioElement.id, "AO_1003"));
    id3 = audioElement.id;
    EXPECT_NE(id2, id3);
    EXPECT_TRUE(CheckNames(*mCoreModel, id3, mNames));
}

TEST_F(DlbAdm06, CoreModelAddElementGroup)
{
    dlb_adm_core_model_counts counts;
    dlb_adm_data_element_group elementGroup;
    dlb_adm_entity_id id1, id2, id3;
    int status;

    status = SetUpNames();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    ::memset(&counts, 0, sizeof(counts));
    ::memset(&elementGroup, 0, sizeof(elementGroup));
    status = ::dlb_adm_core_model_open(&mCoreModel, &counts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    ASSERT_NE(nullptr, mCoreModel);

    status = ::dlb_adm_core_model_add_element_group(nullptr, nullptr, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_add_element_group(mCoreModel, &elementGroup, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);

    elementGroup.gain.gain_unit = DLB_ADM_GAIN_UNIT_COUNT;
    status = ::dlb_adm_core_model_add_element_group(mCoreModel, &elementGroup, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);

    elementGroup.gain.gain_unit = DLB_ADM_GAIN_UNIT_DB;
    status = ::dlb_adm_core_model_add_element_group(mCoreModel, &elementGroup, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);     // Name is required

    status = ::dlb_adm_core_model_add_name(&mNames, "Element Group 1", nullptr);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_core_model_add_element_group(mCoreModel, &elementGroup, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_NE(DLB_ADM_NULL_ENTITY_ID, elementGroup.id);
    EXPECT_TRUE(CompareIds(elementGroup.id, "AO_1001"));
    id1 = elementGroup.id;
    EXPECT_TRUE(CheckNames(*mCoreModel, id1, mNames));

    status = ::dlb_adm_core_model_add_element_group(mCoreModel, &elementGroup, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_ERROR, status);    // Duplicate entry not allowed
    EXPECT_EQ(id1, elementGroup.id);

    elementGroup.id = DLB_ADM_NULL_ENTITY_ID;
    status = ::dlb_adm_core_model_add_element_group(mCoreModel, &elementGroup, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_NE(DLB_ADM_NULL_ENTITY_ID, elementGroup.id);
    EXPECT_TRUE(CompareIds(elementGroup.id, "AO_1002"));
    id2 = elementGroup.id;
    EXPECT_NE(id1, id2);
    EXPECT_TRUE(CheckNames(*mCoreModel, id2, mNames));

    status = ::dlb_adm_core_model_add_label(&mNames, LABEL_1, LANG_1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_add_label(&mNames, LABEL_2, LANG_3);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    elementGroup.id = DLB_ADM_NULL_ENTITY_ID;
    status = ::dlb_adm_core_model_add_element_group(mCoreModel, &elementGroup, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_NE(DLB_ADM_NULL_ENTITY_ID, elementGroup.id);
    EXPECT_TRUE(CompareIds(elementGroup.id, "AO_1003"));
    id3 = elementGroup.id;
    EXPECT_NE(id2, id3);
    EXPECT_TRUE(CheckNames(*mCoreModel, id3, mNames));
}

TEST_F(DlbAdm06, CoreModelAddAlternativeValueSet)
{

    const char * avsIdString1 = "AVS_1001_0001";
    const char * avsIdString2 = "AVS_1001_0002";
    const char * objectIdString = "AO_1001";
    const char * badParentIdString = "APR_1001";

    dlb_adm_core_model_counts counts;
    dlb_adm_data_audio_element audioElement;
    dlb_adm_data_presentation badParent;
    dlb_adm_data_alt_value_set altValSet;

    dlb_adm_entity_id id1, id2;
    dlb_adm_entity_id objectId;
    dlb_adm_entity_id badParentId;

    int status;

    status = SetUpNames();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    ::memset(&counts, 0, sizeof(counts));
    ::memset(&audioElement, 0, sizeof(audioElement));
    ::memset(&altValSet, 0, sizeof(altValSet));
    status = ::dlb_adm_core_model_open(&mCoreModel, &counts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    ASSERT_NE(nullptr, mCoreModel);

    status = ::dlb_adm_read_entity_id(&id1, avsIdString1, ::strlen(avsIdString1)+1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_NE(id1, DLB_ADM_NULL_ENTITY_ID);
    status = ::dlb_adm_read_entity_id(&id2, avsIdString2, ::strlen(avsIdString2)+1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_NE(id2, DLB_ADM_NULL_ENTITY_ID);

    // set up bad parent
    status = ::dlb_adm_read_entity_id(&badParentId, badParentIdString, ::strlen(badParentIdString)+1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_NE(badParentId, DLB_ADM_NULL_ENTITY_ID);
    status = ::dlb_adm_read_entity_id(&badParent.id, badParentIdString, ::strlen(badParentIdString));
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_add_name(&mNames, "Presentation 1", LANG_1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_add_presentation(mCoreModel, &badParent, &mNames);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_clear_names(&mNames);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // status "NULL_POINTER"
    status = ::dlb_adm_core_model_add_alt_value_set(nullptr, DLB_ADM_NULL_ENTITY_ID, nullptr, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_add_alt_value_set(mCoreModel, DLB_ADM_NULL_ENTITY_ID, nullptr, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_add_alt_value_set(mCoreModel, DLB_ADM_NULL_ENTITY_ID, &altValSet, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);

    // wrong parent type
    status = ::dlb_adm_core_model_add_alt_value_set(mCoreModel, badParentId, &altValSet, &mAltValSetLabels);
    EXPECT_EQ(DLB_ADM_STATUS_ERROR, status);

    // Id of parent not in Core Model
    status = ::dlb_adm_read_entity_id(&objectId, objectIdString, ::strlen(objectIdString)+1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_NE(objectId, DLB_ADM_NULL_ENTITY_ID);
    status = ::dlb_adm_core_model_add_alt_value_set(mCoreModel, objectId, &altValSet, &mAltValSetLabels);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);

    // null_id for parent and avs
    objectId = DLB_ADM_NULL_ENTITY_ID;
    status = ::dlb_adm_core_model_add_alt_value_set(mCoreModel, objectId, &altValSet, &mAltValSetLabels);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);

    // set up correct parent audio element
    status = ::dlb_adm_core_model_add_name(&mNames, "Audio Element 1", nullptr);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_add_audio_element(mCoreModel, &audioElement, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    objectId = audioElement.id;

    // both parent and avs has ID
    altValSet.id = id1;
    status = ::dlb_adm_core_model_add_alt_value_set(mCoreModel, objectId, &altValSet, &mAltValSetLabels);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);
    altValSet.id = DLB_ADM_NULL_ENTITY_ID;

    // invalid gain
    altValSet.has_gain = DLB_ADM_TRUE;
    altValSet.gain.gain_unit = DLB_ADM_GAIN_UNIT_COUNT;
    status = ::dlb_adm_core_model_add_alt_value_set(mCoreModel, objectId, &altValSet, &mAltValSetLabels);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);
    altValSet.has_gain = DLB_ADM_FALSE;
    altValSet.gain.gain_unit = DLB_ADM_GAIN_UNIT_LINEAR;

    // AVS can't have name
    status = ::dlb_adm_core_model_add_name(&mAltValSetLabels, "AVS name", nullptr);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_add_alt_value_set(mCoreModel, objectId, &altValSet, &mAltValSetLabels);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);
    status = ::dlb_adm_core_model_clear_names(&mAltValSetLabels);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // correct avs
    status = ::dlb_adm_core_model_add_alt_value_set(mCoreModel, objectId, &altValSet, &mAltValSetLabels);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(id1, altValSet.id);
    EXPECT_TRUE(CheckNames(*mCoreModel, id1, mAltValSetLabels));

    // duplicate entry not allowed
    status = ::dlb_adm_core_model_add_alt_value_set(mCoreModel, DLB_ADM_NULL_ENTITY_ID, &altValSet, &mAltValSetLabels);
    EXPECT_EQ(DLB_ADM_STATUS_ERROR, status);
    EXPECT_EQ(id1, altValSet.id);

    // avs with labels
    altValSet.id = id2;
    status = ::dlb_adm_core_model_add_label(&mAltValSetLabels, LABEL_1, LANG_1);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_add_label(&mAltValSetLabels, LABEL_2, LANG_2);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_add_alt_value_set(mCoreModel, DLB_ADM_NULL_ENTITY_ID, &altValSet, &mAltValSetLabels);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(id2, altValSet.id);
    EXPECT_EQ(id2, id1 + 1u);
    EXPECT_TRUE(CheckNames(*mCoreModel, id2, mAltValSetLabels));
}


TEST_F(DlbAdm06, CoreModelAddComplementaryObjects)
{
    const char * objectIdString = "AO_1001";
    const char * ComplementaryLeaderdString = "AO_1002";

    dlb_adm_core_model_counts counts;
    dlb_adm_data_complementary_element comp_element;
    dlb_adm_data_complementary_element comp_leader_element;
    dlb_adm_data_audio_element audioElement;
    dlb_adm_data_audio_element audioElementLeader;

    dlb_adm_entity_id id1, id2;
    dlb_adm_entity_id objectId;
    dlb_adm_entity_id ComplementaryLeaderId;

    int status;

    status = SetUpNames();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    ::memset(&counts, 0, sizeof(counts));
    ::memset(&comp_element, 0, sizeof(comp_element));
    ::memset(&comp_leader_element, 0, sizeof(comp_leader_element));
    ::memset(&audioElement, 0, sizeof(audioElement));
    ::memset(&audioElementLeader, 0, sizeof(audioElementLeader));
    status = ::dlb_adm_core_model_open(&mCoreModel, &counts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    ASSERT_NE(nullptr, mCoreModel);

    status = ::dlb_adm_read_entity_id(&objectId, objectIdString, ::strlen(objectIdString) + 1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_NE(objectId, DLB_ADM_NULL_ENTITY_ID);
    status = ::dlb_adm_read_entity_id(&ComplementaryLeaderId, ComplementaryLeaderdString, ::strlen(ComplementaryLeaderdString) + 1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_NE(ComplementaryLeaderId, DLB_ADM_NULL_ENTITY_ID);

    comp_element.audio_element_id = objectId;
    comp_element.complementary_leader_id = ComplementaryLeaderId;

    // status "NULL_POINTER"
    status = ::dlb_adm_core_model_add_complementary_element(nullptr, DLB_ADM_NULL_ENTITY_ID, 1, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_add_complementary_element(mCoreModel, DLB_ADM_NULL_ENTITY_ID, 1, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_add_complementary_element(mCoreModel, &comp_element, 0, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);

    // Object not in CoreModel
    status = ::dlb_adm_core_model_add_complementary_element(mCoreModel, &comp_element, 1, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);

    // Compelementary Leader not in CoreModel
    status = ::dlb_adm_core_model_add_name(&mNames, "Audio Element 1", nullptr);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_add_audio_element(mCoreModel, &audioElement, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    comp_element.audio_element_id = audioElement.id;
    status = ::dlb_adm_core_model_clear_names(&mNames);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_core_model_add_complementary_element(mCoreModel, &comp_element, 1, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);

    status = ::dlb_adm_core_model_add_name(&mNames, "Audio Element 2", nullptr);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_add_audio_element(mCoreModel, &audioElementLeader, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_clear_names(&mNames);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    comp_element.complementary_leader_id = audioElementLeader.id;
    comp_leader_element.audio_element_id = audioElementLeader.id;
    comp_leader_element.complementary_leader_id = audioElementLeader.id;

    status = ::dlb_adm_core_model_add_label(&mNames, LABEL_1, LANG_1);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_add_label(&mNames, LABEL_2, LANG_2);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_add_complementary_element(mCoreModel, &comp_element, 1, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_add_complementary_element(mCoreModel, &comp_leader_element, 2, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_FALSE(CheckNames(*mCoreModel, comp_element.id, mNames));
    EXPECT_TRUE(CheckNames(*mCoreModel, comp_leader_element.id, mNames));
    EXPECT_EQ(comp_leader_element.id, comp_element.id + 1);

    // duplicate entry not allowed
    status = ::dlb_adm_core_model_add_complementary_element(mCoreModel, &comp_element, 1, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_ERROR, status);
}

TEST_F(DlbAdm06, CoreModelAddContentGroup)
{
    dlb_adm_core_model_counts counts;
    dlb_adm_data_content_group contentGroup;
    dlb_adm_entity_id id1, id2, id3;
    int status;

    status = SetUpNames();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    ::memset(&counts, 0, sizeof(counts));
    ::memset(&contentGroup, 0, sizeof(contentGroup));

    const dlb_adm_data_loudness sourceLoudness = {3.5, DLB_ADM_LOUDNESS_TYPE_INTEGRATED};
    contentGroup.loudness = sourceLoudness;

    status = ::dlb_adm_core_model_open(&mCoreModel, &counts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    ASSERT_NE(nullptr, mCoreModel);

    status = ::dlb_adm_core_model_add_content_group(nullptr, nullptr, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_add_content_group(mCoreModel, nullptr, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_add_content_group(mCoreModel, &contentGroup, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);

    contentGroup.content_kind = static_cast<DLB_ADM_CONTENT_KIND>(255);
    status = ::dlb_adm_core_model_add_content_group(mCoreModel, &contentGroup, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);

    contentGroup.content_kind = DLB_ADM_CONTENT_KIND_MK_COMPLETE_MAIN;
    status = ::dlb_adm_core_model_add_content_group(mCoreModel, &contentGroup, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);     // Name is required

    status = ::dlb_adm_core_model_add_name(&mNames, "Content Group 1", LANG_1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_core_model_add_content_group(mCoreModel, &contentGroup, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_NE(DLB_ADM_NULL_ENTITY_ID, contentGroup.id);
    EXPECT_TRUE(CompareIds(contentGroup.id, "ACO_1001"));
    id1 = contentGroup.id;
    EXPECT_TRUE(CheckNames(*mCoreModel, id1, mNames));
    EXPECT_TRUE(CheckLoudnessMetadata(*mCoreModel, id1, sourceLoudness, DLB_ADM_ENTITY_TYPE_CONTENT));

    status = ::dlb_adm_core_model_add_content_group(mCoreModel, &contentGroup, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_ERROR, status);    // Duplicate entry not allowed
    EXPECT_EQ(id1, contentGroup.id);

    contentGroup.id = DLB_ADM_NULL_ENTITY_ID;
    status = ::dlb_adm_core_model_add_content_group(mCoreModel, &contentGroup, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_NE(DLB_ADM_NULL_ENTITY_ID, contentGroup.id);
    EXPECT_TRUE(CompareIds(contentGroup.id, "ACO_1002"));
    id2 = contentGroup.id;
    EXPECT_NE(id1, id2);
    EXPECT_TRUE(CheckNames(*mCoreModel, id2, mNames));
    EXPECT_TRUE(CheckLoudnessMetadata(*mCoreModel, id2, sourceLoudness, DLB_ADM_ENTITY_TYPE_CONTENT));

    status = ::dlb_adm_core_model_add_label(&mNames, LABEL_1, LANG_1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_add_label(&mNames, LABEL_2, LANG_3);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    contentGroup.id = DLB_ADM_NULL_ENTITY_ID;
    status = ::dlb_adm_core_model_add_content_group(mCoreModel, &contentGroup, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_NE(DLB_ADM_NULL_ENTITY_ID, contentGroup.id);
    EXPECT_TRUE(CompareIds(contentGroup.id, "ACO_1003"));
    id3 = contentGroup.id;
    EXPECT_NE(id2, id3);
    EXPECT_TRUE(CheckNames(*mCoreModel, id3, mNames));
    EXPECT_TRUE(CheckLoudnessMetadata(*mCoreModel, id3, sourceLoudness, DLB_ADM_ENTITY_TYPE_CONTENT));
}

TEST_F(DlbAdm06, CoreModelAddPresentation)
{
    dlb_adm_core_model_counts counts;
    dlb_adm_data_presentation presentation;
    dlb_adm_entity_id id1, id2, id3;
    int status;

    status = SetUpNames();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    ::memset(&counts, 0, sizeof(counts));
    ::memset(&presentation, 0, sizeof(presentation));

    const dlb_adm_data_loudness sourceLoudness = {2.0, DLB_ADM_LOUDNESS_TYPE_INTEGRATED};
    presentation.loudness = sourceLoudness;

    status = ::dlb_adm_core_model_open(&mCoreModel, &counts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    ASSERT_NE(nullptr, mCoreModel);

    status = ::dlb_adm_core_model_add_presentation(nullptr, nullptr, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_add_presentation(mCoreModel, nullptr, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_add_presentation(mCoreModel, &presentation, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);

    status = ::dlb_adm_core_model_add_presentation(mCoreModel, &presentation, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);     // Name is required

    status = ::dlb_adm_core_model_add_name(&mNames, "Presentation 1", LANG_2);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_core_model_add_presentation(mCoreModel, &presentation, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_NE(DLB_ADM_NULL_ENTITY_ID, presentation.id);
    EXPECT_TRUE(CompareIds(presentation.id, "APR_1001"));
    id1 = presentation.id;
    EXPECT_TRUE(CheckNames(*mCoreModel, id1, mNames));
    EXPECT_TRUE(CheckLoudnessMetadata(*mCoreModel, id1, sourceLoudness, DLB_ADM_ENTITY_TYPE_PROGRAMME));

    status = ::dlb_adm_core_model_add_presentation(mCoreModel, &presentation, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_ERROR, status);    // Duplicate entry not allowed
    EXPECT_EQ(id1, presentation.id);

    presentation.id = DLB_ADM_NULL_ENTITY_ID;
    status = ::dlb_adm_core_model_add_presentation(mCoreModel, &presentation, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_NE(DLB_ADM_NULL_ENTITY_ID, presentation.id);
    EXPECT_TRUE(CompareIds(presentation.id, "APR_1002"));
    id2 = presentation.id;
    EXPECT_NE(id1, id2);
    EXPECT_TRUE(CheckNames(*mCoreModel, id2, mNames));
    EXPECT_TRUE(CheckLoudnessMetadata(*mCoreModel, id2, sourceLoudness, DLB_ADM_ENTITY_TYPE_PROGRAMME));

    status = ::dlb_adm_core_model_add_label(&mNames, LABEL_1, LANG_1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_add_label(&mNames, LABEL_2, LANG_3);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    presentation.id = DLB_ADM_NULL_ENTITY_ID;
    status = ::dlb_adm_core_model_add_presentation(mCoreModel, &presentation, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_NE(DLB_ADM_NULL_ENTITY_ID, presentation.id);
    EXPECT_TRUE(CompareIds(presentation.id, "APR_1003"));
    id3 = presentation.id;
    EXPECT_NE(id2, id3);
    EXPECT_TRUE(CheckNames(*mCoreModel, id3, mNames));
    EXPECT_TRUE(CheckLoudnessMetadata(*mCoreModel, id3, sourceLoudness, DLB_ADM_ENTITY_TYPE_PROGRAMME));
}

TEST_F(DlbAdm06, CoreModelAddElementRecord)
{
    dlb_adm_core_model_counts counts;
    dlb_adm_data_audio_element audioElement;
    dlb_adm_data_audio_track audioTrack;
    dlb_adm_data_target_group targetGroup;
    dlb_adm_data_target target;
    dlb_adm_data_source source;
    int status;

    status = SetUpNames();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    ::memset(&counts, 0, sizeof(counts));
    ::memset(&audioElement, 0, sizeof(audioElement));
    ::memset(&audioTrack, 0, sizeof(audioTrack));
    ::memset(&targetGroup, 0, sizeof(targetGroup));
    ::memset(&target, 0, sizeof(target));
    ::memset(&source, 0, sizeof(source));
    status = ::dlb_adm_core_model_open(&mCoreModel, &counts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    ASSERT_NE(nullptr, mCoreModel);

    // Add some entities to the model

    status = ::dlb_adm_core_model_add_name(&mNames, "Audio Element 1", LANG_1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    audioElement.gain.gain_unit = DLB_ADM_GAIN_UNIT_DB;
    status = ::dlb_adm_core_model_add_audio_element(mCoreModel, &audioElement, &mNames);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_clear_names(&mNames);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_core_model_add_audio_track(mCoreModel, &audioTrack);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    targetGroup.speaker_config = DLB_ADM_SPEAKER_CONFIG_2_0;
    status = ::dlb_adm_core_model_add_name(&mNames, "Stereo_Bed", LANG_1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_add_target_group(mCoreModel, &targetGroup, &mNames);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_clear_names(&mNames);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    target.audio_type = DLB_ADM_AUDIO_TYPE_DIRECT_SPEAKERS;
    target.speaker_label[0] = 'L';
    status = ::dlb_adm_core_model_add_name(&mNames, "RoomCentric_Left", LANG_1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_add_target(mCoreModel, &target, &mNames);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_clear_names(&mNames);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    source.group_id = 1;
    source.channel = 1;
    status = ::dlb_adm_core_model_add_source(mCoreModel, &source);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // Null pointer/invalid argument

    status = ::dlb_adm_core_model_add_element_relation(
        nullptr, DLB_ADM_NULL_ENTITY_ID, DLB_ADM_NULL_ENTITY_ID, DLB_ADM_NULL_ENTITY_ID, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_add_element_relation(mCoreModel,
        DLB_ADM_NULL_ENTITY_ID, DLB_ADM_NULL_ENTITY_ID, DLB_ADM_NULL_ENTITY_ID, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);
    status = ::dlb_adm_core_model_add_element_relation(mCoreModel, audioElement.id,
        DLB_ADM_NULL_ENTITY_ID, DLB_ADM_NULL_ENTITY_ID, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);
    status = ::dlb_adm_core_model_add_element_relation(mCoreModel, audioElement.id, targetGroup.id,
        DLB_ADM_NULL_ENTITY_ID, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);
    status = ::dlb_adm_core_model_add_element_relation(mCoreModel, audioElement.id, targetGroup.id, target.id,
        DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);
    status = ::dlb_adm_core_model_add_element_relation(mCoreModel, audioElement.id, target.id, targetGroup.id, audioTrack.id);
    EXPECT_EQ(DLB_ADM_STATUS_ERROR, status);    // Reversed targetGroup and target (wrong id types)

    // Test adding element relations --

    // Add the left channel of a stereo pair
    status = ::dlb_adm_core_model_add_element_relation(mCoreModel, audioElement.id, targetGroup.id, target.id, audioTrack.id);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_add_element_relation(mCoreModel, audioElement.id, targetGroup.id, target.id, audioTrack.id);
    EXPECT_EQ(DLB_ADM_STATUS_ERROR, status);    // No duplicates allowed

    // Now add the right channel
    target.id = DLB_ADM_NULL_ENTITY_ID;
    target.speaker_label[0] = 'R';
    status = ::dlb_adm_core_model_add_name(&mNames, "RoomCentric_Right", LANG_1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_add_target(mCoreModel, &target, &mNames);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_clear_names(&mNames);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    source.id = DLB_ADM_NULL_ENTITY_ID;
    source.channel = 2;
    status = ::dlb_adm_core_model_add_source(mCoreModel, &source);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_core_model_add_element_relation(mCoreModel, audioElement.id, targetGroup.id, target.id, audioTrack.id);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
}

TEST_F(DlbAdm06, CoreModelAddBlockUpdate)
{
    static const char *parentIdString = "AC_00031001";
    static const char *updateIdString = "AB_00031001_00000002";
    static const char *badParentIdString = "APR_1001";
    dlb_adm_entity_id parentId;
    dlb_adm_entity_id updateId;
    dlb_adm_entity_id id1;
    dlb_adm_entity_id id2;
    dlb_adm_core_model_counts counts;
    dlb_adm_data_target_group parentGroup;
    dlb_adm_data_block_update blockUpdate;
    dlb_adm_data_presentation badParent;
    int status;

    status = SetUpNames();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    ::memset(&counts, 0, sizeof(counts));
    ::memset(&parentGroup, 0, sizeof(parentGroup));
    ::memset(&blockUpdate, 0, sizeof(blockUpdate));
    ::memset(&badParent, 0, sizeof(badParent));
    status = ::dlb_adm_core_model_open(&mCoreModel, &counts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    ASSERT_NE(nullptr, mCoreModel);
    status = ::dlb_adm_read_entity_id(&parentId, parentIdString, ::strlen(parentIdString));
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_read_entity_id(&updateId, updateIdString, ::strlen(updateIdString));
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_read_entity_id(&badParent.id, badParentIdString, ::strlen(badParentIdString));
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_add_name(&mNames, "Presentation 1", LANG_1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_add_presentation(mCoreModel, &badParent, &mNames);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_clear_names(&mNames);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_core_model_add_block_update(nullptr, DLB_ADM_NULL_ENTITY_ID, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_add_block_update(mCoreModel, DLB_ADM_NULL_ENTITY_ID, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);

    status = ::dlb_adm_core_model_add_block_update(mCoreModel, DLB_ADM_NULL_ENTITY_ID, &blockUpdate);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);

    blockUpdate.id = updateId;
    status = ::dlb_adm_core_model_add_block_update(mCoreModel, parentId, &blockUpdate);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);     // both parentId and blockUpdate.id are set

    blockUpdate.id = DLB_ADM_NULL_ENTITY_ID;
    status = ::dlb_adm_core_model_add_block_update(mCoreModel, parentId, &blockUpdate);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);     // parent does not exist

    status = ::dlb_adm_core_model_add_block_update(mCoreModel, badParent.id, &blockUpdate);
    EXPECT_EQ(DLB_ADM_STATUS_ERROR, status);                // parent type is incorrect

    status = ::dlb_adm_core_model_add_name(&mNames, "Object_1", LANG_1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    parentGroup.id = parentId;
    parentGroup.audio_type = DLB_ADM_AUDIO_TYPE_OBJECTS;
    status = ::dlb_adm_core_model_add_target_group(mCoreModel, &parentGroup, &mNames);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    ASSERT_EQ(parentId, parentGroup.id);

    status = ::dlb_adm_core_model_add_block_update(mCoreModel, parentId, &blockUpdate);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    id1 = blockUpdate.id;
    EXPECT_NE(DLB_ADM_NULL_ENTITY_ID, id1);

    blockUpdate.id = updateId;
    status = ::dlb_adm_core_model_add_block_update(mCoreModel, DLB_ADM_NULL_ENTITY_ID, &blockUpdate);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    id2 = blockUpdate.id;
    EXPECT_NE(DLB_ADM_NULL_ENTITY_ID, id2);
    EXPECT_EQ(id2, id1 + 1u);
}

TEST_F(DlbAdm06, CoreModelAddPresentationRecord)
{
    dlb_adm_core_model_counts counts;
    dlb_adm_data_presentation presentation1;
    dlb_adm_data_presentation presentation2;
    dlb_adm_data_content_group contentGroup1;
    dlb_adm_data_content_group contentGroup2;
    dlb_adm_data_content_group contentGroup3;
    dlb_adm_data_audio_element audioElement1;
    dlb_adm_data_audio_element audioElement2;
    dlb_adm_data_audio_element audioElement3;
    dlb_adm_data_alt_value_set altValueSet1;
    dlb_adm_data_complementary_element compElement1;
    dlb_adm_data_complementary_element compElement2;
    int status;

    status = SetUpNames();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    ::memset(&counts, 0, sizeof(counts));
    ::memset(&presentation1, 0, sizeof(presentation1));
    ::memset(&presentation2, 0, sizeof(presentation2));
    ::memset(&contentGroup1, 0, sizeof(contentGroup1));
    ::memset(&contentGroup2, 0, sizeof(contentGroup2));
    ::memset(&contentGroup3, 0, sizeof(contentGroup3));
    ::memset(&audioElement1, 0, sizeof(audioElement1));
    ::memset(&audioElement2, 0, sizeof(audioElement2));
    ::memset(&audioElement3, 0, sizeof(audioElement3));
    ::memset(&altValueSet1, 0, sizeof(altValueSet1));
    ::memset(&compElement1, 0, sizeof(compElement1));
    status = ::dlb_adm_core_model_open(&mCoreModel, &counts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    ASSERT_NE(nullptr, mCoreModel);

    // Add some entities to the model

    status = ::dlb_adm_core_model_add_name(&mNames, "Program 1 in English", LANG_1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_add_presentation(mCoreModel, &presentation1, &mNames);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_clear_names(&mNames);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_core_model_add_name(&mNames, "Programme 1 en Francais", LANG_3);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_add_presentation(mCoreModel, &presentation2, &mNames);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_clear_names(&mNames);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    contentGroup1.content_kind = DLB_ADM_CONTENT_KIND_MK_MIXED;     // M&E
    status = ::dlb_adm_core_model_add_name(&mNames, "Music and Effects", LANG_1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_add_content_group(mCoreModel, &contentGroup1, &mNames);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_clear_names(&mNames);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    contentGroup2.content_kind = DLB_ADM_CONTENT_KIND_DK_DIALOGUE;
    status = ::dlb_adm_core_model_add_name(&mNames, "Dialog in English", LANG_1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_add_content_group(mCoreModel, &contentGroup2, &mNames);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    audioElement2.gain.gain_unit = DLB_ADM_GAIN_UNIT_DB;
    status = ::dlb_adm_core_model_add_audio_element(mCoreModel, &audioElement2, &mNames);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_clear_names(&mNames);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    contentGroup3.content_kind = DLB_ADM_CONTENT_KIND_DK_DIALOGUE;
    status = ::dlb_adm_core_model_add_name(&mNames, "Dialogue en Francais", LANG_3);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_add_content_group(mCoreModel, &contentGroup3, &mNames);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    audioElement3.gain.gain_unit = DLB_ADM_GAIN_UNIT_DB;
    status = ::dlb_adm_core_model_add_audio_element(mCoreModel, &audioElement3, &mNames);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_clear_names(&mNames);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    audioElement1.gain.gain_unit = DLB_ADM_GAIN_UNIT_DB;
    status = ::dlb_adm_core_model_add_name(&mNames, "Music & Effects 5.1 Bed", LANG_1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_add_audio_element(mCoreModel, &audioElement1, &mNames);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_clear_names(&mNames);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_core_model_add_label(&mNames, "Alternative Object label", LANG_1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_add_alt_value_set(mCoreModel, audioElement1.id, &altValueSet1, &mNames);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_clear_names(&mNames);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    compElement1.audio_element_id = audioElement3.id;
    compElement1.complementary_leader_id = audioElement1.id;
    status = ::dlb_adm_core_model_add_complementary_element(mCoreModel, &compElement1, 1, nullptr);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    compElement2.audio_element_id = audioElement1.id;
    compElement2.complementary_leader_id = audioElement1.id;
    status = ::dlb_adm_core_model_add_label(&mNames, "Complementary Group label", LANG_1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_add_complementary_element(mCoreModel, &compElement2, 2, &mNames);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_clear_names(&mNames);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // Null pointer/invalid argument

    status = ::dlb_adm_core_model_add_presentation_relation(
        nullptr, DLB_ADM_NULL_ENTITY_ID, DLB_ADM_NULL_ENTITY_ID, DLB_ADM_NULL_ENTITY_ID, DLB_ADM_NULL_ENTITY_ID, DLB_ADM_NULL_ENTITY_ID, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_add_presentation_relation(mCoreModel,
        DLB_ADM_NULL_ENTITY_ID, DLB_ADM_NULL_ENTITY_ID, DLB_ADM_NULL_ENTITY_ID, DLB_ADM_NULL_ENTITY_ID, DLB_ADM_NULL_ENTITY_ID, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);
    status = ::dlb_adm_core_model_add_presentation_relation(mCoreModel, presentation1.id,
        DLB_ADM_NULL_ENTITY_ID, DLB_ADM_NULL_ENTITY_ID, DLB_ADM_NULL_ENTITY_ID, DLB_ADM_NULL_ENTITY_ID, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);
    status = ::dlb_adm_core_model_add_presentation_relation(mCoreModel, presentation1.id, contentGroup1.id,
        DLB_ADM_NULL_ENTITY_ID, DLB_ADM_NULL_ENTITY_ID, DLB_ADM_NULL_ENTITY_ID, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);
    status = ::dlb_adm_core_model_add_presentation_relation(mCoreModel, contentGroup1.id, presentation1.id,
        DLB_ADM_NULL_ENTITY_ID, audioElement1.id, DLB_ADM_NULL_ENTITY_ID, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(DLB_ADM_STATUS_ERROR, status);        // Presentation and content group reversed -- wrong types

    // Test adding presentation relations --

    // Presentation 1

    status = ::dlb_adm_core_model_add_presentation_relation(mCoreModel, presentation1.id, contentGroup1.id,
        DLB_ADM_NULL_ENTITY_ID, audioElement1.id, altValueSet1.id, DLB_ADM_NULL_ENTITY_ID);  // M&E
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_add_presentation_relation(mCoreModel, presentation1.id, contentGroup1.id,
        DLB_ADM_NULL_ENTITY_ID, audioElement1.id, altValueSet1.id, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(DLB_ADM_STATUS_ERROR, status);        // Duplicates not allowed
    status = ::dlb_adm_core_model_add_presentation_relation(mCoreModel, presentation1.id, contentGroup1.id,
        DLB_ADM_NULL_ENTITY_ID, audioElement2.id, altValueSet1.id, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(DLB_ADM_STATUS_ERROR, status);        // AlternativeValueSet references wrong audioElement
    status = ::dlb_adm_core_model_add_presentation_relation(mCoreModel, presentation1.id, contentGroup2.id,
        DLB_ADM_NULL_ENTITY_ID, audioElement2.id, DLB_ADM_NULL_ENTITY_ID, DLB_ADM_NULL_ENTITY_ID);  // English Dialog
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // Presentation 2

    status = ::dlb_adm_core_model_add_presentation_relation(mCoreModel, presentation2.id, contentGroup1.id,
        DLB_ADM_NULL_ENTITY_ID, audioElement1.id, DLB_ADM_NULL_ENTITY_ID, DLB_ADM_NULL_ENTITY_ID);  // M&E
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_core_model_add_presentation_relation(mCoreModel, presentation2.id, contentGroup3.id,
        DLB_ADM_NULL_ENTITY_ID, audioElement3.id, DLB_ADM_NULL_ENTITY_ID, DLB_ADM_NULL_ENTITY_ID);  // French Dialog
    status = ::dlb_adm_core_model_add_presentation_relation(mCoreModel, presentation2.id, contentGroup3.id,
        DLB_ADM_NULL_ENTITY_ID, audioElement3.id, DLB_ADM_NULL_ENTITY_ID, compElement1.id); // Complementry object
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
}

TEST_F(DlbAdm06, CoreModelAddFrameFormatBasic)
{
    dlb_adm_core_model_counts counts;
    dlb_adm_data_frame_format frameFormat;
    int status;

    ::memset(&counts, 0, sizeof(counts));
    ::memset(&frameFormat, 0, sizeof(frameFormat));
    status = ::dlb_adm_core_model_open(&mCoreModel, &counts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    ASSERT_NE(nullptr, mCoreModel);

    status = ::dlb_adm_core_model_add_frame_format(nullptr, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_add_frame_format(mCoreModel, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);

    status = ::dlb_adm_core_model_add_frame_format(mCoreModel, &frameFormat);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);

    ::snprintf(frameFormat.type,     sizeof(frameFormat.type),     "%s", "intermediate");
    ::snprintf(frameFormat.start,    sizeof(frameFormat.start),    "%s", "00:00:00.00000");
    ::snprintf(frameFormat.duration, sizeof(frameFormat.duration), "%s", "00:00:00.02000");

    status = ::dlb_adm_core_model_add_frame_format(mCoreModel, &frameFormat);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);     // type is not "full"

    ::snprintf(frameFormat.type,     sizeof(frameFormat.type),     "%s", "full");

    status = ::dlb_adm_core_model_add_frame_format(mCoreModel, &frameFormat);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(0x0000000000000001, (frameFormat.id & 0x00000fffffffffff));
}

TEST(dlb_adm_test, ElementDataQueryMem)
{
    size_t memorySize;
    int status;

    status = ::dlb_adm_core_model_query_element_data_memory_size(nullptr, 0, 0);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_query_element_data_memory_size(&memorySize, 0, 0);
    EXPECT_EQ(DLB_ADM_STATUS_OUT_OF_RANGE, status);

    memorySize = 0;
    status = ::dlb_adm_core_model_query_element_data_memory_size(&memorySize, MAX_PMD_BED_CHANNELS, MAX_SADM_ALT_VALUE_SETS);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_LT(0, memorySize);

    memorySize = 0;
    status = ::dlb_adm_core_model_query_element_data_memory_size(&memorySize, MAX_PMD_BED_CHANNELS, 0);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_LT(0, memorySize);
}

TEST_F(DlbAdm06, ElementDataConfigure)
{
    size_t memorySize;
    int status;

    status = ::dlb_adm_core_model_query_element_data_memory_size(&memorySize, MAX_PMD_BED_CHANNELS, MAX_SADM_ALT_VALUE_SETS);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    mElementDataMemory = new uint8_t[memorySize];
    ASSERT_NE(nullptr, mElementDataMemory);

    status = ::dlb_adm_core_model_configure_element_data(nullptr, 0, 0, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_configure_element_data(&mElementData, 0, 0, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_configure_element_data(&mElementData, 0, 0, mElementDataMemory);
    EXPECT_EQ(DLB_ADM_STATUS_OUT_OF_RANGE, status);

    // configure element_data with some memory for AltValSets
    status = ::dlb_adm_core_model_configure_element_data(&mElementData, MAX_PMD_BED_CHANNELS, MAX_SADM_ALT_VALUE_SETS, mElementDataMemory);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(0, mElementData.channel_count);
    EXPECT_EQ(0, mElementData.alt_val_count);
    EXPECT_EQ(MAX_PMD_BED_CHANNELS, mElementData.channel_capacity);
    EXPECT_EQ(MAX_SADM_ALT_VALUE_SETS, mElementData.alt_val_capacity);
    EXPECT_EQ(mElementDataMemory, mElementData.array_storage);

    status = ::dlb_adm_core_model_clear_element_data(&mElementData);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // configure element_data without memory for AltValSets
    status = ::dlb_adm_core_model_configure_element_data(&mElementData, MAX_PMD_BED_CHANNELS, 0, mElementDataMemory);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(0, mElementData.channel_count);
    EXPECT_EQ(MAX_PMD_BED_CHANNELS, mElementData.channel_capacity);
    EXPECT_EQ(0, mElementData.alt_val_capacity);
    EXPECT_EQ(mElementDataMemory, mElementData.array_storage);
}

TEST_F(DlbAdm06, ElementDataGet1)
{
    static const char audioElementIDString[] = "AO_1001";
    static const char altValSetIDString[] = "AVS_1001_0001";
    static const dlb_adm_channel_count CHANNEL_CAPACITY = 16;
    dlb_adm_data_audio_element_data nullData;
    dlb_adm_entity_id audioElementID;
    dlb_adm_entity_id altValSetID;
    size_t memorySize;
    int status;

    status = SetUpTestModel();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_read_entity_id(&audioElementID, audioElementIDString, ::strlen(audioElementIDString) + 1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_read_entity_id(&altValSetID, altValSetIDString, ::strlen(altValSetIDString) + 1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // configure the element data
    ::memset(&nullData, 0, sizeof(nullData));
    status = ::dlb_adm_core_model_query_element_data_memory_size(&memorySize, CHANNEL_CAPACITY, MAX_SADM_ALT_VALUE_SETS);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    mElementDataMemory = new uint8_t[memorySize];
    ASSERT_NE(nullptr, mElementDataMemory);

    // wrong configuration of element_data - not enough memory for AlternativeValueSets
    status = ::dlb_adm_core_model_configure_element_data(&mElementData, CHANNEL_CAPACITY, 0, mElementDataMemory);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(0, mElementData.alt_val_capacity);

    status = ::dlb_adm_core_model_get_element_data(&mElementData, mCoreModel, audioElementID);
    EXPECT_EQ(DLB_ADM_STATUS_OUT_OF_MEMORY, status);

    // correct configuration of element_data
    status = ::dlb_adm_core_model_configure_element_data(&mElementData, CHANNEL_CAPACITY, MAX_SADM_ALT_VALUE_SETS, mElementDataMemory);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // begin testing
    status = ::dlb_adm_core_model_get_element_data(nullptr, nullptr, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_get_element_data(&nullData, nullptr, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_get_element_data(&nullData, mCoreModel, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);
    status = ::dlb_adm_core_model_get_element_data(&mElementData, mCoreModel, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);

    status = ::dlb_adm_core_model_get_element_data(&mElementData, mCoreModel, audioElementID);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(2, mElementData.channel_count);
    EXPECT_EQ(audioElementID, mElementData.audio_element.id);
    EXPECT_EQ(1, mElementData.alt_val_count);
    EXPECT_EQ(MAX_SADM_ALT_VALUE_SETS, mElementData.alt_val_capacity);

    EXPECT_EQ(mElementData.audio_element.audio_object_interaction.onOffInteract, 0u);
    EXPECT_EQ(mElementData.audio_element.audio_object_interaction.gainInteract, 0u);
    EXPECT_EQ(mElementData.audio_element.audio_object_interaction.positionInteract, 1u);
    EXPECT_EQ(mElementData.audio_element.audio_object_interaction.positionRanges[0].cartesian, 1u);
    EXPECT_EQ(mElementData.audio_element.audio_object_interaction.positionRanges[0].coordinate, DLB_ADM_COORDINATE_X);
    EXPECT_FLOAT_EQ(mElementData.audio_element.audio_object_interaction.positionRanges[0].minValue, -1.00);
    EXPECT_FLOAT_EQ(mElementData.audio_element.audio_object_interaction.positionRanges[0].maxValue, 1.00);
    // offset value set to default - not present
    EXPECT_FLOAT_EQ(mElementData.audio_element.position_offset.offset_value, 0.00);
    EXPECT_EQ(mElementData.audio_element.position_offset.cartesian, 0);

    EXPECT_EQ(altValSetID, mElementData.alt_val_sets[0].id);
    EXPECT_EQ(DLB_ADM_TRUE, mElementData.alt_val_sets[0].has_gain);
    EXPECT_FLOAT_EQ(-1.5, mElementData.alt_val_sets[0].gain.gain_value);
    EXPECT_EQ(DLB_ADM_GAIN_UNIT_DB, mElementData.alt_val_sets[0].gain.gain_unit);
    EXPECT_EQ(DLB_ADM_FALSE, mElementData.alt_val_sets[0].has_position_offset);

    // also test dlb_adm_core_model_clear_element_data()
    status = ::dlb_adm_core_model_clear_element_data(nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_clear_element_data(&nullData);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);

    status = ::dlb_adm_core_model_clear_element_data(&mElementData);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(0, mElementData.channel_count);
    EXPECT_EQ(CHANNEL_CAPACITY, mElementData.channel_capacity);
    EXPECT_EQ(DLB_ADM_NULL_ENTITY_ID, mElementData.audio_element.id);
}

TEST_F(DlbAdm06, ElementDataGet2)
{
    static const char audioElementIDString[] = "AO_1002";
    static const dlb_adm_channel_count CHANNEL_CAPACITY = 16;
    dlb_adm_data_audio_element_data nullData;
    dlb_adm_entity_id audioElementID;
    size_t memorySize;
    int status;

    status = SetUpTestModel();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_read_entity_id(&audioElementID, audioElementIDString, ::strlen(audioElementIDString) + 1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // configure the element data
    ::memset(&nullData, 0, sizeof(nullData));
    status = ::dlb_adm_core_model_query_element_data_memory_size(&memorySize, CHANNEL_CAPACITY, MAX_SADM_ALT_VALUE_SETS);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    mElementDataMemory = new uint8_t[memorySize];
    ASSERT_NE(nullptr, mElementDataMemory);
    status = ::dlb_adm_core_model_configure_element_data(&mElementData, CHANNEL_CAPACITY, MAX_SADM_ALT_VALUE_SETS, mElementDataMemory);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_core_model_get_element_data(&mElementData, mCoreModel, audioElementID);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(1, mElementData.channel_count);
    EXPECT_EQ(audioElementID, mElementData.audio_element.id);

    EXPECT_FLOAT_EQ(mElementData.audio_element.position_offset.offset_value, -0.5);
    EXPECT_EQ(mElementData.audio_element.position_offset.cartesian, 1);

    // also test dlb_adm_core_model_clear_element_data()
    status = ::dlb_adm_core_model_clear_element_data(nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_clear_element_data(&nullData);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);

    status = ::dlb_adm_core_model_clear_element_data(&mElementData);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(0, mElementData.channel_count);
    EXPECT_EQ(CHANNEL_CAPACITY, mElementData.channel_capacity);
    EXPECT_EQ(DLB_ADM_NULL_ENTITY_ID, mElementData.audio_element.id);
}

TEST(dlb_adm_test, PresentationDataQueryMem)
{
    size_t memorySize;
    int status;

    status = ::dlb_adm_core_model_query_presentation_data_memory_size(nullptr, 0);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_query_presentation_data_memory_size(&memorySize, 0);
    EXPECT_EQ(DLB_ADM_STATUS_OUT_OF_RANGE, status);

    memorySize = 0;
    status = ::dlb_adm_core_model_query_presentation_data_memory_size(&memorySize, MAX_PMD_PRESENTATION_ELEMENTS);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_LT(0, memorySize);
}

TEST_F(DlbAdm06, PresentationDataConfigure)
{
    size_t memorySize;
    int status;

    status = ::dlb_adm_core_model_query_presentation_data_memory_size(&memorySize, MAX_PMD_PRESENTATION_ELEMENTS);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    mPresentationDataMemory = new uint8_t[memorySize];
    ASSERT_NE(nullptr, mPresentationDataMemory);

    status = ::dlb_adm_core_model_configure_presentation_data(nullptr, 0, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_configure_presentation_data(&mPresentationData, 0, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_configure_presentation_data(&mPresentationData, 0, mPresentationDataMemory);
    EXPECT_EQ(DLB_ADM_STATUS_OUT_OF_RANGE, status);

    status = ::dlb_adm_core_model_configure_presentation_data(&mPresentationData, MAX_PMD_PRESENTATION_ELEMENTS, mPresentationDataMemory);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(0, mPresentationData.element_count);
    EXPECT_EQ(MAX_PMD_PRESENTATION_ELEMENTS, mPresentationData.element_capacity);
    EXPECT_EQ(mPresentationDataMemory, mPresentationData.array_storage);
}

TEST_F(DlbAdm06, PresentationDataGet)
{
    static const char presentationIDString[] = "APR_1001";
    static const char altValSetIDString[] = "AVS_1001_0001";
    dlb_adm_data_presentation_data nullData;
    dlb_adm_entity_id presentationID;
    dlb_adm_entity_id altValSetID;
    size_t memorySize;
    int status;

    status = SetUpTestModel();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_read_entity_id(&presentationID, presentationIDString, ::strlen(presentationIDString) + 1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_read_entity_id(&altValSetID, altValSetIDString, ::strlen(altValSetIDString) + 1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // configure the presentation data
    ::memset(&nullData, 0, sizeof(nullData));
    status = ::dlb_adm_core_model_query_presentation_data_memory_size(&memorySize, MAX_PMD_PRESENTATION_ELEMENTS);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    mPresentationDataMemory = new uint8_t[memorySize];
    ASSERT_NE(nullptr, mPresentationDataMemory);
    status = ::dlb_adm_core_model_configure_presentation_data(&mPresentationData, MAX_PMD_PRESENTATION_ELEMENTS, mPresentationDataMemory);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // begin testing
    status = ::dlb_adm_core_model_get_presentation_data(nullptr, nullptr, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_get_presentation_data(&nullData, nullptr, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_get_presentation_data(&nullData, mCoreModel, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);
    status = ::dlb_adm_core_model_get_presentation_data(&mPresentationData, mCoreModel, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);

    status = ::dlb_adm_core_model_get_presentation_data(&mPresentationData, mCoreModel, presentationID);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(2, mPresentationData.element_count);
    EXPECT_EQ(presentationID, mPresentationData.presentation.id);
    EXPECT_EQ(mPresentationData.presentation.loudness.loudness_type, DLB_ADM_LOUDNESS_TYPE_INTEGRATED);

    EXPECT_EQ(altValSetID, mPresentationData.alt_val_sets[0].id);
    EXPECT_EQ(DLB_ADM_TRUE, mPresentationData.alt_val_sets[0].has_gain);
    EXPECT_FLOAT_EQ(-1.5, mPresentationData.alt_val_sets[0].gain.gain_value);
    EXPECT_EQ(DLB_ADM_GAIN_UNIT_DB, mPresentationData.alt_val_sets[0].gain.gain_unit);
    EXPECT_EQ(DLB_ADM_FALSE, mPresentationData.alt_val_sets[0].has_position_offset);

    // also test dlb_adm_core_model_clear_presentation_data()
    status = ::dlb_adm_core_model_clear_presentation_data(nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_clear_presentation_data(&nullData);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);

    status = ::dlb_adm_core_model_clear_presentation_data(&mPresentationData);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(0, mPresentationData.element_count);
    EXPECT_EQ(MAX_PMD_PRESENTATION_ELEMENTS, mPresentationData.element_capacity);
    EXPECT_EQ(DLB_ADM_NULL_ENTITY_ID, mPresentationData.presentation.id);
}

TEST_F(DlbAdm06, PresentationDataGetCompObject)
{
    static const char presentationIDString[] = "APR_1002";
    static const char compLeaderIDString[] = "AO_1002";
    static const char compObjectIDString[] = "AO_1003";
    dlb_adm_data_presentation_data nullData;
    dlb_adm_entity_id presentationID;
    dlb_adm_entity_id compLeaderID;
    dlb_adm_entity_id compObjectID;
    size_t memorySize;
    int status;

    status = SetUpTestModel();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_read_entity_id(&presentationID, presentationIDString, ::strlen(presentationIDString) + 1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_read_entity_id(&compLeaderID, compLeaderIDString, ::strlen(compLeaderIDString) + 1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_read_entity_id(&compObjectID, compObjectIDString, ::strlen(compObjectIDString) + 1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // configure the presentation data
    ::memset(&nullData, 0, sizeof(nullData));
    status = ::dlb_adm_core_model_query_presentation_data_memory_size(&memorySize, MAX_PMD_PRESENTATION_ELEMENTS);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    mPresentationDataMemory = new uint8_t[memorySize];
    ASSERT_NE(nullptr, mPresentationDataMemory);
    status = ::dlb_adm_core_model_configure_presentation_data(&mPresentationData, MAX_PMD_PRESENTATION_ELEMENTS, mPresentationDataMemory);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // begin testing
    status = ::dlb_adm_core_model_get_presentation_data(&mPresentationData, mCoreModel, presentationID);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(2, mPresentationData.element_count);
    EXPECT_EQ(presentationID, mPresentationData.presentation.id);
    EXPECT_EQ(DLB_ADM_NULL_ENTITY_ID, mPresentationData.comp_elements[0].id);
    EXPECT_EQ(DLB_ADM_NULL_ENTITY_ID, mPresentationData.comp_elements[0].complementary_leader_id);
    EXPECT_EQ(DLB_ADM_NULL_ENTITY_ID, mPresentationData.comp_elements[0].audio_element_id);
    EXPECT_EQ(compLeaderID, mPresentationData.comp_elements[1].complementary_leader_id);
    EXPECT_EQ(compObjectID, mPresentationData.comp_elements[1].audio_element_id);

    status = ::dlb_adm_core_model_clear_presentation_data(&mPresentationData);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(0, mPresentationData.element_count);
    EXPECT_EQ(MAX_PMD_PRESENTATION_ELEMENTS, mPresentationData.element_capacity);
    EXPECT_EQ(DLB_ADM_NULL_ENTITY_ID, mPresentationData.presentation.id);
}

static
int
ForEachCallback
    (const dlb_adm_core_model   *model
    ,dlb_adm_entity_id           entity_id
    ,void                       *callback_arg
    )
{
    if ((model == nullptr) || (callback_arg == nullptr))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    if (entity_id == DLB_ADM_NULL_ENTITY_ID)
    {
        return DLB_ADM_STATUS_INVALID_ARGUMENT;
    }

    unsigned *n = reinterpret_cast<unsigned *>(callback_arg);
    ++(*n);

    return DLB_ADM_STATUS_OK;
}

TEST_F(DlbAdm06, CoreModelForEachEntityIDBasic)
{
    unsigned count;
    int status;

    status = SetUpTestModel();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_core_model_for_each_entity_id(nullptr, DLB_ADM_ENTITY_TYPE_ILLEGAL, nullptr, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_for_each_entity_id(mCoreModel, DLB_ADM_ENTITY_TYPE_ILLEGAL, nullptr, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_for_each_entity_id(mCoreModel, DLB_ADM_ENTITY_TYPE_ILLEGAL, ForEachCallback, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_OUT_OF_RANGE, status);
    status = ::dlb_adm_core_model_for_each_entity_id(mCoreModel, DLB_ADM_ENTITY_TYPE_PROGRAMME, ForEachCallback, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status); // From ForEachCallback()

    count = 0;
    status = ::dlb_adm_core_model_for_each_entity_id(mCoreModel, DLB_ADM_ENTITY_TYPE_PROGRAMME, ForEachCallback, &count);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(2, count);

    count = 0;
    status = ::dlb_adm_core_model_for_each_entity_id(mCoreModel, DLB_ADM_ENTITY_TYPE_AUDIO_TRACK, ForEachCallback, &count);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(4, count);
}

TEST_F(DlbAdm06, CoreModelForEachAudioElementIDBasic)
{
    unsigned count;
    int status;

    status = SetUpTestModel();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_core_model_for_each_audio_element_id(nullptr, nullptr, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_for_each_audio_element_id(mCoreModel, nullptr, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_for_each_audio_element_id(mCoreModel, ForEachCallback, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status); // From ForEachCallback()

    count = 0;
    status = ::dlb_adm_core_model_for_each_audio_element_id(mCoreModel, ForEachCallback, &count);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(3, count);
}

static
int
SourceCallback
    (const dlb_adm_core_model   *model
    ,const dlb_adm_data_source  *source
    ,void                       *callback_arg
    )
{
    if ((model == nullptr) || (source == nullptr) || (callback_arg == nullptr))
    {
        return DLB_ADM_STATUS_NULL_POINTER;
    }

    unsigned *n = reinterpret_cast<unsigned *>(callback_arg);
    ++(*n);

    return DLB_ADM_STATUS_OK;
}

TEST_F(DlbAdm06, CoreModelForEachSourceBasic)
{
    unsigned count;
    int status;

    status = SetUpTestModel();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_core_model_for_each_source(nullptr, nullptr, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_for_each_source(mCoreModel, nullptr, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_for_each_source(mCoreModel, SourceCallback, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status); // because SourceCallback() returns this value

    count = 0;
    status = ::dlb_adm_core_model_for_each_source(mCoreModel, SourceCallback, &count);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(4, count);
}

TEST_F(DlbAdm06, CoreModelGetFlowID)
{
    char uuid[DLB_ADM_DATA_FF_UUID_SZ];
    int status;

    status = SetUpTestModel();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_core_model_get_flow_id(nullptr, nullptr, 0);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_get_flow_id(mCoreModel, nullptr, 0);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_get_flow_id(mCoreModel, uuid, 0);
    EXPECT_EQ(DLB_ADM_STATUS_OUT_OF_MEMORY, status);

    status = ::dlb_adm_core_model_get_flow_id(mCoreModel, uuid, sizeof(uuid));
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(0, ::strcmp(uuid, "f8cc7821-09b2-41cb-bd42-ec35e9fcb9a8"));
}

TEST_F(DlbAdm06, FileIsSadmXml)
{
    dlb_adm_bool isSadm;
    int status;

    SetUpTestInput(stereoXMLInputFileName, stereoXML);
    SetUpTestInput(pmdXMLInputFileName, stereo_2D_PMD);

    status = ::dlb_adm_file_is_sadm_xml(nullptr, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_file_is_sadm_xml(&isSadm, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_file_is_sadm_xml(&isSadm, "");
    EXPECT_EQ(DLB_ADM_STATUS_INVALID_ARGUMENT, status);
    status = ::dlb_adm_file_is_sadm_xml(&isSadm, "xyzzx.xml");
    EXPECT_EQ(DLB_ADM_STATUS_NOT_FOUND, status);

    isSadm = DLB_ADM_TRUE;
    status = ::dlb_adm_file_is_sadm_xml(&isSadm, pmdXMLInputFileName);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_FALSE(isSadm);

    isSadm = DLB_ADM_FALSE;
    status = ::dlb_adm_file_is_sadm_xml(&isSadm, stereoXMLInputFileName);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_TRUE(isSadm);
}

TEST_F(DlbAdm06, CoreModelCountEntities)
{
    dlb_adm_container_counts containerCounts;
    dlb_adm_core_model_counts modelCounts;
    int status;
    size_t n;

    // create the test input file
    SetUpTestInput(stereoXMLInputFileName, stereoXML);

    // open the XML container
    ::memset(&containerCounts, 0, sizeof(containerCounts));
    status = ::dlb_adm_container_open(&mXmlContainer, &containerCounts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // read the XML input file
    status = dlb_adm_container_read_xml_file(mXmlContainer, stereoXMLInputFileName, DLB_ADM_FALSE);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // open the model
    ::memset(&modelCounts, 0, sizeof(modelCounts));
    status = ::dlb_adm_core_model_open(&mCoreModel, &modelCounts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // bad arguments
    status = ::dlb_adm_core_model_count_entities(nullptr, DLB_ADM_ENTITY_TYPE_ILLEGAL, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_count_entities(mCoreModel, DLB_ADM_ENTITY_TYPE_ILLEGAL, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_count_entities(mCoreModel, DLB_ADM_ENTITY_TYPE_ILLEGAL, &n);
    EXPECT_EQ(DLB_ADM_STATUS_OUT_OF_RANGE, status);

    // how many presentations are there?
    n = 1;
    status = ::dlb_adm_core_model_count_entities(mCoreModel, DLB_ADM_ENTITY_TYPE_PROGRAMME, &n);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(0, n);

    // add some stuff
    status = ::dlb_adm_core_model_ingest_xml_container(mCoreModel, mXmlContainer);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // now, how many presentations are there?
    n = 0;
    status = ::dlb_adm_core_model_count_entities(mCoreModel, DLB_ADM_ENTITY_TYPE_PROGRAMME, &n);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(2, n);
}

TEST_F(DlbAdm06, CoreModelClear)
{
    size_t n = 0;
    int status;

    status = SetUpTestModel();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // how many presentations are there?
    status = ::dlb_adm_core_model_count_entities(mCoreModel, DLB_ADM_ENTITY_TYPE_PROGRAMME, &n);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(2, n);

    // bad argument
    status = ::dlb_adm_core_model_clear(nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);

    // I said, "Kill it!"
    status = ::dlb_adm_core_model_clear(mCoreModel);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // how many presentations are there?
    n = 2;
    status = ::dlb_adm_core_model_count_entities(mCoreModel, DLB_ADM_ENTITY_TYPE_PROGRAMME, &n);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(0, n);

    // k is everything gone?
    for (int i = DLB_ADM_ENTITY_TYPE_FIRST; i <= DLB_ADM_ENTITY_TYPE_LAST; ++i)
    {
        n = 1;
        status = ::dlb_adm_core_model_count_entities(mCoreModel, static_cast<DLB_ADM_ENTITY_TYPE>(i), &n);
        EXPECT_EQ(DLB_ADM_STATUS_OK, status);
        EXPECT_EQ(0, n);
    }
}

TEST_F(DlbAdm06, CoreModelIsEmpty)
{
    dlb_adm_core_model_counts counts;
    dlb_adm_core_model *coreModel;
    dlb_adm_bool isEmpty;
    int status;

    ::memset(&counts, 0, sizeof(counts));
    status = ::dlb_adm_core_model_open(&coreModel, &counts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // null argument checks
    status = ::dlb_adm_core_model_is_empty(nullptr, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);
    status = ::dlb_adm_core_model_is_empty(coreModel, nullptr);
    EXPECT_EQ(DLB_ADM_STATUS_NULL_POINTER, status);

    // is a newly-created model empty?
    isEmpty = DLB_ADM_FALSE;
    status = ::dlb_adm_core_model_is_empty(coreModel, &isEmpty);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_TRUE(isEmpty);

    status = ::dlb_adm_core_model_close(&coreModel);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = SetUpTestModel();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // is a model with content loaded empty?
    isEmpty = DLB_ADM_TRUE;
    status = ::dlb_adm_core_model_is_empty(mCoreModel, &isEmpty);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_FALSE(isEmpty);

    // is it empty after calling clear()?
    status = ::dlb_adm_core_model_clear(mCoreModel);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    isEmpty = DLB_ADM_FALSE;
    status = ::dlb_adm_core_model_is_empty(mCoreModel, &isEmpty);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_TRUE(isEmpty);
}

TEST_F(DlbAdm06, CoreModelPresentationLoudnessMetadata)
{
    dlb_adm_core_model_counts counts;
    dlb_adm_data_presentation presentation;
    dlb_adm_entity_id id1, id2, id3;
    int status;

    status = SetUpNames();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    const dlb_adm_data_loudness sourceLoudness1 = {-27.5, DLB_ADM_LOUDNESS_TYPE_INTEGRATED};
    const dlb_adm_data_loudness sourceLoudness2 = {-15.5, DLB_ADM_LOUDNESS_TYPE_NOT_INITIALIZED};
    const dlb_adm_data_loudness sourceLoudness3 = {-5.5, DLB_ADM_LOUDNESS_TYPE_COUNT};
    const dlb_adm_data_loudness sourceLoudnessNotInitialized = {0.0, DLB_ADM_LOUDNESS_TYPE_NOT_INITIALIZED};

    ::memset(&counts, 0, sizeof(counts));
    ::memset(&presentation, 0, sizeof(presentation));

    presentation.loudness = sourceLoudness1;

    status = ::dlb_adm_core_model_open(&mCoreModel, &counts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    ASSERT_NE(nullptr, mCoreModel);

    status = ::dlb_adm_core_model_add_name(&mNames, "Presentation 1", LANG_2);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // initialised loudness
    status = ::dlb_adm_core_model_add_presentation(mCoreModel, &presentation, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_NE(DLB_ADM_NULL_ENTITY_ID, presentation.id);
    id1 = presentation.id;
    EXPECT_TRUE(CheckNames(*mCoreModel, id1, mNames));
    EXPECT_TRUE(CheckLoudnessMetadata(*mCoreModel, id1, sourceLoudness1, DLB_ADM_ENTITY_TYPE_PROGRAMME));

    // uninitialized loudness
    presentation.id = DLB_ADM_NULL_ENTITY_ID;
    presentation.loudness = sourceLoudness2;
    status = ::dlb_adm_core_model_add_presentation(mCoreModel, &presentation, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_NE(DLB_ADM_NULL_ENTITY_ID, presentation.id);
    id2 = presentation.id;
    EXPECT_NE(id1, id2);
    EXPECT_TRUE(CheckNames(*mCoreModel, id2, mNames));
    EXPECT_TRUE(CheckLoudnessMetadata(*mCoreModel, id2, sourceLoudnessNotInitialized, DLB_ADM_ENTITY_TYPE_PROGRAMME));

    // incorrect loudness
    presentation.id = DLB_ADM_NULL_ENTITY_ID;
    presentation.loudness = sourceLoudness3;
    status = ::dlb_adm_core_model_add_presentation(mCoreModel, &presentation, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_NE(DLB_ADM_NULL_ENTITY_ID, presentation.id);
    id3 = presentation.id;
    EXPECT_NE(id2, id3);
    EXPECT_TRUE(CheckNames(*mCoreModel, id3, mNames));
    EXPECT_TRUE(CheckLoudnessMetadata(*mCoreModel, id3, sourceLoudnessNotInitialized, DLB_ADM_ENTITY_TYPE_PROGRAMME));
}

TEST_F(DlbAdm06, CoreModelContentGroupLoudnessMetadata)
{
    dlb_adm_core_model_counts counts;
    dlb_adm_data_content_group contentGroup;
    dlb_adm_entity_id id1, id2, id3;
    int status;

    status = SetUpNames();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    const dlb_adm_data_loudness sourceLoudness1 = {-27.5, DLB_ADM_LOUDNESS_TYPE_INTEGRATED};
    const dlb_adm_data_loudness sourceLoudness2 = {-15.5, DLB_ADM_LOUDNESS_TYPE_NOT_INITIALIZED};
    const dlb_adm_data_loudness sourceLoudness3 = {-5.5, DLB_ADM_LOUDNESS_TYPE_COUNT};
    const dlb_adm_data_loudness sourceLoudnessNotInitialized = {0.0, DLB_ADM_LOUDNESS_TYPE_NOT_INITIALIZED};

    ::memset(&counts, 0, sizeof(counts));
    ::memset(&contentGroup, 0, sizeof(contentGroup));

    status = ::dlb_adm_core_model_open(&mCoreModel, &counts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    ASSERT_NE(nullptr, mCoreModel);

    status = ::dlb_adm_core_model_add_name(&mNames, "Content Group 1", LANG_1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // initialised loudness
    contentGroup.loudness = sourceLoudness1;
    status = ::dlb_adm_core_model_add_content_group(mCoreModel, &contentGroup, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_NE(DLB_ADM_NULL_ENTITY_ID, contentGroup.id);
    id1 = contentGroup.id;
    EXPECT_TRUE(CheckNames(*mCoreModel, id1, mNames));
    EXPECT_TRUE(CheckLoudnessMetadata(*mCoreModel, id1, sourceLoudness1, DLB_ADM_ENTITY_TYPE_CONTENT));

    // uninitialized loudness
    contentGroup.loudness = sourceLoudness2;
    contentGroup.id = DLB_ADM_NULL_ENTITY_ID;
    status = ::dlb_adm_core_model_add_content_group(mCoreModel, &contentGroup, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_NE(DLB_ADM_NULL_ENTITY_ID, contentGroup.id);
    id2 = contentGroup.id;
    EXPECT_NE(id1, id2);
    EXPECT_TRUE(CheckNames(*mCoreModel, id2, mNames));
    EXPECT_TRUE(CheckLoudnessMetadata(*mCoreModel, id2, sourceLoudnessNotInitialized, DLB_ADM_ENTITY_TYPE_CONTENT));

    // incorrect loudness
    contentGroup.loudness = sourceLoudness3;
    contentGroup.id = DLB_ADM_NULL_ENTITY_ID;
    status = ::dlb_adm_core_model_add_content_group(mCoreModel, &contentGroup, &mNames);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_NE(DLB_ADM_NULL_ENTITY_ID, contentGroup.id);
    id3 = contentGroup.id;
    EXPECT_NE(id2, id3);
    EXPECT_TRUE(CheckNames(*mCoreModel, id3, mNames));
    EXPECT_TRUE(CheckLoudnessMetadata(*mCoreModel, id3, sourceLoudnessNotInitialized, DLB_ADM_ENTITY_TYPE_CONTENT));
}
