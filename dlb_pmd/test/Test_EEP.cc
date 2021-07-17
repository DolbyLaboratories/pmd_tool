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
 * @file Test_EEP.cc
 * @brief Test EAC3 Encoding Parameters payload transmits correctly
 */

#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

#include "dlb_pmd_api.h"    

extern "C"
{
#include "src/model/pmd_model.h"
#include "pmd_test_langs.h"
}

#include "TestModel.hh"
#include "gtest/gtest.h"

// Uncomment the next line to remove the tests in this file from the run:
//#define DISABLE_EEP_TESTS

static const int EEP_MAX_PRESENTATIONS = 8;

static
dlb_pmd_dialnorm
make_dialnorm
    (unsigned int enumval
    )
{
    unsigned int dn = (enumval % DLB_PMD_MAX_DIALNORM) + DLB_PMD_MIN_DIALNORM;  // This works with current values...
    return (dlb_pmd_dialnorm)dn;
}

static
dlb_pmd_success
generate_eac3_encoding_parameters
    (TestModel& m
    ,unsigned int options
    ,unsigned int enumval
    ,unsigned int num_presentations
    ,unsigned int *eepid
    )
{
    unsigned int i;
    unsigned int id = m.new_eac3id();
    dlb_pmd_presentation_id pid;

    if (static_cast<int>(num_presentations) == EEP_MAX_PRESENTATIONS)
    {
        num_presentations = static_cast<unsigned int>(PMD_EEP_MAX_PRESENTATIONS);
    }
    
    *eepid = id;
    if (dlb_pmd_add_eac3_encoding_parameters(m, id)) return 1;
    if (options & 1)
    {
        if (dlb_pmd_eep_add_encoder_params(m, id,
                                           (dlb_pmd_compr)(enumval % PMD_NUM_COMPR),
                                           (dlb_pmd_compr)(enumval % PMD_NUM_COMPR),
                                           (dlb_pmd_bool)(enumval & 1),
                                           (unsigned char)(enumval % 32)))
        {
            return 1;
        }
    }
    if (options & 2)
    {
        if (dlb_pmd_eep_add_bitstream_params(m, id,
                                             (dlb_pmd_bsmod)(enumval % PMD_NUM_BSMOD),
                                             (dlb_pmd_surmod)(enumval % PMD_NUM_DSURMOD),
                                             make_dialnorm(enumval),
                                             (dlb_pmd_prefdmix)(enumval % PMD_NUM_PREFDMIX),
                                             (dlb_pmd_cmixlev)(enumval % PMD_NUM_CMIX_LEVEL),
                                             (dlb_pmd_surmixlev)(enumval % PMD_NUM_SURMIX_LEVEL),
                                             (dlb_pmd_cmixlev)(enumval % PMD_NUM_CMIX_LEVEL),
                                             (dlb_pmd_surmixlev)(enumval % PMD_NUM_SURMIX_LEVEL)))
        {
            return 1;
        }
    }
    if (options & 4)
    {
        if (dlb_pmd_eep_add_drc_params(m, id,
                                       (dlb_pmd_compr)(enumval % PMD_NUM_COMPR),
                                       (dlb_pmd_compr)((enumval+1) % PMD_NUM_COMPR),
                                       (dlb_pmd_compr)((enumval+2) % PMD_NUM_COMPR),
                                       (dlb_pmd_compr)((enumval+3) % PMD_NUM_COMPR),
                                       (dlb_pmd_compr)((enumval+4) % PMD_NUM_COMPR)))
        {
            return 1;
        }
    }
    
    /* now add presentations */
    for (i = 0; i != num_presentations; ++i)
    {
        if (i < 4)
        {
            /* use one of the presentations already added */
            if (dlb_pmd_eep_add_presentation(m, id, i+1))
            {
                return 1;
            }
        }
        else
        {
            const char *test_lang = (options & 4)
                ? ISO_639_1_CODES[(num_presentations*enumval) % NUM_ISO_639_1_CODES]
                : ISO_639_2b_CODES[(num_presentations*enumval) % NUM_ISO_639_2b_CODES];
            pid = m.new_presid();
            if (dlb_pmd_add_presentation2(m,
                                          pid,
                                          test_lang,
                                          "TEST_EEP_PREZ",
                                          "eng",
                                          DLB_PMD_SPEAKER_CONFIG_5_1,
                                          1,
                                          1)
                || dlb_pmd_eep_add_presentation(m, id, pid))
            {
                return 1;
            }
        }
    }
    return 0;
}


#ifndef DISABLE_EEP_TESTS
class EEP_PayloadTest: public ::testing::TestWithParam<std::tr1::tuple<int, int, int, int> > {};

TEST_P(EEP_PayloadTest, payload_testing)
{
    unsigned int p = std::tr1::get<0>(GetParam());
    unsigned int options = std::tr1::get<1>(GetParam());
    unsigned int e = std::tr1::get<2>(GetParam());
    TestModel::TestType t = TestModel::TEST_TYPES[std::tr1::get<3>(GetParam())];
    TestModel m;

    dlb_pmd_element_id ignore;
    unsigned int ignore2;
    char tmp[128];

    (void)ISO_639_2t_CODES;

    snprintf(tmp, sizeof(tmp),
             "EEP_%s%s%s_values%d_presentations%d",
             (options & 1) ? "Encoder_" : "",
             (options & 2) ? "Bitstream_" : "",
             (options & 4) ? "DRC_" : "",
             e, p);

    if (   m.populate(&ignore)
        || dlb_pmd_set_title(m, "eac3 encoding parameters testing")
        || generate_eac3_encoding_parameters(m, options, e, p, &ignore2)
       )
    {
        ADD_FAILURE() << "Could not build model " << tmp << ": " << dlb_pmd_error(m);
    }
    else
    {
        m.skip_pcm_samples(p * options * e);
        try
        {
            m.test(t, tmp, options+e);
        }
        catch (TestModel::failure& f)
        {
            ADD_FAILURE() << f.msg;
        }
    }
}


INSTANTIATE_TEST_CASE_P(PMD_EEP, EEP_PayloadTest,
                        testing::Combine(testing::Range(0, EEP_MAX_PRESENTATIONS), // p
                                         testing::Range(0, 8), // options
                                         testing::Range(0, 32),// e
                                         testing::Range(TestModel::FIRST_TEST_TYPE,
                                                        TestModel::LAST_TEST_TYPE+1)));
#endif
