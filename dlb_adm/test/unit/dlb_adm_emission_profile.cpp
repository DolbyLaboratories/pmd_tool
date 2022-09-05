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

#include "dlb_adm/include/dlb_adm_api.h"
#include "dlb_adm_api_pvt.h"

#include "dlb_adm_xml_container.h"
#include "core_model/core_model_defs.h"
#include "AdmIdTranslator.h"
#include "AdmId.h"

#include <stdio.h>
#include <fstream>
#include <string>

#include "dlb_adm_emission_profile_data.h"

static const char dolbyPMDStudioReferenceFileName[] = "dolby_PMD_Studio_reference.xml";
static const char dolbyPMDStudioReferenceOutFileName[] = "dolby_PMD_Studio_reference.out.xml";

static const char generatedEmissionProfileFileName[] = "generated_emission_profile.xml";
static const char generatedEmissionProfileOutFileName[] = "generated_emission_profile.out.xml";

static const char emissionProfileCompliantFileName[] = "emission_profile_compliant.xml";
static const char emissionProfileCompliantOutFileName[] = "emission_profile_compliant.out.xml";
static const char emissionProfileCompliantIngestedOutFileName[] = "emission_profile_compliant.ingested.out.xml";

static const char multipleProgrammesFileName[] = "multiple_programmes.xml";
static const char multipleProgrammesOutFileName[] = "multiple_programmes.out.xml";

static const char complementaryObjectsFileName[] = "complementary_objects.xml";
static const char complementaryObjectsOutFileName[] = "complementary_objects.out.xml";

static const char adAndSpokenSubtitlesFileName[] = "ad_and_spoken_subtitles.xml";
static const char adAndSpokenSubtitlesOutFileName[] = "ad_and_spoken_subtitles.out.xml";

static const char complementaryMultichannelObjectsFileName[] = "complementary_multuchannel_objects.xml";
static const char complementaryMultichannelObjectsOutFileName[] = "complementary_multuchannel_objects.out.xml";

static const char multipleProgrammesUsingMonoObjectsFileName[] = "multiple_programmes_mono_objects.xml";
static const char multipleProgrammesUsingMonoObjectsOutFileName[] = "multiple_programmes_mono_objects.out.xml";

static const char minimalFileName[] = "minimalFileName.xml";
static const char minimalOutFileName[] = "minimalFileName.out.xml";

static const char audioObjectInteractionName[] = "audioObjectInteraction.xml";
static const char audioObjectInteractionOutFileName[] = "audioObjectInteraction.out.xml";
static const char audioObjectInteractionFlattenedOutFileName[] = "audioObjectInteraction.flattened.out.xml";

static const dlb_adm_element_count MAX_PMD_PRESENTATION_ELEMENTS = 128;
static const dlb_adm_alt_val_count MAX_SADM_ALT_VALUE_SETS = 8;
static const dlb_adm_channel_count MAX_PMD_BED_CHANNELS = 16;

//TODO: add "complementary nested objects with boosted dialogue" when ready
//TODO: add "complementary objects with alternative valu set" when ready

class DlbAdmEmissionProfile : public testing::Test
{
protected:

    dlb_adm_container_counts     containerCounts;
    dlb_adm_xml_container       *oryginalContainer;
    dlb_adm_xml_container       *flattenedContainer;
    dlb_adm_core_model          *coreModel;
    size_t                       count;

    uint8_t *mPresentationDataMemory;
    dlb_adm_data_presentation_data mPresentationData;

    uint8_t *mElementDataMemory;
    dlb_adm_data_audio_element_data mElementData;

    static const unsigned int mNameCount = DlbAdm::DEFAULT_NAME_LIMIT;
    char *mNamesMemory0;
    dlb_adm_data_names mNames;

    void SetUpTestInput(const char *path, const char *content)
    {
        std::ifstream ifs(path);

        if (!ifs.good())
        {
            std::ofstream ofs(path);

            ofs << content;
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

        return eq ? DLB_ADM_STATUS_OK : DLB_ADM_STATUS_ERROR;
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

        return status;
    }



    virtual void SetUp()
    {
        int status;
        mNamesMemory0 = nullptr;
        mPresentationDataMemory = nullptr;
        ::memset(&containerCounts, 0, sizeof(containerCounts));
        oryginalContainer = nullptr;
        flattenedContainer = nullptr;
        coreModel = nullptr;

        SetUpTestInput(dolbyPMDStudioReferenceFileName, dolbyPMDStudioReferenceXMLBuffer);
        SetUpTestInput(generatedEmissionProfileFileName, generatedEmissionProfileXMLBuffer);
        SetUpTestInput(emissionProfileCompliantFileName, emissionProfileCompliantXMLBuffer);
        SetUpTestInput(complementaryObjectsFileName, complementaryObjectsXMLBuffer);
        SetUpTestInput(multipleProgrammesFileName, multipleProgrammesXMLBuffer);
        SetUpTestInput(adAndSpokenSubtitlesFileName, adAndSpokenSubtitlesXMLBuffer);
        SetUpTestInput(complementaryMultichannelObjectsFileName, complementaryMultichannelObjectsXMLBuffer);
        SetUpTestInput(multipleProgrammesUsingMonoObjectsFileName, multipleProgrammesUsingMonoObjectsXMLBuffer);
        SetUpTestInput(minimalFileName, minimalFileBuffer);
        SetUpTestInput(audioObjectInteractionName, audioObjectInteractionBuffer);

        status = ::dlb_adm_container_open(&oryginalContainer, &containerCounts);
        ASSERT_EQ(DLB_ADM_STATUS_OK, status);
        status = ::dlb_adm_container_open(&flattenedContainer, &containerCounts);
        ASSERT_EQ(DLB_ADM_STATUS_OK, status);

        status = SetUpNames();
        ASSERT_EQ(DLB_ADM_STATUS_OK, status);

        size_t memorySize;

        // configure presentation_data
        status = ::dlb_adm_core_model_query_presentation_data_memory_size(&memorySize, MAX_PMD_PRESENTATION_ELEMENTS);
        ASSERT_EQ(DLB_ADM_STATUS_OK, status);
        mPresentationDataMemory = new uint8_t[memorySize];
        ASSERT_NE(nullptr, mPresentationDataMemory);

        status = ::dlb_adm_core_model_configure_presentation_data(&mPresentationData, MAX_PMD_PRESENTATION_ELEMENTS, mPresentationDataMemory);
        ASSERT_EQ(DLB_ADM_STATUS_OK, status);
        ASSERT_EQ(0, mPresentationData.element_count);
        ASSERT_EQ(MAX_PMD_PRESENTATION_ELEMENTS, mPresentationData.element_capacity);
        ASSERT_EQ(mPresentationDataMemory, mPresentationData.array_storage);

        // configure element_data
        status = ::dlb_adm_core_model_query_element_data_memory_size(&memorySize, MAX_PMD_BED_CHANNELS, MAX_SADM_ALT_VALUE_SETS);
        ASSERT_EQ(DLB_ADM_STATUS_OK, status);
        mElementDataMemory = new uint8_t[memorySize];
        ASSERT_NE(nullptr, mElementDataMemory);

        status = ::dlb_adm_core_model_configure_element_data(&mElementData, MAX_PMD_BED_CHANNELS, MAX_SADM_ALT_VALUE_SETS, mElementDataMemory);
        ASSERT_EQ(DLB_ADM_STATUS_OK, status);
        ASSERT_EQ(0, mElementData.alt_val_count);
        ASSERT_EQ(0, mElementData.channel_count);
        ASSERT_EQ(MAX_PMD_BED_CHANNELS, mElementData.channel_capacity);
        ASSERT_EQ(MAX_SADM_ALT_VALUE_SETS, mElementData.alt_val_capacity);
        ASSERT_EQ(mElementDataMemory, mElementData.array_storage);
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

        if (mNamesMemory0 != nullptr)
        {
            delete[] mNamesMemory0;
            mNamesMemory0 = nullptr;
        }

        delete [] mPresentationDataMemory;
        delete [] mElementDataMemory;
    }

};


/* Check basic configuration read and write */
TEST_F(DlbAdmEmissionProfile, dolbyPMDStudioReference)
{
    int status;

    status = ::dlb_adm_container_read_xml_file(oryginalContainer, dolbyPMDStudioReferenceFileName, DLB_ADM_TRUE);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // Flattening ADM structure
    status = ::dlb_adm_container_flatten(oryginalContainer, flattenedContainer);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_container_write_xml_file(flattenedContainer, dolbyPMDStudioReferenceOutFileName);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // ingest into the core model
    status = dlb_adm_core_model_open_from_xml_container(&coreModel, flattenedContainer);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // Check if content has been flattend
    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_PROGRAMME, &count);
    ASSERT_EQ(count, 4);

    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_CONTENT, &count);
    ASSERT_EQ(count, 7);

    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_OBJECT, &count);
    ASSERT_EQ(count, 7);

    // TODO: Extend checking: programs, contents and objects Id, gain and language
}


/* Check parsing tags */
TEST_F(DlbAdmEmissionProfile, parse_EmissionProfile)
{
    int status;

    status = ::dlb_adm_container_read_xml_file(oryginalContainer, generatedEmissionProfileFileName, DLB_ADM_TRUE);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_container_write_xml_file(oryginalContainer, generatedEmissionProfileOutFileName);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = CompareFiles(generatedEmissionProfileFileName, generatedEmissionProfileOutFileName);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
}


/* Check basic configuration read and write */
TEST_F(DlbAdmEmissionProfile, flatten_emissionProfileCompliant)
{
    int status;

    status = ::dlb_adm_container_read_xml_file(oryginalContainer, emissionProfileCompliantFileName, DLB_ADM_TRUE);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // Flattening ADM structure
    status = ::dlb_adm_container_flatten(oryginalContainer, flattenedContainer);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_container_write_xml_file(flattenedContainer, emissionProfileCompliantOutFileName);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // ingest into the core model
    status = dlb_adm_core_model_open_from_xml_container(&coreModel, flattenedContainer);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // Check if content has been flattend
    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_PROGRAMME, &count);
    EXPECT_EQ(count, 4);

    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_CONTENT, &count);
    EXPECT_EQ(count, 8);

    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_OBJECT, &count);
    EXPECT_EQ(count, 8);

    // TODO: Extend checking: programs, contents and objects Id, gain and language

    dlb_adm_entity_id entityID;

    static const char objectIDString1[] = "AO_1001";
    status = ::dlb_adm_read_entity_id(&entityID, objectIDString1, ::strlen(objectIDString1) + 1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_core_model_get_names(coreModel, &mNames, entityID);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(4, mNames.name_count);
    EXPECT_EQ(3, mNames.label_count);
    EXPECT_EQ(0, ::strcmp("audioObject_1", mNames.names[0]));
    EXPECT_EQ(0, ::strcmp("", mNames.langs[0]));
    EXPECT_EQ(0, ::strcmp("English", mNames.names[1]));
    EXPECT_EQ(0, ::strcmp("eng", mNames.langs[1]));
    EXPECT_EQ(0, ::strcmp("Ingles", mNames.names[2]));
    EXPECT_EQ(0, ::strcmp("spa", mNames.langs[2]));
    EXPECT_EQ(0, ::strcmp("Yīngyu", mNames.names[3]));
    EXPECT_EQ(0, ::strcmp("zho", mNames.langs[3]));

    static const char objectIDString6[] = "AO_1006";
    status = ::dlb_adm_read_entity_id(&entityID, objectIDString6, ::strlen(objectIDString6) + 1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_core_model_get_names(coreModel, &mNames, entityID);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(3, mNames.name_count);
    EXPECT_EQ(2, mNames.label_count);
    EXPECT_EQ(0, ::strcmp("audioObject_1", mNames.names[0]));
    EXPECT_EQ(0, ::strcmp("", mNames.langs[0]));
    EXPECT_EQ(0, ::strcmp("Angielski", mNames.names[1]));
    EXPECT_EQ(0, ::strcmp("pol", mNames.langs[1]));
    EXPECT_EQ(0, ::strcmp("English", mNames.names[2]));
    EXPECT_EQ(0, ::strcmp("ger", mNames.langs[2]));

    static const char objectIDString7[] = "AO_1007";
    status = ::dlb_adm_read_entity_id(&entityID, objectIDString7, ::strlen(objectIDString7) + 1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_core_model_get_names(coreModel, &mNames, entityID);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(3, mNames.name_count);
    EXPECT_EQ(2, mNames.label_count);
    EXPECT_EQ(0, ::strcmp("audioObject_1", mNames.names[0]));
    EXPECT_EQ(0, ::strcmp("", mNames.langs[0]));
    EXPECT_EQ(0, ::strcmp("Anglais", mNames.names[1]));
    EXPECT_EQ(0, ::strcmp("fre", mNames.langs[1]));
    EXPECT_EQ(0, ::strcmp("Inglese", mNames.names[2]));
    EXPECT_EQ(0, ::strcmp("ita", mNames.langs[2]));

    dlb_adm_bool isEmissionProfile;
    status = ::dlb_adm_core_model_has_profile(coreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE, &isEmissionProfile);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_TRUE, isEmissionProfile);
}

/* Check basic configuration read and write */
TEST_F(DlbAdmEmissionProfile, ingest_emissionProfileCompliant)
{
    int status;

    // set up ids
    static const char presentationIDString1[] = "APR_1001";
    dlb_adm_entity_id presentationID1;
    status = ::dlb_adm_read_entity_id(&presentationID1, presentationIDString1, ::strlen(presentationIDString1) + 1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    ASSERT_NE(DLB_ADM_NULL_ENTITY_ID, presentationID1);

    static const char presentationIDString2[] = "APR_1004";
    dlb_adm_entity_id presentationID4;
    status = ::dlb_adm_read_entity_id(&presentationID4, presentationIDString2, ::strlen(presentationIDString2) + 1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    ASSERT_NE(DLB_ADM_NULL_ENTITY_ID, presentationID4);

    static const char objectIDString[] = "AO_1001";
    dlb_adm_entity_id objectId1;
    status = ::dlb_adm_read_entity_id(&objectId1, objectIDString, ::strlen(objectIDString) + 1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    ASSERT_NE(DLB_ADM_NULL_ENTITY_ID, objectId1);

    static const char altValSetIDString11[] = "AVS_1001_0001";
    dlb_adm_entity_id AltValSetID11;
    status = ::dlb_adm_read_entity_id(&AltValSetID11, altValSetIDString11, ::strlen(altValSetIDString11) + 1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    ASSERT_NE(DLB_ADM_NULL_ENTITY_ID, AltValSetID11);

    static const char altValSetIDString12[] = "AVS_1001_0002";
    dlb_adm_entity_id AltValSetID12;
    status = ::dlb_adm_read_entity_id(&AltValSetID12, altValSetIDString12, ::strlen(altValSetIDString12) + 1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    ASSERT_NE(DLB_ADM_NULL_ENTITY_ID, AltValSetID12);

    static const char altValSetIDString21[] = "AVS_1002_0001";
    dlb_adm_entity_id AltValSetID21;
    status = ::dlb_adm_read_entity_id(&AltValSetID21, altValSetIDString21, ::strlen(altValSetIDString21) + 1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    ASSERT_NE(DLB_ADM_NULL_ENTITY_ID, AltValSetID21);

    status = ::dlb_adm_container_read_xml_file(oryginalContainer, emissionProfileCompliantFileName, DLB_ADM_TRUE);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // ingest into the core model
    status = dlb_adm_core_model_open_from_xml_container(&coreModel, oryginalContainer);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // generate from core model
    status = dlb_adm_container_open_from_core_model(&flattenedContainer, coreModel);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = dlb_adm_container_write_xml_file(flattenedContainer, emissionProfileCompliantIngestedOutFileName);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // TODO: Extend checking: programs, contents and objects Id, gain and language

    dlb_adm_bool isEmissionProfile;
    status = ::dlb_adm_core_model_has_profile(coreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE, &isEmissionProfile);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_TRUE, isEmissionProfile);

    status = ::dlb_adm_core_model_get_presentation_data(&mPresentationData, coreModel, presentationID1);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(2, mPresentationData.element_count);
    EXPECT_EQ(presentationID1, mPresentationData.presentation.id);

    EXPECT_EQ(mPresentationData.presentation.loudness.loudness_type, DLB_ADM_LOUDNESS_TYPE_DIALOGUE);
    EXPECT_FLOAT_EQ(mPresentationData.presentation.loudness.loudness_value, -28.75);

    EXPECT_EQ(mPresentationData.content_groups[0].loudness.loudness_type, DLB_ADM_LOUDNESS_TYPE_INTEGRATED);
    EXPECT_FLOAT_EQ(mPresentationData.content_groups[0].loudness.loudness_value, -30.50);

    EXPECT_EQ(mPresentationData.content_groups[1].loudness.loudness_type, DLB_ADM_LOUDNESS_TYPE_DIALOGUE);
    EXPECT_FLOAT_EQ(mPresentationData.content_groups[1].loudness.loudness_value, -33.50);

    status = ::dlb_adm_core_model_get_presentation_data(&mPresentationData, coreModel, presentationID4);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(3, mPresentationData.element_count);
    EXPECT_EQ(presentationID4, mPresentationData.presentation.id);

    EXPECT_EQ(AltValSetID11, mPresentationData.alt_val_sets[0].id);
    EXPECT_EQ(DLB_ADM_FALSE, mPresentationData.alt_val_sets[0].has_position_offset);
    EXPECT_EQ(DLB_ADM_TRUE, mPresentationData.alt_val_sets[0].has_gain);
    EXPECT_EQ(DLB_ADM_GAIN_UNIT_DB, mPresentationData.alt_val_sets[0].gain.gain_unit);
    EXPECT_FLOAT_EQ(-1.5, mPresentationData.alt_val_sets[0].gain.gain_value);

    EXPECT_EQ(AltValSetID21, mPresentationData.alt_val_sets[1].id);
    EXPECT_EQ(DLB_ADM_TRUE, mPresentationData.alt_val_sets[1].has_position_offset);
    EXPECT_FLOAT_EQ(-0.64, mPresentationData.alt_val_sets[1].position[0]);
    EXPECT_FLOAT_EQ(0, mPresentationData.alt_val_sets[1].position[1]);
    EXPECT_FLOAT_EQ(0, mPresentationData.alt_val_sets[1].position[2]);
    EXPECT_EQ(DLB_ADM_FALSE, mPresentationData.alt_val_sets[1].cartesian);

    status = ::dlb_adm_core_model_get_names(coreModel, &mNames, objectId1);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(4, mNames.name_count);
    EXPECT_EQ(3, mNames.label_count);
    EXPECT_EQ(0, ::strcmp("audioObject_1", mNames.names[0]));
    EXPECT_EQ(0, ::strcmp("", mNames.langs[0]));
    EXPECT_EQ(0, ::strcmp("English", mNames.names[1]));
    EXPECT_EQ(0, ::strcmp("eng", mNames.langs[1]));
    EXPECT_EQ(0, ::strcmp("Ingles", mNames.names[2]));
    EXPECT_EQ(0, ::strcmp("spa", mNames.langs[2]));
    EXPECT_EQ(0, ::strcmp("Yīngyu", mNames.names[3]));
    EXPECT_EQ(0, ::strcmp("zho", mNames.langs[3]));

    status = ::dlb_adm_core_model_get_element_data(&mElementData, coreModel, objectId1);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(6, mElementData.channel_count);
    EXPECT_EQ(objectId1, mElementData.audio_element.id);
    EXPECT_EQ(2, mElementData.alt_val_count);
    EXPECT_EQ(MAX_SADM_ALT_VALUE_SETS, mElementData.alt_val_capacity);

    EXPECT_EQ(AltValSetID11, mElementData.alt_val_sets[0].id);
    EXPECT_EQ(DLB_ADM_FALSE, mElementData.alt_val_sets[0].has_position_offset);
    EXPECT_EQ(DLB_ADM_TRUE, mElementData.alt_val_sets[0].has_gain);
    EXPECT_EQ(DLB_ADM_GAIN_UNIT_DB, mElementData.alt_val_sets[0].gain.gain_unit);
    EXPECT_FLOAT_EQ(-1.5, mElementData.alt_val_sets[0].gain.gain_value);

    EXPECT_EQ(AltValSetID12, mElementData.alt_val_sets[1].id);
    EXPECT_EQ(DLB_ADM_FALSE, mElementData.alt_val_sets[1].has_position_offset);
    EXPECT_EQ(DLB_ADM_TRUE, mElementData.alt_val_sets[1].has_gain);
    EXPECT_EQ(DLB_ADM_GAIN_UNIT_DB, mElementData.alt_val_sets[1].gain.gain_unit);
    EXPECT_FLOAT_EQ(-2.2, mElementData.alt_val_sets[1].gain.gain_value);

}

/* Check basic configuration read and write */
TEST_F(DlbAdmEmissionProfile, multipleProgrammes)
{
    int status;

    status = ::dlb_adm_container_read_xml_file(oryginalContainer, multipleProgrammesFileName, DLB_ADM_TRUE);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // Flattening ADM structure
    status = ::dlb_adm_container_flatten(oryginalContainer, flattenedContainer);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_container_write_xml_file(flattenedContainer, multipleProgrammesOutFileName);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // ingest into the core model
    status = dlb_adm_core_model_open_from_xml_container(&coreModel, flattenedContainer);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // Check if content has been flattend
    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_PROGRAMME, &count);
    ASSERT_EQ(count, 4);

    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_CONTENT, &count);
    ASSERT_EQ(count, 5);

    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_OBJECT, &count);
    ASSERT_EQ(count, 5);

    // TODO: Extend checking: programs, contents and objects Id, gain and language

    dlb_adm_bool isEmissionProfile;
    status = ::dlb_adm_core_model_has_profile(coreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE, &isEmissionProfile);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_TRUE, isEmissionProfile);
}

/* Check basic configuration read and write */
TEST_F(DlbAdmEmissionProfile, complementaryObjectsFlatten)
{
    int status;

    status = ::dlb_adm_container_read_xml_file(oryginalContainer, complementaryObjectsFileName, DLB_ADM_TRUE);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // Flattening ADM structure
    status = ::dlb_adm_container_flatten(oryginalContainer, flattenedContainer);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_container_write_xml_file(flattenedContainer, complementaryObjectsOutFileName);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // ingest into the core model
    status = dlb_adm_core_model_open_from_xml_container(&coreModel, flattenedContainer);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // Check if content has been flattend
    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_PROGRAMME, &count);
    ASSERT_EQ(count, 4);

    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_CONTENT, &count);
    ASSERT_EQ(count, 5);

    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_OBJECT, &count);
    ASSERT_EQ(count, 5);

    // TODO: Extend checking: programs, contents and objects Id, gain and language

    dlb_adm_bool isEmissionProfile;
    status = ::dlb_adm_core_model_has_profile(coreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE, &isEmissionProfile);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_TRUE, isEmissionProfile);
}

/* Check basic configuration read and write */
TEST_F(DlbAdmEmissionProfile, complementaryObjects)
{
    int status;

    status = ::dlb_adm_container_read_xml_file(oryginalContainer, complementaryObjectsFileName, DLB_ADM_TRUE);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_container_write_xml_file(oryginalContainer, complementaryObjectsOutFileName);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // ingest into the core model
    status = dlb_adm_core_model_open_from_xml_container(&coreModel, oryginalContainer);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // Check if content has been flattend
    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_PROGRAMME, &count);
    ASSERT_EQ(count, 1);

    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_CONTENT, &count);
    ASSERT_EQ(count, 5);

    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_OBJECT, &count);
    ASSERT_EQ(count, 5);

    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_COMPLEMENTARY_OBJECT_REF, &count);
    ASSERT_EQ(count, 4);

    dlb_adm_bool isEmissionProfile;
    status = ::dlb_adm_core_model_has_profile(coreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE, &isEmissionProfile);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_TRUE, isEmissionProfile);
}

/* Check basic configuration read and write */
TEST_F(DlbAdmEmissionProfile, adAndSpokenSubtitles)
{
    int status;

    status = ::dlb_adm_container_read_xml_file(oryginalContainer, adAndSpokenSubtitlesFileName, DLB_ADM_TRUE);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // Flattening ADM structure
    status = ::dlb_adm_container_flatten(oryginalContainer, flattenedContainer);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_container_write_xml_file(flattenedContainer, adAndSpokenSubtitlesOutFileName);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // ingest into the core model
    status = dlb_adm_core_model_open_from_xml_container(&coreModel, flattenedContainer);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // Check if content has been flattend
    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_PROGRAMME, &count);
    ASSERT_EQ(count, 4);

    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_CONTENT, &count);
    ASSERT_EQ(count, 6);

    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_OBJECT, &count);
    ASSERT_EQ(count, 6);

    // TODO: Extend checking: programs, contents and objects Id, gain and language

    dlb_adm_bool isEmissionProfile;
    status = ::dlb_adm_core_model_has_profile(coreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE, &isEmissionProfile);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_TRUE, isEmissionProfile);
}

/* Check basic configuration read and write */
TEST_F(DlbAdmEmissionProfile, complementaryMultichannelObjects)
{
    int status;

    status = ::dlb_adm_container_read_xml_file(oryginalContainer, complementaryMultichannelObjectsFileName, DLB_ADM_TRUE);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_container_flatten(oryginalContainer, flattenedContainer);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_container_write_xml_file(flattenedContainer, complementaryMultichannelObjectsOutFileName);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // ingest into the core model
    status = dlb_adm_core_model_open_from_xml_container(&coreModel, flattenedContainer);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // Check if content has been flattend
    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_PROGRAMME, &count);
    ASSERT_EQ(count, 2);

    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_CONTENT, &count);
    ASSERT_EQ(count, 3);

    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_OBJECT, &count);
    ASSERT_EQ(count, 3);

    // TODO: Extend checking: programs, contents and objects Id, gain and language

    dlb_adm_bool isEmissionProfile;
    status = ::dlb_adm_core_model_has_profile(coreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE, &isEmissionProfile);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_TRUE, isEmissionProfile);
}

/* Check basic configuration read and write */
TEST_F(DlbAdmEmissionProfile, multipleProgrammesUsingMonoObjects)
{
    int status;

    status = ::dlb_adm_container_read_xml_file(oryginalContainer, multipleProgrammesUsingMonoObjectsFileName, DLB_ADM_TRUE);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // Flattening ADM structure
    status = ::dlb_adm_container_flatten(oryginalContainer, flattenedContainer);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_container_write_xml_file(flattenedContainer, multipleProgrammesUsingMonoObjectsOutFileName);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // ingest into the core model
    status = dlb_adm_core_model_open_from_xml_container(&coreModel, flattenedContainer);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // Check if content has been flattend
    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_PROGRAMME, &count);
    ASSERT_EQ(count, 2);

    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_CONTENT, &count);
    ASSERT_EQ(count, 7);

    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_OBJECT, &count);
    ASSERT_EQ(count, 7);

    // TODO: Extend checking: programs, contents and objects Id, gain and language

    dlb_adm_bool isEmissionProfile;
    status = ::dlb_adm_core_model_has_profile(coreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE, &isEmissionProfile);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_TRUE, isEmissionProfile);
}

/* Common definitions should not be present in output xml file */
TEST_F(DlbAdmEmissionProfile, noCommonDefinitionsInOutputXML)
{
    int status;
    dlb_adm_xml_container *ingestedContainer = flattenedContainer; // alias

    // read minimal XML file, just to load common defs to XML Container
    status = ::dlb_adm_container_read_xml_file(oryginalContainer, minimalFileName, DLB_ADM_TRUE);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // ingest into the core model
    status = dlb_adm_core_model_open_from_xml_container(&coreModel, oryginalContainer);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // generate XML to another container
    status = ::dlb_adm_container_open_from_core_model(&ingestedContainer, coreModel);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // save XML to file, Common defs should not be saved
    status = ::dlb_adm_container_write_xml_file(ingestedContainer, minimalOutFileName);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // clear container and core model
    status = ::dlb_adm_container_close(&oryginalContainer);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_core_model_clear(coreModel);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // read just created file to XML container (without common definitions)
    status = ::dlb_adm_container_open(&oryginalContainer, &containerCounts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_container_read_xml_file(oryginalContainer, minimalOutFileName, DLB_ADM_FALSE);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // ingest into the core model
    status = dlb_adm_core_model_open_from_xml_container(&coreModel, oryginalContainer);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // check if any common definition is present
    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_PACK_FORMAT, &count);
    EXPECT_EQ(count, 0);

    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_CHANNEL_FORMAT, &count);
    EXPECT_EQ(count, 0);

    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_BLOCK_FORMAT, &count);
    EXPECT_EQ(count, 0);
}

/* Check audio object interaction read and write */
TEST_F(DlbAdmEmissionProfile, audioObjectInteraction)
{
    int status;

    status = ::dlb_adm_container_read_xml_file(oryginalContainer, audioObjectInteractionName, DLB_ADM_TRUE);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_container_write_xml_file(oryginalContainer, audioObjectInteractionOutFileName);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // Flattening ADM structure
    status = ::dlb_adm_container_flatten(oryginalContainer, flattenedContainer);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_container_write_xml_file(flattenedContainer, audioObjectInteractionFlattenedOutFileName);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // ingest into the core model
    status = dlb_adm_core_model_open_from_xml_container(&coreModel, flattenedContainer);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_container_close(&flattenedContainer);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_container_open(&flattenedContainer, &containerCounts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_container_read_xml_file(flattenedContainer, audioObjectInteractionFlattenedOutFileName, DLB_ADM_TRUE);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
}
