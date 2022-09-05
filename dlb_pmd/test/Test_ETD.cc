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
 * @file Test_ETD.cc
 * @brief Test ED2 Turnaround descriptor payload transmits correctly
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
//#define DISABLE_ETD_TESTS


class ETD_PayloadTest: public ::testing::TestWithParam<std::tr1::tuple<int, int, int, int> > {};

#define NUM_PMD_LOW_FRAMERATES (5)

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


static inline
dlb_pmd_success
add_ed2_turnarounds
    (TestModel& m
    ,unsigned int id
    ,unsigned int p
    ,unsigned int options
    ,unsigned int e
    )
{
    unsigned int eepid;
    unsigned int i;
    (void)ISO_639_2t_CODES;

    if (options & 1)
    {
        /* ED2 present */
        if (dlb_pmd_etd_add_ed2(m, id, (dlb_pmd_frame_rate)(e%NUM_PMD_LOW_FRAMERATES))) return 1;
        for (i = 0; i != p; ++i)
        {
            const char *test_lang = (options & 4)
                ? ISO_639_1_CODES[(p*e) % NUM_ISO_639_1_CODES]
                : ISO_639_2b_CODES[(p*e) % NUM_ISO_639_2b_CODES];
            dlb_pmd_speaker_config sc = 
                (dlb_pmd_speaker_config)((e+p)%NUM_PMD_SPEAKER_CONFIGS);
            
            dlb_pmd_presentation_id presid = m.new_presid();
            
            if (dlb_pmd_add_presentation2(m, presid, test_lang, "TEST_PREZ", "eng",
                                          sc, 1, 1)) return 1;
            if (generate_eac3_encoding_parameters(m, options, e, 0, &eepid)) return 1;
            if (dlb_pmd_etd_add_ed2_presentation(m, id, presid, eepid)) return 1;
        }
    }
    return 0;
}


static inline
dlb_pmd_success
add_de_turnarounds
    (TestModel& m
    ,unsigned int id
    ,unsigned int p
    ,unsigned int options
    ,unsigned int e
    )
{
    unsigned int eepid;
    unsigned int i;

    if (options & 2)
    {
        /* DE present */
        if (dlb_pmd_etd_add_de(m, id, (dlb_pmd_frame_rate)(e%NUM_PMD_LOW_FRAMERATES),
                               (dlb_pmd_de_program_config)(e%24))) return 1;
        for (i = 0; i != p; ++i)
        {
            const char *test_lang = (options & 4)
                ? ISO_639_1_CODES[(p*e) % NUM_ISO_639_1_CODES]
                : ISO_639_2b_CODES[(p*e) % NUM_ISO_639_2b_CODES];
            dlb_pmd_speaker_config sc = 
                (dlb_pmd_speaker_config)((e+p)%NUM_PMD_SPEAKER_CONFIGS);
            
            dlb_pmd_presentation_id presid = m.new_presid();
            if (dlb_pmd_add_presentation2(m, presid, test_lang,
                                          "TESTPREZ", "eng", sc, 1, 1)) return 1;
            if (generate_eac3_encoding_parameters(m, options, e, 0, &eepid)) return 1;
            if (dlb_pmd_etd_add_de_presentation(m, id, presid, eepid)) return 1;
        }
    }
    return 0;
}


#ifndef DISABLE_ETD_TESTS
TEST_P(ETD_PayloadTest, payload_testing)
{
    unsigned int p = std::tr1::get<0>(GetParam());
    unsigned int options = std::tr1::get<1>(GetParam());
    unsigned int e = std::tr1::get<2>(GetParam());
    TestModel::TestType t = TestModel::TEST_TYPES[std::tr1::get<3>(GetParam())];
    TestModel m;
    char tmp[128];    

    unsigned int id = m.new_etdid();
    dlb_pmd_element_id ignore;
    if (   m.populate(&ignore)
        || dlb_pmd_set_title(m, "ED2 turnaround testing")
        || dlb_pmd_add_etd(m, id)
        || add_ed2_turnarounds(m, id, p, options, e)
        || add_de_turnarounds(m, id, p, options, e)
       )
    {
        ADD_FAILURE() << "Could not build model: " << dlb_pmd_error(m);
    }
    else
    {
        snprintf(tmp, sizeof(tmp),
                 "ETD_%s%s_values%d_presentations%d",
                 (options & 1) ? "ED2_" : "",
                 (options & 2) ? "DE_" : "",
                 e, p);
        m.skip_pcm_samples(p * options * e);
        try
        {
            m.test(t, tmp, p+options);
        }
        catch (TestModel::failure& f)
        {
            ADD_FAILURE() << f.msg;
        }
    }
}


INSTANTIATE_TEST_CASE_P(PMD_ETD, ETD_PayloadTest,
                        testing::Combine(testing::Range(0, PMD_ETD_MAX_PRESENTATIONS), // p
                                         testing::Range(0, 4), // options
                                         testing::Range(0, 24),// e
                                         testing::Range(TestModel::FIRST_TEST_TYPE,
                                                        TestModel::LAST_TEST_TYPE+1)));
#endif
