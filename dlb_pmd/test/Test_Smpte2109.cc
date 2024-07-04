/************************************************************************
 * dlb_pmd
 * Copyright (c) 2018-2021, Dolby Laboratories Inc.
 * Copyright (c) 2018-2021, Dolby International AB.
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

/**
 * @file Test_Smpte2109.cc
 * @brief test SMPTE 2109 functionality
 */

#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

extern "C"
{
#include "dlb_pmd_api.h"    
#include "dlb_pmd_klv.h"
/* peer into the internals of the implementation */
#include "src/model/pmd_model.h"
#include "src/modules/klv/klv.h"

extern dlb_pmd_bool global_testing_version_numbers;
}


#include "TestModel.hh"
#include "gtest/gtest.h"

// Uncomment the next line to remove the tests in this file from the run:
//#define DISABLE_SMPTE_2109_TESTS

#ifndef DISABLE_SMPTE_2109_TESTS
class SampleOffsetsTest: public ::testing::TestWithParam<std::tr1::tuple<int, int> > {};

TEST_P(SampleOffsetsTest, sample_offset)
{
    unsigned int i = std::tr1::get<0>(GetParam());
    TestModel::TestType t = TestModel::TEST_TYPES[std::tr1::get<1>(GetParam())];
    dlb_pmd_presentation_id pres;
    dlb_pmd_element_id bed;
    TestModel m;

    global_testing_version_numbers = 1;

    bed = m.new_elid();
    pres = m.new_presid();
    
    if (   dlb_pmd_set_title(m, "SMPTE 2109 testing")
        || dlb_pmd_add_signals(m, 2)
        || dlb_pmd_add_bed(m, bed, NULL, DLB_PMD_SPEAKER_CONFIG_2_0, 1, 0)
        || dlb_pmd_add_presentation(m, pres, "eng", "TESTPREZ", "eng",
                                    DLB_PMD_SPEAKER_CONFIG_2_0, 1, &bed)
       )
    {
        ADD_FAILURE() << "Failed to build model";
    }
    else if (dlb_pmd_set_smpte2109_sample_offset(m, (1<<i)-1))
    {
        ADD_FAILURE() << "Could not set SMPTE 2109 sample offset";
    }
    else
    {
        try
        {
            m.test(t, "SMPTE_2109_sample_offset", i);
        }
        catch (TestModel::failure& f)
        {
            ADD_FAILURE() << f.msg;
        }
    }
    global_testing_version_numbers = 0;
}



INSTANTIATE_TEST_CASE_P(PMD_Smpte2109, SampleOffsetsTest,
           testing::Combine(testing::Range(0, 16),
                            testing::Range(TestModel::FIRST_TEST_TYPE,
                                           TestModel::LAST_TEST_TYPE+1)));


class DynamicTagsTest1: public ::testing::TestWithParam<std::tr1::tuple<int, int> > {};

TEST_P(DynamicTagsTest1, dynamic_tags_1)
{
    unsigned int ii = std::tr1::get<0>(GetParam());
    TestModel::TestType t = TestModel::TEST_TYPES[std::tr1::get<1>(GetParam())];

    uint16_t i = (uint16_t)ii;
    uint16_t j;
    uint16_t limit = 12;
    dlb_pmd_element_id bed;
    dlb_pmd_presentation_id pres;

    assert(i < 110);
    if (i + limit > 110) limit = (uint16_t)(110 - i);

    TestModel m;
    bed = m.new_elid();
    pres = m.new_presid();

    if (   dlb_pmd_set_title(m, "SMPTE 2109 testing")
        || dlb_pmd_add_signals(m, 2)
        || dlb_pmd_add_bed(m, bed, NULL, DLB_PMD_SPEAKER_CONFIG_2_0, 1, 0)
        || dlb_pmd_add_presentation(m, pres, "eng", "TESTPREZ", "eng",
                                    DLB_PMD_SPEAKER_CONFIG_2_0, 1, &bed)
       )
    {
        ADD_FAILURE() << "Error: failed to create model: " << dlb_pmd_error(m);
    }
    else
    {
        for (j = i; j != i+limit; ++j)
        {
            uint8_t ul[16];
            memset(ul, j, sizeof(ul));
            if (dlb_pmd_remap_local_tag(m, 127-j, ul))
            {
                ADD_FAILURE() << "Error: could not remap dynamic tag";
            }
        }
        m.skip_pcm_samples(i);
        try
        {
            m.test(t, "Dynamic_Tags_1", i);
        }
        catch (TestModel::failure& f)
        {
            ADD_FAILURE() << f.msg;
        }
    }
}



INSTANTIATE_TEST_CASE_P(PMD_Smpte2109, DynamicTagsTest1,
           testing::Combine(testing::Range(0, 110),
                            testing::Range(TestModel::FIRST_TEST_TYPE,
                                           TestModel::LAST_TEST_TYPE+1)));



class DynamicTagsTest2: public ::testing::TestWithParam<std::tr1::tuple<int, int> > {};

TEST_P(DynamicTagsTest2, dynamic_tags_2)
{
    unsigned int i = std::tr1::get<0>(GetParam());
    TestModel::TestType t = TestModel::TEST_TYPES[std::tr1::get<1>(GetParam())];
    dlb_pmd_presentation_id pres;

    const uint8_t empty_ul[16] = "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000";
    dlb_pmd_element_id bed_20;
    dlb_pmd_element_id bed_51;
    dlb_pmd_element_id bed_512;
    dlb_pmd_element_id bed_514;
    dlb_pmd_element_id o1;

    TestModel m1;
    TestModel m2;
    TestModel m_in;

    bed_20   = m1.new_elid();
    bed_51   = m1.new_elid();
    bed_512  = m1.new_elid();
    bed_514  = m1.new_elid();
    o1       = m1.new_elid();
    pres     = m1.new_presid();
    if (   dlb_pmd_set_title(m1, "SMPTE 2109 dynamic retag test")
        || dlb_pmd_add_signals(m1, 11)
        || dlb_pmd_add_bed(m1, bed_20,  NULL, DLB_PMD_SPEAKER_CONFIG_2_0,   1, 0)
        || dlb_pmd_add_bed(m1, bed_51,  NULL, DLB_PMD_SPEAKER_CONFIG_5_1,   1, 0)
        || dlb_pmd_add_bed(m1, bed_512, NULL, DLB_PMD_SPEAKER_CONFIG_5_1_2, 1, 0)
        || dlb_pmd_add_bed(m1, bed_514, NULL, DLB_PMD_SPEAKER_CONFIG_5_1_4, 1, 0)
        || dlb_pmd_add_generic_obj(m1, o1, NULL, 11, 0.0f, 0.0f, 0.0f, 0.0, 0.0, 0, 1, 0)
        || dlb_pmd_add_presentation(m1, pres, "eng", "TESTPREZ", "eng",
                                    DLB_PMD_SPEAKER_CONFIG_2_0, 1, &bed_20)
           /* and a dynamic retagging */
        || dlb_pmd_remap_local_tag(m1, KLV_PMD_LOCAL_TAG_DYNAMIC_UPDATES, empty_ul)
       )
    {
        ADD_FAILURE() << "Could not create model: " << dlb_pmd_error(m1);
    }
    else
    {
        /* keep a copy */
        m2 = m1;

        m1.pcm_single_frame();

        /* now add an update, which will be ignored by retagging */
        if (dlb_pmd_add_update(m1, o1, 5, 0.1f, 0.1f, 0.1f))
        {
            ADD_FAILURE() << "Could not add retagging victim";
        }
        else
        {
            /* dynamic retagging is a property of KLV translation */
            try
            {
                switch(t)
                {
                    case TestModel::TEST_XML:
                    case TestModel::TEST_MDSET:
                        m1.test(t, "Dynamic_Tags_2", i);
                        break;
                    default:
                        m1.negtest(t, "Dynamic_Tags_2", i);
                        m2.test(t, "Dynamic_Tags_2", i+2);
                        break;
                }
            } 
            catch (TestModel::failure& f)
            {
                ADD_FAILURE() << f.msg;
            }
        }
    }
}



INSTANTIATE_TEST_CASE_P(PMD_Smpte2109, DynamicTagsTest2,
           testing::Combine(testing::Range(0, 2),
                            testing::Range(TestModel::FIRST_TEST_TYPE,
                                           TestModel::LAST_TEST_TYPE+1)));
#endif


