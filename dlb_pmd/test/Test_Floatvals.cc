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
 * @file Test_Floatvals.cc
 * @brief floating-point value testing
 *
 * Check that we can encode and decode floating point values accurately to
 * within PMD quantization.
 */

#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

#include "dlb_pmd_api.h"    
#include "TestModel.hh"
#include "gtest/gtest.h"
#include <math.h>

// Uncomment the next line to remove the tests in this file from the run:
//#define DISABLE_FLOATVALS_TESTS

class FloatingValueTest: public ::testing::TestWithParam<std::tr1::tuple<int, int> > {};


struct Floatvals
{
    float db;
    float x;
    float sz;

    Floatvals(int test_param)
        : db(-INFINITY)
        , x(-1.0f + 0.02f * test_param)
        , sz(0.0f + 0.01f * test_param)
    {
        assert(test_param >= 0 && test_param <= 100);
        if (test_param > 63) test_param -= 63;
        if (test_param > 0)
        {
            db = -25.5f + (0.5f * test_param);
        }
    }
    
    bool setupModel(TestModel& m, dlb_pmd_element_id obj_id)
    {
        dlb_pmd_object object;
        if (dlb_pmd_object_lookup(m, obj_id, &object))
        {
            return true;
        }
        object.source_gain = db;
        object.x = x;
        object.y = x;
        object.z = x;
        object.size = sz;
        if (dlb_pmd_set_object(m, &object))
        {
            return true;
        }
        return false;
    }
};

   
#ifndef DISABLE_FLOATVALS_TESTS
TEST_P(FloatingValueTest, floating_values)
{
    unsigned int i = std::tr1::get<0>(GetParam());
    TestModel::TestType t = TestModel::TEST_TYPES[std::tr1::get<1>(GetParam())];

    TestModel m;
    Floatvals fv(i);
    dlb_pmd_element_id obj;

    if (m.populate(&obj))
    {
        ADD_FAILURE() << "Could not populate model: " << dlb_pmd_error(m);
    }
    else if (dlb_pmd_set_title(m, "floating-pt parameter testing"))
    {
        ADD_FAILURE() << "Could not set title";
    }
    else
    {
        if (fv.setupModel(m, obj))
        {
            ADD_FAILURE() << "Could not set up floating point values for test";
        }
        else
        {
            char tmp[32];
            snprintf(tmp, sizeof(tmp), "Floatvals_%u", i);
            m.skip_pcm_samples(i);
            m.minimal_check();  /* don't care about element or presentation names*/
            try
            {
                m.test(t, tmp, i);
            }
            catch (TestModel::failure& f)
            {
                ADD_FAILURE() << f.msg;
            }
        }
    }
}



INSTANTIATE_TEST_CASE_P(PMD_FloatingValues, FloatingValueTest,
           testing::Combine(testing::Range(0, 100),
                            testing::Range(TestModel::FIRST_TEST_TYPE,
                                           TestModel::LAST_TEST_TYPE+1)));
#endif

