/************************************************************************
 * dlb_pmd
 * Copyright (c) 2018, Dolby Laboratories Inc.
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

#include "dlb_pmd_api.h"    
#include "src/model/pmd_model.h"

#include "PrngKiss.hh"
#include "TestModel.hh"
#include "gtest/gtest.h"

#include <math.h>

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
        unsigned int count = 0;

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

