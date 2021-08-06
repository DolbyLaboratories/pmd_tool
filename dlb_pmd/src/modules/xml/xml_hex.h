/************************************************************************
 * dlb_pmd
 * Copyright (c) 2021, Dolby Laboratories Inc.
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

#ifndef _PMD_XML_HEX_H_
#define _PMD_XML_HEX_H_

/**
 * @file xml_hex.h
 * @brief helper function to write/read hex numbers
 */

#include "dlb_pmd_api.h"
#include <stdint.h>

/**
 * @brief helper to write a byte array as a sequence of hex digits
 */
static inline
void
write_hex
    (const uint8_t *in   /**< byte array to write */
    ,size_t len          /**< size of byte array to write */
    ,char *out           /**< output string, at least 2 * len */
    )
{
    static const char *hexdigits = "0123456789abcdef";
    unsigned int i;

    for (i = 0; i != len; ++i)
    {
        uint8_t c = *in++;
        *out++ = hexdigits[(c>>4)&0xf];
        *out++ = hexdigits[c&0x0f];
    }
}


static inline
int                      /** @return 0 on success, 1 on error */
to_hex
    (char c              /**< [in] hex digit to decode */
    ,uint8_t *val        /**< [out] decimal value */
    )
{
    if (c >= '0' && c <= '9')
    {
        *val = c - '0';
        return 0;
    }
    else if (c >= 'a' && c <= 'f')
    {
        *val = c - 'a' + 10;
        return 0;
    }
    else if (c >= 'A' && c <= 'F')
    {
        *val = c - 'A' + 10;
        return 0;
    }
    return 1;
}


/**
 * @brief helper to read a string of hex digits and decode
 *
 * Note that this assumes we have an even number of digits
 */
static inline
int                      /** @return 0 on success, 1 on error */
read_hex
    (const char *in      /**< character string to read */
    ,size_t len          /**< size of character string to read */
    ,uint8_t *out        /**< output buffer, at least len/2 */
    )
{
    unsigned int i;

    if (len & 1)
    {
        uint8_t l = 0;
        if (to_hex(in[0], &l))
        {
            return 1;
        }
        *out++ = l;
        len -= 1;
    }

    len = len/2;
    for (i = 0; i != len; ++i)
    {
        uint8_t h = 0;
        uint8_t l = 0;

        if (to_hex(in[0], &h) || to_hex(in[1], &l))
        {
            return 1;
        }
        *out++ = (h<<4) | l;
        in += 2;
    }
    return 0;
}


#endif /* _PMD_XML_HEX_H_ */
