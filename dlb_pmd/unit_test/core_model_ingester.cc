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

#include "dlb_pmd_xml_file.h"
#include "dlb_adm/include/dlb_adm_api.h"
#include "dlb_pmd/src/modules/sadm/pmd_speaker_position.h"
#include "dlb_pmd/src/modules/sadm/pmd_speaker_blkupdt.h"
#include "dlb_pmd/src/modules/sadm/pmd_core_model_ingester.h"
#include "test_data.h"

#include <fstream>

#define CHECK_STATUS(X) if ((X) != DLB_ADM_STATUS_OK) return (X)
#define CHECK_SUCCESS(X) if ((X) != PMD_SUCCESS) return PMD_FAIL

static const char stereo_2D_ADM_InputFileName[]  = "Stereo_2D_ADM.xml";
static const char stereo_2D_PMD_InputFileName[]  = "Stereo_2D_PMD.xml";
static const char stereo_2D_PMD_OutputFileName[] = "Stereo_2D_ADM.pmd.out.xml";

class CoreModelIngester : public testing::Test
{
protected:
    dlb_pmd_model *mPmdModel;
    dlb_adm_xml_container *mContainer;
    dlb_adm_core_model *mCoreModel;
    pmd_core_model_ingester *mIngester;

    char *mPmdModelMemory;
    uint8_t *mIngesterMemory;

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

    int InitSmallModel()
    {
        dlb_adm_container_counts counts;
        int status;

        SetUpTestInput(stereo_2D_ADM_InputFileName, stereo_2D_ADM);

        ::memset(&counts, 0, sizeof(counts));
        status = ::dlb_adm_container_open(&mContainer, &counts);
        CHECK_STATUS(status);
        status = ::dlb_adm_container_read_xml_file(mContainer, stereo_2D_ADM_InputFileName, DLB_ADM_FALSE);
        CHECK_STATUS(status);
        status = ::dlb_adm_core_model_open_from_xml_container(&mCoreModel, mContainer);
        (void)::dlb_adm_container_close(&mContainer);

        return status;
    }

    dlb_pmd_success InitIngester()
    {
        size_t memory_sz;
        dlb_pmd_success success;

        success = ::pmd_core_model_ingester_query_memory_size(&memory_sz);
        CHECK_SUCCESS(success);
        mIngesterMemory = new uint8_t[memory_sz];
        if (mIngesterMemory == nullptr)
        {
            return PMD_FAIL;
        }
        success = ::pmd_core_model_ingester_open(&mIngester, mIngesterMemory);
        return success;
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
        mIngester = nullptr;
        mPmdModelMemory = nullptr;
        mIngesterMemory = nullptr;
    }

    virtual void TearDown()
    {
        if (mIngester != nullptr)
        {
            (void)::pmd_core_model_ingester_close(&mIngester);
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

        if (mPmdModelMemory != nullptr)
        {
            delete[] mPmdModelMemory;
            mPmdModelMemory = nullptr;
        }
        if (mIngesterMemory != nullptr)
        {
            delete[] mIngesterMemory;
            mIngesterMemory = nullptr;
        }
    }

};

TEST(core_model_ingester, LookupSpeakerPosition)
{
    static const speaker_position bad1 = { SPEAKER_COORD_LEFT,   SPEAKER_COORD_LEFT,   SPEAKER_COORD_LEFT };
    static const speaker_position bad2 = { SPEAKER_COORD_CENTER, SPEAKER_COORD_CENTER, SPEAKER_COORD_CENTER };
    static const speaker_position bad3 = { SPEAKER_COORD_RIGHT,  SPEAKER_COORD_RIGHT,  SPEAKER_COORD_RIGHT };
    static const speaker_position bad4 = { SPEAKER_COORD_HIGH,    SPEAKER_COORD_HIGH,    SPEAKER_COORD_HIGH };

    static const speaker_position left   = { SPEAKER_COORD_LEFT,   SPEAKER_COORD_FRONT, SPEAKER_COORD_MIDDLE };
    static const speaker_position center = { SPEAKER_COORD_CENTER, SPEAKER_COORD_FRONT, SPEAKER_COORD_MIDDLE };
    static const speaker_position right  = { SPEAKER_COORD_RIGHT,  SPEAKER_COORD_FRONT, SPEAKER_COORD_MIDDLE };

    dlb_pmd_success success;
    dlb_pmd_speaker pmd_speaker;

    success = lookup_speaker_position(nullptr, nullptr, PMD_FALSE);
    EXPECT_EQ(PMD_FAIL, success);
    success = lookup_speaker_position(&pmd_speaker, nullptr, PMD_FALSE);
    EXPECT_EQ(PMD_FAIL, success);

    success = lookup_speaker_position(&pmd_speaker, &left, PMD_FALSE);
    EXPECT_EQ(PMD_SUCCESS, success);
    success = lookup_speaker_position(&pmd_speaker, &center, PMD_FALSE);
    EXPECT_EQ(PMD_SUCCESS, success);
    success = lookup_speaker_position(&pmd_speaker, &right, PMD_FALSE);
    EXPECT_EQ(PMD_SUCCESS, success);

    success = lookup_speaker_position(&pmd_speaker, &bad1, PMD_FALSE);
    EXPECT_EQ(PMD_FAIL, success);
    success = lookup_speaker_position(&pmd_speaker, &bad2, PMD_FALSE);
    EXPECT_EQ(PMD_FAIL, success);
    success = lookup_speaker_position(&pmd_speaker, &bad3, PMD_FALSE);
    EXPECT_EQ(PMD_FAIL, success);
    success = lookup_speaker_position(&pmd_speaker, &bad4, PMD_FALSE);
    EXPECT_EQ(PMD_FAIL, success);
}

TEST(core_model_ingester, AnalyzeCoordinates)
{
    dlb_adm_data_block_update update;
    speaker_position position;
    dlb_pmd_success success;

    memset(&update, 0, sizeof(update));
    update.cartesian = PMD_TRUE;

    success = analyze_coordinates(nullptr, nullptr);
    EXPECT_EQ(PMD_FAIL, success);
    success = analyze_coordinates(&position, nullptr);
    EXPECT_EQ(PMD_FAIL, success);

    success = analyze_coordinates(&position, &update);
    EXPECT_EQ(PMD_SUCCESS, success);
}

/**
 * @brief S-ADM speaker BlockUpdate information for formats using Lss/Rss
 */
static speaker_blkupdt SPHERICAL_SPEAKER_BLKUPDT[] =
{
    /* L */    { "FrontLeft",              "M+030",   30.0f,   0.0f,  1.0f },
    /* R */    { "FrontRight",             "M-030",  -30.0f,   0.0f,  1.0f },
    /* C */    { "FrontCentre",            "M+000",    0.0f,   0.0f,  1.0f },
    /* Lfe */  { "LowFrequencyEffects",    "LFE",      0.0f, -30.0f,  1.0f },
    /* Lss */  { "SideLeft",               "M+090",   90.0f,   0.0f,  1.0f },
    /* Rss */  { "SideRight",              "M-090",  -90.0f,   0.0f,  1.0f },
    /* Lrs */  { "BackLeftMid",            "M+135",  135.0f,   0.0f,  1.0f },
    /* Rrs */  { "BackRightMid",           "M-135", -135.0f,   0.0f,  1.0f },
    /* Ltf */  { "TopFrontLeft",           "U+030",   30.0f,  30.0f,  1.0f },
    /* Rtf */  { "TopFrontRight",          "U-030",  -30.0f,  30.0f,  1.0f },
    /* Ltm */  { "TopSideLeft",            "U+090",   90.0f,  30.0f,  1.0f },
    /* Rtm */  { "TopSideRight",           "U-090",  -90.0f,  30.0f,  1.0f },
    /* Ltr */  { "TopBackLeftMid",         "U+135",  135.0f,  30.0f,  1.0f },
    /* Rtr */  { "TopBackRightMid",        "U-135", -135.0f,  30.0f,  1.0f },
    /* Lw */   { "FrontLeftWide",          "M+060",   60.0f,   0.0f,  1.0f },
    /* Rw */   { "FrontRightWide",         "M-060",  -60.0f,   0.0f,  1.0f },
};

/**
 * @brief S-ADM speaker BlockUpdate information for Ls/Rs
 */
static speaker_blkupdt SPHERICAL_SPEAKER_BLKUPDT_LS_RS[] =
{
    /* Ls */   { "SurroundLeft",  "M+110",  110.0f,  0.0f,  1.0f },
    /* Rs */   { "SurroundRight", "M-110", -110.0f,  0.0f,  1.0f },
};

/**
 * @brief find the block update information for a speaker position
 */
static
const speaker_blkupdt *
find_spherical_speaker_blkupdt
    (dlb_pmd_speaker    spkr
    ,dlb_pmd_bool       alt_spkrs
    )
{
    const speaker_blkupdt *sb = &SPHERICAL_SPEAKER_BLKUPDT[spkr - 1];

    if (!alt_spkrs)
    {
        /* !alt_spkrs means 2.0 - 5.1.4 (i.e., no rear surrounds)
         * when no rear surrounds, Ls and Rs take the Lrs and Rrs
         * speaker positions, plus shorter names
         */
        switch (spkr)
        {
        case PMD_SPEAKER_LS:
            sb = &SPHERICAL_SPEAKER_BLKUPDT_LS_RS[0];
            break;
        case PMD_SPEAKER_RS:
            sb = &SPHERICAL_SPEAKER_BLKUPDT_LS_RS[1];
            break;
        default:
            break;
        }
    }

    return sb;
}

TEST(core_model_ingester, FindSpeakerPosition)
{
    dlb_adm_data_block_update update;
    dlb_pmd_speaker position;
    dlb_pmd_success success;
    dlb_pmd_speaker pos;

    memset(&update, 0, sizeof(update));
    update.cartesian = PMD_TRUE;
    update.position[DLB_ADM_COORDINATE_X] = CARTESIAN_LEFT;
    update.position[DLB_ADM_COORDINATE_Y] = CARTESIAN_FRONT;
    update.position[DLB_ADM_COORDINATE_Z] = CARTESIAN_MIDDLE;

    success = find_speaker_position(nullptr, nullptr, PMD_FALSE);
    EXPECT_EQ(PMD_FAIL, success);
    success = find_speaker_position(&position, nullptr, PMD_FALSE);
    EXPECT_EQ(PMD_FAIL, success);

    success = find_speaker_position(&position, &update, PMD_FALSE);
    EXPECT_EQ(PMD_SUCCESS, success);
    EXPECT_EQ(PMD_SPEAKER_L, position);

    for (pos = PMD_SPEAKER_L; pos <= PMD_SPEAKER_LAST_VALID; pos = (dlb_pmd_speaker)(((int)pos) + 1))
    {
        const speaker_blkupdt *blk = find_speaker_blkupdt(&pos, PMD_TRUE);

        memset(&update, 0, sizeof(update));
        update.cartesian = PMD_TRUE;
        update.position[DLB_ADM_COORDINATE_X] = blk->x;
        update.position[DLB_ADM_COORDINATE_Y] = blk->y;
        update.position[DLB_ADM_COORDINATE_Z] = blk->z;
        success = find_speaker_position(&position, &update, PMD_TRUE);
        EXPECT_EQ(PMD_SUCCESS, success);
        EXPECT_EQ(pos, position);
    }

#if USE_ALT_SPEAKERS
    for (pos = PMD_SPEAKER_LS; pos <= PMD_SPEAKER_RS; pos = (dlb_pmd_speaker)(((int)pos) + 1))
    {
        dlb_pmd_speaker compare = pos;
        const speaker_blkupdt *blk = find_speaker_blkupdt(&pos, PMD_FALSE);

        memset(&update, 0, sizeof(update));
        update.cartesian = PMD_TRUE;
        update.position[DLB_ADM_COORDINATE_X] = blk->x;
        update.position[DLB_ADM_COORDINATE_Y] = blk->y;
        update.position[DLB_ADM_COORDINATE_Z] = blk->z;
        success = find_speaker_position(&position, &update, PMD_FALSE);
        EXPECT_EQ(PMD_SUCCESS, success);
        EXPECT_EQ(compare, position);
    }
#endif

    for (pos = PMD_SPEAKER_L; pos <= PMD_SPEAKER_LAST_VALID; pos = (dlb_pmd_speaker)(((int)pos) + 1))
    {
        const speaker_blkupdt *blk = find_spherical_speaker_blkupdt(pos, PMD_TRUE);

        memset(&update, 0, sizeof(update));
        update.cartesian = PMD_FALSE;
        update.position[DLB_ADM_COORDINATE_X] = blk->x;
        update.position[DLB_ADM_COORDINATE_Y] = blk->y;
        update.position[DLB_ADM_COORDINATE_Z] = blk->z;
        success = find_speaker_position(&position, &update, PMD_TRUE);
        EXPECT_EQ(PMD_SUCCESS, success);
        EXPECT_EQ(pos, position);
    }

#if USE_ALT_SPEAKERS
    for (pos = PMD_SPEAKER_LS; pos <= PMD_SPEAKER_RS; pos = (dlb_pmd_speaker)(((int)pos) + 1))
    {
        dlb_pmd_speaker compare = pos;
        const speaker_blkupdt *blk = find_spherical_speaker_blkupdt(pos, PMD_FALSE);

        memset(&update, 0, sizeof(update));
        update.cartesian = PMD_FALSE;
        update.position[DLB_ADM_COORDINATE_X] = blk->x;
        update.position[DLB_ADM_COORDINATE_Y] = blk->y;
        update.position[DLB_ADM_COORDINATE_Z] = blk->z;
        success = find_speaker_position(&position, &update, PMD_FALSE);
        EXPECT_EQ(PMD_SUCCESS, success);
        EXPECT_EQ(compare, position);
    }
#endif
}

TEST(core_model_ingester, IngesterQueryMem)
{
    size_t memory_sz;
    dlb_pmd_success success;

    success = ::pmd_core_model_ingester_query_memory_size(nullptr);
    EXPECT_EQ(PMD_FAIL, success);

    memory_sz = 0;
    success = ::pmd_core_model_ingester_query_memory_size(&memory_sz);
    EXPECT_EQ(PMD_SUCCESS, success);
    EXPECT_LT(0, memory_sz);
}

TEST_F(CoreModelIngester, IngesterOpen)
{
    size_t memory_sz;
    dlb_pmd_success success;

    success = ::pmd_core_model_ingester_query_memory_size(&memory_sz);
    ASSERT_EQ(PMD_SUCCESS, success);
    mIngesterMemory = new uint8_t[memory_sz];
    ASSERT_NE(nullptr, mIngesterMemory);

    success = ::pmd_core_model_ingester_open(nullptr, nullptr);
    EXPECT_EQ(PMD_FAIL, success);
    success = ::pmd_core_model_ingester_open(&mIngester, nullptr);
    EXPECT_EQ(PMD_FAIL, success);

    success = ::pmd_core_model_ingester_open(&mIngester, mIngesterMemory);
    EXPECT_EQ(PMD_SUCCESS, success);
}

TEST_F(CoreModelIngester, IngesterClose)
{
    pmd_core_model_ingester *p = nullptr;
    dlb_pmd_success success;

    success = InitIngester();
    ASSERT_EQ(PMD_SUCCESS, success);

    success = pmd_core_model_ingester_close(nullptr);
    EXPECT_EQ(PMD_FAIL, success);
    success = pmd_core_model_ingester_close(&p);
    EXPECT_EQ(PMD_FAIL, success);

    success = pmd_core_model_ingester_close(&mIngester);
    EXPECT_EQ(PMD_SUCCESS, success);
}

TEST_F(CoreModelIngester, IngestSmallModel)
{
    dlb_pmd_success success;
    int status;

    SetUpTestInput(stereo_2D_PMD_InputFileName, stereo_2D_PMD);

    status = InitPmdModel();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    status = InitSmallModel();
    ASSERT_EQ(DLB_ADM_STATUS_OK, status);
    success = InitIngester();
    ASSERT_EQ(PMD_SUCCESS, success);

    success = ::pmd_core_model_ingester_ingest(nullptr, nullptr, nullptr, nullptr);
    EXPECT_EQ(PMD_FAIL, success);
    success = ::pmd_core_model_ingester_ingest(mIngester, nullptr, nullptr, nullptr);
    EXPECT_EQ(PMD_FAIL, success);
    success = ::pmd_core_model_ingester_ingest(mIngester, mPmdModel, nullptr, nullptr);
    EXPECT_EQ(PMD_FAIL, success);
    success = ::pmd_core_model_ingester_ingest(mIngester, mPmdModel, "Small Stereo Program", nullptr);
    EXPECT_EQ(PMD_FAIL, success);

    success = ::pmd_core_model_ingester_ingest(mIngester, mPmdModel, "Small Stereo Program", mCoreModel);
    EXPECT_EQ(PMD_SUCCESS, success);
    EXPECT_EQ(4, dlb_pmd_num_signals(mPmdModel));
    EXPECT_EQ(1, dlb_pmd_num_beds(mPmdModel));
    EXPECT_EQ(2, dlb_pmd_num_objects(mPmdModel));
    EXPECT_EQ(2, dlb_pmd_num_presentations(mPmdModel));
    EXPECT_EQ(1, dlb_pmd_num_iat(mPmdModel));

    success = ::dlb_xmlpmd_file_write(stereo_2D_PMD_OutputFileName, mPmdModel);
    EXPECT_EQ(PMD_SUCCESS, success);
    EXPECT_TRUE(CompareFiles(stereo_2D_PMD_InputFileName, stereo_2D_PMD_OutputFileName));
}
