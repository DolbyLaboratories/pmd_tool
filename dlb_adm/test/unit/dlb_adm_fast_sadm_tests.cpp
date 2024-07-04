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

static const char mFileNameSimple[] = "SADM_1CM.out.xml";
static const char mFileName2CM[] = "SADM_2CM.out.xml";
static const char mFileNameME2D[] = "SADM_ME_2D.out.xml";

class DlbFastXMLGenerationTests : public testing::Test
{

protected:

    static const unsigned int mNameCount = 5;
    static const unsigned int mMaxChannels = 16u;

    dlb_adm_core_model             *mCoreModel;
    dlb_adm_xml_container          *mXmlContainer;        
    dlb_adm_data_names              mNames;
    char                           *mNamesMemory;

    void LoadCommonDefs()
    {
        dlb_adm_xml_container* CommonDefContainer = nullptr;
        dlb_adm_container_counts containerCounts;
        memset(&containerCounts, 0, sizeof(containerCounts));
        ASSERT_EQ(dlb_adm_container_open(&CommonDefContainer, &containerCounts), DLB_ADM_STATUS_OK); 
        dlb_adm_container_load_common_definitions(CommonDefContainer);

        dlb_adm_core_model_ingest_common_definitions_container(mCoreModel, CommonDefContainer);

        ASSERT_EQ(dlb_adm_container_close(&CommonDefContainer), DLB_ADM_STATUS_OK);    
    }

    virtual void SetUp()
    {
        mCoreModel = nullptr;
        mXmlContainer = nullptr;             
        dlb_adm_core_model_counts counts;
        

        memset(&counts, 0, sizeof(dlb_adm_core_model_counts));
        ASSERT_EQ(dlb_adm_core_model_open(&mCoreModel, &counts), DLB_ADM_STATUS_OK);

        size_t namesMemorySize = 0;
        size_t nameLimit = 5; // 1 for programme name and 4 for labels
        ASSERT_EQ(dlb_adm_core_model_query_names_memory_size(&namesMemorySize, DLB_ADM_DATA_NAME_SZ, 5), DLB_ADM_STATUS_OK);
        mNamesMemory = new char[namesMemorySize];
        ASSERT_EQ(dlb_adm_core_model_configure_names(&mNames, nameLimit, mNamesMemory, namesMemorySize), DLB_ADM_STATUS_OK);      
    }    

    virtual void TearDown()
    {
        if (mCoreModel != nullptr)
        {
            if (dlb_adm_core_model_close(&mCoreModel))
            {
                mCoreModel = nullptr;
            }
        }

        if (mNamesMemory != nullptr)
        {
            delete[] mNamesMemory;
            mNamesMemory = nullptr;
        }

        ASSERT_EQ(dlb_adm_container_close(&mXmlContainer), DLB_ADM_STATUS_OK);
    }
};

TEST_F(DlbFastXMLGenerationTests, simpleSADM)
{
    dlb_adm_element_ids elements_ids;
    unsigned int sources[10] = {1,2};
    char lang_tag[4] = "eng";

    LoadCommonDefs();

    dlb_adm_core_model_add_profile(mCoreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE);

    dlb_adm_data_frame_format frame_format;
    memset(&frame_format, 0, sizeof(dlb_adm_data_frame_format));
    snprintf(frame_format.type, sizeof(frame_format.type), "%s", "full");
    snprintf(frame_format.timeReference, sizeof(frame_format.timeReference),"%s", "local");
    frame_format.duration.fraction_numerator = 1920;
    frame_format.duration.fraction_denominator = 48000;
    dlb_adm_core_model_add_frame_format(mCoreModel, &frame_format);

    elements_ids = dlb_adm_add_audio_element
                    (mCoreModel
                    ,DLB_ADM_SPEAKER_CONFIG_2_0
                    ,DLB_ADM_CONTENT_KIND_MK_COMPLETE_MAIN
                    ,lang_tag
                    ,sources
                    ,2
                    ,&mNames
                    );

    char programme_name[DLB_ADM_DATA_NAME_SZ] = "Presentation 1";
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

    dlb_adm_entity_id returnedProgrammeId = dlb_adm_add_audio_programme(mCoreModel, programme_name, language_tag, labels, num_labels, &mNames);

    dlb_adm_add_element_to_programme(mCoreModel, returnedProgrammeId, elements_ids.content_group_id, elements_ids.element_id);

    ASSERT_EQ(DLB_ADM_STATUS_OK, dlb_adm_container_open_from_core_model(&mXmlContainer, mCoreModel));
    ASSERT_EQ(DLB_ADM_STATUS_OK, dlb_adm_container_write_xml_file(mXmlContainer, mFileNameSimple));
}

TEST_F(DlbFastXMLGenerationTests, SADM_2CM)
{
    dlb_adm_element_ids elements_ids_1, elements_ids_2;
    unsigned int sources_1[10] = {3,4,5,6,7,9,10,11,12,13};
    unsigned int sources_2[2] = {1,2};
    char lang_tag_1[4] = "fre";    
    char lang_tag_2[4] = "eng";

    LoadCommonDefs();

    dlb_adm_core_model_add_profile(mCoreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE);

    dlb_adm_data_frame_format frame_format;
    memset(&frame_format, 0, sizeof(dlb_adm_data_frame_format));
    snprintf(frame_format.type, sizeof(frame_format.type), "%s", "full");
    snprintf(frame_format.timeReference, sizeof(frame_format.timeReference),"%s", "local");
    frame_format.duration.fraction_numerator = 1920;
    frame_format.duration.fraction_denominator = 48000;
    dlb_adm_core_model_add_frame_format(mCoreModel, &frame_format);

    elements_ids_1 = dlb_adm_add_audio_element
                    (mCoreModel
                    ,DLB_ADM_SPEAKER_CONFIG_5_1_4
                    ,DLB_ADM_CONTENT_KIND_MK_COMPLETE_MAIN
                    ,lang_tag_1
                    ,sources_1
                    ,10
                    ,&mNames
                    );

    elements_ids_2 = dlb_adm_add_audio_element
                    (mCoreModel
                    ,DLB_ADM_SPEAKER_CONFIG_2_0
                    ,DLB_ADM_CONTENT_KIND_MK_COMPLETE_MAIN
                    ,lang_tag_2
                    ,sources_2
                    ,2
                    ,&mNames
                    );

    dlb_adm_entity_id programme_id_1 = DLB_ADM_NULL_ENTITY_ID;
    dlb_adm_entity_id programme_id_2 = DLB_ADM_NULL_ENTITY_ID;    
    char programme_name_1[DLB_ADM_DATA_NAME_SZ] = "Presentation 1";
    char programme_name_2[DLB_ADM_DATA_NAME_SZ] = "Presentation 2";  

    dlb_adm_presentation_label labels_1[2];
    strcpy(labels_1[0].language_tag, "fre");
    strcpy(labels_1[0].presentation_label, "Programme 1");
    strcpy(labels_1[1].language_tag, "eng");
    strcpy(labels_1[1].presentation_label, "Presentation 1");
    dlb_adm_presentation_label labels_2[2];
    strcpy(labels_2[0].language_tag, "eng");
    strcpy(labels_2[0].presentation_label, "Presentation 2");
    strcpy(labels_2[1].language_tag, "fre");
    strcpy(labels_2[1].presentation_label, "Programme 2");    
    unsigned int num_labels = 2;

    programme_id_1 = dlb_adm_add_audio_programme(mCoreModel, programme_name_1, lang_tag_1, labels_1, num_labels, &mNames);
    programme_id_2 = dlb_adm_add_audio_programme(mCoreModel, programme_name_2, lang_tag_2, labels_2, num_labels, &mNames);

    dlb_adm_add_element_to_programme(mCoreModel, programme_id_1, elements_ids_1.content_group_id, elements_ids_1.element_id);
    dlb_adm_add_element_to_programme(mCoreModel, programme_id_2, elements_ids_2.content_group_id, elements_ids_2.element_id);

    ASSERT_EQ(DLB_ADM_STATUS_OK, dlb_adm_container_open_from_core_model(&mXmlContainer, mCoreModel));
    ASSERT_EQ(DLB_ADM_STATUS_OK, dlb_adm_container_write_xml_file(mXmlContainer, mFileName2CM));
}

TEST_F(DlbFastXMLGenerationTests, SADM_ME_2D)
{
    dlb_adm_element_ids elements_ids_bed, elements_ids_obj_1, elements_ids_obj_2;
    unsigned int sources_bed[10] = {1,2,3,4,5,6,9,10,11,12};
    unsigned int sources_obj_1[1] = {7};
    unsigned int sources_obj_2[1] = {8};
    char lang_tag_bed[4] = "und";    
    char lang_tag_obj_1[4] = "eng";
    char lang_tag_obj_2[4] = "pol";

    LoadCommonDefs();

    dlb_adm_core_model_add_profile(mCoreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE);

    dlb_adm_data_frame_format frame_format;
    memset(&frame_format, 0, sizeof(dlb_adm_data_frame_format));
    snprintf(frame_format.type, sizeof(frame_format.type), "%s", "full");
    snprintf(frame_format.timeReference, sizeof(frame_format.timeReference),"%s", "local");
    frame_format.duration.fraction_numerator = 1920;
    frame_format.duration.fraction_denominator = 48000;
    dlb_adm_core_model_add_frame_format(mCoreModel, &frame_format);

    elements_ids_bed = dlb_adm_add_audio_element
                    (mCoreModel
                    ,DLB_ADM_SPEAKER_CONFIG_5_1_4
                    ,DLB_ADM_CONTENT_KIND_MK_COMPLETE_MAIN
                    ,lang_tag_bed
                    ,sources_bed
                    ,10
                    ,&mNames
                    );

    elements_ids_obj_1 = dlb_adm_add_audio_element
                    (mCoreModel
                    ,DLB_ADM_SPEAKER_CONFIG_NONE
                    ,DLB_ADM_CONTENT_KIND_DK_DIALOGUE
                    ,lang_tag_obj_1
                    ,sources_obj_1
                    ,1
                    ,&mNames
                    );

    elements_ids_obj_2 = dlb_adm_add_audio_element
                    (mCoreModel
                    ,DLB_ADM_SPEAKER_CONFIG_NONE
                    ,DLB_ADM_CONTENT_KIND_DK_DIALOGUE
                    ,lang_tag_obj_2
                    ,sources_obj_2
                    ,1
                    ,&mNames
                    );                    

    dlb_adm_entity_id programme_id_1 = DLB_ADM_NULL_ENTITY_ID;
    dlb_adm_entity_id programme_id_2 = DLB_ADM_NULL_ENTITY_ID;    
    char programme_name_1[DLB_ADM_DATA_NAME_SZ] = "Presentation 1";
    char programme_name_2[DLB_ADM_DATA_NAME_SZ] = "Presentation 2";  

    dlb_adm_presentation_label labels_1[2];
    strcpy(labels_1[0].language_tag, "fre");
    strcpy(labels_1[0].presentation_label, "Programme 1");
    strcpy(labels_1[1].language_tag, "eng");
    strcpy(labels_1[1].presentation_label, "Presentation 1");
    dlb_adm_presentation_label labels_2[2];
    strcpy(labels_2[0].language_tag, "eng");
    strcpy(labels_2[0].presentation_label, "Presentation 2");
    strcpy(labels_2[1].language_tag, "fre");
    strcpy(labels_2[1].presentation_label, "Programme 2");    
    unsigned int num_labels = 2;

    programme_id_1 = dlb_adm_add_audio_programme(mCoreModel, programme_name_1, lang_tag_obj_1, labels_1, num_labels, &mNames);
    dlb_adm_add_element_to_programme(mCoreModel, programme_id_1, elements_ids_bed.content_group_id, elements_ids_bed.element_id);
    dlb_adm_add_element_to_programme(mCoreModel, programme_id_1, elements_ids_obj_1.content_group_id, elements_ids_obj_1.element_id);

    programme_id_2 = dlb_adm_add_audio_programme(mCoreModel, programme_name_2, lang_tag_obj_2, labels_2, num_labels, &mNames);
    dlb_adm_add_element_to_programme(mCoreModel, programme_id_2, elements_ids_bed.content_group_id, elements_ids_bed.element_id);
    dlb_adm_add_element_to_programme(mCoreModel, programme_id_2, elements_ids_obj_2.content_group_id, elements_ids_obj_2.element_id);

    ASSERT_EQ(DLB_ADM_STATUS_OK, dlb_adm_container_open_from_core_model(&mXmlContainer, mCoreModel));
    ASSERT_EQ(DLB_ADM_STATUS_OK, dlb_adm_container_write_xml_file(mXmlContainer, mFileNameME2D));
}

