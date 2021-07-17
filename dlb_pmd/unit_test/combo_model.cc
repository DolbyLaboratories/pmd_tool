/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2021 by Dolby Laboratories,
 *                Copyright (C) 2021 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

#include "gtest/gtest.h"

#include "DlbPmdModelWrapper.h"
#include "dlb_pmd_api.h"
#include "dlb_pmd_xml_file.h"
#include "dlb_adm/include/dlb_adm_api.h"
#include "test_data.h"

#include <cstring>
#include <fstream>

static const char stereo_2D_PMD_InputFileName[] = "Stereo_2D_PMD_combo.xml";
static const char stereo_2D_PMD_OutputFileName[] = "Stereo_2D_PMD_combo.pmd.out.xml";

static const char stereo_2D_ADM_InputFileName[] = "Stereo_2D_ADM_combo.xml";
//static const char stereo_2D_ADM_OutputFileName[] = "Stereo_2D_ADM_combo.sadm.out.xml";

static const char extraObject_PMD_InputFileName[] = "Extra_Object_PMD_combo.xml";
//static const char extraObject_PMD_OutputFileName[] = "Extra_Object_PMD_combo.pmd.out.xml";

static const char extraObject_ADM_InputFileName[] = "Extra_Object_ADM_combo.xml";
static const char extraObject_ADM_OutputFileName[] = "Extra_Object_ADM_combo.sadm.out.xml";

class ComboModel : public testing::Test
{
protected:
    dlb_pmd_model *mPmdModel;
    dlb_pmd_model_combo *mPmdModelCombo;
    uint8_t *mPmdModelMemory;
    uint8_t *mPmdModelComboMemory;

    dlb_adm_container_counts mContainerCounts;
    dlb_adm_xml_container *mContainer;

    dlb_adm_core_model_counts mCoreModelCounts;
    dlb_adm_core_model *mCoreModel;

    void SetUpTestInput(const char *path, const char *content)
    {
        std::ifstream ifs(path);

        if (!ifs.good())
        {
            std::ofstream ofs(path);

            ofs << content;
        }
    }

    bool ReadXmlFile(const char *filename)
    {
        bool good = true;

        try
        {
            int status;

            status = ::dlb_adm_container_open(&mContainer, &mContainerCounts);
            if (status != DLB_ADM_STATUS_OK) throw false;
            status = ::dlb_adm_container_read_xml_file(mContainer, filename, DLB_ADM_FALSE);
            if (status != DLB_ADM_STATUS_OK) throw false;
        }
        catch (...)
        {
            good = false;
        }

        return good;
    }

    bool InitPmdModel()
    {
        bool good = true;

        try
        {
            size_t sz = ::dlb_pmd_query_mem();

            if (sz == 0) throw false;
            mPmdModelMemory = new uint8_t[sz];
            if (mPmdModelMemory == nullptr) throw false;
            dlb_pmd_init(&mPmdModel, mPmdModelMemory);
            if (mPmdModel == nullptr) throw false;
        }
        catch (...)
        {
            good = false;
        }

        return good;
    }

    bool InitCoreModel()
    {
        bool good = true;
        int status;

        status = ::dlb_adm_core_model_open(&mCoreModel, &mCoreModelCounts);
        if ((status != DLB_ADM_STATUS_OK) || (mCoreModel == nullptr))
        {
            good = false;
        }

        return good;
    }

    bool InitComboModel(dlb_pmd_model *existing_pmd_model, dlb_adm_core_model *existing_core_model)
    {
        bool good = true;

        try
        {
            size_t sz = ::dlb_pmd_model_combo_query_mem(existing_pmd_model, existing_core_model);
            dlb_pmd_success success;

            if (sz == 0) throw false;
            mPmdModelComboMemory = new uint8_t[sz];
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
        mPmdModelCombo = nullptr;
        mPmdModelMemory = nullptr;
        mPmdModelComboMemory = nullptr;

        ::memset(&mContainerCounts, 0, sizeof(mContainerCounts));
        mContainer = nullptr;

        ::memset(&mCoreModelCounts, 0, sizeof(mCoreModelCounts));
        mCoreModel = nullptr;
    }

    virtual void TearDown()
    {
        if (mPmdModelCombo)
        {
            (void)dlb_pmd_model_combo_destroy(&mPmdModelCombo);
        }
        if (mCoreModel != nullptr)
        {
            (void)::dlb_adm_core_model_close(&mCoreModel);
        }
        if (mContainer != nullptr)
        {
            (void)::dlb_adm_container_close(&mContainer);
        }
        if (mPmdModelComboMemory != nullptr)
        {
            delete[] mPmdModelComboMemory;
        }
        if (mPmdModelMemory != nullptr)
        {
            delete[] mPmdModelMemory;
        }
    }

};

TEST_F(ComboModel, QueryMem)
{
    int status;
    size_t sz1;
    size_t sz2;
    size_t sz3;

    sz1 = ::dlb_pmd_query_mem();
    ASSERT_NE(0, sz1);
    mPmdModelMemory = new uint8_t[sz1];
    ASSERT_NE(nullptr, mPmdModelMemory);
    dlb_pmd_init(&mPmdModel, mPmdModelMemory);
    ASSERT_NE(nullptr, mPmdModel);
    status = ::dlb_adm_core_model_open(&mCoreModel, &mCoreModelCounts);
    ASSERT_NE(nullptr, mCoreModel);

    sz1 = ::dlb_pmd_model_combo_query_mem(nullptr, nullptr);
    EXPECT_LT(0, sz1);
    sz2 = ::dlb_pmd_model_combo_query_mem(mPmdModel, nullptr);
    EXPECT_LT(0, sz2);
    EXPECT_LT(sz2, sz1);    // No PMD model memory needed, so it is smaller
    sz3 = ::dlb_pmd_model_combo_query_mem(mPmdModel, mCoreModel);
    EXPECT_LT(0, sz2);
    EXPECT_EQ(sz2, sz3);    // Existing core model makes no difference in this version
}

TEST_F(ComboModel, InitAndDestroy)
{
    dlb_pmd_success success;
    size_t sz;

    sz = ::dlb_pmd_model_combo_query_mem(nullptr, nullptr);
    ASSERT_LT(0, sz);
    mPmdModelComboMemory = new uint8_t[sz];
    ASSERT_NE(nullptr, mPmdModelComboMemory);

    success = ::dlb_pmd_model_combo_init(nullptr, nullptr, nullptr, PMD_FALSE, nullptr);
    EXPECT_EQ(PMD_FAIL, success);

    success = ::dlb_pmd_model_combo_init(&mPmdModelCombo, nullptr, nullptr, PMD_FALSE, mPmdModelComboMemory);
    EXPECT_EQ(PMD_SUCCESS, success);
    EXPECT_NE(nullptr, mPmdModelCombo);

    success = ::dlb_pmd_model_combo_destroy(&mPmdModelCombo);
    ASSERT_EQ(PMD_SUCCESS, success);
    ASSERT_EQ(nullptr, mPmdModelCombo);

    ASSERT_TRUE(InitPmdModel());
    ASSERT_NE(nullptr, mPmdModel);

    success = ::dlb_pmd_model_combo_init(&mPmdModelCombo, mPmdModel, nullptr, PMD_FALSE, mPmdModelComboMemory);
    EXPECT_EQ(PMD_SUCCESS, success);
    EXPECT_NE(nullptr, mPmdModelCombo);

    success = ::dlb_pmd_model_combo_destroy(&mPmdModelCombo);
    ASSERT_EQ(PMD_SUCCESS, success);
    ASSERT_EQ(nullptr, mPmdModelCombo);
    ASSERT_NE(nullptr, mPmdModel);

    ASSERT_TRUE(InitCoreModel());
    ASSERT_NE(nullptr, mCoreModel);

    success = ::dlb_pmd_model_combo_init(&mPmdModelCombo, nullptr, mCoreModel, PMD_FALSE, mPmdModelComboMemory);
    EXPECT_EQ(PMD_SUCCESS, success);
    EXPECT_NE(nullptr, mPmdModelCombo);

    success = ::dlb_pmd_model_combo_destroy(&mPmdModelCombo);
    ASSERT_EQ(PMD_SUCCESS, success);
    ASSERT_EQ(nullptr, mPmdModelCombo);
    ASSERT_NE(nullptr, mCoreModel);

    success = ::dlb_pmd_model_combo_init(&mPmdModelCombo, mPmdModel, mCoreModel, PMD_FALSE, mPmdModelComboMemory);
    EXPECT_EQ(PMD_SUCCESS, success);
    EXPECT_NE(nullptr, mPmdModelCombo);

    success = ::dlb_pmd_model_combo_destroy(&mPmdModelCombo);
    ASSERT_EQ(PMD_SUCCESS, success);
    ASSERT_EQ(nullptr, mPmdModelCombo);
    ASSERT_NE(nullptr, mPmdModel);
    ASSERT_NE(nullptr, mCoreModel);
}

TEST_F(ComboModel, Mallocate)
{
    dlb_pmd_success success;

    success = ::dlb_pmd_model_combo_init(&mPmdModelCombo, nullptr, nullptr, PMD_FALSE, nullptr);
    EXPECT_EQ(PMD_SUCCESS, success);
}

TEST_F(ComboModel, GetReadablePmdModel)
{
    const dlb_pmd_model *const_pmd_model = nullptr;
    dlb_pmd_success success;

    ASSERT_TRUE(InitComboModel(nullptr, nullptr));

    success = ::dlb_pmd_model_combo_get_readable_pmd_model(nullptr, nullptr, PMD_FALSE);
    EXPECT_EQ(PMD_FAIL, success);
    success = ::dlb_pmd_model_combo_get_readable_pmd_model(mPmdModelCombo, nullptr, PMD_FALSE);
    EXPECT_EQ(PMD_FAIL, success);

    success = ::dlb_pmd_model_combo_get_readable_pmd_model(mPmdModelCombo, &const_pmd_model, PMD_FALSE);
    EXPECT_EQ(PMD_SUCCESS, success);
    EXPECT_NE(nullptr, const_pmd_model);
}

TEST_F(ComboModel, GetWritablePmdModel)
{
    dlb_pmd_model *pmd_model = nullptr;
    dlb_pmd_success success;

    ASSERT_TRUE(InitComboModel(nullptr, nullptr));

    success = ::dlb_pmd_model_combo_get_writable_pmd_model(nullptr, nullptr, PMD_TRUE);
    EXPECT_EQ(PMD_FAIL, success);
    success = ::dlb_pmd_model_combo_get_writable_pmd_model(mPmdModelCombo, nullptr, PMD_TRUE);
    EXPECT_EQ(PMD_FAIL, success);

    success = ::dlb_pmd_model_combo_get_writable_pmd_model(mPmdModelCombo, &pmd_model, PMD_TRUE);
    EXPECT_EQ(PMD_SUCCESS, success);
    EXPECT_NE(nullptr, pmd_model);
}

TEST_F(ComboModel, GetReadableCoreModel)
{
    const dlb_adm_core_model *const_core_model = nullptr;
    dlb_pmd_success success;

    ASSERT_TRUE(InitComboModel(nullptr, nullptr));

    success = ::dlb_pmd_model_combo_get_readable_core_model(nullptr, nullptr);
    EXPECT_EQ(PMD_FAIL, success);
    success = ::dlb_pmd_model_combo_get_readable_core_model(mPmdModelCombo, nullptr);
    EXPECT_EQ(PMD_FAIL, success);

    success = ::dlb_pmd_model_combo_get_readable_core_model(mPmdModelCombo, &const_core_model);
    EXPECT_EQ(PMD_SUCCESS, success);
    EXPECT_NE(nullptr, const_core_model);
}

TEST_F(ComboModel, GetWritableCoreModel)
{
    dlb_adm_core_model *core_model = nullptr;
    dlb_pmd_success success;

    ASSERT_TRUE(InitComboModel(nullptr, nullptr));

    success = ::dlb_pmd_model_combo_get_writable_core_model(nullptr, nullptr);
    EXPECT_EQ(PMD_FAIL, success);
    success = ::dlb_pmd_model_combo_get_writable_core_model(mPmdModelCombo, nullptr);
    EXPECT_EQ(PMD_FAIL, success);

    success = ::dlb_pmd_model_combo_get_writable_core_model(mPmdModelCombo, &core_model);
    EXPECT_EQ(PMD_SUCCESS, success);
    EXPECT_NE(nullptr, core_model);
}

TEST_F(ComboModel, ConvertToPmdModel)
{
    const dlb_pmd_model *pmd_model = nullptr;
    const dlb_adm_core_model *const_core_model = nullptr;
    dlb_adm_core_model *core_model = nullptr;
    DLB_PMD_MODEL_COMBO_STATE pmd_model_state;
    DLB_PMD_MODEL_COMBO_STATE core_model_state;
    dlb_pmd_success success;
    int status;

    SetUpTestInput(stereo_2D_PMD_InputFileName, stereo_2D_PMD);
    SetUpTestInput(stereo_2D_ADM_InputFileName, stereo_2D_ADM);

    ASSERT_TRUE(ReadXmlFile(stereo_2D_ADM_InputFileName));
    ASSERT_TRUE(InitComboModel(nullptr, nullptr));
    success = ::dlb_pmd_model_combo_get_writable_core_model(mPmdModelCombo, &core_model);
    ASSERT_EQ(PMD_SUCCESS, success);
    status = ::dlb_adm_core_model_ingest_xml_container(core_model, mContainer);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    success = ::dlb_pmd_model_combo_get_state(mPmdModelCombo, &pmd_model_state, &core_model_state);
    ASSERT_EQ(PMD_SUCCESS, success);
    ASSERT_EQ(DLB_PMD_MODEL_COMBO_STATE_IS_EMPTY, pmd_model_state);
    ASSERT_EQ(DLB_PMD_MODEL_COMBO_STATE_IS_PRIMARY, core_model_state);

    success = ::dlb_pmd_model_combo_convert_to_pmd_model(nullptr, nullptr, nullptr);
    EXPECT_EQ(PMD_FAIL, success);
    success = ::dlb_pmd_model_combo_convert_to_pmd_model(mPmdModelCombo, nullptr, nullptr);
    EXPECT_EQ(PMD_FAIL, success);

    // lather
    success = ::dlb_pmd_model_combo_convert_to_pmd_model(mPmdModelCombo, "Small Stereo Program", &pmd_model);
    EXPECT_EQ(PMD_SUCCESS, success);

    success = ::dlb_xmlpmd_file_write(stereo_2D_PMD_OutputFileName, pmd_model);
    ASSERT_EQ(PMD_SUCCESS, success);
    EXPECT_TRUE(CompareFiles(stereo_2D_PMD_InputFileName, stereo_2D_PMD_OutputFileName));

    // rinse, repeat
    success = ::dlb_pmd_model_combo_convert_to_pmd_model(mPmdModelCombo, "Small Stereo Program", &pmd_model);
    EXPECT_EQ(PMD_SUCCESS, success);

    success = ::dlb_xmlpmd_file_write(stereo_2D_PMD_OutputFileName, pmd_model);
    ASSERT_EQ(PMD_SUCCESS, success);
    EXPECT_TRUE(CompareFiles(stereo_2D_PMD_InputFileName, stereo_2D_PMD_OutputFileName));

    // the combo model should be one-way now
    success = ::dlb_pmd_model_combo_convert_to_core_model(mPmdModelCombo, &const_core_model);
    EXPECT_EQ(PMD_FAIL, success);
}

TEST_F(ComboModel, ConvertToCoreModel)
{
    dlb_pmd_model *pmd_model = nullptr;
    const dlb_pmd_model *const_pmd_model = nullptr;
    const dlb_adm_core_model *core_model = nullptr;
    DLB_PMD_MODEL_COMBO_STATE pmd_model_state;
    DLB_PMD_MODEL_COMBO_STATE core_model_state;
    dlb_pmd_success success;
    int status;

    SetUpTestInput(extraObject_PMD_InputFileName, extraObjectPMD);
    SetUpTestInput(extraObject_ADM_InputFileName, extraObjectADM);

    ASSERT_TRUE(InitComboModel(nullptr, nullptr));
    success = ::dlb_pmd_model_combo_get_writable_pmd_model(mPmdModelCombo, &pmd_model, PMD_TRUE);
    ASSERT_EQ(PMD_SUCCESS, success);
    success = ::dlb_xmlpmd_file_read(extraObject_PMD_InputFileName, pmd_model, PMD_FALSE, nullptr, nullptr);
    ASSERT_EQ(PMD_SUCCESS, success);
    success = ::dlb_pmd_model_combo_get_state(mPmdModelCombo, &pmd_model_state, &core_model_state);
    ASSERT_EQ(PMD_SUCCESS, success);
    ASSERT_EQ(DLB_PMD_MODEL_COMBO_STATE_IS_PRIMARY, pmd_model_state);
    ASSERT_EQ(DLB_PMD_MODEL_COMBO_STATE_IS_EMPTY, core_model_state);

    success = ::dlb_pmd_model_combo_convert_to_core_model(nullptr, nullptr);
    EXPECT_EQ(PMD_FAIL, success);
    success = ::dlb_pmd_model_combo_convert_to_core_model(mPmdModelCombo, nullptr);
    EXPECT_EQ(PMD_FAIL, success);

    // lather
    success = ::dlb_pmd_model_combo_convert_to_core_model(mPmdModelCombo, &core_model);
    EXPECT_EQ(PMD_SUCCESS, success);

    status = ::dlb_adm_container_open_from_core_model(&mContainer, core_model);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_container_write_xml_file(mContainer, extraObject_ADM_OutputFileName);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_TRUE(CompareFiles(extraObject_ADM_InputFileName, extraObject_ADM_OutputFileName));

    // rinse, repeat
    success = ::dlb_pmd_model_combo_convert_to_core_model(mPmdModelCombo, &core_model);
    EXPECT_EQ(PMD_SUCCESS, success);

    status = ::dlb_adm_container_open_from_core_model(&mContainer, core_model);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = ::dlb_adm_container_write_xml_file(mContainer, extraObject_ADM_OutputFileName);
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    EXPECT_TRUE(CompareFiles(extraObject_ADM_InputFileName, extraObject_ADM_OutputFileName));

    // the combo model should be one-way now
    success = ::dlb_pmd_model_combo_convert_to_pmd_model(mPmdModelCombo, nullptr, &const_pmd_model);
    EXPECT_EQ(PMD_FAIL, success);
}

TEST_F(ComboModel, HasContent)
{
    dlb_pmd_model *pmd_model = nullptr;
    const dlb_adm_core_model *const_core_model = nullptr;
    dlb_pmd_bool pmd_model_has_content;
    dlb_pmd_bool core_model_has_content;
    dlb_pmd_success success;

    SetUpTestInput(extraObject_PMD_InputFileName, extraObjectPMD);
    ASSERT_TRUE(InitComboModel(nullptr, nullptr));

    // null arguments (one bool null is ok, both null is not)
    success = ::dlb_pmd_model_combo_has_content(nullptr, nullptr, nullptr);
    EXPECT_EQ(PMD_FAIL, success);
    success = ::dlb_pmd_model_combo_has_content(mPmdModelCombo, nullptr, nullptr);
    EXPECT_EQ(PMD_FAIL, success);

    // does PMD model have content?
    pmd_model_has_content = PMD_TRUE;
    success = ::dlb_pmd_model_combo_has_content(mPmdModelCombo, &pmd_model_has_content, nullptr);
    EXPECT_EQ(PMD_SUCCESS, success);
    EXPECT_FALSE(pmd_model_has_content);

    // does core model have content?
    core_model_has_content = PMD_TRUE;
    success = ::dlb_pmd_model_combo_has_content(mPmdModelCombo, nullptr, &core_model_has_content);
    EXPECT_EQ(PMD_SUCCESS, success);
    EXPECT_FALSE(core_model_has_content);

    // check both at once
    pmd_model_has_content = PMD_TRUE;
    core_model_has_content = PMD_TRUE;
    success = ::dlb_pmd_model_combo_has_content(mPmdModelCombo, &pmd_model_has_content, &core_model_has_content);
    EXPECT_EQ(PMD_SUCCESS, success);
    EXPECT_FALSE(pmd_model_has_content);
    EXPECT_FALSE(core_model_has_content);

    // add some PMD
    success = ::dlb_pmd_model_combo_get_writable_pmd_model(mPmdModelCombo, &pmd_model, PMD_TRUE);
    ASSERT_EQ(PMD_SUCCESS, success);
    success = ::dlb_xmlpmd_file_read(extraObject_PMD_InputFileName, pmd_model, PMD_FALSE, nullptr, nullptr);
    ASSERT_EQ(PMD_SUCCESS, success);

    // check again
    success = ::dlb_pmd_model_combo_has_content(mPmdModelCombo, &pmd_model_has_content, &core_model_has_content);
    EXPECT_EQ(PMD_SUCCESS, success);
    EXPECT_TRUE(pmd_model_has_content);
    EXPECT_FALSE(core_model_has_content);

    // convert to core model
    success = ::dlb_pmd_model_combo_convert_to_core_model(mPmdModelCombo, &const_core_model);
    ASSERT_EQ(PMD_SUCCESS, success);

    // check again
    success = ::dlb_pmd_model_combo_has_content(mPmdModelCombo, &pmd_model_has_content, &core_model_has_content);
    EXPECT_EQ(PMD_SUCCESS, success);
    EXPECT_TRUE(pmd_model_has_content);
    EXPECT_TRUE(core_model_has_content);
}

TEST_F(ComboModel, GetStateBasic)
{
    dlb_pmd_model *pmd_model = nullptr;
    const dlb_adm_core_model *const_core_model = nullptr;
    DLB_PMD_MODEL_COMBO_STATE pmd_model_state;
    DLB_PMD_MODEL_COMBO_STATE core_model_state;
    dlb_pmd_success success;

    SetUpTestInput(extraObject_PMD_InputFileName, extraObjectPMD);
    ASSERT_TRUE(InitComboModel(nullptr, nullptr));

    // null arguments (one state arg == null is ok, both null is not)
    success = ::dlb_pmd_model_combo_get_state(nullptr, nullptr, nullptr);
    EXPECT_EQ(PMD_FAIL, success);
    success = ::dlb_pmd_model_combo_get_state(mPmdModelCombo, nullptr, nullptr);
    EXPECT_EQ(PMD_FAIL, success);

    // PMD model only
    pmd_model_state = DLB_PMD_MODEL_COMBO_STATE_IS_PRIMARY;
    success = ::dlb_pmd_model_combo_get_state(mPmdModelCombo, &pmd_model_state, nullptr);
    EXPECT_EQ(PMD_SUCCESS, success);
    EXPECT_EQ(DLB_PMD_MODEL_COMBO_STATE_IS_EMPTY, pmd_model_state);

    // core model only
    core_model_state = DLB_PMD_MODEL_COMBO_STATE_IS_PRIMARY;
    success = ::dlb_pmd_model_combo_get_state(mPmdModelCombo, nullptr, &core_model_state);
    EXPECT_EQ(PMD_SUCCESS, success);
    EXPECT_EQ(DLB_PMD_MODEL_COMBO_STATE_IS_EMPTY, core_model_state);

    // both
    pmd_model_state = DLB_PMD_MODEL_COMBO_STATE_IS_PRIMARY;
    core_model_state = DLB_PMD_MODEL_COMBO_STATE_IS_PRIMARY;
    success = ::dlb_pmd_model_combo_get_state(mPmdModelCombo, &pmd_model_state, &core_model_state);
    EXPECT_EQ(PMD_SUCCESS, success);
    EXPECT_EQ(DLB_PMD_MODEL_COMBO_STATE_IS_EMPTY, pmd_model_state);
    EXPECT_EQ(DLB_PMD_MODEL_COMBO_STATE_IS_EMPTY, core_model_state);

    // add some PMD
    success = ::dlb_pmd_model_combo_get_writable_pmd_model(mPmdModelCombo, &pmd_model, PMD_TRUE);
    ASSERT_EQ(PMD_SUCCESS, success);
    success = ::dlb_xmlpmd_file_read(extraObject_PMD_InputFileName, pmd_model, PMD_FALSE, nullptr, nullptr);
    ASSERT_EQ(PMD_SUCCESS, success);

    // check again
    success = ::dlb_pmd_model_combo_get_state(mPmdModelCombo, &pmd_model_state, &core_model_state);
    EXPECT_EQ(PMD_SUCCESS, success);
    EXPECT_EQ(DLB_PMD_MODEL_COMBO_STATE_IS_PRIMARY, pmd_model_state);
    EXPECT_EQ(DLB_PMD_MODEL_COMBO_STATE_IS_EMPTY, core_model_state);

    // convert to core model
    success = ::dlb_pmd_model_combo_convert_to_core_model(mPmdModelCombo, &const_core_model);
    ASSERT_EQ(PMD_SUCCESS, success);

    // check again
    success = ::dlb_pmd_model_combo_get_state(mPmdModelCombo, &pmd_model_state, &core_model_state);
    EXPECT_EQ(PMD_SUCCESS, success);
    EXPECT_EQ(DLB_PMD_MODEL_COMBO_STATE_IS_PRIMARY, pmd_model_state);
    EXPECT_EQ(DLB_PMD_MODEL_COMBO_STATE_IS_CONVERTED, core_model_state);
}

TEST_F(ComboModel, Clear)
{
    dlb_pmd_model *pmd_model = nullptr;
    const dlb_adm_core_model *const_core_model = nullptr;
    DLB_PMD_MODEL_COMBO_STATE pmd_model_state;
    DLB_PMD_MODEL_COMBO_STATE core_model_state;
    dlb_pmd_success success;

    SetUpTestInput(extraObject_PMD_InputFileName, extraObjectPMD);
    ASSERT_TRUE(InitComboModel(nullptr, nullptr));

    success = ::dlb_pmd_model_combo_get_state(mPmdModelCombo, &pmd_model_state, &core_model_state);
    ASSERT_EQ(PMD_SUCCESS, success);
    ASSERT_EQ(DLB_PMD_MODEL_COMBO_STATE_IS_EMPTY, pmd_model_state);
    ASSERT_EQ(DLB_PMD_MODEL_COMBO_STATE_IS_EMPTY, core_model_state);

    success = ::dlb_pmd_model_combo_clear(nullptr);
    EXPECT_EQ(PMD_FAIL, success);

    // clearing an empty model should not change the state
    success = ::dlb_pmd_model_combo_clear(mPmdModelCombo);
    EXPECT_EQ(PMD_SUCCESS, success);
    success = ::dlb_pmd_model_combo_get_state(mPmdModelCombo, &pmd_model_state, &core_model_state);
    ASSERT_EQ(PMD_SUCCESS, success);
    EXPECT_EQ(DLB_PMD_MODEL_COMBO_STATE_IS_EMPTY, pmd_model_state);
    EXPECT_EQ(DLB_PMD_MODEL_COMBO_STATE_IS_EMPTY, core_model_state);

    // add some PMD and convert to core model
    success = ::dlb_pmd_model_combo_get_writable_pmd_model(mPmdModelCombo, &pmd_model, PMD_TRUE);
    ASSERT_EQ(PMD_SUCCESS, success);
    success = ::dlb_xmlpmd_file_read(extraObject_PMD_InputFileName, pmd_model, PMD_FALSE, nullptr, nullptr);
    ASSERT_EQ(PMD_SUCCESS, success);
    success = ::dlb_pmd_model_combo_convert_to_core_model(mPmdModelCombo, &const_core_model);
    ASSERT_EQ(PMD_SUCCESS, success);

    // now we should have different state values
    success = ::dlb_pmd_model_combo_get_state(mPmdModelCombo, &pmd_model_state, &core_model_state);
    ASSERT_EQ(PMD_SUCCESS, success);
    EXPECT_EQ(DLB_PMD_MODEL_COMBO_STATE_IS_PRIMARY, pmd_model_state);
    EXPECT_EQ(DLB_PMD_MODEL_COMBO_STATE_IS_CONVERTED, core_model_state);

    // clear the model, we should be back to empty
    success = ::dlb_pmd_model_combo_clear(mPmdModelCombo);
    EXPECT_EQ(PMD_SUCCESS, success);
    success = ::dlb_pmd_model_combo_get_state(mPmdModelCombo, &pmd_model_state, &core_model_state);
    ASSERT_EQ(PMD_SUCCESS, success);
    EXPECT_EQ(DLB_PMD_MODEL_COMBO_STATE_IS_EMPTY, pmd_model_state);
    EXPECT_EQ(DLB_PMD_MODEL_COMBO_STATE_IS_EMPTY, core_model_state);
}

TEST_F(ComboModel, DestroyBasic)
{
    dlb_pmd_success success;

    success = ::dlb_pmd_model_combo_destroy(nullptr);
    EXPECT_EQ(PMD_FAIL, success);
    success = ::dlb_pmd_model_combo_destroy(&mPmdModelCombo);
    EXPECT_EQ(PMD_FAIL, success);       // mPmdModelCombo is null

    ASSERT_TRUE(InitComboModel(nullptr, nullptr));

    success = ::dlb_pmd_model_combo_destroy(&mPmdModelCombo);
    EXPECT_EQ(PMD_SUCCESS, success);
    EXPECT_EQ(nullptr, mPmdModelCombo);

    // Additional use cases in InitAndDestroy
}

TEST_F(ComboModel, ModelWrapper)
{
    using namespace DlbAdm;

    dlb_pmd_model_combo *model_combo = nullptr;
    dlb_pmd_success success;

    SetUpTestInput(extraObject_PMD_InputFileName, extraObjectPMD);
    ASSERT_TRUE(InitPmdModel());
    success = ::dlb_xmlpmd_file_read(extraObject_PMD_InputFileName, mPmdModel, PMD_FALSE, nullptr, nullptr);
    ASSERT_EQ(PMD_SUCCESS, success);
    
    {
        DlbPmdModelWrapper wrapper(&model_combo, mPmdModel, PMD_FALSE);
        const dlb_pmd_model *const_pmd_model = nullptr;
        const dlb_adm_core_model *const_core_model = nullptr;

        EXPECT_NE(nullptr, model_combo);
        success = ::dlb_pmd_model_combo_get_readable_pmd_model(model_combo, &const_pmd_model, PMD_FALSE);
        EXPECT_EQ(PMD_SUCCESS, success);
        EXPECT_EQ(mPmdModel, const_pmd_model);
        success = ::dlb_pmd_model_combo_convert_to_core_model(model_combo, &const_core_model);
        EXPECT_EQ(PMD_SUCCESS, success);
        EXPECT_NE(nullptr, const_core_model);
    }

    EXPECT_EQ(nullptr, model_combo);
}
