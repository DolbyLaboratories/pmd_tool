/************************************************************************
 * dlb_pmd
 * Copyright (c) 2020, Dolby Laboratories Inc.
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

#include "dlb_pmd/include/dlb_pmd_api.h"
#include "dlb_pmd/include/dlb_pmd_pcm.h"

#include <stdint.h>

class DlbPmdPcm01 : public testing::Test
{
protected:
    static const size_t MODEL_MEMORY_SIZE = 4000000;    // 3734704 on Windows
    static const size_t AUGMENTOR_MEMORY_SIZE = 12000;  // 10184 on Windows

    uint8_t          mModelMemory[MODEL_MEMORY_SIZE];
    uint8_t          mAugmentorMemory[AUGMENTOR_MEMORY_SIZE];
    dlb_pmd_model   *mModel;

    dlb_pmd_success AddBasicModel()
    {
        dlb_pmd_success success;
        char obj_name[99];
        char i;

        success = dlb_pmd_add_signals(mModel, 16);
        if (success != PMD_SUCCESS)
        {
            return success;
        }

        success = dlb_pmd_add_bed(mModel, 101, "Bed 1", DLB_PMD_SPEAKER_CONFIG_5_1_4, 2, 0);
        if (success != PMD_SUCCESS)
        {
            return success;
        }

        for (i = 1; i <= 5; i++)
        {
            sprintf(obj_name, "Object %u", static_cast<unsigned>(i));
            success = dlb_pmd_add_generic_obj2(mModel, i, obj_name, i + 11, 0.0, 0.0, 0.0);
            if (success != PMD_SUCCESS)
            {
                return success;
            }
        }

        dlb_pmd_element_id ids[] = { 101, 1, 2, 3, 4, 5 };
        int id_count = sizeof(ids) / sizeof(dlb_pmd_element_id);

        success = dlb_pmd_add_presentation(
            mModel, 1, "en", "Presentation 1", "en", DLB_PMD_SPEAKER_CONFIG_5_1_4, id_count, ids);
        if (success != PMD_SUCCESS)
        {
            return success;
        }
 
        success = dlb_pmd_iat_add(mModel, 0);

        return success;
    }

    dlb_pmd_success AddMediumModel()
    {
        dlb_pmd_success success;
        unsigned int entityId, ch;
        char name[99];
        char i;

        success = dlb_pmd_add_signals(mModel, 128);
        if (success != PMD_SUCCESS)
        {
            return success;
        }

        for (entityId = 101, ch = 2, i = 1; i <= 15; entityId++, ch += 8, i++)
        {
            sprintf(name, "Bed %u", entityId);
            success = dlb_pmd_add_bed(mModel, static_cast<dlb_pmd_element_id>(entityId), name, DLB_PMD_SPEAKER_CONFIG_5_1_2, ch, 0);
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
                mModel, static_cast<dlb_pmd_presentation_id>(entityId), "en", name, "en", DLB_PMD_SPEAKER_CONFIG_5_1_2, 1, &elementId);
            if (success != PMD_SUCCESS)
            {
                return success;
            }
        }

        success = dlb_pmd_iat_add(mModel, 0);

        return success;
    }

    dlb_pmd_success AddLargeModel()
    {
        dlb_pmd_success success;
        unsigned int entityId, ch;
        char name[99];
        char i;

        success = dlb_pmd_add_signals(mModel, 128);
        if (success != PMD_SUCCESS)
        {
            return success;
        }

        for (entityId = 101, ch = 2, i = 1; i <= 11; entityId++, ch += 10, i++)
        {
            sprintf(name, "This is a 5.1.4 bed with id = %u", entityId);
            success = dlb_pmd_add_bed(mModel, static_cast<dlb_pmd_element_id>(entityId), name, DLB_PMD_SPEAKER_CONFIG_5_1_4, ch, 0);
            if (success != PMD_SUCCESS)
            {
                return success;
            }
        }

        for (entityId = 1; ch <= 128; entityId++, ch++)
        {
            sprintf(name, "This is a generic object with id = %u", entityId);
            success = dlb_pmd_add_generic_obj2(mModel, static_cast<dlb_pmd_element_id>(entityId), name, ch, 0.0, 0.0, 0.0);
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
                mModel, static_cast<dlb_pmd_presentation_id>(entityId), "en", name, "en", DLB_PMD_SPEAKER_CONFIG_5_1_4, 2, elementIds);
            if (success != PMD_SUCCESS)
            {
                return success;
            }
        }

        success = dlb_pmd_iat_add(mModel, 0);

        return success;
    }

    virtual void SetUp()
    {
        ::memset(mModelMemory, 0, sizeof(mModelMemory));
        ::memset(mAugmentorMemory, 0, sizeof(mAugmentorMemory));

        mModel = NULL;
        dlb_pmd_init(&mModel, mModelMemory);
    }

    virtual void TearDown()
    {
        if (mModel != NULL)
        {
            dlb_pmd_finish(mModel);
            mModel = NULL;
        }
    }
};

TEST_F(DlbPmdPcm01, MemoryCheck)
{
    size_t model_sz = MODEL_MEMORY_SIZE;
    size_t aug_sz = AUGMENTOR_MEMORY_SIZE;
    size_t n = dlb_pmd_query_mem();

    if (MODEL_MEMORY_SIZE < n)
    {
        printf("Memory needed for model: %u\n", (unsigned)n);
    }
    EXPECT_LE(n, model_sz);

    n = dlb_pcmpmd_augmentor_query_mem();
    if (AUGMENTOR_MEMORY_SIZE < n)
    {
        printf("Memory needed for augmentor: %u\n", (unsigned)n);
    }
    EXPECT_LE(n, aug_sz);
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

    uint32_t buffer[FRAME_SIZE * CHANNEL_COUNT];    // TODO: make certain this works on Linux
    dlb_pcmpmd_write_status write_status;
    dlb_pmd_success success;

    ::memset(buffer, 0, sizeof(buffer));
    success = AddBasicModel();
    ASSERT_EQ((dlb_pmd_success)PMD_SUCCESS, success);

    write_status = dlb_pcmpmd_augmentor_model_try_frame(NULL, NULL, 0, 0, NUM_PMD_FRAMERATES, PMD_FALSE, PMD_FALSE);
    EXPECT_EQ(DLB_PCMPMD_WRITE_STATUS_ERROR, write_status);
    write_status = dlb_pcmpmd_augmentor_model_try_frame(mModel, NULL, 0, 0, NUM_PMD_FRAMERATES, PMD_FALSE, PMD_FALSE);
    EXPECT_EQ(DLB_PCMPMD_WRITE_STATUS_ERROR, write_status);
    write_status = dlb_pcmpmd_augmentor_model_try_frame(mModel, buffer, 0, 0, NUM_PMD_FRAMERATES, PMD_FALSE, PMD_FALSE);
    EXPECT_EQ(DLB_PCMPMD_WRITE_STATUS_ERROR, write_status);
    write_status = dlb_pcmpmd_augmentor_model_try_frame(mModel, buffer, CHANNEL_COUNT, 0, NUM_PMD_FRAMERATES, PMD_FALSE, PMD_FALSE);
    EXPECT_EQ(DLB_PCMPMD_WRITE_STATUS_ERROR, write_status);
    write_status = dlb_pcmpmd_augmentor_model_try_frame(mModel, buffer, CHANNEL_COUNT, FRAME_SIZE, NUM_PMD_FRAMERATES, PMD_FALSE, PMD_FALSE);
    EXPECT_EQ(DLB_PCMPMD_WRITE_STATUS_ERROR, write_status);

    /* Not enough samples for 30fps... */
    write_status = dlb_pcmpmd_augmentor_model_try_frame(mModel, buffer, CHANNEL_COUNT, FRAME_SIZE, DLB_PMD_FRAMERATE_3000, PMD_FALSE, PMD_FALSE);
    EXPECT_EQ(DLB_PCMPMD_WRITE_STATUS_ERROR, write_status);

    write_status = dlb_pcmpmd_augmentor_model_try_frame(mModel, buffer, CHANNEL_COUNT, FRAME_SIZE, DLB_PMD_FRAMERATE_12000, PMD_FALSE, PMD_FALSE);
    EXPECT_EQ(DLB_PCMPMD_WRITE_STATUS_GREEN, write_status);
}

TEST_F(DlbPmdPcm01, ModelTryFrame_Medium)
{
    static const size_t FRAME_SIZE = 400;
    static const size_t CHANNEL_COUNT = 2;

    uint32_t buffer[FRAME_SIZE * CHANNEL_COUNT];    // TODO: make certain this works on Linux
    dlb_pcmpmd_write_status write_status;
    dlb_pmd_success success;

    ::memset(buffer, 0, sizeof(buffer));
    success = AddMediumModel();
    ASSERT_EQ((dlb_pmd_success)PMD_SUCCESS, success);

    write_status = dlb_pcmpmd_augmentor_model_try_frame(mModel, buffer, CHANNEL_COUNT, FRAME_SIZE, DLB_PMD_FRAMERATE_12000, false, false);
    EXPECT_EQ(DLB_PCMPMD_WRITE_STATUS_YELLOW, write_status);
}

TEST_F(DlbPmdPcm01, ModelTryFrame_Large)
{
    static const size_t FRAME_SIZE = 400;
    static const size_t CHANNEL_COUNT = 2;

    uint32_t buffer[FRAME_SIZE * CHANNEL_COUNT];    // TODO: make certain this works on Linux
    dlb_pcmpmd_write_status write_status;
    dlb_pmd_success success;

    ::memset(buffer, 0, sizeof(buffer));
    success = AddLargeModel();
    ASSERT_EQ((dlb_pmd_success)PMD_SUCCESS, success);

    write_status = dlb_pcmpmd_augmentor_model_try_frame(mModel, buffer, CHANNEL_COUNT, FRAME_SIZE, DLB_PMD_FRAMERATE_12000, false, false);
    EXPECT_EQ(DLB_PCMPMD_WRITE_STATUS_RED, write_status);
}

TEST_F(DlbPmdPcm01, TryFrame_Bad_Small)
{
    static const size_t FRAME_SIZE = 400;
    static const size_t CHANNEL_COUNT = 2;

    uint32_t buffer[FRAME_SIZE * CHANNEL_COUNT];    // TODO: make certain this works on Linux
    dlb_pcmpmd_augmentor *aug = NULL;
    dlb_pcmpmd_write_status write_status;
    dlb_pmd_success success;

    ::memset(buffer, 0, sizeof(buffer));
    success = AddBasicModel();
    ASSERT_EQ((dlb_pmd_success)PMD_SUCCESS, success);

    dlb_pcmpmd_augmentor_init(
        &aug, mModel, mAugmentorMemory,
        DLB_PMD_FRAMERATE_12000, DLB_PMD_KLV_UL_ST2109,
        false, CHANNEL_COUNT, CHANNEL_COUNT, false, 0);

    write_status = dlb_pcmpmd_augmentor_try_frame(NULL, NULL, 0, 0);
    EXPECT_EQ(DLB_PCMPMD_WRITE_STATUS_ERROR, write_status);
    write_status = dlb_pcmpmd_augmentor_try_frame(aug, NULL, 0, 0);
    EXPECT_EQ(DLB_PCMPMD_WRITE_STATUS_ERROR, write_status);
    write_status = dlb_pcmpmd_augmentor_try_frame(aug, buffer, 0, 0);
    EXPECT_EQ(DLB_PCMPMD_WRITE_STATUS_ERROR, write_status);
    write_status = dlb_pcmpmd_augmentor_try_frame(aug, buffer, CHANNEL_COUNT, 0);
    EXPECT_EQ(DLB_PCMPMD_WRITE_STATUS_ERROR, write_status);

    write_status = dlb_pcmpmd_augmentor_try_frame(aug, buffer, CHANNEL_COUNT, FRAME_SIZE);
    EXPECT_EQ(DLB_PCMPMD_WRITE_STATUS_GREEN, write_status);
}

TEST_F(DlbPmdPcm01, TryFrame_Medium)
{
    static const size_t FRAME_SIZE = 400;
    static const size_t CHANNEL_COUNT = 2;

    uint32_t buffer[FRAME_SIZE * CHANNEL_COUNT];    // TODO: make certain this works on Linux
    dlb_pcmpmd_augmentor *aug = NULL;
    dlb_pcmpmd_write_status write_status;
    dlb_pmd_success success;

    ::memset(buffer, 0, sizeof(buffer));
    success = AddMediumModel();
    ASSERT_EQ((dlb_pmd_success)PMD_SUCCESS, success);

    dlb_pcmpmd_augmentor_init(
        &aug, mModel, mAugmentorMemory,
        DLB_PMD_FRAMERATE_12000, DLB_PMD_KLV_UL_ST2109,
        false, CHANNEL_COUNT, CHANNEL_COUNT, false, 0);

    write_status = dlb_pcmpmd_augmentor_try_frame(aug, buffer, CHANNEL_COUNT, FRAME_SIZE);
    EXPECT_EQ(DLB_PCMPMD_WRITE_STATUS_YELLOW, write_status);
}

TEST_F(DlbPmdPcm01, TryFrame_Large)
{
    static const size_t FRAME_SIZE = 400;
    static const size_t CHANNEL_COUNT = 2;

    uint32_t buffer[FRAME_SIZE * CHANNEL_COUNT];    // TODO: make certain this works on Linux
    dlb_pcmpmd_augmentor *aug = NULL;
    dlb_pcmpmd_write_status write_status;
    dlb_pmd_success success;

    ::memset(buffer, 0, sizeof(buffer));
    success = AddLargeModel();
    ASSERT_EQ((dlb_pmd_success)PMD_SUCCESS, success);

    dlb_pcmpmd_augmentor_init(
        &aug, mModel, mAugmentorMemory,
        DLB_PMD_FRAMERATE_12000, DLB_PMD_KLV_UL_ST2109,
        false, CHANNEL_COUNT, CHANNEL_COUNT, false, 0);

    write_status = dlb_pcmpmd_augmentor_try_frame(aug, buffer, CHANNEL_COUNT, FRAME_SIZE);
    EXPECT_EQ(DLB_PCMPMD_WRITE_STATUS_RED, write_status);
}
