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
 * @file Test_Versions.cc
 * @brief version number testing
 *
 * Test that we can set the version numbers correctly in PMD.  Note
 * that in normal use, the version numbers are hard-coded by the
 * version of the dlb_pmd_lib library.
 *
 * This test breaks the rules to make sure that version testing is
 * correct.
 */

#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

extern "C"
{
#include "dlb_pmd_api.h"    

extern dlb_pmd_bool global_testing_version_numbers;
}

#include "TestModel.hh"
#include "gtest/gtest.h"

// Uncomment the next line to remove the tests in this file from the run:
//#define DISABLE_VERSIONS_TESTS


#ifndef DISABLE_VERSIONS_TESTS
class VersionTest: public ::testing::TestWithParam<std::tr1::tuple<int, int> >
{
public:

    static void SetUpTestCase()
    {
        global_testing_version_numbers = 1;
    }
    
    static void TearDownTestCase()
    {
        global_testing_version_numbers = 0;
    }
    
};


TEST_P(VersionTest, version_numbers)
{
    /**
     * hidden reference to a global variable used to determine whether
     * or not we're checking version numbers.
     *
     * This is currently defined in the pmd_xml_reader.c file.
     */

    unsigned int ii = std::tr1::get<0>(GetParam());
    TestModel::TestType t = TestModel::TEST_TYPES[std::tr1::get<1>(GetParam())];
    dlb_pmd_element_id bed;
    dlb_pmd_presentation_id pres;
    TestModel m;

    bed = m.new_elid();
    pres = m.new_presid();

    uint8_t i = (uint8_t)ii;
    m.illegal_set_version(i, 255-i);

    if (   dlb_pmd_add_signals(m, 2)
        || dlb_pmd_set_title(m, "version testing")
        || dlb_pmd_add_bed(m, bed, NULL, DLB_PMD_SPEAKER_CONFIG_2_0, 1, 0)
        || dlb_pmd_add_presentation(m, pres, "eng", "TESTPREZ", "eng",
                                    DLB_PMD_SPEAKER_CONFIG_2_0, 1, &bed)
       )
    {
        ADD_FAILURE() << "ERROR: could not build model: " << dlb_pmd_error(m);
    }
    else 
    {
        m.skip_pcm_samples(i);

        try
        {
            m.test(t, "version", i);
        }
        catch (TestModel::failure& f)
        {
            ADD_FAILURE() << f.msg;
        }
    }
}



INSTANTIATE_TEST_CASE_P(PMD, VersionTest,
           testing::Combine(testing::Range(0, 256),
                            testing::Range(TestModel::FIRST_TEST_TYPE,
                                           TestModel::LAST_TEST_TYPE+1)));
#endif
