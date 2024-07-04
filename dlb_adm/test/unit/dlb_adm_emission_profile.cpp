/************************************************************************
 * dlb_adm
 * Copyright (c) 2022-2024, Dolby Laboratories Inc.
 * Copyright (c) 2022-2024, Dolby International AB.
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
#include <vector>

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

static const char BroadcastMixName[] = "broadcastMixXMLBuffer.xml";
static const char BroadcastMixOutFileName[] = "broadcastMixXMLBuffer.out.xml";

static const char multipleComplementaryName[] = "multipleComplementary.xml";
static const char multipleComplementaryFileName[] = "multipleComplementary.out.xml";

static const char complementaryAndAvsName[] = "complementaryAndAvs.xml";
static const char complementaryAndAvsFileName[] = "complementaryAndAvs.out.xml";

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
        SetUpTestInput(BroadcastMixName, broadcastMixXMLBuffer);
        SetUpTestInput(multipleComplementaryName, multipleComplementaryBuffer);
        SetUpTestInput(complementaryAndAvsName, complementaryAndAvsBuffer);

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

    /* dlb_adm_container_open_from_core_model will open new container */
    status = dlb_adm_container_close(&flattenedContainer);
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

    status = ::dlb_adm_container_close(&ingestedContainer);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_core_model_close(&coreModel);
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

TEST_F(DlbAdmEmissionProfile, parseBroacastMix)
{
    int status;

    // set up ids
    static const char presentationIDString1[] = "APR_1001";
    dlb_adm_entity_id presentationID1;
    status = ::dlb_adm_read_entity_id(&presentationID1, presentationIDString1, ::strlen(presentationIDString1) + 1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    ASSERT_NE(DLB_ADM_NULL_ENTITY_ID, presentationID1);

    static const char presentationIDString2[] = "APR_1002";
    dlb_adm_entity_id presentationID2;
    status = ::dlb_adm_read_entity_id(&presentationID2, presentationIDString2, ::strlen(presentationIDString2) + 1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    ASSERT_NE(DLB_ADM_NULL_ENTITY_ID, presentationID2);

    static const char objectIDString[] = "AO_1001";
    dlb_adm_entity_id objectId1;
    status = ::dlb_adm_read_entity_id(&objectId1, objectIDString, ::strlen(objectIDString) + 1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    ASSERT_NE(DLB_ADM_NULL_ENTITY_ID, objectId1);

    static const char objectIDString2[] = "AO_1002";
    dlb_adm_entity_id objectId2;
    status = ::dlb_adm_read_entity_id(&objectId2, objectIDString2, ::strlen(objectIDString2) + 1);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    ASSERT_NE(DLB_ADM_NULL_ENTITY_ID, objectId2);

    /* load S-ADM to Core Model*/
    status = ::dlb_adm_container_read_xml_file(oryginalContainer, BroadcastMixName, DLB_ADM_TRUE);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // ingest into the core model
    status = dlb_adm_core_model_open_from_xml_container(&coreModel, oryginalContainer);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    /* Validate Core Model */
    dlb_adm_bool isEmissionProfile;
    status = ::dlb_adm_core_model_has_profile(coreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE, &isEmissionProfile);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_TRUE, isEmissionProfile);

    status = ::dlb_adm_core_model_get_presentation_data(&mPresentationData, coreModel, presentationID1);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(1, mPresentationData.element_count);
    EXPECT_EQ(presentationID1, mPresentationData.presentation.id);
    EXPECT_EQ(DLB_ADM_CONTENT_KIND_MK_MIXED, mPresentationData.content_groups[0].content_kind);

    status = ::dlb_adm_core_model_get_presentation_data(&mPresentationData, coreModel, presentationID2);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(1, mPresentationData.element_count);
    EXPECT_EQ(presentationID2, mPresentationData.presentation.id);
    EXPECT_EQ(DLB_ADM_CONTENT_KIND_MK_VISUALLY_IMPAIRED, mPresentationData.content_groups[0].content_kind);

    status = ::dlb_adm_core_model_get_element_data(&mElementData, coreModel, objectId1);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(6, mElementData.channel_count);
    EXPECT_EQ(objectId1, mElementData.audio_element.id);
    EXPECT_EQ(0, mElementData.alt_val_count);

    status = ::dlb_adm_core_model_get_element_data(&mElementData, coreModel, objectId2);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(2, mElementData.channel_count);
    EXPECT_EQ(objectId2, mElementData.audio_element.id);
    EXPECT_EQ(0, mElementData.alt_val_count);

    /* Write S-ADM from Core Model */
    status = ::dlb_adm_container_write_xml_file(oryginalContainer, BroadcastMixOutFileName);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    /* Compare received and resulted S-ADMs*/
    status = CompareFiles(BroadcastMixName, BroadcastMixOutFileName);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
}

static bool GetId(const std::string& strId, dlb_adm_entity_id& id)
{
    dlb_adm_entity_id tempId = DLB_ADM_NULL_ENTITY_ID;
    int status = DLB_ADM_STATUS_OK;
    status = ::dlb_adm_read_entity_id(&tempId, strId.c_str(), strId.size()+1);
    if(status != DLB_ADM_STATUS_OK || tempId == DLB_ADM_NULL_ENTITY_ID)
    {
        return false;
    }
    id = tempId;
    return true;
}

static bool prepareIds(const std::vector<std::string>& stringIds, std::vector<dlb_adm_entity_id>& ids)
{
    dlb_adm_entity_id tempId = DLB_ADM_NULL_ENTITY_ID;
    for(auto stringId : stringIds)
    {
        if(!GetId(stringId, tempId))
        {
            return false;
        }
        ids.push_back(tempId);
    }
    return true; 
};

TEST_F(DlbAdmEmissionProfile, flattenComplementary_complementaryObjectsFlatten)
{
    int status;

    status = ::dlb_adm_container_read_xml_file(oryginalContainer, complementaryObjectsFileName, DLB_ADM_TRUE);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // Flattening ADM structure
    status = ::dlb_adm_container_flatten_complementary(oryginalContainer, flattenedContainer);
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

    dlb_adm_bool isEmissionProfile;
    status = ::dlb_adm_core_model_has_profile(coreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE, &isEmissionProfile);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_TRUE, isEmissionProfile);

    std::vector<std::string>presentationIdStrings{"APR_1001","APR_1002","APR_1003","APR_1004"};
    std::vector<std::string>contentIdStrings{"ACO_1001","ACO_1002","ACO_1003","ACO_1004","ACO_1005"};
    std::vector<std::string>objectIdStrings{"AO_1001","AO_1002","AO_1003","AO_1004","AO_1005"};
    std::vector<std::string>trackIdStrings{"ATU_00000001","ATU_00000007","ATU_00000008","ATU_00000009","ATU_0000000a"};

    std::vector<dlb_adm_entity_id>presentationIds;
    std::vector<dlb_adm_entity_id>contentIds;
    std::vector<dlb_adm_entity_id>objectIds;
    std::vector<dlb_adm_entity_id>trackIds;

    ASSERT_TRUE(prepareIds(presentationIdStrings, presentationIds));
    ASSERT_TRUE(prepareIds(contentIdStrings, contentIds));
    ASSERT_TRUE(prepareIds(objectIdStrings, objectIds));
    ASSERT_TRUE(prepareIds(trackIdStrings, trackIds));

    status = ::dlb_adm_core_model_get_presentation_data(&mPresentationData, coreModel, presentationIds[0]);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(2, mPresentationData.element_count);
    EXPECT_EQ(presentationIds[0], mPresentationData.presentation.id);
    EXPECT_EQ(DLB_ADM_CONTENT_KIND_NK_UNDEFINED, mPresentationData.content_groups[0].content_kind);
    EXPECT_EQ(contentIds[0], mPresentationData.content_groups[0].id);
    EXPECT_EQ(objectIds[0], mPresentationData.audio_elements[0].id);
    EXPECT_EQ(DLB_ADM_CONTENT_KIND_DK_DIALOGUE, mPresentationData.content_groups[1].content_kind);
    EXPECT_EQ(contentIds[1], mPresentationData.content_groups[1].id);
    EXPECT_EQ(objectIds[1], mPresentationData.audio_elements[1].id);
    status = ::dlb_adm_core_model_get_names(coreModel, &mNames, presentationIds[0]);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(5, mNames.name_count);
    EXPECT_EQ(4, mNames.label_count);
    EXPECT_EQ(0, ::strcmp("audioProgramme_1", mNames.names[0]));
    EXPECT_EQ(0, ::strcmp("eng", mNames.langs[0]));
    EXPECT_EQ(0, ::strcmp("English", mNames.names[1]));
    EXPECT_EQ(0, ::strcmp("eng", mNames.langs[1]));
    EXPECT_EQ(0, ::strcmp("Anglais", mNames.names[2]));
    EXPECT_EQ(0, ::strcmp("fre", mNames.langs[2]));
    EXPECT_EQ(0, ::strcmp("Inglese", mNames.names[3]));
    EXPECT_EQ(0, ::strcmp("ita", mNames.langs[3]));
    EXPECT_EQ(0, ::strcmp("Englisch", mNames.names[4]));
    EXPECT_EQ(0, ::strcmp("ger", mNames.langs[4]));

    status = ::dlb_adm_core_model_get_presentation_data(&mPresentationData, coreModel, presentationIds[1]);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(2, mPresentationData.element_count);
    EXPECT_EQ(presentationIds[1], mPresentationData.presentation.id);
    EXPECT_EQ(DLB_ADM_CONTENT_KIND_NK_UNDEFINED, mPresentationData.content_groups[0].content_kind);
    EXPECT_EQ(contentIds[0], mPresentationData.content_groups[0].id);
    EXPECT_EQ(objectIds[0], mPresentationData.audio_elements[0].id);
    EXPECT_EQ(DLB_ADM_CONTENT_KIND_DK_DIALOGUE, mPresentationData.content_groups[1].content_kind);
    EXPECT_EQ(contentIds[2], mPresentationData.content_groups[1].id);
    EXPECT_EQ(objectIds[2], mPresentationData.audio_elements[1].id);
    status = ::dlb_adm_core_model_get_names(coreModel, &mNames, presentationIds[1]);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(5, mNames.name_count);
    EXPECT_EQ(4, mNames.label_count);
    EXPECT_EQ(0, ::strcmp("audioProgramme_1", mNames.names[0]));
    EXPECT_EQ(0, ::strcmp("fre", mNames.langs[0]));
    EXPECT_EQ(0, ::strcmp("English", mNames.names[1]));
    EXPECT_EQ(0, ::strcmp("eng", mNames.langs[1]));
    EXPECT_EQ(0, ::strcmp("Anglais", mNames.names[2]));
    EXPECT_EQ(0, ::strcmp("fre", mNames.langs[2]));
    EXPECT_EQ(0, ::strcmp("Inglese", mNames.names[3]));
    EXPECT_EQ(0, ::strcmp("ita", mNames.langs[3]));
    EXPECT_EQ(0, ::strcmp("Englisch", mNames.names[4]));
    EXPECT_EQ(0, ::strcmp("ger", mNames.langs[4]));

    status = ::dlb_adm_core_model_get_presentation_data(&mPresentationData, coreModel, presentationIds[2]);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(2, mPresentationData.element_count);
    EXPECT_EQ(presentationIds[2], mPresentationData.presentation.id);
    EXPECT_EQ(DLB_ADM_CONTENT_KIND_NK_UNDEFINED, mPresentationData.content_groups[0].content_kind);
    EXPECT_EQ(contentIds[0], mPresentationData.content_groups[0].id);
    EXPECT_EQ(objectIds[0], mPresentationData.audio_elements[0].id);
    EXPECT_EQ(DLB_ADM_CONTENT_KIND_DK_DIALOGUE, mPresentationData.content_groups[1].content_kind);
    EXPECT_EQ(contentIds[3], mPresentationData.content_groups[1].id);
    EXPECT_EQ(objectIds[3], mPresentationData.audio_elements[1].id);
    status = ::dlb_adm_core_model_get_names(coreModel, &mNames, presentationIds[2]);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(5, mNames.name_count);
    EXPECT_EQ(4, mNames.label_count);
    EXPECT_EQ(0, ::strcmp("audioProgramme_1", mNames.names[0]));
    EXPECT_EQ(0, ::strcmp("ita", mNames.langs[0]));
    EXPECT_EQ(0, ::strcmp("English", mNames.names[1]));
    EXPECT_EQ(0, ::strcmp("eng", mNames.langs[1]));
    EXPECT_EQ(0, ::strcmp("Anglais", mNames.names[2]));
    EXPECT_EQ(0, ::strcmp("fre", mNames.langs[2]));
    EXPECT_EQ(0, ::strcmp("Inglese", mNames.names[3]));
    EXPECT_EQ(0, ::strcmp("ita", mNames.langs[3]));
    EXPECT_EQ(0, ::strcmp("Englisch", mNames.names[4]));
    EXPECT_EQ(0, ::strcmp("ger", mNames.langs[4]));

    status = ::dlb_adm_core_model_get_presentation_data(&mPresentationData, coreModel, presentationIds[3]);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(2, mPresentationData.element_count);
    EXPECT_EQ(presentationIds[3], mPresentationData.presentation.id);
    EXPECT_EQ(DLB_ADM_CONTENT_KIND_NK_UNDEFINED, mPresentationData.content_groups[0].content_kind);
    EXPECT_EQ(contentIds[0], mPresentationData.content_groups[0].id);
    EXPECT_EQ(objectIds[0], mPresentationData.audio_elements[0].id);
    EXPECT_EQ(DLB_ADM_CONTENT_KIND_DK_DIALOGUE, mPresentationData.content_groups[1].content_kind);
    EXPECT_EQ(contentIds[4], mPresentationData.content_groups[1].id);
    EXPECT_EQ(objectIds[4], mPresentationData.audio_elements[1].id);
    status = ::dlb_adm_core_model_get_names(coreModel, &mNames, presentationIds[3]);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(5, mNames.name_count);
    EXPECT_EQ(4, mNames.label_count);
    EXPECT_EQ(0, ::strcmp("audioProgramme_1", mNames.names[0]));
    EXPECT_EQ(0, ::strcmp("ger", mNames.langs[0]));
    EXPECT_EQ(0, ::strcmp("English", mNames.names[1]));
    EXPECT_EQ(0, ::strcmp("eng", mNames.langs[1]));
    EXPECT_EQ(0, ::strcmp("Anglais", mNames.names[2]));
    EXPECT_EQ(0, ::strcmp("fre", mNames.langs[2]));
    EXPECT_EQ(0, ::strcmp("Inglese", mNames.names[3]));
    EXPECT_EQ(0, ::strcmp("ita", mNames.langs[3]));
    EXPECT_EQ(0, ::strcmp("Englisch", mNames.names[4]));
    EXPECT_EQ(0, ::strcmp("ger", mNames.langs[4]));

    status = ::dlb_adm_core_model_get_element_data(&mElementData, coreModel, objectIds[0]);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(6, mElementData.channel_count);
    EXPECT_EQ(objectIds[0], mElementData.audio_element.id);
    EXPECT_EQ(0, mElementData.alt_val_count);
    EXPECT_EQ(trackIds[0], mElementData.audio_tracks->id);

    status = ::dlb_adm_core_model_get_element_data(&mElementData, coreModel, objectIds[1]);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(1, mElementData.channel_count);
    EXPECT_EQ(objectIds[1], mElementData.audio_element.id);
    EXPECT_EQ(0, mElementData.alt_val_count);
    EXPECT_EQ(DLB_ADM_AUDIO_TYPE_OBJECTS, mElementData.targets->audio_type);
    EXPECT_EQ(trackIds[1], mElementData.audio_tracks->id);

    status = ::dlb_adm_core_model_get_element_data(&mElementData, coreModel, objectIds[2]);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(1, mElementData.channel_count);
    EXPECT_EQ(objectIds[2], mElementData.audio_element.id);
    EXPECT_EQ(0, mElementData.alt_val_count);
    EXPECT_EQ(DLB_ADM_AUDIO_TYPE_OBJECTS, mElementData.targets->audio_type);
    EXPECT_EQ(trackIds[2], mElementData.audio_tracks->id);

    status = ::dlb_adm_core_model_get_element_data(&mElementData, coreModel, objectIds[3]);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(1, mElementData.channel_count);
    EXPECT_EQ(objectIds[3], mElementData.audio_element.id);
    EXPECT_EQ(0, mElementData.alt_val_count);
    EXPECT_EQ(DLB_ADM_AUDIO_TYPE_OBJECTS, mElementData.targets->audio_type);
    EXPECT_EQ(trackIds[3], mElementData.audio_tracks->id);

    status = ::dlb_adm_core_model_get_element_data(&mElementData, coreModel, objectIds[4]);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(1, mElementData.channel_count);
    EXPECT_EQ(objectIds[4], mElementData.audio_element.id);
    EXPECT_EQ(0, mElementData.alt_val_count);
    EXPECT_EQ(DLB_ADM_AUDIO_TYPE_OBJECTS, mElementData.targets->audio_type);
    EXPECT_EQ(trackIds[4], mElementData.audio_tracks->id);
}

void validatePresentation( dlb_adm_data_presentation_data* presData
                         , dlb_adm_data_names* namesData
                         , dlb_adm_core_model* coreModel
                         , std::string presIdString
                         , std::string contentIdString_1
                         , std::string contentIdString_2
                         , std::string objIdString_1
                         , std::string objIdString_2
                         , std::string programmeName
                         , std::string programmeLang
                         , std::string programmeLabel
                         , std::string programmeLabelLang)
{
    dlb_adm_entity_id presId;
    dlb_adm_entity_id contentId_1;
    dlb_adm_entity_id contentId_2;
    dlb_adm_entity_id objId_1;
    dlb_adm_entity_id objId_2;

    ASSERT_TRUE(GetId(presIdString, presId));
    ASSERT_TRUE(GetId(contentIdString_1, contentId_1));
    ASSERT_TRUE(GetId(contentIdString_2, contentId_2));
    ASSERT_TRUE(GetId(objIdString_1, objId_1));
    ASSERT_TRUE(GetId(objIdString_2, objId_2));
    int status = ::dlb_adm_core_model_get_presentation_data(presData, coreModel, presId);
    EXPECT_EQ(status, DLB_ADM_STATUS_OK);
    EXPECT_EQ(presData->element_count, 2);
    EXPECT_EQ(presData->presentation.id, presId);
    EXPECT_EQ(presData->content_groups[0].content_kind, DLB_ADM_CONTENT_KIND_NK_EFFECTS);
    EXPECT_EQ(presData->content_groups[0].id, contentId_1);
    EXPECT_EQ(presData->audio_elements[0].id, objId_1);
    EXPECT_EQ(presData->alt_val_sets[0].id, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(presData->content_groups[1].content_kind, DLB_ADM_CONTENT_KIND_DK_DIALOGUE);
    EXPECT_EQ(presData->content_groups[1].id, contentId_2);
    EXPECT_EQ(presData->audio_elements[1].id, objId_2);
    EXPECT_EQ(presData->alt_val_sets[1].id, DLB_ADM_NULL_ENTITY_ID);
    status = ::dlb_adm_core_model_get_names(coreModel, namesData, presId);
    EXPECT_EQ(status, DLB_ADM_STATUS_OK);
    EXPECT_EQ(namesData->name_count, 2);
    EXPECT_EQ(namesData->label_count, 1);
    EXPECT_EQ(0, ::strcmp(programmeName.c_str(), namesData->names[0]));
    EXPECT_EQ(0, ::strcmp(programmeLang.c_str(), namesData->langs[0]));
    EXPECT_EQ(0, ::strcmp(programmeLabel.c_str(), namesData->names[1]));
    EXPECT_EQ(0, ::strcmp(programmeLabelLang.c_str(), namesData->langs[1]));
}

TEST_F(DlbAdmEmissionProfile, flattenComplementary_multipleComplementary)
{
    int status;

    status = ::dlb_adm_container_read_xml_file(oryginalContainer, multipleComplementaryName, DLB_ADM_TRUE);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // Flattening ADM structure
    status = ::dlb_adm_container_flatten_complementary(oryginalContainer, flattenedContainer);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_container_write_xml_file(flattenedContainer, multipleComplementaryFileName);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // ingest into the core model
    status = dlb_adm_core_model_open_from_xml_container(&coreModel, flattenedContainer);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // Check if content has been flattend
    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_PROGRAMME, &count);
    ASSERT_EQ(count, 15);

    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_CONTENT, &count);
    ASSERT_EQ(count, 7);

    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_OBJECT, &count);
    ASSERT_EQ(count, 7);

    // TODO: Extend checking: programs, contents and objects Id, gain and language

    dlb_adm_bool isEmissionProfile;
    status = ::dlb_adm_core_model_has_profile(coreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE, &isEmissionProfile);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_TRUE, isEmissionProfile);

    validatePresentation( &mPresentationData
                        , &mNames
                        , coreModel
                        , std::string("APR_1001")
                        , std::string("ACO_1001")
                        , std::string("ACO_1004")
                        , std::string("AO_1001")
                        , std::string("AO_1004")
                        , std::string("audioProgramme_1")
                        , std::string("eng")
                        , std::string("Standard")
                        , std::string("eng"));

    validatePresentation( &mPresentationData
                        , &mNames
                        , coreModel
                        , std::string("APR_1002")
                        , std::string("ACO_1001")
                        , std::string("ACO_1005")
                        , std::string("AO_1001")
                        , std::string("AO_1005")
                        , std::string("audioProgramme_1")
                        , std::string("ger")
                        , std::string("Standard")
                        , std::string("eng"));

    validatePresentation( &mPresentationData
                        , &mNames
                        , coreModel
                        , std::string("APR_1003")
                        , std::string("ACO_1001")
                        , std::string("ACO_1006")
                        , std::string("AO_1001")
                        , std::string("AO_1006")
                        , std::string("audioProgramme_1")
                        , std::string("ita")
                        , std::string("Standard")
                        , std::string("eng"));

    validatePresentation( &mPresentationData
                        , &mNames
                        , coreModel
                        , std::string("APR_1004")
                        , std::string("ACO_1002")
                        , std::string("ACO_1004")
                        , std::string("AO_1002")
                        , std::string("AO_1004")
                        , std::string("audioProgramme_1")
                        , std::string("eng")
                        , std::string("Standard")
                        , std::string("eng"));
}

TEST_F(DlbAdmEmissionProfile, flattenComplementary_complementaryAndAvs)
{
    int status;

    status = ::dlb_adm_container_read_xml_file(oryginalContainer, complementaryAndAvsName, DLB_ADM_TRUE);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // Flattening ADM structure
    status = ::dlb_adm_container_flatten_complementary(oryginalContainer, flattenedContainer);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_container_write_xml_file(flattenedContainer, complementaryAndAvsFileName);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // ingest into the core model
    status = dlb_adm_core_model_open_from_xml_container(&coreModel, flattenedContainer);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // Check if content has been flattend
    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_PROGRAMME, &count);
    ASSERT_EQ(count, 6);

    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_CONTENT, &count);
    ASSERT_EQ(count, 3);

    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_OBJECT, &count);
    ASSERT_EQ(count, 3);

    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_ALT_VALUE_SET, &count);
    ASSERT_EQ(count, 4);

    dlb_adm_bool isEmissionProfile;
    status = ::dlb_adm_core_model_has_profile(coreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE, &isEmissionProfile);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_TRUE, isEmissionProfile);

    std::vector<std::string>presentationIdStrings{"APR_1001","APR_1002","APR_1003","APR_1004","APR_1005","APR_1006"};
    std::vector<std::string>contentIdStrings{"ACO_1001","ACO_1002","ACO_1003"};
    std::vector<std::string>objectIdStrings{"AO_1001","AO_1002","AO_1003"};
    std::vector<std::string>trackIdStrings{"ATU_00000001","ATU_00000007","ATU_00000008"};
    std::vector<std::string>avsIdStrings{"AVS_1002_0001","AVS_1002_0002","AVS_1003_0001","AVS_1003_0002"};

    std::vector<dlb_adm_entity_id>presentationIds;
    std::vector<dlb_adm_entity_id>contentIds;
    std::vector<dlb_adm_entity_id>objectIds;
    std::vector<dlb_adm_entity_id>trackIds;
    std::vector<dlb_adm_entity_id>avsIds;

    ASSERT_TRUE(prepareIds(presentationIdStrings, presentationIds));
    ASSERT_TRUE(prepareIds(contentIdStrings, contentIds));
    ASSERT_TRUE(prepareIds(objectIdStrings, objectIds));
    ASSERT_TRUE(prepareIds(trackIdStrings, trackIds));
    ASSERT_TRUE(prepareIds(avsIdStrings, avsIds));

    dlb_adm_entity_id presId = presentationIds[0];
    status = ::dlb_adm_core_model_get_presentation_data(&mPresentationData, coreModel, presId);
    EXPECT_EQ(status, DLB_ADM_STATUS_OK);
    EXPECT_EQ(mPresentationData.element_count, 2);
    EXPECT_EQ(mPresentationData.presentation.id, presId);
    EXPECT_EQ(mPresentationData.content_groups[0].content_kind, DLB_ADM_CONTENT_KIND_NK_UNDEFINED);
    EXPECT_EQ(mPresentationData.content_groups[0].id, contentIds[0]);
    EXPECT_EQ(mPresentationData.audio_elements[0].id, objectIds[0]);
    EXPECT_EQ(mPresentationData.alt_val_sets[0].id, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(mPresentationData.content_groups[1].content_kind, DLB_ADM_CONTENT_KIND_DK_DIALOGUE);
    EXPECT_EQ(mPresentationData.content_groups[1].id, contentIds[1]);
    EXPECT_EQ(mPresentationData.audio_elements[1].id, objectIds[1]);
    EXPECT_EQ(mPresentationData.alt_val_sets[1].id, DLB_ADM_NULL_ENTITY_ID);
    status = ::dlb_adm_core_model_get_names(coreModel, &mNames, presId);
    EXPECT_EQ(status, DLB_ADM_STATUS_OK);
    EXPECT_EQ(mNames.name_count, 3);
    EXPECT_EQ(mNames.label_count, 2);
    EXPECT_EQ(0, ::strcmp("audioProgramme_1", mNames.names[0]));
    EXPECT_EQ(0, ::strcmp("eng", mNames.langs[0]));
    EXPECT_EQ(0, ::strcmp("English", mNames.names[1]));
    EXPECT_EQ(0, ::strcmp("eng", mNames.langs[1]));
    EXPECT_EQ(0, ::strcmp("Anglais", mNames.names[2]));
    EXPECT_EQ(0, ::strcmp("fre", mNames.langs[2]));

    presId = presentationIds[1];
    status = ::dlb_adm_core_model_get_presentation_data(&mPresentationData, coreModel, presId);
    EXPECT_EQ(status, DLB_ADM_STATUS_OK);
    EXPECT_EQ(mPresentationData.element_count, 2);
    EXPECT_EQ(mPresentationData.presentation.id, presId);
    EXPECT_EQ(mPresentationData.content_groups[0].content_kind, DLB_ADM_CONTENT_KIND_NK_UNDEFINED);
    EXPECT_EQ(mPresentationData.content_groups[0].id, contentIds[0]);
    EXPECT_EQ(mPresentationData.audio_elements[0].id, objectIds[0]);
    EXPECT_EQ(mPresentationData.alt_val_sets[0].id, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(mPresentationData.content_groups[1].content_kind, DLB_ADM_CONTENT_KIND_DK_DIALOGUE);
    EXPECT_EQ(mPresentationData.content_groups[1].id, contentIds[2]);
    EXPECT_EQ(mPresentationData.audio_elements[1].id, objectIds[2]);
    EXPECT_EQ(mPresentationData.alt_val_sets[1].id, DLB_ADM_NULL_ENTITY_ID);
    status = ::dlb_adm_core_model_get_names(coreModel, &mNames, presId);
    EXPECT_EQ(status, DLB_ADM_STATUS_OK);
    EXPECT_EQ(mNames.name_count, 3);
    EXPECT_EQ(mNames.label_count, 2);
    EXPECT_EQ(0, ::strcmp("audioProgramme_1", mNames.names[0]));
    EXPECT_EQ(0, ::strcmp("fre", mNames.langs[0]));
    EXPECT_EQ(0, ::strcmp("English", mNames.names[1]));
    EXPECT_EQ(0, ::strcmp("eng", mNames.langs[1]));
    EXPECT_EQ(0, ::strcmp("Anglais", mNames.names[2]));
    EXPECT_EQ(0, ::strcmp("fre", mNames.langs[2]));

    presId = presentationIds[2];
    status = ::dlb_adm_core_model_get_presentation_data(&mPresentationData, coreModel, presId);
    EXPECT_EQ(status, DLB_ADM_STATUS_OK);
    EXPECT_EQ(mPresentationData.element_count, 2);
    EXPECT_EQ(mPresentationData.presentation.id, presId);
    EXPECT_EQ(mPresentationData.content_groups[0].content_kind, DLB_ADM_CONTENT_KIND_NK_UNDEFINED);
    EXPECT_EQ(mPresentationData.content_groups[0].id, contentIds[0]);
    EXPECT_EQ(mPresentationData.audio_elements[0].id, objectIds[0]);
    EXPECT_EQ(mPresentationData.alt_val_sets[0].id, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(mPresentationData.content_groups[1].content_kind, DLB_ADM_CONTENT_KIND_DK_DIALOGUE);
    EXPECT_EQ(mPresentationData.content_groups[1].id, contentIds[1]);
    EXPECT_EQ(mPresentationData.audio_elements[1].id, objectIds[1]);
    EXPECT_EQ(mPresentationData.alt_val_sets[1].id, avsIds[0]);
    status = ::dlb_adm_core_model_get_names(coreModel, &mNames, presId);
    EXPECT_EQ(status, DLB_ADM_STATUS_OK);
    EXPECT_EQ(mNames.name_count, 3);
    EXPECT_EQ(mNames.label_count, 2);
    EXPECT_EQ(0, ::strcmp("audioProgramme_2", mNames.names[0]));
    EXPECT_EQ(0, ::strcmp("eng", mNames.langs[0]));
    EXPECT_EQ(0, ::strcmp("English Boosted Dialog", mNames.names[1]));
    EXPECT_EQ(0, ::strcmp("eng", mNames.langs[1]));
    EXPECT_EQ(0, ::strcmp("Anglais Boosted Dialog", mNames.names[2]));
    EXPECT_EQ(0, ::strcmp("fre", mNames.langs[2]));

    presId = presentationIds[3];
    status = ::dlb_adm_core_model_get_presentation_data(&mPresentationData, coreModel, presId);
    EXPECT_EQ(status, DLB_ADM_STATUS_OK);
    EXPECT_EQ(mPresentationData.element_count, 2);
    EXPECT_EQ(mPresentationData.presentation.id, presId);
    EXPECT_EQ(mPresentationData.content_groups[0].content_kind, DLB_ADM_CONTENT_KIND_NK_UNDEFINED);
    EXPECT_EQ(mPresentationData.content_groups[0].id, contentIds[0]);
    EXPECT_EQ(mPresentationData.audio_elements[0].id, objectIds[0]);
    EXPECT_EQ(mPresentationData.alt_val_sets[0].id, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(mPresentationData.content_groups[1].content_kind, DLB_ADM_CONTENT_KIND_DK_DIALOGUE);
    EXPECT_EQ(mPresentationData.content_groups[1].id, contentIds[2]);
    EXPECT_EQ(mPresentationData.audio_elements[1].id, objectIds[2]);
    EXPECT_EQ(mPresentationData.alt_val_sets[1].id, avsIds[2]);
    status = ::dlb_adm_core_model_get_names(coreModel, &mNames, presId);
    EXPECT_EQ(status, DLB_ADM_STATUS_OK);
    EXPECT_EQ(mNames.name_count, 3);
    EXPECT_EQ(mNames.label_count, 2);
    EXPECT_EQ(0, ::strcmp("audioProgramme_2", mNames.names[0]));
    EXPECT_EQ(0, ::strcmp("fre", mNames.langs[0]));
    EXPECT_EQ(0, ::strcmp("English Boosted Dialog", mNames.names[1]));
    EXPECT_EQ(0, ::strcmp("eng", mNames.langs[1]));
    EXPECT_EQ(0, ::strcmp("Anglais Boosted Dialog", mNames.names[2]));
    EXPECT_EQ(0, ::strcmp("fre", mNames.langs[2]));

    presId = presentationIds[4];
    status = ::dlb_adm_core_model_get_presentation_data(&mPresentationData, coreModel, presId);
    EXPECT_EQ(status, DLB_ADM_STATUS_OK);
    EXPECT_EQ(mPresentationData.element_count, 2);
    EXPECT_EQ(mPresentationData.presentation.id, presId);
    EXPECT_EQ(mPresentationData.content_groups[0].content_kind, DLB_ADM_CONTENT_KIND_NK_UNDEFINED);
    EXPECT_EQ(mPresentationData.content_groups[0].id, contentIds[0]);
    EXPECT_EQ(mPresentationData.audio_elements[0].id, objectIds[0]);
    EXPECT_EQ(mPresentationData.alt_val_sets[0].id, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(mPresentationData.content_groups[1].content_kind, DLB_ADM_CONTENT_KIND_DK_DIALOGUE);
    EXPECT_EQ(mPresentationData.content_groups[1].id, contentIds[1]);
    EXPECT_EQ(mPresentationData.audio_elements[1].id, objectIds[1]);
    EXPECT_EQ(mPresentationData.alt_val_sets[1].id, avsIds[1]);
    status = ::dlb_adm_core_model_get_names(coreModel, &mNames, presId);
    EXPECT_EQ(status, DLB_ADM_STATUS_OK);
    EXPECT_EQ(mNames.name_count, 3);
    EXPECT_EQ(mNames.label_count, 2);
    EXPECT_EQ(0, ::strcmp("audioProgramme_3", mNames.names[0]));
    EXPECT_EQ(0, ::strcmp("eng", mNames.langs[0]));
    EXPECT_EQ(0, ::strcmp("English Only Compl Leader with AVS", mNames.names[1]));
    EXPECT_EQ(0, ::strcmp("eng", mNames.langs[1]));
    EXPECT_EQ(0, ::strcmp("Anglais Only Compl Leader with AVS", mNames.names[2]));
    EXPECT_EQ(0, ::strcmp("fre", mNames.langs[2]));

    presId = presentationIds[5];
    status = ::dlb_adm_core_model_get_presentation_data(&mPresentationData, coreModel, presId);
    EXPECT_EQ(status, DLB_ADM_STATUS_OK);
    EXPECT_EQ(mPresentationData.element_count, 2);
    EXPECT_EQ(mPresentationData.presentation.id, presId);
    EXPECT_EQ(mPresentationData.content_groups[0].content_kind, DLB_ADM_CONTENT_KIND_NK_UNDEFINED);
    EXPECT_EQ(mPresentationData.content_groups[0].id, contentIds[0]);
    EXPECT_EQ(mPresentationData.audio_elements[0].id, objectIds[0]);
    EXPECT_EQ(mPresentationData.alt_val_sets[0].id, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(mPresentationData.content_groups[1].content_kind, DLB_ADM_CONTENT_KIND_DK_DIALOGUE);
    EXPECT_EQ(mPresentationData.content_groups[1].id, contentIds[2]);
    EXPECT_EQ(mPresentationData.audio_elements[1].id, objectIds[2]);
    EXPECT_EQ(mPresentationData.alt_val_sets[1].id, avsIds[3]);
    status = ::dlb_adm_core_model_get_names(coreModel, &mNames, presId);
    EXPECT_EQ(status, DLB_ADM_STATUS_OK);
    EXPECT_EQ(mNames.name_count, 3);
    EXPECT_EQ(mNames.label_count, 2);
    EXPECT_EQ(0, ::strcmp("audioProgramme_4", mNames.names[0]));
    EXPECT_EQ(0, ::strcmp("fre", mNames.langs[0]));
    EXPECT_EQ(0, ::strcmp("English Only Compl Member with AVS", mNames.names[1]));
    EXPECT_EQ(0, ::strcmp("eng", mNames.langs[1]));
    EXPECT_EQ(0, ::strcmp("Anglais Only Compl Member with AVS", mNames.names[2]));
    EXPECT_EQ(0, ::strcmp("fre", mNames.langs[2]));

    status = ::dlb_adm_core_model_get_element_data(&mElementData, coreModel, objectIds[0]);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(6, mElementData.channel_count);
    EXPECT_EQ(objectIds[0], mElementData.audio_element.id);
    EXPECT_EQ(0, mElementData.alt_val_count);
    EXPECT_EQ(trackIds[0], mElementData.audio_tracks->id);

    status = ::dlb_adm_core_model_get_element_data(&mElementData, coreModel, objectIds[1]);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(mElementData.channel_count, 1);
    EXPECT_EQ(mElementData.audio_element.id, objectIds[1]);
    EXPECT_EQ(mElementData.alt_val_count, 2);
    EXPECT_EQ(mElementData.alt_val_sets[0].id, avsIds[0]);
    EXPECT_EQ(mElementData.alt_val_sets[0].has_gain, 1);
    EXPECT_FLOAT_EQ(mElementData.alt_val_sets[0].gain.gain_value, 3.0);
    EXPECT_EQ(mElementData.alt_val_sets[1].id, avsIds[1]);
    EXPECT_EQ(mElementData.alt_val_sets[1].has_gain, 1);
    EXPECT_FLOAT_EQ(mElementData.alt_val_sets[1].gain.gain_value, -6.0);
    EXPECT_EQ(mElementData.targets->audio_type, DLB_ADM_AUDIO_TYPE_OBJECTS);
    EXPECT_EQ(mElementData.audio_tracks->id, trackIds[1]);

    status = ::dlb_adm_core_model_get_element_data(&mElementData, coreModel, objectIds[2]);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(mElementData.channel_count, 1);
    EXPECT_EQ(mElementData.audio_element.id, objectIds[2]);
    EXPECT_EQ(mElementData.alt_val_count, 2);
    EXPECT_EQ(mElementData.alt_val_sets[0].id, avsIds[2]);
    EXPECT_EQ(mElementData.alt_val_sets[0].has_gain, 1);
    EXPECT_FLOAT_EQ(mElementData.alt_val_sets[0].gain.gain_value, 4.0);
    EXPECT_EQ(mElementData.alt_val_sets[1].id, avsIds[3]);
    EXPECT_EQ(mElementData.alt_val_sets[1].has_gain, 1);
    EXPECT_FLOAT_EQ(mElementData.alt_val_sets[1].gain.gain_value, -7.0);
    EXPECT_EQ(mElementData.targets->audio_type, DLB_ADM_AUDIO_TYPE_OBJECTS);
    EXPECT_EQ(mElementData.audio_tracks->id, trackIds[2]);
}

TEST_F(DlbAdmEmissionProfile, flattenComplementary_emissionProfileCompliant)
{
    int status;

    status = ::dlb_adm_container_read_xml_file(oryginalContainer, emissionProfileCompliantFileName, DLB_ADM_TRUE);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // Flattening ADM structure
    status = ::dlb_adm_container_flatten_complementary(oryginalContainer, flattenedContainer);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_container_write_xml_file(flattenedContainer, complementaryAndAvsFileName);
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

    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_ALT_VALUE_SET, &count);
    ASSERT_EQ(count, 3);

    dlb_adm_bool isEmissionProfile;
    status = ::dlb_adm_core_model_has_profile(coreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE, &isEmissionProfile);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_TRUE, isEmissionProfile);

    std::vector<std::string>presentationIdStrings{"APR_1001","APR_1002","APR_1003","APR_1004"};
    std::vector<std::string>contentIdStrings{"ACO_1001","ACO_1002","ACO_1003","ACO_1004","ACO_1005"};
    std::vector<std::string>objectIdStrings{"AO_1001","AO_1002","AO_1003","AO_1004","AO_1005"};
    std::vector<std::string>trackIdStrings{"ATU_00000001","ATU_00000007","ATU_00000008"};
    std::vector<std::string>avsIdStrings{"AVS_1001_0001","AVS_1001_0002","AVS_1002_0001"};

    std::vector<dlb_adm_entity_id>presentationIds;
    std::vector<dlb_adm_entity_id>contentIds;
    std::vector<dlb_adm_entity_id>objectIds;
    std::vector<dlb_adm_entity_id>trackIds;
    std::vector<dlb_adm_entity_id>avsIds;

    ASSERT_TRUE(prepareIds(presentationIdStrings, presentationIds));
    ASSERT_TRUE(prepareIds(contentIdStrings, contentIds));
    ASSERT_TRUE(prepareIds(objectIdStrings, objectIds));
    ASSERT_TRUE(prepareIds(trackIdStrings, trackIds));
    ASSERT_TRUE(prepareIds(avsIdStrings, avsIds));

    dlb_adm_entity_id presId = presentationIds[0];
    status = ::dlb_adm_core_model_get_presentation_data(&mPresentationData, coreModel, presId);
    EXPECT_EQ(status, DLB_ADM_STATUS_OK);
    EXPECT_EQ(mPresentationData.element_count, 2);
    EXPECT_EQ(mPresentationData.presentation.id, presId);
    EXPECT_EQ(mPresentationData.content_groups[0].content_kind, DLB_ADM_CONTENT_KIND_NK_UNDEFINED);
    EXPECT_EQ(mPresentationData.content_groups[0].id, contentIds[0]);
    EXPECT_EQ(mPresentationData.audio_elements[0].id, objectIds[0]);
    EXPECT_EQ(mPresentationData.alt_val_sets[0].id, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(mPresentationData.content_groups[1].content_kind, DLB_ADM_CONTENT_KIND_DK_DIALOGUE);
    EXPECT_EQ(mPresentationData.content_groups[1].id, contentIds[1]);
    EXPECT_EQ(mPresentationData.audio_elements[1].id, objectIds[1]);
    EXPECT_EQ(mPresentationData.alt_val_sets[1].id, DLB_ADM_NULL_ENTITY_ID);
    status = ::dlb_adm_core_model_get_names(coreModel, &mNames, presId);
    EXPECT_EQ(status, DLB_ADM_STATUS_OK);
    EXPECT_EQ(mNames.name_count, 4);
    EXPECT_EQ(mNames.label_count, 3);
    EXPECT_EQ(0, ::strcmp("English", mNames.names[0]));
    EXPECT_EQ(0, ::strcmp("eng", mNames.langs[0]));

    presId = presentationIds[1];
    status = ::dlb_adm_core_model_get_presentation_data(&mPresentationData, coreModel, presId);
    EXPECT_EQ(status, DLB_ADM_STATUS_OK);
    EXPECT_EQ(mPresentationData.element_count, 2);
    EXPECT_EQ(mPresentationData.presentation.id, presId);
    EXPECT_EQ(mPresentationData.content_groups[0].content_kind, DLB_ADM_CONTENT_KIND_NK_UNDEFINED);
    EXPECT_EQ(mPresentationData.content_groups[0].id, contentIds[0]);
    EXPECT_EQ(mPresentationData.audio_elements[0].id, objectIds[0]);
    EXPECT_EQ(mPresentationData.alt_val_sets[0].id, avsIds[0]);
    EXPECT_EQ(mPresentationData.content_groups[1].content_kind, DLB_ADM_CONTENT_KIND_DK_DIALOGUE);
    EXPECT_EQ(mPresentationData.content_groups[1].id, contentIds[2]);
    EXPECT_EQ(mPresentationData.audio_elements[1].id, objectIds[2]);
    EXPECT_EQ(mPresentationData.alt_val_sets[1].id, DLB_ADM_NULL_ENTITY_ID);
    status = ::dlb_adm_core_model_get_names(coreModel, &mNames, presId);
    EXPECT_EQ(status, DLB_ADM_STATUS_OK);
    EXPECT_EQ(mNames.name_count, 4);
    EXPECT_EQ(mNames.label_count, 3);
    EXPECT_EQ(0, ::strcmp("Spanish", mNames.names[0]));
    EXPECT_EQ(0, ::strcmp("spa", mNames.langs[0]));

    presId = presentationIds[2];
    status = ::dlb_adm_core_model_get_presentation_data(&mPresentationData, coreModel, presId);
    EXPECT_EQ(status, DLB_ADM_STATUS_OK);
    EXPECT_EQ(mPresentationData.element_count, 2);
    EXPECT_EQ(mPresentationData.presentation.id, presId);
    EXPECT_EQ(mPresentationData.content_groups[0].content_kind, DLB_ADM_CONTENT_KIND_NK_UNDEFINED);
    EXPECT_EQ(mPresentationData.content_groups[0].id, contentIds[0]);
    EXPECT_EQ(mPresentationData.audio_elements[0].id, objectIds[0]);
    EXPECT_EQ(mPresentationData.alt_val_sets[0].id, avsIds[1]);
    EXPECT_EQ(mPresentationData.content_groups[1].content_kind, DLB_ADM_CONTENT_KIND_DK_DIALOGUE);
    EXPECT_EQ(mPresentationData.content_groups[1].id, contentIds[3]);
    EXPECT_EQ(mPresentationData.audio_elements[1].id, objectIds[3]);
    EXPECT_EQ(mPresentationData.alt_val_sets[1].id, DLB_ADM_NULL_ENTITY_ID);
    status = ::dlb_adm_core_model_get_names(coreModel, &mNames, presId);
    EXPECT_EQ(status, DLB_ADM_STATUS_OK);
    EXPECT_EQ(mNames.name_count, 4);
    EXPECT_EQ(mNames.label_count, 3);
    EXPECT_EQ(0, ::strcmp("Chinese", mNames.names[0]));
    EXPECT_EQ(0, ::strcmp("zho", mNames.langs[0]));

    presId = presentationIds[3];
    status = ::dlb_adm_core_model_get_presentation_data(&mPresentationData, coreModel, presId);
    EXPECT_EQ(status, DLB_ADM_STATUS_OK);
    EXPECT_EQ(mPresentationData.element_count, 3);
    EXPECT_EQ(mPresentationData.presentation.id, presId);
    EXPECT_EQ(mPresentationData.content_groups[0].content_kind, DLB_ADM_CONTENT_KIND_NK_UNDEFINED);
    EXPECT_EQ(mPresentationData.content_groups[0].id, contentIds[0]);
    EXPECT_EQ(mPresentationData.audio_elements[0].id, objectIds[0]);
    EXPECT_EQ(mPresentationData.alt_val_sets[0].id, avsIds[0]);
    EXPECT_EQ(mPresentationData.content_groups[1].content_kind, DLB_ADM_CONTENT_KIND_DK_DIALOGUE);
    EXPECT_EQ(mPresentationData.content_groups[1].id, contentIds[1]);
    EXPECT_EQ(mPresentationData.audio_elements[1].id, objectIds[1]);
    EXPECT_EQ(mPresentationData.alt_val_sets[1].id, avsIds[2]);
    EXPECT_EQ(mPresentationData.content_groups[2].content_kind, DLB_ADM_CONTENT_KIND_DK_DESCRIPTION);
    EXPECT_EQ(mPresentationData.content_groups[2].id, contentIds[4]);
    EXPECT_EQ(mPresentationData.audio_elements[2].id, objectIds[4]);
    EXPECT_EQ(mPresentationData.alt_val_sets[2].id, DLB_ADM_NULL_ENTITY_ID);
    status = ::dlb_adm_core_model_get_names(coreModel, &mNames, presId);
    EXPECT_EQ(status, DLB_ADM_STATUS_OK);
    EXPECT_EQ(mNames.name_count, 4);
    EXPECT_EQ(mNames.label_count, 3);
    EXPECT_EQ(0, ::strcmp("English (VDS)", mNames.names[0]));
    EXPECT_EQ(0, ::strcmp("eng", mNames.langs[0]));
}

TEST_F(DlbAdmEmissionProfile, flattenComplementary_singleComplementaryMemberInProgramme)
{
    int status;

    status = ::dlb_adm_container_read_xml_buffer(oryginalContainer, singleComplementaryMemberInProgramme.c_str(), singleComplementaryMemberInProgramme.size(), DLB_ADM_TRUE);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // Flattening ADM structure
    status = ::dlb_adm_container_flatten_complementary(oryginalContainer, flattenedContainer);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // ingest into the core model
    status = dlb_adm_core_model_open_from_xml_container(&coreModel, flattenedContainer);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // Check if content has been flattend
    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_PROGRAMME, &count);
    ASSERT_EQ(count, 1);

    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_CONTENT, &count);
    ASSERT_EQ(count, 5);

    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_OBJECT, &count);
    ASSERT_EQ(count, 5);
}

TEST_F(DlbAdmEmissionProfile, flattenComplementary_plainCopyNonComplementaryProgrammes)
{
    int status;

    status = ::dlb_adm_container_read_xml_buffer(oryginalContainer, sadmUnconsistentLanguages.c_str(), sadmUnconsistentLanguages.size(), DLB_ADM_TRUE);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // Flattening ADM structure
    status = ::dlb_adm_container_flatten_complementary(oryginalContainer, flattenedContainer);
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

    dlb_adm_bool isEmissionProfile;
    status = ::dlb_adm_core_model_has_profile(coreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE, &isEmissionProfile);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_TRUE, isEmissionProfile);

    std::vector<std::string>presentationIdStrings{"APR_1001","APR_1002"};
    std::vector<std::string>contentIdStrings{"ACO_1001","ACO_1002","ACO_1003"};
    std::vector<std::string>objectIdStrings{"AO_1001","AO_1002","AO_1003"};

    std::vector<dlb_adm_entity_id>presentationIds;
    std::vector<dlb_adm_entity_id>contentIds;
    std::vector<dlb_adm_entity_id>objectIds;

    ASSERT_TRUE(prepareIds(presentationIdStrings, presentationIds));
    ASSERT_TRUE(prepareIds(contentIdStrings, contentIds));
    ASSERT_TRUE(prepareIds(objectIdStrings, objectIds));

    dlb_adm_entity_id presId = presentationIds[0];
    status = ::dlb_adm_core_model_get_presentation_data(&mPresentationData, coreModel, presId);
    EXPECT_EQ(status, DLB_ADM_STATUS_OK);
    EXPECT_EQ(mPresentationData.element_count, 2);
    EXPECT_EQ(mPresentationData.presentation.id, presId);
    EXPECT_EQ(mPresentationData.content_groups[0].content_kind, DLB_ADM_CONTENT_KIND_NK_UNDEFINED);
    EXPECT_EQ(mPresentationData.content_groups[0].id, contentIds[0]);
    EXPECT_EQ(mPresentationData.audio_elements[0].id, objectIds[0]);
    EXPECT_EQ(mPresentationData.alt_val_sets[0].id, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(mPresentationData.content_groups[1].content_kind, DLB_ADM_CONTENT_KIND_DK_DIALOGUE);
    EXPECT_EQ(mPresentationData.content_groups[1].id, contentIds[1]);
    EXPECT_EQ(mPresentationData.audio_elements[1].id, objectIds[1]);
    EXPECT_EQ(mPresentationData.alt_val_sets[1].id, DLB_ADM_NULL_ENTITY_ID);
    status = ::dlb_adm_core_model_get_names(coreModel, &mNames, presId);
    EXPECT_EQ(status, DLB_ADM_STATUS_OK);
    EXPECT_EQ(mNames.name_count, 2);
    EXPECT_EQ(mNames.label_count, 1);
    EXPECT_EQ(0, ::strcmp("Presentation 1", mNames.names[0]));
    EXPECT_EQ(0, ::strcmp("fr", mNames.langs[0]));
    EXPECT_EQ(0, ::strcmp("Presentation 1", mNames.names[1]));
    EXPECT_EQ(0, ::strcmp("fr", mNames.langs[1]));

    presId = presentationIds[1];
    status = ::dlb_adm_core_model_get_presentation_data(&mPresentationData, coreModel, presId);
    EXPECT_EQ(status, DLB_ADM_STATUS_OK);
    EXPECT_EQ(mPresentationData.element_count, 2);
    EXPECT_EQ(mPresentationData.presentation.id, presId);
    EXPECT_EQ(mPresentationData.content_groups[0].content_kind, DLB_ADM_CONTENT_KIND_NK_UNDEFINED);
    EXPECT_EQ(mPresentationData.content_groups[0].id, contentIds[0]);
    EXPECT_EQ(mPresentationData.audio_elements[0].id, objectIds[0]);
    EXPECT_EQ(mPresentationData.alt_val_sets[0].id, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(mPresentationData.content_groups[1].content_kind, DLB_ADM_CONTENT_KIND_DK_DESCRIPTION);
    EXPECT_EQ(mPresentationData.content_groups[1].id, contentIds[2]);
    EXPECT_EQ(mPresentationData.audio_elements[1].id, objectIds[2]);
    EXPECT_EQ(mPresentationData.alt_val_sets[1].id, DLB_ADM_NULL_ENTITY_ID);
    status = ::dlb_adm_core_model_get_names(coreModel, &mNames, presId);
    EXPECT_EQ(status, DLB_ADM_STATUS_OK);
    EXPECT_EQ(mNames.name_count, 2);
    EXPECT_EQ(mNames.label_count, 1);
    EXPECT_EQ(0, ::strcmp("Presentation 2", mNames.names[0]));
    EXPECT_EQ(0, ::strcmp("en", mNames.langs[0]));
    EXPECT_EQ(0, ::strcmp("Presentation 2", mNames.names[1]));
    EXPECT_EQ(0, ::strcmp("en", mNames.langs[1]));

    status = ::dlb_adm_core_model_get_names(coreModel, &mNames, contentIds[0]);
    EXPECT_EQ(status, DLB_ADM_STATUS_OK);
    EXPECT_EQ(mNames.name_count, 1);
    EXPECT_EQ(mNames.label_count, 0);
    EXPECT_EQ(0, ::strcmp("musicAndEffects", mNames.names[0]));
    EXPECT_EQ(0, ::strcmp("en", mNames.langs[0]));

    status = ::dlb_adm_core_model_get_names(coreModel, &mNames, contentIds[1]);
    EXPECT_EQ(status, DLB_ADM_STATUS_OK);
    EXPECT_EQ(mNames.name_count, 1);
    EXPECT_EQ(mNames.label_count, 0);
    EXPECT_EQ(0, ::strcmp("dialog", mNames.names[0]));
    EXPECT_EQ(0, ::strcmp("fr", mNames.langs[0]));

    status = ::dlb_adm_core_model_get_names(coreModel, &mNames, contentIds[2]);
    EXPECT_EQ(status, DLB_ADM_STATUS_OK);
    EXPECT_EQ(mNames.name_count, 1);
    EXPECT_EQ(mNames.label_count, 0);
    EXPECT_EQ(0, ::strcmp("radioCommentator", mNames.names[0]));
    EXPECT_EQ(0, ::strcmp("en", mNames.langs[0]));
}

TEST_F(DlbAdmEmissionProfile, flattenComplementary_sadm_51_MnE_3D_complementary)
{
    int status;

    status = ::dlb_adm_container_read_xml_buffer(oryginalContainer, sadm_51_MnE_3D_complementary.c_str(), sadm_51_MnE_3D_complementary.size(), DLB_ADM_TRUE);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // Flattening ADM structure
    status = ::dlb_adm_container_flatten_complementary(oryginalContainer, flattenedContainer);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    status = ::dlb_adm_container_write_xml_file(flattenedContainer, "sadm_51_MnE_3D_complementary.flattned.out.xml");
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // ingest into the core model
    status = dlb_adm_core_model_open_from_xml_container(&coreModel, flattenedContainer);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    dlb_adm_xml_container *coreModelContainer = nullptr;
    status = dlb_adm_container_open_from_core_model(&coreModelContainer, coreModel);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_container_write_xml_file(coreModelContainer, "sadm_51_MnE_3D_complementary.flattned.coreModel.out.xml");
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // Check if content has been flattend
    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_PROGRAMME, &count);
    ASSERT_EQ(count, 3);

    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_CONTENT, &count);
    ASSERT_EQ(count, 4);

    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_OBJECT, &count);
    ASSERT_EQ(count, 4);

    dlb_adm_bool isEmissionProfile;
    status = ::dlb_adm_core_model_has_profile(coreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE, &isEmissionProfile);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_TRUE, isEmissionProfile);
}

TEST_F(DlbAdmEmissionProfile, flattenComplementary_xml_me_2D_AVS)
{
    int status;

    status = ::dlb_adm_container_read_xml_buffer(oryginalContainer, xml_me_2D_AVS.c_str(), xml_me_2D_AVS.size(), DLB_ADM_TRUE);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // Flattening ADM structure
    status = ::dlb_adm_container_flatten_complementary(oryginalContainer, flattenedContainer);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    // ingest into the core model
    status = dlb_adm_core_model_open_from_xml_container(&coreModel, flattenedContainer);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // Check if content has been flattend
    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_PROGRAMME, &count);
    ASSERT_EQ(count, 2);

    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_CONTENT, &count);
    ASSERT_EQ(count, 2);

    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_OBJECT, &count);
    ASSERT_EQ(count, 2);

    dlb_adm_core_model_count_entities(coreModel, DLB_ADM_ENTITY_TYPE_ALT_VALUE_SET, &count);
    ASSERT_EQ(count, 1);

    dlb_adm_bool isEmissionProfile;
    status = ::dlb_adm_core_model_has_profile(coreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE, &isEmissionProfile);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_TRUE, isEmissionProfile);

    std::vector<std::string>presentationIdStrings{"APR_1001","APR_1002"};
    std::vector<std::string>contentIdStrings{"ACO_1001","ACO_1002"};
    std::vector<std::string>objectIdStrings{"AO_1001","AO_1002"};
    std::vector<std::string>avsIdStrings{"AVS_1002_0001"};

    std::vector<dlb_adm_entity_id>presentationIds;
    std::vector<dlb_adm_entity_id>contentIds;
    std::vector<dlb_adm_entity_id>objectIds;
    std::vector<dlb_adm_entity_id>avsIds;

    ASSERT_TRUE(prepareIds(presentationIdStrings, presentationIds));
    ASSERT_TRUE(prepareIds(contentIdStrings, contentIds));
    ASSERT_TRUE(prepareIds(objectIdStrings, objectIds));
    ASSERT_TRUE(prepareIds(avsIdStrings, avsIds));

    dlb_adm_entity_id presId = presentationIds[0];
    status = ::dlb_adm_core_model_get_presentation_data(&mPresentationData, coreModel, presId);
    EXPECT_EQ(status, DLB_ADM_STATUS_OK);
    EXPECT_EQ(mPresentationData.element_count, 2);
    EXPECT_EQ(mPresentationData.presentation.id, presId);
    EXPECT_EQ(mPresentationData.content_groups[0].content_kind, DLB_ADM_CONTENT_KIND_NK_EFFECTS);
    EXPECT_EQ(mPresentationData.content_groups[0].id, contentIds[0]);
    EXPECT_EQ(mPresentationData.audio_elements[0].id, objectIds[0]);
    EXPECT_EQ(mPresentationData.alt_val_sets[0].id, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(mPresentationData.content_groups[1].content_kind, DLB_ADM_CONTENT_KIND_DK_DIALOGUE);
    EXPECT_EQ(mPresentationData.content_groups[1].id, contentIds[1]);
    EXPECT_EQ(mPresentationData.audio_elements[1].id, objectIds[1]);
    EXPECT_EQ(mPresentationData.alt_val_sets[1].id, DLB_ADM_NULL_ENTITY_ID);
    status = ::dlb_adm_core_model_get_names(coreModel, &mNames, presId);
    EXPECT_EQ(status, DLB_ADM_STATUS_OK);
    EXPECT_EQ(mNames.name_count, 2);
    EXPECT_EQ(mNames.label_count, 1);
    EXPECT_EQ(0, ::strcmp("Presentation 1", mNames.names[0]));
    EXPECT_EQ(0, ::strcmp("pol", mNames.langs[0]));
    EXPECT_EQ(0, ::strcmp("Presentation 1", mNames.names[1]));
    EXPECT_EQ(0, ::strcmp("pol", mNames.langs[1]));

    presId = presentationIds[1];
    status = ::dlb_adm_core_model_get_presentation_data(&mPresentationData, coreModel, presId);
    EXPECT_EQ(status, DLB_ADM_STATUS_OK);
    EXPECT_EQ(mPresentationData.element_count, 2);
    EXPECT_EQ(mPresentationData.presentation.id, presId);
    EXPECT_EQ(mPresentationData.content_groups[0].content_kind, DLB_ADM_CONTENT_KIND_NK_EFFECTS);
    EXPECT_EQ(mPresentationData.content_groups[0].id, contentIds[0]);
    EXPECT_EQ(mPresentationData.audio_elements[0].id, objectIds[0]);
    EXPECT_EQ(mPresentationData.alt_val_sets[0].id, DLB_ADM_NULL_ENTITY_ID);
    EXPECT_EQ(mPresentationData.content_groups[1].content_kind, DLB_ADM_CONTENT_KIND_DK_DIALOGUE);
    EXPECT_EQ(mPresentationData.content_groups[1].id, contentIds[1]);
    EXPECT_EQ(mPresentationData.audio_elements[1].id, objectIds[1]);
    EXPECT_EQ(mPresentationData.alt_val_sets[1].id, avsIds[0]);
    status = ::dlb_adm_core_model_get_names(coreModel, &mNames, presId);
    EXPECT_EQ(status, DLB_ADM_STATUS_OK);
    EXPECT_EQ(mNames.name_count, 2);
    EXPECT_EQ(mNames.label_count, 1);
    EXPECT_EQ(0, ::strcmp("Presentation 2", mNames.names[0]));
    EXPECT_EQ(0, ::strcmp("pol", mNames.langs[0]));
    EXPECT_EQ(0, ::strcmp("Presentation 2", mNames.names[1]));
    EXPECT_EQ(0, ::strcmp("pol", mNames.langs[1]));

    status = ::dlb_adm_core_model_get_names(coreModel, &mNames, contentIds[0]);
    EXPECT_EQ(status, DLB_ADM_STATUS_OK);
    EXPECT_EQ(mNames.name_count, 1);
    EXPECT_EQ(mNames.label_count, 0);
    EXPECT_EQ(0, ::strcmp("Bed 1", mNames.names[0]));
    EXPECT_EQ(0, ::strcmp("und", mNames.langs[0]));

    status = ::dlb_adm_core_model_get_names(coreModel, &mNames, contentIds[1]);
    EXPECT_EQ(status, DLB_ADM_STATUS_OK);
    EXPECT_EQ(mNames.name_count, 1);
    EXPECT_EQ(mNames.label_count, 0);
    EXPECT_EQ(0, ::strcmp("Object 1", mNames.names[0]));
    EXPECT_EQ(0, ::strcmp("pol", mNames.langs[0]));

    status = ::dlb_adm_core_model_get_element_data(&mElementData, coreModel, objectIds[0]);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(2, mElementData.channel_count);
    EXPECT_EQ(objectIds[0], mElementData.audio_element.id);
    EXPECT_EQ(0, mElementData.alt_val_count);
    EXPECT_EQ(MAX_SADM_ALT_VALUE_SETS, mElementData.alt_val_capacity);

    status = ::dlb_adm_core_model_get_element_data(&mElementData, coreModel, objectIds[0]);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(2, mElementData.channel_count);
    EXPECT_EQ(objectIds[0], mElementData.audio_element.id);
    EXPECT_EQ(0, mElementData.alt_val_count);
    EXPECT_EQ(MAX_SADM_ALT_VALUE_SETS, mElementData.alt_val_capacity);

    status = ::dlb_adm_core_model_get_element_data(&mElementData, coreModel, objectIds[1]);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(1, mElementData.channel_count);
    EXPECT_EQ(objectIds[1], mElementData.audio_element.id);
    EXPECT_EQ(1, mElementData.alt_val_count);
    EXPECT_EQ(MAX_SADM_ALT_VALUE_SETS, mElementData.alt_val_capacity);

    EXPECT_EQ(avsIds[0], mElementData.alt_val_sets[0].id);
    EXPECT_EQ(DLB_ADM_TRUE, mElementData.alt_val_sets[0].has_position_offset);
    EXPECT_EQ(DLB_ADM_TRUE, mElementData.alt_val_sets[0].has_gain);
    EXPECT_EQ(DLB_ADM_GAIN_UNIT_DB, mElementData.alt_val_sets[0].gain.gain_unit);
    EXPECT_FLOAT_EQ(-3.0, mElementData.alt_val_sets[0].gain.gain_value);
}

