/************************************************************************
 * dlb_pmd
 * Copyright (c) 2021-2025, Dolby Laboratories Inc.
 * Copyright (c) 2021-2025, Dolby International AB.
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

#include "dlb_pmd/include/dlb_pmd_xml_file.h"
#include "dlb_pmd/include/dlb_pmd_sadm_file.h"
#include "dlb_pmd/include/dlb_pmd_model_combo.h"
#include "dlb_pmd/frontend/pmd_tool/xml.h"

#include "dlb_adm/include/dlb_adm_api.h"
#include "dlb_pmd/src/modules/sadm/pmd_core_model_generator.h"
#include "dlb_pmd/src/modules/sadm/pmd_bitset.h"
#include "test_data.h"

#include "DlbPmdModelWrapper.h"

#include <fstream>

#define CHECK_STATUS(X) if ((X) != DLB_ADM_STATUS_OK) return (X)

static const char smallModel2InputFileName[] = "SmallADMModel2.xml";
static const char smallModel2OutputFileName[] = "SmallADMModel2Output.xml";

static const char dolbyReferenceInputFileName[] = "DolbyReference.xml";
static const char dolbyReference_1_7_OutputFileName[] = "DolbyReference_1_7_Output.xml";
static const char dolbyReferenceOutputFileName[] = "DolbyReferenceOutput.xml";

static const char extraObjectPMDInputFileName[] = "ExtraObject.pmd.xml";
static const char extraObjectADMInputFileName[] = "ExtraObject.sadm.xml";
static const char extraObjectADMOutputFileName[] = "ExtraObject.out.sadm.xml";

static const char altSpkrADMInputFileName[] = "AltSpkr.sadm.xml";
static const char altSpkrADMOutputFileName[] = "AltSpkr.out.sadm.xml";

static const char altSpkrPMDOriginalFileName[] = "AltSpkr.pmd.xml";
static const char altSpkrPMDOutputFileName[] = "AltSpkr.out.pmd.xml";

static const char contentKindADMInputFileName[] = "ContentKind.sadm.xml";
static const char contentKindADMOutputFileName[] = "ContentKind.out.sadm.xml";

static const char contentKindPMDOriginalFileName[] = "ContentKind.pmd.xml";
static const char contentKindPMDOutputFileName[] = "ContentKind.out.pmd.xml";

static const char bedClassCM_PMDInputFileName[] = "BedClassCM.pmd.xml";
static const char bedClassCM_SADMInputFileName[] = "BedClassCM.sadm.xml";
static const char bedClassCM_SADMOutputFileName[] = "BedClassCM.out.sadm.xml";

static const char bedClassME_D_PMDInputFileName[] = "BedClassME_D.pmd.xml";
static const char bedClassME_D_SADMInputFileName[] = "BedClassME_D.sadm.xml";
static const char bedClassME_D_SADMOutputFileName[] = "BedClassME_D.out.sadm.xml";

static const char bedClassDEF_CM_BM_PMDInputFileName[] = "BedClassDEF_CM_BM.pmd.xml";
static const char bedClassDEF_CM_BM_SADMInputFileName[] = "BedClassDEF_CM_BM.sadm.xml";
static const char bedClassDEF_CM_BM_SADMOutputFileName[] = "BedClassDEF_CM_BM.out.sadm.xml";

static const char content_label_diff_lang_PMDInputFileName[] = "content_label_diff_lang.pmd.xml";
static const char content_label_diff_lang_SADMInputFileName[] = "content_label_diff_lang.sadm.xml";
static const char content_label_diff_lang_SADMOutputFileName[] = "content_label_diff_lang.out.sadm.xml";

static const char content_label_same_lang_PMDInputFileName[] = "content_label_same_lang.pmd.xml";
static const char content_label_same_lang_SADMInputFileName[] = "content_label_same_lang.sadm.xml";
static const char content_label_same_lang_SADMOutputFileName[] = "content_label_same_lang.out.sadm.xml";

class CoreModelGenerator : public testing::Test
{
protected:
    dlb_pmd_model *mPmdModel;
    dlb_pmd_model_combo *mPmdModelCombo;
    dlb_adm_xml_container *mContainer;
    dlb_adm_core_model *mCoreModel;
    pmd_core_model_generator *mGenerator;

    char *mPmdModelMemory;
    char *mPmdModelComboMemory;
    char *mGeneratorMemory;

    void SetUpTestInput(const char *path, const char *content)
    {
        std::ifstream ifs(path);

        if (!ifs.good())
        {
            std::ofstream ofs(path);

            ofs << content;
        }
    }

    int InitPmdModel()
    {
        dlb_pmd_model_constraints c;
        size_t sz;

        ::dlb_pmd_max_constraints(&c, PMD_FALSE);
        sz = ::dlb_pmd_query_mem_constrained(&c);
        mPmdModelMemory = new char[sz];
        if (mPmdModelMemory == nullptr)
        {
            return DLB_ADM_STATUS_OUT_OF_MEMORY;
        }
        dlb_pmd_init_constrained(&mPmdModel, &c, mPmdModelMemory);

        return DLB_ADM_STATUS_OK;
    }

    int InitCoreModel()
    {
        dlb_adm_core_model_counts counts;
        int status;

        ::memset(&counts, 0, sizeof(counts));
        status = ::dlb_adm_core_model_open(&mCoreModel, &counts);

        return status;
    }

    bool InitComboModel(dlb_pmd_model *existing_pmd_model, dlb_adm_core_model *existing_core_model)
    {
        bool good = true;

        try
        {
            size_t sz = ::dlb_pmd_model_combo_query_mem(existing_pmd_model, existing_core_model);
            dlb_pmd_success success;

            if (sz == 0) throw false;
            mPmdModelComboMemory = new char[sz];
            if (mPmdModelComboMemory == nullptr) throw false;
            success = ::dlb_pmd_model_combo_init(&mPmdModelCombo, existing_pmd_model, existing_core_model, PMD_FALSE, mPmdModelComboMemory);
            if ((success == PMD_FAIL) || (mPmdModelCombo == nullptr)) throw false;
        }
        catch (...)
        {
            good = false;
        }

        return good;
    }

    dlb_pmd_success InitGenerator()
    {
        size_t sz;

        if (::pmd_core_model_generator_query_memory_size(&sz))
        {
            return PMD_FAIL;
        }

        mGeneratorMemory = new char[sz];
        if (mGeneratorMemory == nullptr)
        {
            return PMD_FAIL;
        }

        if (::pmd_core_model_generator_open(&mGenerator, mGeneratorMemory))
        {
            return PMD_FAIL;
        }

        return PMD_SUCCESS;
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
        mPmdModel = nullptr;
        mContainer = nullptr;
        mCoreModel = nullptr;
        mGenerator = nullptr;

        mPmdModelMemory = nullptr;
        mGeneratorMemory = nullptr;
    }

    virtual void TearDown()
    {
        if (mGenerator != nullptr)
        {
            (void)::pmd_core_model_generator_close(&mGenerator);
        }
        if (mCoreModel != nullptr)
        {
            (void)::dlb_adm_core_model_close(&mCoreModel);
        }
        if (mContainer != nullptr)
        {
            (void)::dlb_adm_container_close(&mContainer);
        }
        if (mPmdModel != nullptr)
        {
            ::dlb_pmd_finish(mPmdModel);
            mPmdModel = nullptr;
        }

        if (mGeneratorMemory != nullptr)
        {
            delete[] mGeneratorMemory;
            mGeneratorMemory = nullptr;
        }
        if (mPmdModelMemory != nullptr)
        {
            delete[] mPmdModelMemory;
            mPmdModelMemory = nullptr;
        }
    }

};

TEST(core_model_generator, BitSet)
{
    uint8_t              element_set[(DLB_PMD_MAX_AUDIO_ELEMENTS + (BYTE_BITS - 1)) / BYTE_BITS];
    dlb_pmd_element_id   fib[] = { 0, 1 };
    dlb_pmd_element_id   element_id;
    dlb_pmd_element_id   previous_id;
    const uint8_t        all_bits = (uint8_t)(~0);
    size_t               i;

    ::memset(element_set, 0, sizeof(element_set));

    element_id = fib[0] + fib[1];
    while (element_id <= DLB_PMD_MAX_AUDIO_ELEMENTS)
    {
        set_element_bit(element_set, element_id);
        fib[0] = fib[1];
        fib[1] = element_id;
        element_id += fib[0];
    }

    fib[0] = 0;
    fib[1] = 1;
    EXPECT_FALSE(get_element_bit(element_set, fib[0]));
    element_id = fib[0] + fib[1];
    previous_id = element_id;
    while (element_id <= DLB_PMD_MAX_AUDIO_ELEMENTS)
    {
        while (++previous_id < element_id)
        {
            EXPECT_FALSE(get_element_bit(element_set, previous_id));
        }

        EXPECT_TRUE(get_element_bit(element_set, element_id));

        previous_id = element_id;
        fib[0] = fib[1];
        fib[1] = element_id;
        element_id += fib[0];
    }

    ::memset(element_set, 0, sizeof(element_set));
    for (element_id = 0; element_id < sizeof(element_set) * BYTE_BITS; element_id++)
    {
        set_element_bit(element_set, element_id);
    }
    for (i = 0; i < sizeof(element_set); i++)
    {
        EXPECT_EQ(all_bits, element_set[i]);
    }
}

TEST(core_model_generator, QueryMem)
{
    dlb_pmd_success success;
    size_t sz;

    success = pmd_core_model_generator_query_memory_size(nullptr);
    EXPECT_EQ(PMD_FAIL, success);

    sz = 0;
    success = pmd_core_model_generator_query_memory_size(&sz);
    EXPECT_EQ(PMD_SUCCESS, success);
    EXPECT_LT(0, sz);
}

TEST_F(CoreModelGenerator, Open)
{
    dlb_pmd_success success;
    size_t sz;

    success = pmd_core_model_generator_query_memory_size(&sz);
    ASSERT_EQ(PMD_SUCCESS, success);
    mGeneratorMemory = new char[sz];
    ASSERT_NE(nullptr, mGeneratorMemory);

    success = ::pmd_core_model_generator_open(nullptr, nullptr);
    EXPECT_EQ(PMD_FAIL, success);
    success = ::pmd_core_model_generator_open(&mGenerator, nullptr);
    EXPECT_EQ(PMD_FAIL, success);

    success = ::pmd_core_model_generator_open(&mGenerator, mGeneratorMemory);
    EXPECT_EQ(PMD_SUCCESS, success);
    EXPECT_NE(nullptr, mGenerator);
}

TEST_F(CoreModelGenerator, GenerateBasic)
{
    const dlb_pmd_model *const_pmd_model;
    dlb_pmd_success success;
    int status;

    SetUpTestInput(smallModel2InputFileName, smallXML2);
    status = InitPmdModel();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    ASSERT_TRUE(InitComboModel(mPmdModel, nullptr));

    status = ::xml_read(smallModel2InputFileName, mPmdModelCombo, PMD_FALSE, PMD_FALSE);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    success = ::dlb_pmd_model_combo_convert_to_pmd_model(mPmdModelCombo, smallModel2InputFileName, &const_pmd_model);
    ASSERT_EQ(PMD_SUCCESS, success);
    ASSERT_EQ(mPmdModel, const_pmd_model);

    status = InitCoreModel();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    success = InitGenerator();
    ASSERT_EQ(PMD_SUCCESS, success);

    success = ::pmd_core_model_generator_generate(nullptr, nullptr, nullptr);
    EXPECT_EQ(PMD_FAIL, success);
    success = ::pmd_core_model_generator_generate(mGenerator, nullptr, nullptr);
    EXPECT_EQ(PMD_FAIL, success);
    success = ::pmd_core_model_generator_generate(mGenerator, mCoreModel, nullptr);
    EXPECT_EQ(PMD_FAIL, success);

    success = ::pmd_core_model_generator_generate(mGenerator, mCoreModel, mPmdModel);
    EXPECT_EQ(PMD_SUCCESS, success);
    status = ::dlb_adm_container_open_from_core_model(&mContainer, mCoreModel);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    status = dlb_adm_container_write_xml_file(mContainer, smallModel2OutputFileName);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    EXPECT_TRUE(CompareFiles(smallModel2InputFileName, smallModel2OutputFileName));
}

TEST_F(CoreModelGenerator, Close)
{
    pmd_core_model_generator *p = nullptr;
    dlb_pmd_success success;

    success = InitGenerator();
    ASSERT_EQ(PMD_SUCCESS, success);
    ASSERT_NE(nullptr, mGenerator);

    success = ::pmd_core_model_generator_close(nullptr);
    EXPECT_EQ(PMD_FAIL, success);
    success = ::pmd_core_model_generator_close(&p);
    EXPECT_EQ(PMD_FAIL, success);

    success = ::pmd_core_model_generator_close(&mGenerator);
    EXPECT_EQ(PMD_SUCCESS, success);
    EXPECT_EQ(nullptr, mGenerator);
}

TEST_F(CoreModelGenerator, PMDExtraObjectGen)
{
    dlb_pmd_success success;
    int status;

    SetUpTestInput(extraObjectPMDInputFileName, extraObjectPMD);
    SetUpTestInput(extraObjectADMInputFileName, extraObjectADM);
    status = InitPmdModel();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    ASSERT_TRUE(InitComboModel(mPmdModel, nullptr));

    status = ::xml_read(extraObjectPMDInputFileName, mPmdModelCombo, PMD_FALSE, PMD_FALSE);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = InitCoreModel();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    success = InitGenerator();
    ASSERT_EQ(PMD_SUCCESS, success);

    success = ::pmd_core_model_generator_generate(mGenerator, mCoreModel, mPmdModel);
    EXPECT_EQ(PMD_SUCCESS, success);
    status = ::dlb_adm_container_open_from_core_model(&mContainer, mCoreModel);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    status = dlb_adm_container_write_xml_file(mContainer, extraObjectADMOutputFileName);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    EXPECT_TRUE(CompareFiles(extraObjectADMInputFileName, extraObjectADMOutputFileName));
}

TEST_F(CoreModelGenerator, AltSpkrs)
{
    dlb_pmd_model_combo *comboModel;
    dlb_pmd_element_id eid;
    dlb_pmd_success success;
    int status;

    SetUpTestInput(altSpkrADMInputFileName, altSpkrADM);
    status = InitPmdModel();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = InitCoreModel();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // Set up and write out the PMD model
    success = ::dlb_pmd_set_title(mPmdModel, "Converted from Serial ADM");
    ASSERT_EQ(PMD_SUCCESS, success);
    success = ::dlb_pmd_add_signals(mPmdModel, 18);
    ASSERT_EQ(PMD_SUCCESS, success);
    success = ::dlb_pmd_add_bed(mPmdModel, 1, "Bed 1$[ME]$[C]", DLB_PMD_SPEAKER_CONFIG_5_1,   1, 0);
    ASSERT_EQ(PMD_SUCCESS, success);
    success = ::dlb_pmd_add_bed(mPmdModel, 2, "Bed 2", DLB_PMD_SPEAKER_CONFIG_7_1_4, 7, 0);
    ASSERT_EQ(PMD_SUCCESS, success);
    eid = 1;
    success = ::dlb_pmd_add_presentation(mPmdModel, 1, "en", "Pres 1", "en", DLB_PMD_SPEAKER_CONFIG_5_1, 1, &eid);
    ASSERT_EQ(PMD_SUCCESS, success);
    eid = 2;
    success = ::dlb_pmd_add_presentation(mPmdModel, 2, "en", "Pres 2", "en", DLB_PMD_SPEAKER_CONFIG_7_1_4, 1, &eid);
    ASSERT_EQ(PMD_SUCCESS, success);
    success = ::dlb_xmlpmd_file_write(altSpkrPMDOriginalFileName, mPmdModel);
    ASSERT_EQ(PMD_SUCCESS, success);

    // Wrap the model, write the S-ADM and compare with the "gold standard"
    DlbAdm::DlbPmdModelWrapper pmdWrapper(&comboModel, mPmdModel, PMD_FALSE);

    success = ::dlb_pmd_sadm_file_write(altSpkrADMOutputFileName, comboModel);
    EXPECT_EQ(PMD_SUCCESS, success);

    EXPECT_TRUE(CompareFiles(altSpkrADMInputFileName, altSpkrADMOutputFileName));

    // Read the S-ADM, convert it to PMD, write it out and compare with the original PMD
    DlbAdm::DlbPmdModelWrapper sadmWrapper(&comboModel, mCoreModel, PMD_FALSE);
    const dlb_pmd_model *pmdModel;

    success = ::dlb_pmd_sadm_file_read(altSpkrADMOutputFileName, comboModel, PMD_TRUE, nullptr, nullptr);
    ASSERT_EQ(PMD_SUCCESS, success);
    success = ::dlb_pmd_model_combo_ensure_readable_pmd_model(comboModel, &pmdModel, PMD_FALSE);
    ASSERT_EQ(PMD_SUCCESS, success);
    success = ::dlb_xmlpmd_file_write(altSpkrPMDOutputFileName, pmdModel);
    ASSERT_EQ(PMD_SUCCESS, success);

    EXPECT_TRUE(CompareFiles(altSpkrPMDOriginalFileName, altSpkrPMDOutputFileName));
}

TEST_F(CoreModelGenerator, ContentKindCheck)
{
    dlb_pmd_model_combo *comboModel;
    dlb_pmd_element_id eid;
    dlb_pmd_success success;
    int status;

    SetUpTestInput(contentKindADMInputFileName, contentKindADM);
    status = InitPmdModel();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = InitCoreModel();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    // Set up and write out the PMD model
    success = ::dlb_pmd_set_title(mPmdModel, "Converted from Serial ADM");
    ASSERT_EQ(PMD_SUCCESS, success);
    success = ::dlb_pmd_add_signals(mPmdModel, 12);
    ASSERT_EQ(PMD_SUCCESS, success);
    success = ::dlb_pmd_add_bed(mPmdModel, 1, "Bed 1$[CM]$[C]", DLB_PMD_SPEAKER_CONFIG_5_1,   1, 0);
    ASSERT_EQ(PMD_SUCCESS, success);
    success = ::dlb_pmd_add_bed(mPmdModel, 2, "Bed 2$[ME]", DLB_PMD_SPEAKER_CONFIG_2_0, 7, 0);
    ASSERT_EQ(PMD_SUCCESS, success);
    success = ::dlb_pmd_add_bed(mPmdModel, 3, "Bed 3$[ML]$[C]", DLB_PMD_SPEAKER_CONFIG_2_0, 9, 0);
    ASSERT_EQ(PMD_SUCCESS, success);
    success = ::dlb_pmd_add_bed(mPmdModel, 4, "Bed 4$[BM]", DLB_PMD_SPEAKER_CONFIG_2_0, 11, 0);
    ASSERT_EQ(PMD_SUCCESS, success);
    eid = 1;
    success = ::dlb_pmd_add_presentation(mPmdModel, 1, "en", "Pres 1", "en", DLB_PMD_SPEAKER_CONFIG_5_1, 1, &eid);
    ASSERT_EQ(PMD_SUCCESS, success);
    eid = 2;
    success = ::dlb_pmd_add_presentation(mPmdModel, 2, "en", "Pres 2", "en", DLB_PMD_SPEAKER_CONFIG_2_0, 1, &eid);
    ASSERT_EQ(PMD_SUCCESS, success);
    eid = 3;
    success = ::dlb_pmd_add_presentation(mPmdModel, 3, "en", "Pres 3", "en", DLB_PMD_SPEAKER_CONFIG_2_0, 1, &eid);
    ASSERT_EQ(PMD_SUCCESS, success);
    eid = 4;
    success = ::dlb_pmd_add_presentation(mPmdModel, 4, "en", "Pres 4", "en", DLB_PMD_SPEAKER_CONFIG_2_0, 1, &eid);
    ASSERT_EQ(PMD_SUCCESS, success);
    success = ::dlb_xmlpmd_file_write(contentKindPMDOriginalFileName, mPmdModel);
    ASSERT_EQ(PMD_SUCCESS, success);

    // Wrap the model, write the S-ADM and compare with the "gold standard"
    DlbAdm::DlbPmdModelWrapper pmdWrapper(&comboModel, mPmdModel, PMD_FALSE);

    success = ::dlb_pmd_sadm_file_write(contentKindADMOutputFileName, comboModel);
    EXPECT_EQ(PMD_SUCCESS, success);

    EXPECT_TRUE(CompareFiles(contentKindADMInputFileName, contentKindADMOutputFileName));

    // Read the S-ADM, convert it to PMD, write it out and compare with the original PMD
    DlbAdm::DlbPmdModelWrapper sadmWrapper(&comboModel, mCoreModel, PMD_FALSE);
    const dlb_pmd_model *pmdModel;

    success = ::dlb_pmd_sadm_file_read(contentKindADMOutputFileName, comboModel, PMD_TRUE, nullptr, nullptr);
    ASSERT_EQ(PMD_SUCCESS, success);
    success = ::dlb_pmd_model_combo_ensure_readable_pmd_model(comboModel, &pmdModel, PMD_FALSE);
    ASSERT_EQ(PMD_SUCCESS, success);
    success = ::dlb_xmlpmd_file_write(contentKindPMDOutputFileName, pmdModel);
    ASSERT_EQ(PMD_SUCCESS, success);

    EXPECT_TRUE(CompareFiles(contentKindPMDOriginalFileName, contentKindPMDOutputFileName));
}

TEST_F(CoreModelGenerator, PMD_bedClass_CM_to_SADM)
{
    dlb_pmd_success success;
    int status;
    dlb_adm_xml_container    *container = NULL;
    dlb_adm_container_counts  counts;

    SetUpTestInput(bedClassCM_PMDInputFileName, bedClassCM_PMD);
    SetUpTestInput(bedClassCM_SADMInputFileName, bedClassCM_SADM);
    status = InitPmdModel();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    ASSERT_TRUE(InitComboModel(mPmdModel, nullptr));

    status = ::xml_read(bedClassCM_PMDInputFileName, mPmdModelCombo, PMD_FALSE, PMD_TRUE);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = InitCoreModel();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    success = dlb_adm_core_model_add_profile(mCoreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE);
    EXPECT_EQ(PMD_SUCCESS, success);
    status = dlb_adm_container_open(&container, &counts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = dlb_adm_container_load_common_definitions(container);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = dlb_adm_core_model_ingest_common_definitions_container(mCoreModel, container);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    success = InitGenerator();
    ASSERT_EQ(PMD_SUCCESS, success);
    success = ::pmd_core_model_generator_generate(mGenerator, mCoreModel, mPmdModel);
    EXPECT_EQ(PMD_SUCCESS, success);
    status = ::dlb_adm_container_open_from_core_model(&mContainer, mCoreModel);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    status = dlb_adm_container_write_xml_file(mContainer, bedClassCM_SADMOutputFileName);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    EXPECT_TRUE(CompareFiles(bedClassCM_SADMInputFileName, bedClassCM_SADMOutputFileName));
}

TEST_F(CoreModelGenerator, PMD_bedClass_ME_D_to_SADM)
{
    dlb_pmd_success success;
    int status;
    dlb_adm_xml_container    *container = NULL;
    dlb_adm_container_counts  counts;

    SetUpTestInput(bedClassME_D_PMDInputFileName, bedClassME_D_PMD);
    SetUpTestInput(bedClassME_D_SADMInputFileName, bedClassME_D_SADM);
    status = InitPmdModel();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    ASSERT_TRUE(InitComboModel(mPmdModel, nullptr));

    status = ::xml_read(bedClassME_D_PMDInputFileName, mPmdModelCombo, PMD_FALSE, PMD_TRUE);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = InitCoreModel();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    success = dlb_adm_core_model_add_profile(mCoreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE);
    EXPECT_EQ(PMD_SUCCESS, success);
    status = dlb_adm_container_open(&container, &counts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = dlb_adm_container_load_common_definitions(container);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = dlb_adm_core_model_ingest_common_definitions_container(mCoreModel, container);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    success = InitGenerator();
    ASSERT_EQ(PMD_SUCCESS, success);
    success = ::pmd_core_model_generator_generate(mGenerator, mCoreModel, mPmdModel);
    EXPECT_EQ(PMD_SUCCESS, success);
    status = ::dlb_adm_container_open_from_core_model(&mContainer, mCoreModel);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    status = dlb_adm_container_write_xml_file(mContainer, bedClassME_D_SADMOutputFileName);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    EXPECT_TRUE(CompareFiles(bedClassME_D_SADMInputFileName, bedClassME_D_SADMOutputFileName));
}

TEST_F(CoreModelGenerator, PMD_bedClass_DEF_CM_BM_to_SADM)
{
    dlb_pmd_success success;
    int status;
    dlb_adm_xml_container    *container = NULL;
    dlb_adm_container_counts  counts;

    SetUpTestInput(bedClassDEF_CM_BM_PMDInputFileName, bedClassDEF_CM_BM_PMD);
    SetUpTestInput(bedClassDEF_CM_BM_SADMInputFileName, bedClassDEF_CM_BM_SADM);
    status = InitPmdModel();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    ASSERT_TRUE(InitComboModel(mPmdModel, nullptr));

    status = ::xml_read(bedClassDEF_CM_BM_PMDInputFileName, mPmdModelCombo, PMD_FALSE, PMD_TRUE);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = InitCoreModel();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    success = dlb_adm_core_model_add_profile(mCoreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE);
    EXPECT_EQ(PMD_SUCCESS, success);
    status = dlb_adm_container_open(&container, &counts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = dlb_adm_container_load_common_definitions(container);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = dlb_adm_core_model_ingest_common_definitions_container(mCoreModel, container);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    success = InitGenerator();
    ASSERT_EQ(PMD_SUCCESS, success);
    success = ::pmd_core_model_generator_generate(mGenerator, mCoreModel, mPmdModel);
    EXPECT_EQ(PMD_SUCCESS, success);
    status = ::dlb_adm_container_open_from_core_model(&mContainer, mCoreModel);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    status = dlb_adm_container_write_xml_file(mContainer, bedClassDEF_CM_BM_SADMOutputFileName);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    EXPECT_TRUE(CompareFiles(bedClassDEF_CM_BM_SADMInputFileName, bedClassDEF_CM_BM_SADMOutputFileName));
}

TEST_F(CoreModelGenerator, contentLabelDiffLang)
{
    dlb_pmd_success success;
    int status;
    dlb_adm_xml_container    *container = NULL;
    dlb_adm_container_counts  counts;

    SetUpTestInput(content_label_diff_lang_PMDInputFileName, content_label_diff_lang_PMD);
    SetUpTestInput(content_label_diff_lang_SADMInputFileName, content_label_diff_lang_SADM);
    status = InitPmdModel();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    ASSERT_TRUE(InitComboModel(mPmdModel, nullptr));

    status = ::xml_read(content_label_diff_lang_PMDInputFileName, mPmdModelCombo, PMD_FALSE, PMD_TRUE);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = InitCoreModel();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    success = dlb_adm_core_model_add_profile(mCoreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE);
    EXPECT_EQ(PMD_SUCCESS, success);
    status = dlb_adm_container_open(&container, &counts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = dlb_adm_container_load_common_definitions(container);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = dlb_adm_core_model_ingest_common_definitions_container(mCoreModel, container);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    success = InitGenerator();
    ASSERT_EQ(PMD_SUCCESS, success);
    success = ::pmd_core_model_generator_generate(mGenerator, mCoreModel, mPmdModel);
    EXPECT_EQ(PMD_SUCCESS, success);
    status = ::dlb_adm_container_open_from_core_model(&mContainer, mCoreModel);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    status = dlb_adm_container_write_xml_file(mContainer, content_label_diff_lang_SADMOutputFileName);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    EXPECT_TRUE(CompareFiles(content_label_diff_lang_SADMInputFileName, content_label_diff_lang_SADMOutputFileName));
}

TEST_F(CoreModelGenerator, contentLabelSameLang)
{
    dlb_pmd_success success;
    int status;
    dlb_adm_xml_container    *container = NULL;
    dlb_adm_container_counts  counts;

    SetUpTestInput(content_label_same_lang_PMDInputFileName, content_label_same_lang_PMD);
    SetUpTestInput(content_label_same_lang_SADMInputFileName, content_label_same_lang_SADM);
    status = InitPmdModel();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    ASSERT_TRUE(InitComboModel(mPmdModel, nullptr));

    status = ::xml_read(content_label_same_lang_PMDInputFileName, mPmdModelCombo, PMD_FALSE, PMD_TRUE);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    status = InitCoreModel();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    success = dlb_adm_core_model_add_profile(mCoreModel, DLB_ADM_PROFILE_SADM_EMISSION_PROFILE);
    EXPECT_EQ(PMD_SUCCESS, success);
    status = dlb_adm_container_open(&container, &counts);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = dlb_adm_container_load_common_definitions(container);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = dlb_adm_core_model_ingest_common_definitions_container(mCoreModel, container);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);

    success = InitGenerator();
    ASSERT_EQ(PMD_SUCCESS, success);
    success = ::pmd_core_model_generator_generate(mGenerator, mCoreModel, mPmdModel);
    EXPECT_EQ(PMD_SUCCESS, success);
    status = ::dlb_adm_container_open_from_core_model(&mContainer, mCoreModel);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);
    status = dlb_adm_container_write_xml_file(mContainer, content_label_same_lang_SADMOutputFileName);
    EXPECT_EQ(DLB_ADM_STATUS_OK, status);

    EXPECT_TRUE(CompareFiles(content_label_same_lang_SADMInputFileName, content_label_same_lang_SADMOutputFileName));
}
