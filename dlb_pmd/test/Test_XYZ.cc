/************************************************************************
 * dlb_pmd
 * Copyright (c) 2018, Dolby Laboratories Inc.
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

#include "src/model/pmd_model.h"
#include "dlb_pmd_api.h"    

#include "TestModel.hh"
#include "gtest/gtest.h"


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

    unsigned int num_blocks = (frame_length - GUARDBAND) / 160;
    num_updates = num_blocks * (160 / 32);

    for (time = 1; time < num_updates; ++time)
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
        m.test(t, "PCM_Dynamic_Update_test1", (int)t, false);
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
        if (t > TestModel::TEST_KLV)
        {
            /* shouldn't match if you don't apply updates to original model */
            m.negtest(t, "PCM_Dynamic_Update_test2a", (int)t, false);
        }
        m.test(t, "PCM_Dynamic_Update_test2b", (int)t, true);
    }
}


INSTANTIATE_TEST_CASE_P(PMD_XYZ, UpdateTest2,
                        testing::Range(TestModel::FIRST_TEST_TYPE,
                                       TestModel::LAST_TEST_TYPE+1));


