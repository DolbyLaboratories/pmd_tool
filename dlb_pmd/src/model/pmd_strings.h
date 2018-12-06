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

#ifndef PMD_STRINGS_H
#define PMD_STRINGS_H

#ifdef __GNUC__
#  define MAY_BE_UNUSED __attribute__((unused))
#else
#  define MAY_BE_UNUSED
#endif

#ifdef _MSC_VER
__pragma(warning(disable:4505))
#endif

#include "pmd_types.h"
#include <string.h>

/**
 * @brief verify PMS strings contain valid UTF-8 characters
 */
MAY_BE_UNUSED
static inline
pmd_bool                       /** return 1 on success, 0 on failure */
pmd_decode_utf8_contbyte
    (const char c
    ,unsigned char *cptr
    )
{
    if ((c & 0xC0) != 0x80)
    {
        return 0;
    }
    *cptr = (c & 0x3F);
    return 1;
}


/**
 * @brief verify PMS strings contain valid UTF-8 characters
 */
MAY_BE_UNUSED
static
pmd_bool                       /** return 1 on success, 0 on failure */
pmd_decode_utf8char
    (unsigned char c0
    ,const char **textptr
    ,const char *end
    ,unsigned int *unicode
    )
{
    unsigned char c1;
    unsigned char c2;
    unsigned char c3;
    const char *text = *textptr;
    unsigned int u;

    if (0xC0 == (c0 & 0xE0))
    {
        /* one continuation byte */
        if (text >= end
            || !pmd_decode_utf8_contbyte(text[0], &c1)
            )
        {
            return 0;
        }
        u = ((c0 & 0x1F) << 6) | (c1 & 0x3F);
        text += 1;
    }
    else if (0xE0 == (c0 & 0xF0))
    {
        /* two continuation bytes */
        if (text+1 >= end
            || !pmd_decode_utf8_contbyte(text[0], &c1)
            || !pmd_decode_utf8_contbyte(text[1], &c2))
        {
            return 0;
        }
        u = ((c0 & 0x0F) << 12) | (c1 << 6) | c2;
        text += 2;
    }
    else if (0xF0 == (c0 & 0xF8)) 
    {
        /* three continuation bytes */
        if (text+2 >= end
            || !pmd_decode_utf8_contbyte(text[0], &c1)
            || !pmd_decode_utf8_contbyte(text[1], &c2)
            || !pmd_decode_utf8_contbyte(text[2], &c3))
        {
            return 0;
        }
        u = ((c0 & 0x07) << 18) | (c1 << 12) | (c2 << 6) | c3;
        text += 3;
    }
    else
    {
        /* illegal UTF-8 code */
        return 0;
    }
    
    /* check valid ranges: legal chars are Unicode and ISO/IEC 10646 */
    if (   (u < 0xd800)
        || (u >= 0xe000 && u <= 0xfffd)
        || (u >= 0x10000 && u <= 0x10ffff)
        )
    {
        *unicode = u;
        *textptr = text;
        return 1;
    }
    return 0;
}


/**
 * @brief check that the string contains legal XML characters
 */
MAY_BE_UNUSED
static
pmd_bool                         /** @return 0 on failure, 1 on success */
pmd_string_valid
    (const char *text
    )
{
    const char *end = text + strlen(text);
    unsigned int unicode = 0;
    unsigned char c;

    while (text < end)
    {
        c = *text;
        ++text;
        if (c < ' ' && c != '\t' && c != '\r' && c != '\n')
        {
            return 0;
        }
        else if (c > 127)
        {
            if (!pmd_decode_utf8char(c, &text, end, &unicode))
            {
                return 0;
            }
        }
    }
    return 1;
}


#endif /* PMD_STRINGS_H */
