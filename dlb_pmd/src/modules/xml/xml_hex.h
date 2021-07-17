/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2017-2019 by Dolby Laboratories,
 *                Copyright (C) 2017-2019 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

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
