/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2018-2021 by Dolby Laboratories,
 *                Copyright (C) 2018-2021 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

/**
 * @file Test_XYZ.cc
 * @brief Test Dynamic updates
 */

#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

#include "src/model/pmd_model.h"
#include "dlb_pmd_api.h"    

#include "TestModel.hh"
#include "gtest/gtest.h"

// Uncomment the next line to remove the tests in this file from the run:
//#define DISABLE_XYZ_TESTS


#ifndef DISABLE_XYZ_TESTS
class UpdateTest1: public ::testing::TestWithParam<int> {};
class UpdateTest2: public ::testing::TestWithParam<int> {};


static const int GUARDBAND=32;

static inline
unsigned int
compute_frame_length
    (TestModel::TestType t
    )
{
    switch (t)
    {
        case TestModel::TEST_XML:            return 2002;
        case TestModel::TEST_MDSET:          return 2002;
        case TestModel::TEST_KLV:            return 2002;

        case TestModel::TEST_PCM_PAIR_2398:  return 2002;
        case TestModel::TEST_PCM_PAIR_2400:  return 2000;
        case TestModel::TEST_PCM_PAIR_2500:  return 1920;
        case TestModel::TEST_PCM_PAIR_2997:  return 1601;
        case TestModel::TEST_PCM_PAIR_3000:  return 1600;
        case TestModel::TEST_PCM_PAIR_5000:  return  960;
        case TestModel::TEST_PCM_PAIR_5994:  return  800;
        case TestModel::TEST_PCM_PAIR_6000:  return  800;
        case TestModel::TEST_PCM_PAIR_10000: return  480;
        case TestModel::TEST_PCM_PAIR_11988: return  400;
        case TestModel::TEST_PCM_PAIR_12000: return  400;

        case TestModel::TEST_PCM_CHAN_2398:  return 2002;
        case TestModel::TEST_PCM_CHAN_2400:  return 2000;
        case TestModel::TEST_PCM_CHAN_2500:  return 1920;
        case TestModel::TEST_PCM_CHAN_2997:  return 1601;
        case TestModel::TEST_PCM_CHAN_3000:  return 1600;
        case TestModel::TEST_PCM_CHAN_5000:  return  960;
        case TestModel::TEST_PCM_CHAN_5994:  return  800;
        case TestModel::TEST_PCM_CHAN_6000:  return  800;
        case TestModel::TEST_PCM_CHAN_10000: return  480;
        case TestModel::TEST_PCM_CHAN_11988: return  400;
        case TestModel::TEST_PCM_CHAN_12000: return  400;

        default: return 2002;
    }
}


static inline
dlb_pmd_success
add_updates
    (dlb_pmd_model *m
    ,dlb_pmd_element_id obj
    ,TestModel::TestType t
    )
{
    unsigned int frame_length = compute_frame_length(t);
    unsigned int num_updates = frame_length / 32;  /* resolution of update time is 32 samples */
    unsigned int time;
    float increment = 1.0f / num_updates;
    float x = 0.0f;

    /* no updates in 1st block */
    if (t <= TestModel::TEST_PCM_CHAN_12000)
    {
        unsigned int num_blocks = ((frame_length - GUARDBAND) / 160);
        num_updates = num_blocks * (160 / 32);
    }

    /* skip 1st block */
    for (time = 1+(160/32); time < num_updates; ++time)
    {
        x += increment;
        if (dlb_pmd_add_update(m, obj, time, x, x, x)) return 1;
    }
    return 0;
}


/**
 * PCM test 1: check that in 1st frame the updates transmitted equal
 * the updates received.
 */
TEST_P(UpdateTest1, payload_testing)
{
    TestModel::TestType t = TestModel::TEST_TYPES[GetParam()];

    TestModel m;
    dlb_pmd_element_id bed = m.new_elid();
    dlb_pmd_element_id obj = m.new_elid();
    dlb_pmd_presentation_id pres = m.new_presid();
    
    m.pcm_single_frame();

    if (   dlb_pmd_add_signals(m, 7)
        || dlb_pmd_set_title(m, "Dynamic update testing")
        || dlb_pmd_add_bed(m, bed, NULL, DLB_PMD_SPEAKER_CONFIG_5_1, 1, 0)
        || dlb_pmd_add_generic_obj(m, obj, NULL, 7, 0.0f, 0.0f, 0.0f, 0.0f, 0.0, 0, 1, 0)
        || dlb_pmd_add_presentation2(m, pres, "eng", "TESTPREZ", "eng",
                                     DLB_PMD_SPEAKER_CONFIG_5_1, 2, bed, obj)
        || add_updates(m, obj, t)
       )
    {
        ADD_FAILURE() << "Failed to create test model: " << dlb_pmd_error(m);
    }
    else
    {
        try
        {
            m.test(t, "PCM_Dynamic_Update_test1", (int)t, false);
        }
        catch (TestModel::failure& f)
        {
            ADD_FAILURE() << f.msg;
        }
    }
}


INSTANTIATE_TEST_CASE_P(PMD_XYZ, UpdateTest1,
                        testing::Range(TestModel::FIRST_TEST_TYPE,
                                       TestModel::LAST_TEST_TYPE+1));



/**
 * PCM test 2: check that in 2nd frame there are no updates transmitted:
 * The 1st block of the second frame should equal the original model
 * after all the updates have happened
 */
TEST_P(UpdateTest2, payload_testing)
{
    TestModel::TestType t = TestModel::TEST_TYPES[GetParam()];

    TestModel m;
    dlb_pmd_element_id bed = m.new_elid();
    dlb_pmd_element_id obj = m.new_elid();
    dlb_pmd_presentation_id pres = m.new_presid();
    
    if (   dlb_pmd_add_signals(m, 7)
        || dlb_pmd_set_title(m, "Dynamic update testing")
        || dlb_pmd_add_bed(m, bed, NULL, DLB_PMD_SPEAKER_CONFIG_5_1, 1, 0)
        || dlb_pmd_add_generic_obj(m, obj, NULL, 7, 0.0f, 0.0f, 0.0f, 0.0f, 0.0, 0, 1, 0)
        || dlb_pmd_add_presentation2(m, pres, "eng", "TESTPREZ", "eng",
                                     DLB_PMD_SPEAKER_CONFIG_5_1, 2, bed, obj)
        || add_updates(m, obj, t)
       )
    {
        ADD_FAILURE() << "Failed to create test model: " << dlb_pmd_error(m);
    }
    else
    {
        try
        {
            if (t > TestModel::TEST_KLV)
            {
                /* shouldn't match if you don't apply updates to original model */
                m.negtest(t, "PCM_Dynamic_Update_test2a", (int)t, false);
            }
            m.test(t, "PCM_Dynamic_Update_test2b", (int)t, true);
        }
        catch (TestModel::failure& f)
        {
            ADD_FAILURE() << f.msg;
        }
    }
}


INSTANTIATE_TEST_CASE_P(PMD_XYZ, UpdateTest2,
                        testing::Range(TestModel::FIRST_TEST_TYPE,
                                       TestModel::LAST_TEST_TYPE+1));
#endif


