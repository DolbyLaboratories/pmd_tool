/************************************************************************
 * dlb_pmd
 * Copyright (c) 2023, Dolby Laboratories Inc.
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

#ifndef PRNG_KISS_HH_
#define PRNG_KISS_HH_

#include <stdint.h>

#ifdef _MSC_VER
__pragma(warning(disable:4244))
#endif

/**
 * @brief Marsaglia's Keep It Simple Stupid (KISS) Pseudo Random
 * Number Generator, dervied from algorithm listed in a 2003 usenet
 * posting.  Period of 2^124, not suitable for crypto!
 *
 * http://mathforum.org/kb/thread.jspa?forumID=226&threadID=541450&messageID=1643078
 *
 * The code below is a totally new rewrite that implements the
 * mathematics described in the above post, but shares none of the
 * actual code.
 */


class PrngKiss
{
    uint32_t x_;
    uint32_t y_;
    uint32_t z_;
    uint32_t c_;

public:
    PrngKiss()
        : x_(123456789)
        , y_(362436000)
        , z_(521288629)
        , c_(7654321)
    {}

    ~PrngKiss() {}

    void seed(uint32_t x) { x_ = x; }

    uint32_t next()
    {
        unsigned long long a = 698769069LL;
        unsigned long long t;

        x_ = 69069 * x_ + 12345;
        y_ ^= (y_<<13);
        y_ ^= (y_>>17);
        y_ ^= (y_<<5);
        t  = a * z_ + c_;
        c_ = (t>>32);

        return x_ + y_ + (z_=t);
    }

    
    void gen_rand_bytearray
        (uint8_t *out       /**< [in] memory to fill */
        ,int len            /**< [in] capacity of buffer to fill */
        ,bool ascii         /**< [in] do we want printable ASCII or not? */
        )
    {
        int i;

        for (i = 0; i != len; ++i)
        {
            if (ascii)
            {
                out[i] = ' ' + (next() % ('~'-' '));
            }
            else
            {
                out[i] = next() & 0xff;
            }
        }
    }
};


#endif /* PRNG_KISS_HH_ */
