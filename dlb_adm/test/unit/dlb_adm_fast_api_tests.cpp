/************************************************************************
 * dlb_adm
 * Copyright (c) 2023, Dolby Laboratories Inc.
 * Copyright (c) 2023, Dolby International AB.
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

#include "dlb_adm/include/dlb_adm_fast_api.h"
#include "dlb_adm/include/dlb_adm_api.h"
#include "dlb_adm/include/dlb_adm_api_types.h"

#include "AdmIdTranslator.h"

class DlbFastApiTests : public testing::Test
{
protected:

    static const unsigned int mNameCount = 5;
    static const unsigned int mMaxChannels = 16u;

    dlb_adm_core_model             *coreModel;
    dlb_adm_data_names              mNames;
    dlb_adm_data_audio_element_data mAudioElements;
    char                           *mNamesMemory;    
    uint8_t                        *mAudioElementsMemory;
    dlb_adm_xml_container          *mCommonDefContainer;  

    uint8_t                       *mPresentationDataMemory;
    dlb_adm_data_presentation_data mPresentationData;

    virtual void SetUp()
    {
        coreModel = nullptr;
        mCommonDefContainer = nullptr;
        dlb_adm_core_model_counts counts;
        dlb_adm_container_counts containerCounts;        

        memset(&counts, 0, sizeof(dlb_adm_core_model_counts));
        ASSERT_EQ(dlb_adm_core_model_open(&coreModel, &counts), DLB_ADM_STATUS_OK);

        size_t namesMemorySize = 0;
        size_t elementsSize = 0;
        size_t nameLimit = 5; // 1 for programme name and 4 for labels
        ASSERT_EQ(dlb_adm_core_model_query_names_memory_size(&namesMemorySize, DLB_ADM_DATA_NAME_SZ, 5), DLB_ADM_STATUS_OK);
        mNamesMemory = new char[namesMemorySize];
        ASSERT_EQ(dlb_adm_core_model_configure_names(&mNames, nameLimit, mNamesMemory, namesMemorySize), DLB_ADM_STATUS_OK);
        ASSERT_EQ(dlb_adm_core_model_query_element_data_memory_size(&elementsSize, mMaxChannels, 0), DLB_ADM_STATUS_OK);
        mAudioElementsMemory = new uint8_t[elementsSize];
        ASSERT_EQ(dlb_adm_core_model_configure_element_data(&mAudioElements, mMaxChannels, 0, mAudioElementsMemory), DLB_ADM_STATUS_OK); 

        memset(&containerCounts, 0, sizeof(containerCounts));
        ASSERT_EQ(dlb_adm_container_open(&mCommonDefContainer, &containerCounts), DLB_ADM_STATUS_OK); 
        dlb_adm_container_load_common_definitions(mCommonDefContainer);

        dlb_adm_core_model_ingest_common_definitions_container(coreModel, mCommonDefContainer);   

        size_t memorySize;

        // configure presentation_data
        int status = ::dlb_adm_core_model_query_presentation_data_memory_size(&memorySize, 8);
        ASSERT_EQ(DLB_ADM_STATUS_OK, status);
        mPresentationDataMemory = new uint8_t[memorySize];
        ASSERT_NE(nullptr, mPresentationDataMemory);

        status = ::dlb_adm_core_model_configure_presentation_data(&mPresentationData, 8, mPresentationDataMemory);
        ASSERT_EQ(DLB_ADM_STATUS_OK, status);
        ASSERT_EQ(0, mPresentationData.element_count);
        ASSERT_EQ(8, mPresentationData.element_capacity);
        ASSERT_EQ(mPresentationDataMemory, mPresentationData.array_storage);    
    }    

    virtual void TearDown()
    {
        if (coreModel != nullptr)
        {
            if (dlb_adm_core_model_close(&coreModel))
            {
                coreModel = nullptr;
            }
        }

        if (mNamesMemory != nullptr)
        {
            delete[] mNamesMemory;
            mNamesMemory = nullptr;
        }
        if (mAudioElementsMemory != nullptr)
        {
            delete[] mAudioElementsMemory;
            mAudioElementsMemory = nullptr;
        }
        if (mPresentationDataMemory != nullptr)
        {
            delete[] mPresentationDataMemory;
            mPresentationDataMemory = nullptr;
        }
        ASSERT_EQ(dlb_adm_container_close(&mCommonDefContainer), DLB_ADM_STATUS_OK);   
    }
};

TEST_F(DlbFastApiTests, addAudioElement)
{
    dlb_adm_element_ids elements_ids;
    unsigned int sources[1] = {1};
    char lang_tag[4] = "eng";

    elements_ids = dlb_adm_add_audio_element
                    (coreModel
                    ,DLB_ADM_SPEAKER_CONFIG_NONE
                    ,DLB_ADM_CONTENT_KIND_DK_DIALOGUE
                    ,lang_tag
                    ,sources
                    ,1
                    ,&mNames
                    );

    EXPECT_EQ(504420754746507264, elements_ids.element_id);
}

TEST_F(DlbFastApiTests, addSeveralAudioElement)
{
    dlb_adm_element_ids elements_ids;
    unsigned int sources_1[1] = {1};
    unsigned int sources_2[1] = {2};
    char lang_tag_1[4] = "eng";
    char lang_tag_2[4] = "fra";      

    elements_ids = dlb_adm_add_audio_element
                    (coreModel
                    ,DLB_ADM_SPEAKER_CONFIG_NONE
                    ,DLB_ADM_CONTENT_KIND_DK_DIALOGUE
                    ,lang_tag_1
                    ,sources_1
                    ,1
                    ,&mNames                    
                    );

    EXPECT_EQ(504420754746507264, elements_ids.element_id);                    

    elements_ids = dlb_adm_add_audio_element
                    (coreModel
                    ,DLB_ADM_SPEAKER_CONFIG_NONE
                    ,DLB_ADM_CONTENT_KIND_DK_DIALOGUE
                    ,lang_tag_2
                    ,sources_2
                    ,1
                    ,&mNames                    
                    );                    

    EXPECT_EQ(504420759041474560, elements_ids.element_id);
}

TEST_F(DlbFastApiTests, addBedAudioElement)
{
    dlb_adm_element_ids elements_ids;
    unsigned int sources[10] = {1,2,3,5,4,6,7,8,9,10};
    char lang_tag[4] = "eng";

    elements_ids = dlb_adm_add_audio_element
                    (coreModel
                    ,DLB_ADM_SPEAKER_CONFIG_5_1_4
                    ,DLB_ADM_CONTENT_KIND_MK_COMPLETE_MAIN
                    ,lang_tag
                    ,sources
                    ,10
                    ,&mNames
                    );

    EXPECT_EQ(504420754746507264, elements_ids.element_id);

/*    int status = dlb_adm_core_model_get_element_data(&mAudioElements, coreModel, elements_ids.element_id);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    EXPECT_EQ(0, mAudioElements.audio_element.gain.gain_value);
    EXPECT_EQ(0, mAudioElements.audio_element.interact);
    EXPECT_EQ(DLB_ADM_OBJECT_CLASS_GENERIC, mAudioElements.audio_element.object_class);
    EXPECT_EQ(DLB_ADM_SPEAKER_CONFIG_5_1_4, mAudioElements.target_group.speaker_config);
  */  
}

TEST_F(DlbFastApiTests, addBedVIAudioElement)
{
    dlb_adm_element_ids elements_ids;
    unsigned int sources[10] = {1,2,3,5,4,6,7,8,9,10};
    char lang_tag[4] = "eng";

    elements_ids = dlb_adm_add_audio_element
                    (coreModel
                    ,DLB_ADM_SPEAKER_CONFIG_5_1_4
                    ,DLB_ADM_CONTENT_KIND_MK_VISUALLY_IMPAIRED
                    ,lang_tag
                    ,sources
                    ,10
                    ,&mNames
                    );

    EXPECT_EQ(504420754746507264, elements_ids.element_id);

    char programme_name[DLB_ADM_DATA_NAME_SZ] = "Programme 1";
    char language_tag[DLB_ADM_DATA_LANG_SZ] = "eng";
    dlb_adm_presentation_label labels[1];
    strcpy(labels[0].language_tag, "pol");
    strcpy(labels[0].presentation_label, "Programe 1");

    dlb_adm_entity_id returnedProgrammeId = dlb_adm_add_audio_programme(coreModel, programme_name, language_tag, labels, 1, &mNames);
    ASSERT_EQ(returnedProgrammeId, DlbAdm::AdmIdTranslator().Translate("APR_1001"));

    dlb_adm_add_element_to_programme(coreModel, returnedProgrammeId, elements_ids.content_group_id, elements_ids.element_id);

    int status = dlb_adm_core_model_get_presentation_data(&mPresentationData, coreModel, returnedProgrammeId);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_EQ(DLB_ADM_CONTENT_KIND_MK_VISUALLY_IMPAIRED, mPresentationData.content_groups[0].content_kind);
}

TEST_F(DlbFastApiTests, addAudioProgramme)
{
    char programme_name[DLB_ADM_DATA_NAME_SZ] = "Programme 1";
    char language_tag[DLB_ADM_DATA_LANG_SZ] = "eng";
    dlb_adm_presentation_label labels[4];
    
    strcpy(labels[0].language_tag, "pol");
    strcpy(labels[0].presentation_label, "Programe 1");
    strcpy(labels[1].language_tag, "ger");
    strcpy(labels[1].presentation_label, "Programme 1");
    strcpy(labels[2].language_tag, "lav");
    strcpy(labels[2].presentation_label, "Programmme 1");
    strcpy(labels[3].language_tag, "lit");
    strcpy(labels[3].presentation_label, "Programmmme 1");
    unsigned int num_labels = 4;

    dlb_adm_entity_id returnedProgrammeId = dlb_adm_add_audio_programme(coreModel, programme_name, language_tag, labels, num_labels, &mNames);
    ASSERT_EQ(returnedProgrammeId, DlbAdm::AdmIdTranslator().Translate("APR_1001"));
    ASSERT_EQ(dlb_adm_core_model_get_names(coreModel, &mNames, returnedProgrammeId), DLB_ADM_STATUS_OK);
    EXPECT_EQ(0, strcmp("Programme 1",  mNames.names[0]));
    EXPECT_EQ(0, strcmp("eng",          mNames.langs[0]));
    EXPECT_EQ(0, strcmp("Programe 1",   mNames.names[1]));
    EXPECT_EQ(0, strcmp("pol",          mNames.langs[1]));
    EXPECT_EQ(0, strcmp("Programme 1",  mNames.names[2]));
    EXPECT_EQ(0, strcmp("ger",          mNames.langs[2]));
    EXPECT_EQ(0, strcmp("Programmme 1", mNames.names[3]));
    EXPECT_EQ(0, strcmp("lav",          mNames.langs[3]));
    EXPECT_EQ(0, strcmp("Programmmme 1",mNames.names[4]));
    EXPECT_EQ(0, strcmp("lit",          mNames.langs[4]));

    strcpy(programme_name, "Programme 2");
    strcpy(language_tag, "rus");

    strcpy(labels[0].language_tag, "rum");
    strcpy(labels[0].presentation_label, "Programe 2");
    strcpy(labels[1].language_tag, "slv");
    strcpy(labels[1].presentation_label, "Programme 2");
    num_labels = 2;

    returnedProgrammeId = dlb_adm_add_audio_programme(coreModel, programme_name, language_tag, labels, num_labels, &mNames);
    ASSERT_EQ(returnedProgrammeId, DlbAdm::AdmIdTranslator().Translate("APR_1002"));
    ASSERT_EQ(dlb_adm_core_model_get_names(coreModel, &mNames, returnedProgrammeId), DLB_ADM_STATUS_OK);
    EXPECT_EQ(0, strcmp("Programme 2",  mNames.names[0]));
    EXPECT_EQ(0, strcmp("rus",          mNames.langs[0]));
    EXPECT_EQ(0, strcmp("Programe 2",   mNames.names[1]));
    EXPECT_EQ(0, strcmp("rum",          mNames.langs[1]));
    EXPECT_EQ(0, strcmp("Programme 2",  mNames.names[2]));
    EXPECT_EQ(0, strcmp("slv",          mNames.langs[2]));
}

