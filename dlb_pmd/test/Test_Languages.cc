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

