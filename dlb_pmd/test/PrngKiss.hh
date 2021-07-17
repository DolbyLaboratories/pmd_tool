/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2018 by Dolby Laboratories,
 *                Copyright (C) 2018 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

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
