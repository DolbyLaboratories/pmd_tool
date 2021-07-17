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
 * @file Test_HED.cc
 * @brief Test Headphone Element Description payload transmits correctly
 */

#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

#include "PrngKiss.hh"
#include "TestModel.hh"

#include "dlb_pmd_api.h"    
#include "src/model/pmd_model.h"

#include "gtest/gtest.h"

// Uncomment the next line to remove the tests in this file from the run:
//#define DISABLE_HED_TESTS

/**
 * @brief number of channels in each different speaker config
 */
static const unsigned int BED_SIZES[ NUM_PMD_SPEAKER_CONFIGS ] =
{
    2,  /* 2.0 */
    3,  /* 3.0 */
    6,  /* 5.1 */
    8,  /* 5.1.2 */
    10, /* 5.1.4 */
    12, /* 7.1.4 */
    16, /* 9.1.6 */
    2,  /* portable speaker */
    2   /* portable headphones */
};

#ifndef DISABLE_HED_TESTS
class PayloadTest: public ::testing::TestWithParam<std::tr1::tuple<int, int> > {};

TEST_P(PayloadTest, payload_testing)
{
    unsigned int options = std::tr1::get<0>(GetParam());
    TestModel::TestType t = TestModel::TEST_TYPES[std::tr1::get<1>(GetParam())];

    TestModel m;
    dlb_pmd_speaker_config cfg = (dlb_pmd_speaker_config)(options % NUM_PMD_SPEAKER_CONFIGS);
    dlb_pmd_element_id bed = m.new_elid();
    dlb_pmd_element_id obj = m.new_elid();
    dlb_pmd_presentation_id pres = m.new_presid();
    unsigned int bed_signals;
    dlb_pmd_headphone bedhed;
    dlb_pmd_headphone objhed;
    PrngKiss prng;
    
    prng.seed(options);        
    bed_signals = BED_SIZES[cfg];
    bedhed.audio_element_id = bed;
    bedhed.head_tracking_enabled = (options & 3) != 0;
    bedhed.render_mode = options;
    bedhed.channel_mask = prng.next() & 0xffff;
    objhed.audio_element_id = obj;
    objhed.head_tracking_enabled = (options & 8) != 0;
    objhed.render_mode = 127 - options;
    objhed.channel_mask = prng.next() & 0xffff;
    
    if (   dlb_pmd_add_signals(m, bed_signals+1)
        || dlb_pmd_set_title(m, "headphone element descriptions testing")
        || dlb_pmd_add_bed(m, bed, NULL, cfg, 1, 0)
        || dlb_pmd_add_generic_obj2(m, obj, NULL, bed_signals+1, 0.0f, 0.0f, 0.0f)
        || dlb_pmd_add_presentation2(m, pres, "eng", "TESTPREZ", "eng", cfg, 2, bed, obj)
        || dlb_pmd_set_headphone_element(m, &bedhed)
        || dlb_pmd_set_headphone_element(m, &objhed)
       )
    {
        ADD_FAILURE() << "Failed to create test model: " << dlb_pmd_error(m);
    }
    else
    {
        char tmp[32];
        snprintf(tmp, sizeof(tmp), "HED_Payload_%u", options);
        m.skip_pcm_samples(options*7);

        try
        {
            m.test(t, tmp, options);
        }
        catch (TestModel::failure& f)
        {
            ADD_FAILURE() << f.msg;
        }
    }
}

INSTANTIATE_TEST_CASE_P(PMD_HED, PayloadTest,
                        testing::Combine(testing::Range(0, 128),
                                         testing::Range(TestModel::FIRST_TEST_TYPE,
                                                        TestModel::LAST_TEST_TYPE+1)));
#endif


