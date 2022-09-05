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
 * @file Test_Languages.cc
 * @brief language encoding tests
 *
 * Check that we can encode and decode PMD language codes properly
 */

#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

#include "dlb_pmd_api.h"    

extern "C"
{
#include "pmd_test_langs.h"
}

#include "TestModel.hh"
#include "gtest/gtest.h"

// Uncomment the next line to remove the tests in this file from the run:
//#define DISABLE_LANGUAGES_TESTS

#ifndef DISABLE_LANGUAGES_TESTS
class Iso639_1_Test: public ::testing::TestWithParam<std::tr1::tuple<int, int> > {};
class Iso639_2b_Test: public ::testing::TestWithParam<std::tr1::tuple<int, int> > {};
class Iso639_2t_Test: public ::testing::TestWithParam<std::tr1::tuple<int, int> > {};
class Iso639_Illegal_Test: public ::testing::TestWithParam<std::tr1::tuple<int, int> > {};

TEST_P(Iso639_1_Test, iso639_1)
{
    unsigned int i = std::tr1::get<0>(GetParam());
    TestModel::TestType t = TestModel::TEST_TYPES[std::tr1::get<1>(GetParam())];

    dlb_pmd_element_id obj;
    dlb_pmd_presentation_id p;
    TestModel m;

    p = m.new_presid();
    if (   m.populate(&obj)
        || dlb_pmd_set_title(m, "presentation ISO 639-1 language code testing")
        || dlb_pmd_add_presentation2(m, p, ISO_639_1_CODES[i],
                                     "TESTPREZ", "eng",
                                     DLB_PMD_SPEAKER_CONFIG_5_1, 1, 1))
    {
        ADD_FAILURE() << "Could not populate model: " << dlb_pmd_error(m);
    }
    else
    {
        try
        {
            m.test(t, "ISO_639_1_language_code", i);
        }
        catch (TestModel::failure& f)
        {
            ADD_FAILURE() << f.msg;
        }
    }
}


TEST_P(Iso639_2b_Test, iso639_2b)
{
    unsigned int i = std::tr1::get<0>(GetParam());
    TestModel::TestType t = TestModel::TEST_TYPES[std::tr1::get<1>(GetParam())];

    dlb_pmd_element_id obj;
    dlb_pmd_presentation_id p;
    TestModel m;

    p = m.new_presid();
    if (   m.populate(&obj)
        || dlb_pmd_set_title(m, "presentation ISO 639-2/B language code testing")
        || dlb_pmd_add_presentation2(m, p, ISO_639_2b_CODES[i],
                                     "TESTPREZ", "eng",
                                     DLB_PMD_SPEAKER_CONFIG_5_1, 1, 1))
    {
        ADD_FAILURE() << "Could not populate model: " << dlb_pmd_error(m);
    }
    else
    {
        try
        {
            m.test(t, "ISO_639_2b_language_code", i);
        }
        catch (TestModel::failure& f)
        {
            ADD_FAILURE() << f.msg;
        }
    }
}


TEST_P(Iso639_2t_Test, iso639_2t)
{
    unsigned int i = std::tr1::get<0>(GetParam());
    TestModel::TestType t = TestModel::TEST_TYPES[std::tr1::get<1>(GetParam())];

    dlb_pmd_element_id obj;
    dlb_pmd_presentation_id p;
    TestModel m;

    p = m.new_presid();
    if (   m.populate(&obj)
        || dlb_pmd_set_title(m, "presentation ISO 639-2/T language code testing")
        || dlb_pmd_add_presentation2(m, p, ISO_639_2t_CODES[i],
                                     "TESTPREZ", "eng",
                                     DLB_PMD_SPEAKER_CONFIG_5_1, 1, 1))
    {
        ADD_FAILURE() << "Could not populate model: " << dlb_pmd_error(m);
    }
    else
    {
        try
        {
            m.test(t, "ISO_639_2t_language_code", i);
        }
        catch (TestModel::failure& f)
        {
            ADD_FAILURE() << f.msg;
        }
    }
}


TEST_P(Iso639_Illegal_Test, bad_languages)
{
    unsigned int i = std::tr1::get<0>(GetParam());
    dlb_pmd_element_id obj;
    dlb_pmd_presentation_id p;
    TestModel m;

    p = m.new_presid();
    if (   m.populate(&obj)
        || dlb_pmd_set_title(m, "presentation illegal language code testing")
       )
    {
        ADD_FAILURE() << "Could not populate model " << dlb_pmd_error(m);
    }
    
    const char *illegal_codes[] = { NULL, "", "x", "1234", "xy", "xyz" };
    assert(i < sizeof(illegal_codes)/sizeof(illegal_codes[0]));
    
    if (!dlb_pmd_add_presentation2(m, p, illegal_codes[i],
                                   "TESTPREZ", "eng", DLB_PMD_SPEAKER_CONFIG_5_1, 1, 1))
    {
        if (NULL == illegal_codes[i])
        {
            ADD_FAILURE() << "Should not have been able to use NULL language code";
        }
        else
        {
            ADD_FAILURE() << "Should not have been able to use illegal code \""
                          << illegal_codes[i]
                          << "\"";
        }
    }
}


INSTANTIATE_TEST_CASE_P(PMD_Language, Iso639_1_Test,
           testing::Combine(testing::Range(0, (int)NUM_ISO_639_1_CODES),
                            testing::Range(TestModel::FIRST_TEST_TYPE,
                                           TestModel::LAST_TEST_TYPE+1)));

INSTANTIATE_TEST_CASE_P(PMD_Language, Iso639_2b_Test,
           testing::Combine(testing::Range(0, (int)NUM_ISO_639_2b_CODES),
                            testing::Range(TestModel::FIRST_TEST_TYPE,
                                           TestModel::LAST_TEST_TYPE+1)));

INSTANTIATE_TEST_CASE_P(PMD_Language, Iso639_2t_Test,
           testing::Combine(testing::Range(0, (int)NUM_ISO_639_2t_CODES),
                            testing::Range(TestModel::FIRST_TEST_TYPE,
                                           TestModel::LAST_TEST_TYPE+1)));

INSTANTIATE_TEST_CASE_P(PMD_Language, Iso639_Illegal_Test,
           testing::Combine(testing::Range(0, 6),
                            testing::Range(TestModel::FIRST_TEST_TYPE, 
                                           TestModel::FIRST_TEST_TYPE+1)));
#endif

