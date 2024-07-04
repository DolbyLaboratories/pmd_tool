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
 * @file Test_PLD.cc
 * @brief Test Presentation Loudness payload transmits correctly
 */

#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

#include "dlb_pmd_api.h"    
#include "src/model/pmd_model.h"

#include "PrngKiss.hh"
#include "TestModel.hh"
#include "gtest/gtest.h"

// Uncomment the next line to remove the tests in this file from the run:
//#define DISABLE_PLD_TESTS


#define LUFS(i) ((dlb_pmd_lufs)(((float)(((i) & 0x3ff)) - 1024.0f) / 10.0f))
#define LRA(i) ((dlb_pmd_lu)(((i) & 0x3ff) * 0.1f))

#ifndef DISABLE_PLD_TESTS
class PLD_FloatTest: public ::testing::TestWithParam<std::tr1::tuple<int, int> > {};
class PLD_OptionsTest: public ::testing::TestWithParam<std::tr1::tuple<int, int, int> > {};

/* test PLD floating point values */
TEST_P(PLD_FloatTest, float_testing)
{
    unsigned int i = std::tr1::get<0>(GetParam());
    TestModel::TestType t = TestModel::TEST_TYPES[std::tr1::get<1>(GetParam())];

    dlb_pmd_element_id ignore;
    dlb_pmd_loudness loudness;
    TestModel m;
    
    memset(&loudness, '\0', sizeof(loudness));
    loudness.presid = 1;
    loudness.loud_prac_type = (dlb_pmd_loudness_practice)1;
    loudness.loudcorr_gating = (dlb_pmd_dialgate_practice)1;
    loudness.loudcorr_type = (dlb_pmd_correction_type)1;
    loudness.loudspch_gating = (dlb_pmd_dialgate_practice)1;
    
    /* enable all floating-pt lufs/lu values */
    loudness.b_loudcorr_gating = 1;
    loudness.b_prgmbndy = 0;
    loudness.b_prgmbndy_offset = 0;
    loudness.b_loudrelgat = 1;
    loudness.b_loudspchgat = 1;
    loudness.b_loudstrm3s = 1;
    loudness.b_max_loudstrm3s = 1;
    loudness.b_truepk = 1;
    loudness.b_max_truepk = 1;
    loudness.b_lra = 1;
    loudness.b_loudmntry = 1;
    loudness.b_max_loudmntry = 1;

    loudness.loudrelgat     = LUFS(i);
    loudness.loudspchgat    = LUFS(i+1);
    loudness.loudstrm3s     = LUFS(i+2);
    loudness.max_loudstrm3s = LUFS(i+3);
    loudness.truepk         = LUFS(i+4);
    loudness.max_truepk     = LUFS(i+5);
    loudness.lra            = LRA(i+6);
    loudness.loudmntry      = LUFS(i+7);
    loudness.max_loudmntry  = LUFS(i+8);

    if (   m.populate(&ignore)
        || dlb_pmd_set_title(m, "loudness values testing")
        || dlb_pmd_set_loudness(m, &loudness))
    {
        ADD_FAILURE() << "Failed to populate test model: " << dlb_pmd_error(m);
    }
    else
    {
        try
        {
            m.test(t, "Loudness_floating_point_test", i);
        }
        catch (TestModel::failure& f)
        {
            ADD_FAILURE() << f.msg;
        }
    }
}

    
/* test actual structure of PLD payload, in all its combinations:
 * optional data:
 *   - loudness correction type  1
 *   - loudness correction dialgate 2
 *   - loudrelgat 4
 *   - loudspchgat 8
 *   - loudsrtrm3s 16
 *   - max loudsrtrm3s 32
 *   - truepk 64
 *   - max truepk 128
 *   - prgmbndy 256
 *   - prgmbndy offset 512
 *   - lra 1024
 *   - loudmntry 2048
 *   - max loudmntr 4096
 *   - extension 8192
 */
static void populate_loudness_payload(dlb_pmd_loudness *pld, unsigned int options, unsigned int e)
{
    static const unsigned int NUM_VALID_PRAC_TYPES =
        (PMD_PLD_LOUDNESS_PRACTICE_CONSUMER_LEVELLER + 1) - (PMD_PLD_LOUDNESS_PRACTICE_RESERVED_13 - PMD_PLD_LOUDNESS_PRACTICE_RESERVED_05 + 1);

    PrngKiss prng;
    prng.seed(options * e);
    
    memset(pld, '\0', sizeof(*pld));
    if (options & PMD_PLD_OPT_PRGMBNDY_OFFSET)
    {
        options |= PMD_PLD_OPT_PRGMBNDY;
    }
    
    pld->presid = 1;
    pld->loud_prac_type = (dlb_pmd_loudness_practice)(e%NUM_VALID_PRAC_TYPES);
    if (pld->loud_prac_type == PMD_PLD_LOUDNESS_PRACTICE_RESERVED_05)
    {
        pld->loud_prac_type = PMD_PLD_LOUDNESS_PRACTICE_MANUAL;
    }
    else if (pld->loud_prac_type == PMD_PLD_LOUDNESS_PRACTICE_RESERVED_06)
    {
        pld->loud_prac_type = PMD_PLD_LOUDNESS_PRACTICE_CONSUMER_LEVELLER;
    }

    if (pld->loud_prac_type != PMD_PLD_LOUDNESS_PRACTICE_NOT_INDICATED)
    {
        if (options & PMD_PLD_OPT_LOUDCORR_DIALGATE)
        {
            pld->b_loudcorr_gating = 1;
            pld->loudcorr_gating = (dlb_pmd_dialgate_practice)((e+1)%4);
            pld->loudcorr_type = (dlb_pmd_correction_type)((e+2)%2);
        }
    }
    
    if (options & PMD_PLD_OPT_LOUDRELGAT)
    {
        pld->b_loudrelgat = 1;
        pld->loudrelgat = LUFS(prng.next());
    }
    if (options & PMD_PLD_OPT_LOUDSPCHGAT)
    {
        pld->b_loudspchgat = 1;
        pld->loudspchgat = LUFS(prng.next());
        pld->loudspch_gating = (dlb_pmd_dialgate_practice)((e+2)%4);
    }
    if (options & PMD_PLD_OPT_LOUDSTRM3S)
    {
        pld->b_loudstrm3s = 1;
        pld->loudstrm3s = LUFS(prng.next());
    }
    if (options & PMD_PLD_OPT_MAX_LOUDSTRM3S)
    {
        pld->b_max_loudstrm3s = 1;
        pld->max_loudstrm3s = LUFS(prng.next());
    }
    if (options & PMD_PLD_OPT_TRUEPK)
    {
        pld->b_truepk = 1;
        pld->truepk = LUFS(prng.next());
    }
    if (options & PMD_PLD_OPT_MAX_TRUEPK)
    {
        pld->b_max_truepk = 1;
        pld->max_truepk = LUFS(prng.next());
    }
    if (options & PMD_PLD_OPT_PRGMBNDY)
    {
        pld->b_prgmbndy = 1;
        pld->prgmbndy = ((prng.next() % 9)+1) * (e%2?1:-1);
    }
    if (options & PMD_PLD_OPT_PRGMBNDY_OFFSET)
    {
        pld->b_prgmbndy_offset = 1;
        pld->prgmbndy_offset = prng.next() & 0x7ff;
    }
    if (options & PMD_PLD_OPT_LRA)
    {
        pld->b_lra = 1;
        pld->lra = LRA(prng.next());
        pld->lra_prac_type = (dlb_pmd_loudness_range_practice)(e%2);
    }
    if (options & PMD_PLD_OPT_LOUDMNTRY)
    {
        pld->b_loudmntry = 1;
        pld->loudmntry = LUFS(prng.next());
    }
    if (options & PMD_PLD_OPT_MAX_LOUDMNTRY)
    {
        pld->b_max_loudmntry = 1;
        pld->max_loudmntry = LUFS(prng.next());
    }
    if (options & PMD_PLD_OPT_EXTENSION)
    {
        /* xml reader can only read bytes */
        pld->extension.size
            = prng.next() & ((8 * sizeof(pld->extension.data)) - 1);
        
        if (e & 1)
        {
            /* test muliples of 8 too */
            pld->extension.size = pld->extension.size & ~0x7;
        }
        for (unsigned int i = 0; i != pld->extension.size/8; ++i)
        {
            pld->extension.data[i] = prng.next() & 0xff;
        }
    }
}


TEST_P(PLD_OptionsTest, payload_tests)
{
    unsigned int options = std::tr1::get<0>(GetParam());
    unsigned int e = std::tr1::get<1>(GetParam());
    TestModel::TestType t = TestModel::TEST_TYPES[std::tr1::get<2>(GetParam())];
    dlb_pmd_element_id ignore;
    dlb_pmd_loudness loudness;
    TestModel m;

    populate_loudness_payload(&loudness, options, e);

    if (   m.populate(&ignore)
        || dlb_pmd_set_title(m, "loudness testing")
        || dlb_pmd_set_loudness(m, &loudness))
    {
        ADD_FAILURE() << "Could not populate model";
    }
    else
    {
        char tmp[64];
        snprintf(tmp, sizeof(tmp), "PLD_options%u_e%u", options, e);
        m.skip_pcm_samples(options * e);
        try
        {
            m.test(t, tmp, e);
        }
        catch (TestModel::failure& f)
        {
            ADD_FAILURE() << f.msg;
        }
    }
}


INSTANTIATE_TEST_CASE_P(PMD_PLD, PLD_FloatTest,
                        testing::Combine(testing::Range(0, 2048),
                                         testing::Range(TestModel::FIRST_TEST_TYPE,
                                                        TestModel::LAST_TEST_TYPE+1)));

#ifdef REALLY_LONG_TEST_OK
INSTANTIATE_TEST_CASE_P(PMD_PLD, PLD_OptionsTest,
                        testing::Combine(testing::Range(0, 1<<14),  // TODO: 8G memory allocation...
                                         testing::Range(0, 8),
                                         testing::Range(TestModel::FIRST_TEST_TYPE,
                                                        TestModel::LAST_TEST_TYPE+1)));
#endif
#endif
