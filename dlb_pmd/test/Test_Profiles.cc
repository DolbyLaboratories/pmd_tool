/************************************************************************
 * dlb_pmd
 * Copyright (c) 2020 - 2022, Dolby Laboratories Inc.
 * Copyright (c) 2022, Dolby International AB.
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
 * @file Test_Profiles.cc
 * @brief Test Profiles and levels
 */

#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

#include "dlb_pmd_generate.h"
#include "TestModel.hh"
#include "gtest/gtest.h"

// Uncomment the next line to remove the tests in this file from the run:
//#define DISABLE_PROFILES_TESTS


class ProfileTest: public ::testing::TestWithParam<std::tr1::tuple<int, int, int, int, int> >
{
protected:

    struct ProfLevel
    {
        unsigned int max_signals;
        unsigned int max_elements;
        unsigned int max_presentations;

        ProfLevel(unsigned int s, unsigned int e, unsigned int p)
            : max_signals(s)
            , max_elements(e)
            , max_presentations(p)
        {}
    };
    
    static const unsigned int NUM_PROF1_LEVELS = 3;

    static ProfLevel levels1_[NUM_PROF1_LEVELS];
};


    
ProfileTest::ProfLevel ProfileTest::levels1_[NUM_PROF1_LEVELS] =
{
    ProfLevel(16, 10, 8),  ProfLevel(16, 20, 16),  ProfLevel(16, 50, 48)
};



#ifndef DISABLE_PROFILES_TESTS
TEST_P(ProfileTest, profile_level_after)
{
    /* num_beds * num_bed_types * num_objs */
    /* how to randomly generate presets? only 16 of these
     * 6 beds, 64 dyn objects
     */

    unsigned int profile  = std::tr1::get<0>(GetParam());
    unsigned int level    = std::tr1::get<1>(GetParam());
    unsigned int num_sigs = std::tr1::get<2>(GetParam());
    unsigned int num_els  = std::tr1::get<3>(GetParam());
    unsigned int num_pres = std::tr1::get<4>(GetParam());

    unsigned int num_beds = num_els / 3;
    unsigned int num_objs = num_els - num_beds;
    unsigned int seed = 0x12345678 + profile + level + num_sigs + num_beds + num_objs + num_pres;

    dlb_pmd_metadata_count counts;
    TestModel m;

    memset(&counts, '\0', sizeof(counts));
    counts.num_signals = num_sigs;
    counts.num_beds = num_beds;
    counts.num_objects = num_objs;
    counts.num_presentations = num_pres;

    if (dlb_pmd_generate_random(m, &counts, seed, 0, 0))
    {
        ADD_FAILURE() << "Could not generate random model";
    }
    else
    {
        dlb_pmd_success ok = dlb_pmd_set_profile(m, profile, level);
        dlb_pmd_success expected = 0;
        
        if (   (profile != 1)
            || (level > 3)
            || (           num_sigs > levels1_[level-1].max_signals)
            || (num_beds + num_objs > levels1_[level-1].max_elements)
            || (           num_pres > levels1_[level-1].max_presentations)
           )
        {
            expected = 1;
        }

        if (ok != expected)
        {
            if (expected)
            {
                ADD_FAILURE() << "expected to fail, instead succeeded";
            }
            else
            {
                ADD_FAILURE() << "expected to succeed, but failed";
            }
        }
    }
}


TEST_P(ProfileTest, profile_level_before)
{
    /* num_beds * num_bed_types * num_objs */
    /* how to randomly generate presets? only 16 of these
     * 6 beds, 64 dyn objects
     */

    unsigned int profile  = std::tr1::get<0>(GetParam());
    unsigned int level    = std::tr1::get<1>(GetParam());
    unsigned int num_sigs = std::tr1::get<2>(GetParam());
    unsigned int num_els  = std::tr1::get<3>(GetParam());
    unsigned int num_pres = std::tr1::get<4>(GetParam());

    unsigned int num_beds = num_els / 3;
    unsigned int num_objs = num_els - num_beds;

    unsigned int seed = 0x12345678 + profile + level + num_sigs + num_beds + num_objs + num_pres;

    dlb_pmd_metadata_count counts;
    dlb_pmd_success expected = 0;
    dlb_pmd_success ok;
    TestModel m;

    memset(&counts, '\0', sizeof(counts));
    counts.num_signals = num_sigs;
    counts.num_beds = num_beds;
    counts.num_objects = num_objs;
    counts.num_presentations = num_pres;

    if (   (profile != 1)
        || (level > 3)
        || (           num_sigs > levels1_[level-1].max_signals)
        || (num_beds + num_objs > levels1_[level-1].max_elements)
        || (           num_pres > levels1_[level-1].max_presentations)
           )
    {
        expected = 1;
    }


    ok   = dlb_pmd_set_profile(m, profile, level)
        || dlb_pmd_generate_random(m, &counts, seed, 0, 0);
    

    if (ok != expected)
    {
        if (expected)
        {
            ADD_FAILURE() << "expected to fail, instead succeeded";
        }
        else
        {
            ADD_FAILURE() << "expected to succeed, but failed";
        }
    }
}


INSTANTIATE_TEST_CASE_P(PMD_Profile, ProfileTest,
                        testing::Combine(testing::Range(1, 3),   /* profiles 1, 2 */
                                         testing::Range(1, 4),   /* levels 1, 3 */
                                         testing::Range(1, 18),  /* num signals */
                                         testing::Range(1, 52),  /* num elements */
                                         testing::Range(1, 50)   /* num presentations */
                                         ));
#endif






