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
 * @file Test_API.cc
 * @brief Test PMD API functions
 *
 * @todo: flesh this out more comprehensively
 */

#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

#include "dlb_pmd_api.h"    
#include "src/model/pmd_model.h"

#include "PrngKiss.hh"
#include "TestModel.hh"
#include "gtest/gtest.h"

#include <math.h>

#if defined(_MSC_VER)
#include <intrin.h>
#endif

// Uncomment the next line to remove the tests in this file from the run:
//#define DISABLE_API_TESTS

class PMD_APITest: public ::testing::TestWithParam<std::tr1::tuple<int, int> > {};
class PMD_SignalAPITest: public PMD_APITest
{
  protected:
    uint32_t binomial_coefficients_[33][33];
    PrngKiss prng_;

    /* generate table of binomial_coefficients_ using Pascal's Triangle */
    void SetUp() 
    {
        int i;
        int j;

        memset(binomial_coefficients_, 0, sizeof(binomial_coefficients_));
        for (i = 0; i < 33; ++i)
        {
            binomial_coefficients_[i][0] = 1;
            binomial_coefficients_[i][i] = 1;
        }

        for (i = 1; i < 33; ++i)
        {
            binomial_coefficients_[i][0] = 1;
            binomial_coefficients_[i][i] = 1;
            for (j = 1; j < 32; ++j)
            {
                binomial_coefficients_[i][j] =
                    binomial_coefficients_[i-1][j-1] +
                    binomial_coefficients_[i-1][j];
            }
        }
    }


    /** @return the number of 1 bits */
    unsigned int pop_count_(uint32_t a)
    {
#if defined(__GNUC__)
        return (unsigned int) __builtin_popcount(a);
#elif defined (_MSC_VER)
        return __popcnt(a);
#else
        /* default implementation is lifted from the intrinsics DLB_Uones32 function */
        a = a - ((a >> 1u) & 0x55555555);
        a = (((a >> 2u) & 0x33333333) + (a & 0x33333333));
        a = (((a >> 4u) + a) & 0x0f0f0f0f);
        a += (a >> 8u);
        a += (a >> 16u);
        return a & 0x3f;
#endif
    }


    /**
     * @brief generate a random word containing #num_bits bits.
     *
     * This is based on the idea of 'enumerative coding', in which you
     * order all the N-bit words containing exactly k-bits set in
     * ascending order.  The enumerative coding of the N-bit word is
     * just the position in that ascending order of that word.
     */ 
    uint32_t random_bitset_(unsigned total_bits, unsigned int num_bits)
    {
        assert(num_bits <= total_bits);
        assert(total_bits <= 32);
        if (num_bits == 32)
        {
            return ~0u;
        }
        else if (num_bits == total_bits)
        {
            return (1u << total_bits)-1;
        }
        else if (num_bits == 0)
        {
            return 0;
        }
        else
        {
            uint32_t limit = binomial_coefficients_[total_bits][num_bits]-1;
            uint32_t random = prng_.next() % limit;
            uint32_t result = 0;

            int count = num_bits;
        
            for (uint32_t bit = total_bits-1; count > 0; --bit)
            {
                uint32_t bc = binomial_coefficients_[bit][num_bits];
                if (random >= bc)
                {
                    random -= bc;
                    result |= 1u << bit;
                    count -= 1;
                }
            }
            assert(pop_count_(result) == num_bits);
            return result;
        }
    }

};


#ifndef DISABLE_API_TESTS
TEST_P(PMD_SignalAPITest, signals)
{
    unsigned int num = std::tr1::get<0>(GetParam());
    TestModel::TestType t = TestModel::TEST_TYPES[std::tr1::get<1>(GetParam())];
    unsigned int i;
    TestModel m;
    
    uint32_t bitset[8];
    int bits_per_word[8];

    memset(bitset, 0, sizeof(bitset));

    prng_.seed(0x12345678 + num * (int)t);

    /* note that we can only have 32 bits per word; 256 bits require 8 words.
     * we want to distribute num bits across each 8 words equally to simplify
     * computation.
     */
    float avg_wordbits = num / 8.0f;
    float cumulative = 0.0f;
    for (i = 0; i != 8; ++i)
    {
        float next = cumulative + avg_wordbits;
        bits_per_word[i] = floorf(next) - floorf(cumulative);
        cumulative = next;
    }
    
    if (bits_per_word[7] == 32)
    {
        /* now we only allow signal numbers 1 - 255, so we must disallow the top bit
         * of the 8th word.
         */
        for (i = 0; i != 7; ++i)
        {
            if (bits_per_word[i] < 32)
            {
                bits_per_word[i] += 1;
                bits_per_word[7] -= 1;
                break;
            }
        }

        assert(i != 7); /* this will be true when the totoal signal count < 256 */
    }
    

    /* now, actually generate random bitsets */
    for (i = 0; i != 8; ++i)
    {
        unsigned int total_bits = (i == 7) ? 31 : 32;  /* disallow signal #256 */
        bitset[i] = random_bitset_(total_bits, bits_per_word[i]);
    }

    /* now we have our random bitset, use this to add signals */
    dlb_pmd_signal signal = 0;
    unsigned int count = 0;
    for (i = 0; i != 8; ++i)
    {
        uint32_t bit = 1;
        for (int j = 0; j != 32; ++j)
        {
            if (bitset[i] & bit)
            {
                if (dlb_pmd_add_signal(m, signal+1))
                {
                    ADD_FAILURE() << "Could not add signal id " << (int)signal+1;
                }
                ++count;
            }
            bit = bit << 1;
            ++signal;
        }
    }
    assert(count == num);

    /* now check that the signal enumerator API works */
    if (num != dlb_pmd_num_signals(m))
    {
        ADD_FAILURE() << "Number of signals out " << dlb_pmd_num_signals(m)
                      << " does not equal number of signals in " << num;
    }
    else
    {
        dlb_pmd_signal_iterator si;
        dlb_pmd_signal sig;

        count = 0;
        if (dlb_pmd_signal_iterator_init(&si, m))
        {
            ADD_FAILURE() << "Could not initialize signal iterator " << dlb_pmd_error(m);
        }
        else
        {
            uint32_t seen[8];
            int idx;
            int bit;
            memset(seen, 0, sizeof(seen));
            
            while (!dlb_pmd_signal_iterator_next(&si, &sig))
            {
                sig -= 1;
                idx = (int)sig / 32;
                bit = 1 << ((int)sig % 32);

                assert(idx < 8);

                if (!(bitset[idx] & bit))
                {
                    ADD_FAILURE() << "found incorrect signal " << (int)sig;
                }
                if (seen[idx] & bit)
                {
                    ADD_FAILURE() << "Signal " << (int)sig << " returned more than once";
                }
                seen[idx] |= bit;
                ++count;
            }

            if (count != num)
            {
                ADD_FAILURE() << "Signal iterator iterated " << count << "times, not " << num;
            }
        }
    }
}

    
INSTANTIATE_TEST_CASE_P(PMD_API, PMD_SignalAPITest,
                        testing::Combine(testing::Range(1, 255),
                                         testing::Range(TestModel::FIRST_TEST_TYPE,
                                                        TestModel::LAST_TEST_TYPE+1)));


TEST(PMD_API_Constrained, signals)
{
    dlb_pmd_model_constraints constraints;
    size_t max = dlb_pmd_query_mem();
    size_t prev= 0;
    size_t sz;
    unsigned int i;

    constraints.max_elements            = DLB_PMD_MAX_AUDIO_ELEMENTS;
    constraints.max_presentation_names  = MAX_PRESENTATION_NAMES;
    constraints.max.num_signals         = DLB_PMD_MAX_SIGNALS;
    constraints.max.num_beds            = DLB_PMD_MAX_AUDIO_ELEMENTS;
    constraints.max.num_objects         = DLB_PMD_MAX_AUDIO_ELEMENTS;
    constraints.max.num_updates         = DLB_PMD_MAX_UPDATES;
    constraints.max.num_presentations   = DLB_PMD_MAX_PRESENTATIONS;
    constraints.max.num_loudness        = DLB_PMD_MAX_PRESENTATIONS;
    constraints.max.num_iat             = 1;
    constraints.max.num_eac3            = DLB_PMD_MAX_EAC3_ENCODING_PARAMETERS;
    constraints.max.num_ed2_system      = 1;
    constraints.max.num_ed2_turnarounds = DLB_PMD_MAX_ED2_TURNAROUNDS;
    constraints.max.num_headphone_desc  = DLB_PMD_MAX_HEADPHONE;

    for (i = 1; i != DLB_PMD_MAX_SIGNALS; ++i)
    {
        constraints.max.num_signals = i;
        sz = dlb_pmd_query_mem_constrained(&constraints);    
        if (sz < prev || sz > max)
        {
            ADD_FAILURE() << "query_mem_constrained failure";
        }
        prev = sz;
    }
}

    
TEST(PMD_API_Constrained, elements)
{
    dlb_pmd_model_constraints constraints;
    size_t max = dlb_pmd_query_mem();
    size_t prev= 0;
    size_t sz;
    unsigned int i;

    constraints.max_elements            = DLB_PMD_MAX_AUDIO_ELEMENTS;
    constraints.max_presentation_names  = MAX_PRESENTATION_NAMES;
    constraints.max.num_signals         = DLB_PMD_MAX_SIGNALS;
    constraints.max.num_beds            = DLB_PMD_MAX_AUDIO_ELEMENTS;
    constraints.max.num_objects         = DLB_PMD_MAX_AUDIO_ELEMENTS;
    constraints.max.num_updates         = DLB_PMD_MAX_UPDATES;
    constraints.max.num_presentations   = DLB_PMD_MAX_PRESENTATIONS;
    constraints.max.num_loudness        = DLB_PMD_MAX_PRESENTATIONS;
    constraints.max.num_iat             = 1;
    constraints.max.num_eac3            = DLB_PMD_MAX_EAC3_ENCODING_PARAMETERS;
    constraints.max.num_ed2_system      = 1;
    constraints.max.num_ed2_turnarounds = DLB_PMD_MAX_ED2_TURNAROUNDS;
    constraints.max.num_headphone_desc  = DLB_PMD_MAX_HEADPHONE;

    for (i = 1; i != DLB_PMD_MAX_AUDIO_ELEMENTS; ++i)
    {
        constraints.max_elements = i;
        sz = dlb_pmd_query_mem_constrained(&constraints);    
        if (sz <= prev || sz > max)
        {
            ADD_FAILURE() << "query_mem_constrained failure";
        }
        prev = sz;
    }
}


TEST(PMD_API_Constrained, beds)
{
    dlb_pmd_model_constraints constraints;
    size_t max = dlb_pmd_query_mem();
    size_t prev= 0;
    size_t sz;
    unsigned int i;

    constraints.max_elements            = DLB_PMD_MAX_AUDIO_ELEMENTS;
    constraints.max_presentation_names  = MAX_PRESENTATION_NAMES;
    constraints.max.num_signals         = DLB_PMD_MAX_SIGNALS;
    constraints.max.num_beds            = DLB_PMD_MAX_AUDIO_ELEMENTS;
    constraints.max.num_objects         = DLB_PMD_MAX_AUDIO_ELEMENTS;
    constraints.max.num_updates         = DLB_PMD_MAX_UPDATES;
    constraints.max.num_presentations   = DLB_PMD_MAX_PRESENTATIONS;
    constraints.max.num_loudness        = DLB_PMD_MAX_PRESENTATIONS;
    constraints.max.num_iat             = 1;
    constraints.max.num_eac3            = DLB_PMD_MAX_EAC3_ENCODING_PARAMETERS;
    constraints.max.num_ed2_system      = 1;
    constraints.max.num_ed2_turnarounds = DLB_PMD_MAX_ED2_TURNAROUNDS;
    constraints.max.num_headphone_desc  = DLB_PMD_MAX_HEADPHONE;

    for (i = 1; i != DLB_PMD_MAX_AUDIO_ELEMENTS; ++i)
    {
        constraints.max.num_beds = i;
        sz = dlb_pmd_query_mem_constrained(&constraints);    
        if (sz < prev || sz > max)
        {
            ADD_FAILURE() << "query_mem_constrained failure";
        }
        prev = sz;
    }
}


TEST(PMD_API_Constrained, objects)
{
    dlb_pmd_model_constraints constraints;
    size_t max = dlb_pmd_query_mem();
    size_t prev= 0;
    size_t sz;
    unsigned int i;

    constraints.max_elements            = DLB_PMD_MAX_AUDIO_ELEMENTS;
    constraints.max_presentation_names  = MAX_PRESENTATION_NAMES;
    constraints.max.num_signals         = DLB_PMD_MAX_SIGNALS;
    constraints.max.num_beds            = DLB_PMD_MAX_AUDIO_ELEMENTS;
    constraints.max.num_objects         = DLB_PMD_MAX_AUDIO_ELEMENTS;
    constraints.max.num_updates         = DLB_PMD_MAX_UPDATES;
    constraints.max.num_presentations   = DLB_PMD_MAX_PRESENTATIONS;
    constraints.max.num_loudness        = DLB_PMD_MAX_PRESENTATIONS;
    constraints.max.num_iat             = 1;
    constraints.max.num_eac3            = DLB_PMD_MAX_EAC3_ENCODING_PARAMETERS;
    constraints.max.num_ed2_system      = 1;
    constraints.max.num_ed2_turnarounds = DLB_PMD_MAX_ED2_TURNAROUNDS;
    constraints.max.num_headphone_desc  = DLB_PMD_MAX_HEADPHONE;

    for (i = 1; i != DLB_PMD_MAX_AUDIO_ELEMENTS; ++i)
    {
        constraints.max.num_objects = i;
        sz = dlb_pmd_query_mem_constrained(&constraints);    
        if (sz < prev || sz > max)
        {
            ADD_FAILURE() << "query_mem_constrained failure";
        }
        prev = sz;
    }
}


TEST(PMD_API_Constrained, presentations)
{
    dlb_pmd_model_constraints constraints;
    size_t max = dlb_pmd_query_mem();
    size_t prev= 0;
    size_t sz;
    unsigned int i;

    constraints.max_elements            = DLB_PMD_MAX_AUDIO_ELEMENTS;
    constraints.max_presentation_names  = MAX_PRESENTATION_NAMES;
    constraints.max.num_signals         = DLB_PMD_MAX_SIGNALS;
    constraints.max.num_beds            = DLB_PMD_MAX_AUDIO_ELEMENTS;
    constraints.max.num_objects         = DLB_PMD_MAX_AUDIO_ELEMENTS;
    constraints.max.num_updates         = DLB_PMD_MAX_UPDATES;
    constraints.max.num_presentations   = DLB_PMD_MAX_PRESENTATIONS;
    constraints.max.num_loudness        = DLB_PMD_MAX_PRESENTATIONS;
    constraints.max.num_iat             = 1;
    constraints.max.num_eac3            = DLB_PMD_MAX_EAC3_ENCODING_PARAMETERS;
    constraints.max.num_ed2_system      = 1;
    constraints.max.num_ed2_turnarounds = DLB_PMD_MAX_ED2_TURNAROUNDS;
    constraints.max.num_headphone_desc  = DLB_PMD_MAX_HEADPHONE;

    for (i = 1; i != DLB_PMD_MAX_PRESENTATIONS; ++i)
    {
        constraints.max.num_presentations = i;
        sz = dlb_pmd_query_mem_constrained(&constraints);    
        if (sz <= prev || sz > max)
        {
            ADD_FAILURE() << "query_mem_constrained failure";
        }
        prev = sz;
    }
}


TEST(PMD_API_Constrained, updates)
{
    dlb_pmd_model_constraints constraints;
    size_t max = dlb_pmd_query_mem();
    size_t prev= 0;
    size_t sz;
    unsigned int i;

    constraints.max_elements            = DLB_PMD_MAX_AUDIO_ELEMENTS;
    constraints.max_presentation_names  = MAX_PRESENTATION_NAMES;
    constraints.max.num_signals         = DLB_PMD_MAX_SIGNALS;
    constraints.max.num_beds            = DLB_PMD_MAX_AUDIO_ELEMENTS;
    constraints.max.num_objects         = DLB_PMD_MAX_AUDIO_ELEMENTS;
    constraints.max.num_updates         = DLB_PMD_MAX_UPDATES;
    constraints.max.num_presentations   = DLB_PMD_MAX_PRESENTATIONS;
    constraints.max.num_loudness        = DLB_PMD_MAX_PRESENTATIONS;
    constraints.max.num_iat             = 1;
    constraints.max.num_eac3            = DLB_PMD_MAX_EAC3_ENCODING_PARAMETERS;
    constraints.max.num_ed2_system      = 1;
    constraints.max.num_ed2_turnarounds = DLB_PMD_MAX_ED2_TURNAROUNDS;
    constraints.max.num_headphone_desc  = DLB_PMD_MAX_HEADPHONE;

    for (i = 0; i != DLB_PMD_MAX_UPDATES; ++i)
    {
        constraints.max.num_updates = i;
        sz = dlb_pmd_query_mem_constrained(&constraints);    
        if (sz <= prev || sz > max)
        {
            ADD_FAILURE() << "query_mem_constrained failure";
        }
        prev = sz;
    }
}


TEST(PMD_API_Constrained, loudness)
{
    dlb_pmd_model_constraints constraints;
    size_t max = dlb_pmd_query_mem();
    size_t prev= 0;
    size_t sz;
    unsigned int i;

    constraints.max_elements            = DLB_PMD_MAX_AUDIO_ELEMENTS;
    constraints.max_presentation_names  = MAX_PRESENTATION_NAMES;
    constraints.max.num_signals         = DLB_PMD_MAX_SIGNALS;
    constraints.max.num_beds            = DLB_PMD_MAX_AUDIO_ELEMENTS;
    constraints.max.num_objects         = DLB_PMD_MAX_AUDIO_ELEMENTS;
    constraints.max.num_updates         = DLB_PMD_MAX_UPDATES;
    constraints.max.num_presentations   = DLB_PMD_MAX_PRESENTATIONS;
    constraints.max.num_loudness        = DLB_PMD_MAX_PRESENTATIONS;
    constraints.max.num_iat             = 1;
    constraints.max.num_eac3            = DLB_PMD_MAX_EAC3_ENCODING_PARAMETERS;
    constraints.max.num_ed2_system      = 1;
    constraints.max.num_ed2_turnarounds = DLB_PMD_MAX_ED2_TURNAROUNDS;
    constraints.max.num_headphone_desc  = DLB_PMD_MAX_HEADPHONE;

    for (i = 0; i != DLB_PMD_MAX_PRESENTATIONS; ++i)
    {
        constraints.max.num_loudness = i;
        sz = dlb_pmd_query_mem_constrained(&constraints);    
        if (sz <= prev || sz > max)
        {
            ADD_FAILURE() << "query_mem_constrained failure";
        }
        prev = sz;
    }
}


TEST(PMD_API_Constrained, iat)
{
    dlb_pmd_model_constraints constraints;
    size_t max = dlb_pmd_query_mem();
    size_t prev= 0;
    size_t sz;
    unsigned int i;

    constraints.max_elements            = DLB_PMD_MAX_AUDIO_ELEMENTS;
    constraints.max_presentation_names  = MAX_PRESENTATION_NAMES;
    constraints.max.num_signals         = DLB_PMD_MAX_SIGNALS;
    constraints.max.num_beds            = DLB_PMD_MAX_AUDIO_ELEMENTS;
    constraints.max.num_objects         = DLB_PMD_MAX_AUDIO_ELEMENTS;
    constraints.max.num_updates         = DLB_PMD_MAX_UPDATES;
    constraints.max.num_presentations   = DLB_PMD_MAX_PRESENTATIONS;
    constraints.max.num_loudness        = DLB_PMD_MAX_PRESENTATIONS;
    constraints.max.num_iat             = 1;
    constraints.max.num_eac3            = DLB_PMD_MAX_EAC3_ENCODING_PARAMETERS;
    constraints.max.num_ed2_system      = 1;
    constraints.max.num_ed2_turnarounds = DLB_PMD_MAX_ED2_TURNAROUNDS;
    constraints.max.num_headphone_desc  = DLB_PMD_MAX_HEADPHONE;

    for (i = 0; i != 2; ++i)
    {
        constraints.max.num_iat = i;
        sz = dlb_pmd_query_mem_constrained(&constraints);    
        if (sz <= prev || sz > max)
        {
            ADD_FAILURE() << "query_mem_constrained failure";
        }
        prev = sz;
    }
}


TEST(PMD_API_Constrained, eac3)
{
    dlb_pmd_model_constraints constraints;
    size_t max = dlb_pmd_query_mem();
    size_t prev= 0;
    size_t sz;
    unsigned int i;

    constraints.max_elements            = DLB_PMD_MAX_AUDIO_ELEMENTS;
    constraints.max_presentation_names  = MAX_PRESENTATION_NAMES;
    constraints.max.num_signals         = DLB_PMD_MAX_SIGNALS;
    constraints.max.num_beds            = DLB_PMD_MAX_AUDIO_ELEMENTS;
    constraints.max.num_objects         = DLB_PMD_MAX_AUDIO_ELEMENTS;
    constraints.max.num_updates         = DLB_PMD_MAX_UPDATES;
    constraints.max.num_presentations   = DLB_PMD_MAX_PRESENTATIONS;
    constraints.max.num_loudness        = DLB_PMD_MAX_PRESENTATIONS;
    constraints.max.num_iat             = 1;
    constraints.max.num_eac3            = DLB_PMD_MAX_EAC3_ENCODING_PARAMETERS;
    constraints.max.num_ed2_system      = 1;
    constraints.max.num_ed2_turnarounds = DLB_PMD_MAX_ED2_TURNAROUNDS;
    constraints.max.num_headphone_desc  = DLB_PMD_MAX_HEADPHONE;

    for (i = 0; i != DLB_PMD_MAX_EAC3_ENCODING_PARAMETERS; ++i)
    {
        constraints.max.num_eac3 = i;
        sz = dlb_pmd_query_mem_constrained(&constraints);    
        if (sz <= prev || sz > max)
        {
            ADD_FAILURE() << "query_mem_constrained failure";
        }
        prev = sz;
    }
}


TEST(PMD_API_Constrained, etd)
{
    dlb_pmd_model_constraints constraints;
    size_t max = dlb_pmd_query_mem();
    size_t prev= 0;
    size_t sz;
    unsigned int i;

    constraints.max_elements            = DLB_PMD_MAX_AUDIO_ELEMENTS;
    constraints.max_presentation_names  = MAX_PRESENTATION_NAMES;
    constraints.max.num_signals         = DLB_PMD_MAX_SIGNALS;
    constraints.max.num_beds            = DLB_PMD_MAX_AUDIO_ELEMENTS;
    constraints.max.num_objects         = DLB_PMD_MAX_AUDIO_ELEMENTS;
    constraints.max.num_updates         = DLB_PMD_MAX_UPDATES;
    constraints.max.num_presentations   = DLB_PMD_MAX_PRESENTATIONS;
    constraints.max.num_loudness        = DLB_PMD_MAX_PRESENTATIONS;
    constraints.max.num_iat             = 1;
    constraints.max.num_eac3            = DLB_PMD_MAX_EAC3_ENCODING_PARAMETERS;
    constraints.max.num_ed2_system      = 1;
    constraints.max.num_ed2_turnarounds = DLB_PMD_MAX_ED2_TURNAROUNDS;
    constraints.max.num_headphone_desc  = DLB_PMD_MAX_HEADPHONE;

    for (i = 0; i != DLB_PMD_MAX_ED2_TURNAROUNDS; ++i)
    {
        constraints.max.num_ed2_turnarounds = i;
        sz = dlb_pmd_query_mem_constrained(&constraints);    
        if (sz <= prev || sz > max)
        {
            ADD_FAILURE() << "query_mem_constrained failure";
        }
        prev = sz;
    }
}


TEST(PMD_API_Constrained, hed)
{
    dlb_pmd_model_constraints constraints;
    size_t max = dlb_pmd_query_mem();
    size_t prev= 0;
    size_t sz;
    unsigned int i;

    constraints.max_elements            = DLB_PMD_MAX_AUDIO_ELEMENTS;
    constraints.max_presentation_names  = MAX_PRESENTATION_NAMES;
    constraints.max.num_signals         = DLB_PMD_MAX_SIGNALS;
    constraints.max.num_beds            = DLB_PMD_MAX_AUDIO_ELEMENTS;
    constraints.max.num_objects         = DLB_PMD_MAX_AUDIO_ELEMENTS;
    constraints.max.num_updates         = DLB_PMD_MAX_UPDATES;
    constraints.max.num_presentations   = DLB_PMD_MAX_PRESENTATIONS;
    constraints.max.num_loudness        = DLB_PMD_MAX_PRESENTATIONS;
    constraints.max.num_iat             = 1;
    constraints.max.num_eac3            = DLB_PMD_MAX_EAC3_ENCODING_PARAMETERS;
    constraints.max.num_ed2_system      = 1;
    constraints.max.num_ed2_turnarounds = DLB_PMD_MAX_ED2_TURNAROUNDS;
    constraints.max.num_headphone_desc  = DLB_PMD_MAX_HEADPHONE;

    for (i = 0; i != DLB_PMD_MAX_HEADPHONE; ++i)
    {
        constraints.max.num_headphone_desc = i;
        sz = dlb_pmd_query_mem_constrained(&constraints);    
        if (sz <= prev || sz > max)
        {
            ADD_FAILURE() << "query_mem_constrained failure";
        }
        prev = sz;
    }
}
#endif

