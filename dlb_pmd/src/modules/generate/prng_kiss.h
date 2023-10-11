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

#ifndef PRNG_KISS_H_
#define PRNG_KISS_H_

#ifdef _MSC_VER
__pragma(warning(disable:4244))
#endif

/**
 * @brief Marsaglia's Keep It Simple Stupid (KISS) Pseudo Random
 * Number Generator, derived from an algorithm listed in a 2003 usenet
 * posting.  Period of 2^124, not suitable for crypto!
 *
 * http://mathforum.org/kb/thread.jspa?forumID=226&threadID=541450&messageID=1643078
 *
 * The code below is a totally new rewrite that implements the
 * mathematics described in the above post, but shares none of the
 * actual code.
 */
typedef struct
{
    uint32_t x;
    uint32_t y;
    uint32_t z;
    uint32_t c;
} prng;


/**
 * @brief initialize the PRNG
 */
static
void
prng_init
    (prng *p            /**< [in] prng structure */
    )
{
    p->x = 123456789;
    p->y = 362436000;
    p->z = 521288629;
    p->c = 7654321;
}


/**
 * @brief set the PRNG seed
 */
static
void
prng_seed
    (prng *p            /**< [in] prng structure */
    ,unsigned int seed  /**< [in] seed */
    )
{
    p->x = seed;
}

    
/**
 * @brief generate the next pseudo-random number
 */
static
uint32_t                /** @return pseudo-random number */
prng_next
    (prng *p            /**< [in] prng structure */
    )
{
    unsigned long long a = 698769069LL;
    unsigned long long t;
    
    p->x = 69069 * p->x + 12345;
    p->y ^= (p->y<<13);
    p->y ^= (p->y>>17);
    p->y ^= (p->y<<5);
    t  = a * p->z + p->c;
    p->c = (t>>32);
    
    return p->x + p->y + (p->z=t);
}


/**
 * @brief write a unicode character to buffer
 */
static
int                       /** @return 0 on failure, 1 on success */
write_utf8_char
    (uint8_t **bufptr     /**< [in] buffer to write to */
    ,uint8_t *end         /**< [in] 1st character after buffer end */
    ,unsigned int unicode /**< [in] unicode character to write */
    )
{
    uint8_t *buf = *bufptr;
    if (unicode < 128)
    {
        if (buf >= end)
        {
            return 0;
        }
        *buf++ = (uint8_t)unicode;
    }
    else if (unicode < 0x800)
    {
        if (buf+1 >= end)
        {
            return 0;
        }
        *buf++ = 0xC0 | ((unicode >> 6) & 0x1F);
        *buf++ = 0x80 | (unicode & 0x3F);
    }
    else if (unicode < 0x10000)
    {
        if (buf+2 >= end)
        {
            return 0;
        }
        *buf++ = 0xE0 | ((unicode >> 12) & 0x0F);
        *buf++ = 0x80 | ((unicode >> 6) & 0x3F);
        *buf++ = 0x80 | (unicode & 0x3F);
    }
    else if (unicode < 0x110000)
    {
        if (buf+3 >= end)
        {
            return 0;
        }
        *buf++ = 0xF0 | ((unicode >> 18) & 0x07);
        *buf++ = 0x80 | ((unicode >> 12) & 0x3F);
        *buf++ = 0x80 | ((unicode >> 6) & 0x3F);
        *buf++ = 0x80 | (unicode & 0x3F);
    }
    else
    {
        return 0;
    }
    *bufptr = buf;
    return 1;
}


/**
 * @brief describe a legal unicode range
 */
typedef struct
{
    unsigned int start;
    unsigned int end;
} utf8_range;


/**
 * @brief enumerate legal unicode ranges
 */
static const utf8_range codes[3] =
{
    { 32,      0xd7fa },
    { 0xe000,  0xfff8 },
    { 0x10000, 0x10fffa }
};
    

/**
 * @def RANGE0_SIZE
 * @brief number of codes in nth range
 */
#define RANGE_SIZE(n) (codes[n].end - codes[n].start)

/**
 * @def TOTAL_CODES
 * @brief total number of legal unicode characters for random generation
 */
#define TOTAL_CODES (RANGE_SIZE(0) + RANGE_SIZE(1) + RANGE_SIZE(2))


/**
 * @brief generate a random printable ASCII string
 *
 * we simply generate random bytes in the range 32 - 127 (' ' to '~')
 */
static
void
prng_ascii
    (prng *p            /**< [in] prng structure */
    ,uint8_t *array     /**< [in] place to store string */
    ,size_t size        /**< [in] capacity of #array */
    )
{
    memset(array, '\0', size);
    if (size > 1)
    {
        int length = (int)(prng_next(p) % (size-1));
        uint8_t *end = array + length;

        while (array < end)
        {
            char c = (char)(prng_next(p) % ('~' - ' ')) + ' ';

            /* work around printf escape characters */
            if (c == '%' || c == '\\')
            {
                if (array+1 < end)
                {
                    *array++ = c;
                }
                else
                {
                    c = '\0';
                }
            }
            *array++ = c;
        }
    }
}


/**
 * @brief generate a random UTF-8 string
 *
 * UTF-8 includes the following unicode ranges:
 *
 *  32      - 127       (ASCII ' ' to '~')
 *  128     - 0xd7f9
 *  0xe000  - 0xfff7
 *  0x10000 - 0x10fff9
 */
static
void
prng_utf8
    (prng *p            /**< [in] prng structure */
    ,uint8_t *array     /**< [in] place to store string */
    ,size_t size        /**< [in] capacity of #array */
    )
{
    memset(array, '\0', size);
    if (size > 1)
    {
        int length = (int)(prng_next(p) % (size-1));
        uint8_t *end = array + length;

        while (array < end)
        {
            unsigned int n = prng_next(p) % TOTAL_CODES;
            unsigned int code = n;

            if (n < RANGE_SIZE(0))
            {
                code = n + codes[0].start;
                assert(code < codes[0].end);
            }
            else if (n < RANGE_SIZE(0) + RANGE_SIZE(1))
            {
                code = n - RANGE_SIZE(0) + codes[1].start;
                assert(code < codes[1].end);
            }
            else
            {
                code = n - (RANGE_SIZE(0) + RANGE_SIZE(1)) + codes[2].start;
                assert(code < codes[2].end);
            }

            if (!write_utf8_char(&array, end, code))
            {
                return;
            }
        }
    }
}



#endif /* PRNG_KISS_H_ */

