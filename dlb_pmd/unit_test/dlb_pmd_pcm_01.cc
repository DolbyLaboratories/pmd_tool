/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2019-2021 by Dolby Laboratories,
 *                Copyright (C) 2019-2021 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

#include "gtest/gtest.h"

#include "dlb_pmd/include/dlb_pmd_api.h"
#include "dlb_pmd/include/dlb_pmd_pcm.h"

#include <stdint.h>
#include <string.h>

class DlbPmdPcm01 : public testing::Test
{
protected:
    static const size_t MODEL_MEMORY_SIZE = 4000000;    // 3734704 on Windows
    static const size_t AUGMENTOR_MEMORY_SIZE = 13000;  // 12208 on Windows
    static const size_t EXTRACTOR_MEMORY_SIZE = 13000;  // 12256 on Windows
    static const size_t TRY_FRAME_MEMORY_SIZE = 13000;  // 12208 on Windows

    uint8_t              mModel1Memory[MODEL_MEMORY_SIZE];
    uint8_t              mModel2Memory[MODEL_MEMORY_SIZE];
    uint8_t              mAugmentorMemory[AUGMENTOR_MEMORY_SIZE];
    uint8_t              mExtractorMemory[EXTRACTOR_MEMORY_SIZE];
    uint8_t              mTryFrameMemory[TRY_FRAME_MEMORY_SIZE];
    uint8_t             *mSadmTryFrameMemory;
    dlb_pmd_model       *mModel1;
    dlb_pmd_model       *mModel2;

    uint8_t             *mPmdModelComboMemory1;
    uint8_t             *mPmdModelComboMemory2;
    dlb_pmd_model_combo *mPmdModelCombo1;
    dlb_pmd_model_combo *mPmdModelCombo2;

    dlb_pmd_success AddBasicModel()
    {
        dlb_pmd_success success;
        char obj_name[99];
        char i;

        success = dlb_pmd_add_signals(mModel1, 16);
        if (success != PMD_SUCCESS)
        {
            return success;
        }

        success = dlb_pmd_add_bed(mModel1, 101, "Bed 1", DLB_PMD_SPEAKER_CONFIG_5_1_4, 2, 0);
        if (success != PMD_SUCCESS)
        {
            return success;
        }

        for (i = 1; i <= 5; i++)
        {
            sprintf(obj_name, "Object %u", static_cast<unsigned>(i));
            success = dlb_pmd_add_generic_obj2(mModel1, i, obj_name, i + 11, 0.0, 0.0, 0.0);
            if (success != PMD_SUCCESS)
            {
                return success;
            }
        }

        dlb_pmd_element_id ids[] = { 101, 1, 2, 3, 4, 5 };
        int id_count = sizeof(ids) / sizeof(dlb_pmd_element_id);

        success = dlb_pmd_add_presentation(
            mModel1, 1, "en", "Presentation 1", "en", DLB_PMD_SPEAKER_CONFIG_5_1_4, id_count, ids);
        if (success != PMD_SUCCESS)
        {
            return success;
        }
 
        success = dlb_pmd_iat_add(mModel1, 0);

        return success;
    }

    dlb_pmd_success AddMediumModel()
    {
        dlb_pmd_success success;
        unsigned int entityId, ch;
        char name[99];
        char i;

        success = dlb_pmd_add_signals(mModel1, 128);
        if (success != PMD_SUCCESS)
        {
            return success;
        }

        for (entityId = 101, ch = 2, i = 1; i <= 15; entityId++, ch += 8, i++)
        {
            sprintf(name, "Bed %u", entityId);
            success = dlb_pmd_add_bed(mModel1, static_cast<dlb_pmd_element_id>(entityId), name, DLB_PMD_SPEAKER_CONFIG_5_1_2, ch, 0);
            if (success != PMD_SUCCESS)
            {
                return success;
            }
        }

        for (entityId = 1; entityId <= 15; entityId++)
        {
            unsigned int bedId = entityId + 100;
            dlb_pmd_element_id elementId;

            sprintf(name, "Presentation %u", entityId);
            elementId = static_cast<dlb_pmd_element_id>(bedId);
            success = dlb_pmd_add_presentation(
                mModel1, static_cast<dlb_pmd_presentation_id>(entityId), "en", name, "en", DLB_PMD_SPEAKER_CONFIG_5_1_2, 1, &elementId);
            if (success != PMD_SUCCESS)
            {
                return success;
            }
        }

        success = dlb_pmd_iat_add(mModel1, 0);

        return success;
    }

    dlb_pmd_success AddLargeModel()
    {
        dlb_pmd_success success;
        unsigned int entityId, ch;
        char name[99];
        char i;

        success = dlb_pmd_add_signals(mModel1, 128);
        if (success != PMD_SUCCESS)
        {
            return success;
        }

        for (entityId = 101, ch = 2, i = 1; i <= 11; entityId++, ch += 10, i++)
        {
            sprintf(name, "This is a 5.1.4 bed with id = %u", entityId);
            success = dlb_pmd_add_bed(mModel1, static_cast<dlb_pmd_element_id>(entityId), name, DLB_PMD_SPEAKER_CONFIG_5_1_4, ch, 0);
            if (success != PMD_SUCCESS)
            {
                return success;
            }
        }

        for (entityId = 1; ch <= 128; entityId++, ch++)
        {
            sprintf(name, "This is a generic object with id = %u", entityId);
            success = dlb_pmd_add_generic_obj2(mModel1, static_cast<dlb_pmd_element_id>(entityId), name, ch, 0.0, 0.0, 0.0);
            if (success != PMD_SUCCESS)
            {
                return success;
            }
        }

        for (entityId = 1; entityId <= 11; entityId++)
        {
            unsigned int bedId = entityId + 100;
            dlb_pmd_element_id elementIds[2];

            elementIds[0] = static_cast<dlb_pmd_element_id>(bedId);
            elementIds[1] = static_cast<dlb_pmd_element_id>(entityId);
            sprintf(name, "This is a presentation with id = %u", entityId);
            success = dlb_pmd_add_presentation(
                mModel1, static_cast<dlb_pmd_presentation_id>(entityId), "en", name, "en", DLB_PMD_SPEAKER_CONFIG_5_1_4, 2, elementIds);
            if (success != PMD_SUCCESS)
            {
                return success;
            }
        }

        success = dlb_pmd_iat_add(mModel1, 0);

        return success;
    }

    bool InitComboModel1(dlb_pmd_model *existing_pmd_model, dlb_adm_core_model *existing_core_model)
    {
        bool good = true;

        try
        {
            size_t sz = ::dlb_pmd_model_combo_query_mem(existing_pmd_model, existing_core_model);
            dlb_pmd_success success;

            if (sz == 0) throw false;
            mPmdModelComboMemory1 = new uint8_t[sz];
            if (mPmdModelComboMemory1 == nullptr) throw false;
            success = ::dlb_pmd_model_combo_init(&mPmdModelCombo1, existing_pmd_model, existing_core_model, PMD_FALSE, mPmdModelComboMemory1);
            if ((success == PMD_FAIL) || (mPmdModelCombo1 == nullptr)) throw false;
        }
        catch (...)
        {
            good = false;
        }

        return good;
    }

    bool InitComboModel2(dlb_pmd_model *existing_pmd_model, dlb_adm_core_model *existing_core_model)
    {
        bool good = true;

        try
        {
            size_t sz = ::dlb_pmd_model_combo_query_mem(existing_pmd_model, existing_core_model);
            dlb_pmd_success success;

            if (sz == 0) throw false;
            mPmdModelComboMemory2 = new uint8_t[sz];
            if (mPmdModelComboMemory2 == nullptr) throw false;
            success = ::dlb_pmd_model_combo_init(&mPmdModelCombo2, existing_pmd_model, existing_core_model, PMD_FALSE, mPmdModelComboMemory2);
            if ((success == PMD_FAIL) || (mPmdModelCombo1 == nullptr)) throw false;
        }
        catch (...)
        {
            good = false;
        }

        return good;
    }

    virtual void SetUp()
    {
        ::memset(mModel1Memory, 0, sizeof(mModel1Memory));
        ::memset(mModel2Memory, 0, sizeof(mModel2Memory));
        ::memset(mAugmentorMemory, 0, sizeof(mAugmentorMemory));
        ::memset(mExtractorMemory, 0, sizeof(mExtractorMemory));
        ::memset(mTryFrameMemory, 0, sizeof(mTryFrameMemory));

        mSadmTryFrameMemory = NULL;

        mModel1 = NULL;
        dlb_pmd_init(&mModel1, mModel1Memory);

        mModel2 = NULL;
        dlb_pmd_init(&mModel2, mModel2Memory);

        mPmdModelComboMemory1 = nullptr;
        mPmdModelComboMemory2 = nullptr;
        mPmdModelCombo1 = nullptr;
        mPmdModelCombo2 = nullptr;
    }

    virtual void TearDown()
    {
        if (mPmdModelCombo2 != nullptr)
        {
            (void)dlb_pmd_model_combo_destroy(&mPmdModelCombo2);
        }
        if (mPmdModelComboMemory2 != nullptr)
        {
            delete[] mPmdModelComboMemory2;
            mPmdModelComboMemory2 = nullptr;
        }
        if (mPmdModelCombo1 != nullptr)
        {
            (void)dlb_pmd_model_combo_destroy(&mPmdModelCombo1);
        }
        if (mPmdModelComboMemory1 != nullptr)
        {
            delete[] mPmdModelComboMemory1;
            mPmdModelComboMemory1 = nullptr;
        }
        if (mModel2 != NULL)
        {
            dlb_pmd_finish(mModel2);
            mModel2 = NULL;
        }
        if (mModel1 != NULL)
        {
            dlb_pmd_finish(mModel1);
            mModel1 = NULL;
        }
        if (mSadmTryFrameMemory != NULL)
        {
            delete[] mSadmTryFrameMemory;
            mSadmTryFrameMemory = NULL;
        }
    }
};

TEST_F(DlbPmdPcm01, MemoryCheck)
{
    size_t model_sz = MODEL_MEMORY_SIZE;
    size_t aug_sz = AUGMENTOR_MEMORY_SIZE;
    size_t ext_sz = EXTRACTOR_MEMORY_SIZE;
    size_t try_sz = TRY_FRAME_MEMORY_SIZE;
    size_t n = dlb_pmd_query_mem();

    if (MODEL_MEMORY_SIZE < n)
    {
        printf("Memory needed for model: %u\n", (unsigned)n);
    }
    EXPECT_LE(n, model_sz);

    n = dlb_pcmpmd_augmentor_query_mem(PMD_FALSE);
    if (AUGMENTOR_MEMORY_SIZE < n)
    {
        printf("Memory needed for augmentor: %u\n", (unsigned)n);
    }
    EXPECT_LE(n, aug_sz);

    n = dlb_pcmpmd_extractor_query_mem(PMD_FALSE);
    if (EXTRACTOR_MEMORY_SIZE < n)
    {
        printf("Memory needed for extractor: %u\n", (unsigned)n);
    }
    EXPECT_LE(n, ext_sz);

    dlb_pmd_success success = AddLargeModel();
    ASSERT_EQ((dlb_pmd_success)PMD_SUCCESS, success);
    ASSERT_TRUE(InitComboModel1(mModel1, nullptr));

    n = dlb_pcmpmd_augmentor_model_try_frame_query_mem(mPmdModelCombo1, PMD_FALSE);
    if (TRY_FRAME_MEMORY_SIZE < n)
    {
        printf("Memory needed for augmentor try frame: %u\n", (unsigned)n);
    }
    EXPECT_LE(n, try_sz);
}

TEST(DlbPmdPcm, MallocatedModel)
{
    try
    {
        dlb_pmd_model *m = NULL;

        dlb_pmd_init(&m, NULL);
        EXPECT_NE(nullptr, m);
        dlb_pmd_finish(m);
    }
    catch (...)
    {
        FAIL();
    }
}

static unsigned int calc_min_samples(dlb_pmd_frame_rate frame_rate)
{
    unsigned int s;
    float r;

    switch (frame_rate)
    {
    case DLB_PMD_FRAMERATE_2398:
        return 2002;    // Calculation doesn't work without rounding
//         r = 23.98f;
//         break;
    case DLB_PMD_FRAMERATE_2400:
        r = 24.0f;
        break;
    case DLB_PMD_FRAMERATE_2500:
        r = 25.0f;
        break;
    case DLB_PMD_FRAMERATE_2997:
        r = 29.97f;
        break;
    case DLB_PMD_FRAMERATE_3000:
        r = 30.0f;
        break;
    case DLB_PMD_FRAMERATE_5000:
        r = 50.0f;
        break;
    case DLB_PMD_FRAMERATE_5994:
        r = 59.94f;
        break;
    case DLB_PMD_FRAMERATE_6000:
        r = 60.0f;
        break;
    case DLB_PMD_FRAMERATE_10000:
        r = 100.0f;
        break;
    case DLB_PMD_FRAMERATE_11988:
        r = 119.88f;
        break;
    case DLB_PMD_FRAMERATE_12000:
        r = 120.0f;
        break;
    default:
        return 0;
    }

    s = static_cast<unsigned int>(48000.0f / r);

    return s;
}

TEST(DlbPmdPcm, MinFrameSize)
{
    dlb_pmd_frame_rate lastPlusOne = static_cast<dlb_pmd_frame_rate>(DLB_PMD_FRAMERATE_LAST + 1);
    unsigned int sz;
    int i;

    sz = dlb_pcmpmd_min_frame_size(lastPlusOne);
    EXPECT_EQ(0u, sz);

    for (i = DLB_PMD_FRAMERATE_2398; i <= DLB_PMD_FRAMERATE_LAST; i++)
    {
        dlb_pmd_frame_rate r = static_cast<dlb_pmd_frame_rate>(i);
        unsigned int cs = calc_min_samples(r);
        sz = dlb_pcmpmd_min_frame_size(r);
        EXPECT_EQ(cs, sz);
    }
}

TEST_F(DlbPmdPcm01, ModelTryFrame_Bad_Small)
{
    static const size_t FRAME_SIZE = 400;
    static const size_t CHANNEL_COUNT = 2;

    uint32_t buffer[FRAME_SIZE * CHANNEL_COUNT];
    dlb_pcmpmd_write_status write_status;
    dlb_pmd_success success;

    ::memset(buffer, 0, sizeof(buffer));
    success = AddBasicModel();
    ASSERT_EQ((dlb_pmd_success)PMD_SUCCESS, success);
    ASSERT_TRUE(InitComboModel1(mModel1, nullptr));

    write_status = dlb_pcmpmd_augmentor_model_try_frame(NULL, NULL, NULL, 0, 0, NUM_PMD_FRAMERATES, PMD_FALSE, PMD_FALSE);
    EXPECT_EQ(DLB_PCMPMD_WRITE_STATUS_ERROR, write_status);
    write_status = dlb_pcmpmd_augmentor_model_try_frame(mPmdModelCombo1, NULL, NULL, 0, 0, NUM_PMD_FRAMERATES, PMD_FALSE, PMD_FALSE);
    EXPECT_EQ(DLB_PCMPMD_WRITE_STATUS_ERROR, write_status);
    write_status = dlb_pcmpmd_augmentor_model_try_frame(mPmdModelCombo1, mTryFrameMemory, NULL, 0, 0, NUM_PMD_FRAMERATES, PMD_FALSE, PMD_FALSE);
    EXPECT_EQ(DLB_PCMPMD_WRITE_STATUS_ERROR, write_status);
    write_status = dlb_pcmpmd_augmentor_model_try_frame(mPmdModelCombo1, mTryFrameMemory, buffer, 0, 0, NUM_PMD_FRAMERATES, PMD_FALSE, PMD_FALSE);
    EXPECT_EQ(DLB_PCMPMD_WRITE_STATUS_ERROR, write_status);
    write_status = dlb_pcmpmd_augmentor_model_try_frame(mPmdModelCombo1, mTryFrameMemory, buffer, CHANNEL_COUNT, 0, NUM_PMD_FRAMERATES, PMD_FALSE, PMD_FALSE);
    EXPECT_EQ(DLB_PCMPMD_WRITE_STATUS_ERROR, write_status);
    write_status = dlb_pcmpmd_augmentor_model_try_frame(mPmdModelCombo1, mTryFrameMemory, buffer, CHANNEL_COUNT, FRAME_SIZE, NUM_PMD_FRAMERATES, PMD_FALSE, PMD_FALSE);
    EXPECT_EQ(DLB_PCMPMD_WRITE_STATUS_ERROR, write_status);

    // Not enough samples for 30fps...
    write_status = dlb_pcmpmd_augmentor_model_try_frame(mPmdModelCombo1, mTryFrameMemory, buffer, CHANNEL_COUNT, FRAME_SIZE, DLB_PMD_FRAMERATE_3000, PMD_FALSE, PMD_FALSE);
    EXPECT_EQ(DLB_PCMPMD_WRITE_STATUS_ERROR, write_status);

    // Try it
    write_status = dlb_pcmpmd_augmentor_model_try_frame(mPmdModelCombo1, mTryFrameMemory, buffer, CHANNEL_COUNT, FRAME_SIZE, DLB_PMD_FRAMERATE_12000, PMD_FALSE, PMD_FALSE);
    EXPECT_EQ(DLB_PCMPMD_WRITE_STATUS_GREEN, write_status);
}

TEST_F(DlbPmdPcm01, ModelTryFrame_Medium)
{
    static const size_t FRAME_SIZE = 400;
    static const size_t CHANNEL_COUNT = 2;

    uint32_t buffer[FRAME_SIZE * CHANNEL_COUNT];
    dlb_pcmpmd_write_status write_status;
    dlb_pmd_success success;

    ::memset(buffer, 0, sizeof(buffer));
    success = AddMediumModel();
    ASSERT_EQ((dlb_pmd_success)PMD_SUCCESS, success);
    ASSERT_TRUE(InitComboModel1(mModel1, nullptr));

    // Try it
    write_status = dlb_pcmpmd_augmentor_model_try_frame(mPmdModelCombo1, mTryFrameMemory, buffer, CHANNEL_COUNT, FRAME_SIZE, DLB_PMD_FRAMERATE_12000, false, false);
    EXPECT_EQ(DLB_PCMPMD_WRITE_STATUS_YELLOW, write_status);
}

TEST_F(DlbPmdPcm01, ModelTryFrame_Large)
{
    static const size_t FRAME_SIZE = 400;
    static const size_t CHANNEL_COUNT = 2;

    uint32_t buffer[FRAME_SIZE * CHANNEL_COUNT];
    dlb_pcmpmd_write_status write_status;
    dlb_pmd_success success;

    ::memset(buffer, 0, sizeof(buffer));
    success = AddLargeModel();
    ASSERT_EQ((dlb_pmd_success)PMD_SUCCESS, success);
    ASSERT_TRUE(InitComboModel1(mModel1, nullptr));

    // Try it
    write_status = dlb_pcmpmd_augmentor_model_try_frame(mPmdModelCombo1, mTryFrameMemory, buffer, CHANNEL_COUNT, FRAME_SIZE, DLB_PMD_FRAMERATE_12000, false, false);
    EXPECT_EQ(DLB_PCMPMD_WRITE_STATUS_RED, write_status);
}

TEST_F(DlbPmdPcm01, TryFrame_Bad_Small)
{
    static const size_t FRAME_SIZE = 400;
    static const size_t CHANNEL_COUNT = 2;

    uint32_t buffer[FRAME_SIZE * CHANNEL_COUNT];
    dlb_pcmpmd_augmentor *aug = NULL;
    dlb_pcmpmd_write_status write_status;
    dlb_pmd_success success;

    ::memset(buffer, 0, sizeof(buffer));
    success = AddBasicModel();
    ASSERT_EQ((dlb_pmd_success)PMD_SUCCESS, success);
    ASSERT_TRUE(InitComboModel1(mModel1, nullptr));

    dlb_pcmpmd_augmentor_init(
        &aug, mPmdModelCombo1, mAugmentorMemory,
        DLB_PMD_FRAMERATE_12000, DLB_PMD_KLV_UL_ST2109,
        false, CHANNEL_COUNT, CHANNEL_COUNT, false, 0);

    write_status = dlb_pcmpmd_augmentor_try_frame(NULL, NULL, NULL, 0, 0);
    EXPECT_EQ(DLB_PCMPMD_WRITE_STATUS_ERROR, write_status);
    write_status = dlb_pcmpmd_augmentor_try_frame(aug, NULL, NULL, 0, 0);
    EXPECT_EQ(DLB_PCMPMD_WRITE_STATUS_ERROR, write_status);
    write_status = dlb_pcmpmd_augmentor_try_frame(aug, mTryFrameMemory, NULL, 0, 0);
    EXPECT_EQ(DLB_PCMPMD_WRITE_STATUS_ERROR, write_status);
    write_status = dlb_pcmpmd_augmentor_try_frame(aug, mTryFrameMemory, buffer, 0, 0);
    EXPECT_EQ(DLB_PCMPMD_WRITE_STATUS_ERROR, write_status);
    write_status = dlb_pcmpmd_augmentor_try_frame(aug, mTryFrameMemory, buffer, CHANNEL_COUNT, 0);
    EXPECT_EQ(DLB_PCMPMD_WRITE_STATUS_ERROR, write_status);

    // Try it
    write_status = dlb_pcmpmd_augmentor_try_frame(aug, mTryFrameMemory, buffer, CHANNEL_COUNT, FRAME_SIZE);
    EXPECT_EQ(DLB_PCMPMD_WRITE_STATUS_GREEN, write_status);
}

TEST_F(DlbPmdPcm01, TryFrame_Medium)
{
    static const size_t FRAME_SIZE = 400;
    static const size_t CHANNEL_COUNT = 2;

    uint32_t buffer[FRAME_SIZE * CHANNEL_COUNT];
    dlb_pcmpmd_augmentor *aug = NULL;
    dlb_pcmpmd_write_status write_status;
    dlb_pmd_success success;

    ::memset(buffer, 0, sizeof(buffer));
    success = AddMediumModel();
    ASSERT_EQ((dlb_pmd_success)PMD_SUCCESS, success);
    ASSERT_TRUE(InitComboModel1(mModel1, nullptr));

    dlb_pcmpmd_augmentor_init(
        &aug, mPmdModelCombo1, mAugmentorMemory,
        DLB_PMD_FRAMERATE_12000, DLB_PMD_KLV_UL_ST2109,
        false, CHANNEL_COUNT, CHANNEL_COUNT, false, 0);

    // Try it
    write_status = dlb_pcmpmd_augmentor_try_frame(aug, mTryFrameMemory, buffer, CHANNEL_COUNT, FRAME_SIZE);
    EXPECT_EQ(DLB_PCMPMD_WRITE_STATUS_YELLOW, write_status);
}

TEST_F(DlbPmdPcm01, TryFrame_Large)
{
    static const size_t FRAME_SIZE = 400;
    static const size_t CHANNEL_COUNT = 2;

    uint32_t buffer[FRAME_SIZE * CHANNEL_COUNT];
    dlb_pcmpmd_augmentor *aug = NULL;
    dlb_pcmpmd_write_status write_status;
    dlb_pmd_success success;

    ::memset(buffer, 0, sizeof(buffer));
    success = AddLargeModel();
    ASSERT_EQ((dlb_pmd_success)PMD_SUCCESS, success);
    ASSERT_TRUE(InitComboModel1(mModel1, nullptr));

    dlb_pcmpmd_augmentor_init(
        &aug, mPmdModelCombo1, mAugmentorMemory,
        DLB_PMD_FRAMERATE_12000, DLB_PMD_KLV_UL_ST2109,
        false, CHANNEL_COUNT, CHANNEL_COUNT, false, 0);

    // Try it
    write_status = dlb_pcmpmd_augmentor_try_frame(aug, mTryFrameMemory, buffer, CHANNEL_COUNT, FRAME_SIZE);
    EXPECT_EQ(DLB_PCMPMD_WRITE_STATUS_RED, write_status);
}

TEST_F(DlbPmdPcm01, ModelTryFrame_Small_sADM_30_fps)
{
    static const size_t FRAME_SIZE = 1600;
    static const size_t CHANNEL_COUNT = 2;

    uint32_t buffer[FRAME_SIZE * CHANNEL_COUNT];
    dlb_pcmpmd_write_status write_status;
    dlb_pmd_success success;

    ::memset(buffer, 0, sizeof(buffer));
    success = AddBasicModel();
    ASSERT_EQ((dlb_pmd_success)PMD_SUCCESS, success);
    ASSERT_TRUE(InitComboModel1(mModel1, nullptr));

    // Allocate memory
    size_t sz = dlb_pcmpmd_augmentor_model_try_frame_query_mem(mPmdModelCombo1, PMD_TRUE);
    mSadmTryFrameMemory = new uint8_t[sz];
    ASSERT_NE(nullptr, mSadmTryFrameMemory);

    // Try it
    write_status = dlb_pcmpmd_augmentor_model_try_frame(
        mPmdModelCombo1, mSadmTryFrameMemory, buffer, CHANNEL_COUNT, FRAME_SIZE, DLB_PMD_FRAMERATE_3000, PMD_FALSE, PMD_TRUE);
    EXPECT_EQ(DLB_PCMPMD_WRITE_STATUS_GREEN, write_status);
}

TEST_F(DlbPmdPcm01, ModelTryFrame_Medium_sADM_30_fps)
{
    static const size_t FRAME_SIZE = 1600;
    static const size_t CHANNEL_COUNT = 2;

    uint32_t buffer[FRAME_SIZE * CHANNEL_COUNT];
    dlb_pcmpmd_write_status write_status;
    dlb_pmd_success success;

    ::memset(buffer, 0, sizeof(buffer));
    success = AddMediumModel();
    ASSERT_EQ((dlb_pmd_success)PMD_SUCCESS, success);
    ASSERT_TRUE(InitComboModel1(mModel1, nullptr));

    // Allocate memory
    size_t sz = dlb_pcmpmd_augmentor_model_try_frame_query_mem(mPmdModelCombo1, PMD_TRUE);
    mSadmTryFrameMemory = new uint8_t[sz];
    ASSERT_NE(nullptr, mSadmTryFrameMemory);

    // Try it
    write_status = dlb_pcmpmd_augmentor_model_try_frame(
        mPmdModelCombo1, mSadmTryFrameMemory, buffer, CHANNEL_COUNT, FRAME_SIZE, DLB_PMD_FRAMERATE_3000, PMD_FALSE, PMD_TRUE);
    EXPECT_EQ(DLB_PCMPMD_WRITE_STATUS_GREEN, write_status);
}

TEST_F(DlbPmdPcm01, ModelTryFrame_Large_sADM_60_fps)
{
    static const size_t FRAME_SIZE = 800;
    static const size_t CHANNEL_COUNT = 2;

    uint32_t buffer[FRAME_SIZE * CHANNEL_COUNT];
    dlb_pcmpmd_write_status write_status;
    dlb_pmd_success success;

    ::memset(buffer, 0, sizeof(buffer));
    success = AddLargeModel();
    ASSERT_EQ((dlb_pmd_success)PMD_SUCCESS, success);
    ASSERT_TRUE(InitComboModel1(mModel1, nullptr));

    // Allocate memory
    size_t sz = dlb_pcmpmd_augmentor_model_try_frame_query_mem(mPmdModelCombo1, PMD_TRUE);
    mSadmTryFrameMemory = new uint8_t[sz];
    ASSERT_NE(nullptr, mSadmTryFrameMemory);

    // Try it
    write_status = dlb_pcmpmd_augmentor_model_try_frame(
        mPmdModelCombo1, mSadmTryFrameMemory, buffer, CHANNEL_COUNT, FRAME_SIZE, DLB_PMD_FRAMERATE_6000, PMD_FALSE, PMD_TRUE);
    EXPECT_EQ(DLB_PCMPMD_WRITE_STATUS_RED, write_status);
}

TEST_F(DlbPmdPcm01, ModelTryFrame_Large_sADM_24fps)
{
    static const size_t FRAME_SIZE = 2000;
    static const size_t CHANNEL_COUNT = 2;

    uint32_t buffer[FRAME_SIZE * CHANNEL_COUNT];
    dlb_pcmpmd_write_status write_status;
    dlb_pmd_success success;

    ::memset(buffer, 0, sizeof(buffer));
    success = AddLargeModel();
    ASSERT_EQ((dlb_pmd_success)PMD_SUCCESS, success);
    ASSERT_TRUE(InitComboModel1(mModel1, nullptr));

    // Allocate memory
    size_t sz = dlb_pcmpmd_augmentor_model_try_frame_query_mem(mPmdModelCombo1, PMD_TRUE);
    mSadmTryFrameMemory = new uint8_t[sz];
    ASSERT_NE(nullptr, mSadmTryFrameMemory);

    // Try it
    write_status = dlb_pcmpmd_augmentor_model_try_frame(
        mPmdModelCombo1, mSadmTryFrameMemory, buffer, CHANNEL_COUNT, FRAME_SIZE, DLB_PMD_FRAMERATE_2400, PMD_FALSE, PMD_TRUE);
    EXPECT_EQ(DLB_PCMPMD_WRITE_STATUS_GREEN, write_status);
}

TEST_F(DlbPmdPcm01, ReadPresentationIdAndLoudnessCorrectionType)    // PMDLIB-138, PMDLIB-139
{
    static const size_t FRAME_SIZE = 1600;
    static const size_t CHANNEL_COUNT = 2;
    static const dlb_pmd_presentation_id PRESENTATION_ID = 7;       // Somewhere in the middle of all the presentations

    uint32_t buffer[FRAME_SIZE * CHANNEL_COUNT];
    dlb_pcmpmd_augmentor *aug = NULL;
    dlb_pcmpmd_extractor *ext = NULL;
    dlb_pmd_success success;

    ::memset(buffer, 0, sizeof(buffer));
    success = AddMediumModel();
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);
    ASSERT_TRUE(InitComboModel1(mModel1, nullptr));
    ASSERT_TRUE(InitComboModel2(mModel2, nullptr));

    dlb_pmd_loudness pld;

    ::memset(&pld, 0, sizeof(pld));
    pld.presid = 7;
    pld.loud_prac_type = PMD_PLD_LOUDNESS_PRACTICE_CONSUMER_LEVELLER;
    pld.loudcorr_type = PMD_PLD_CORRECTION_REALTIME;
    success = dlb_pmd_set_loudness(mModel1, &pld);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);

    dlb_pcmpmd_augmentor_init(
        &aug, mPmdModelCombo1, mAugmentorMemory,
        DLB_PMD_FRAMERATE_3000, DLB_PMD_KLV_UL_ST2109,
        false, CHANNEL_COUNT, CHANNEL_COUNT, true, 0);

    dlb_pcmpmd_augment(aug, buffer, FRAME_SIZE, 0);

    dlb_pcmpmd_extractor_init(
        &ext, mExtractorMemory, DLB_PMD_FRAMERATE_3000, 0, CHANNEL_COUNT, true, mPmdModelCombo2, nullptr);

    success = dlb_pcmpmd_extract(ext, buffer, FRAME_SIZE, 0);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);

    dlb_pmd_loudness_iterator it;

    success = dlb_pmd_loudness_iterator_init(&it, mModel2);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);
    ::memset(&pld, 0, sizeof(pld));
    success = dlb_pmd_loudness_iterator_next(&it, &pld);
    ASSERT_EQ(static_cast<dlb_pmd_success>(PMD_SUCCESS), success);
    EXPECT_EQ(PRESENTATION_ID, pld.presid);                     // PMDLIB-138
    EXPECT_EQ(PMD_PLD_CORRECTION_REALTIME, pld.loudcorr_type);  // PMDLIB-139
}
