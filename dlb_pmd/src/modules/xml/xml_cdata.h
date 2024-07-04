/************************************************************************
 * dlb_pmd
 * Copyright (c) 2017-2019, Dolby Laboratories Inc.
 * Copyright (c) 2017-2019, Dolby International AB.
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
 * @file xml_cdata.h
 * @brief helper functions to generate/parse XML character arrays
 *
 * Sometimes we need to write a character array, or an array of bytes.
 *
 * This may require escaping characters, or binary encoding.
 */

#ifndef PMD_XML_CDATA_H_
#define PMD_XML_CDATA_H_

#include "pmd_types.h"

/**
 * @brief examine and escape array of 'unparsed XML' character data
 *
 * (Where 'unparsed' here means the characters are not part of a
 * tag or attribute).
 */
static inline
int                       /** @return 0 on success, 1 on failure */
encode_cdata
    (const uint8_t *input /**< [in] array of bytes to write */
    ,size_t insize        /**< [in] size of input array */
    ,char *output         /**< [out] destination string, we assume it has been
                            *        zeroed out before this function is invoked. */
    ,size_t *outsize      /**< [in/out] in = capacity of destination string
                           *            out = size of generated string, or 0
                           *            if original array is printable as is
                           */
    ,pmd_bool *hex        /**< [out]  0 - not hex-encoded, 1 = hex encoded */
    )
{
    pmd_bool hex_encode = 0;
    pmd_bool escape = 0;
    size_t linesize;
    unsigned int i;

    linesize = insize;
    /* walk through array looking for escape characters */
    for (i = 0 ; i != insize; ++i)
    {
        uint8_t c = input[i];

        switch (c)
        {
            case '&':
                escape = 1;
                linesize += 4;  /* replace with &amp; */
                break;
            case '<':
                escape = 1;
                linesize += 3;  /* replace with &lt; */
                break;
            case '>':
                escape = 1;
                linesize += 3;  /* replace with &gt; */
                break;
            case '\"':
                escape = 1;
                linesize += 2;  /* replace with \\" */
                break;
            case '\\':
                escape = 1;
                linesize += 3;  /* replace with \\\\ */
                break;
            default:
                if (c < ' ' || c > '~')
                {
                    hex_encode = 1;
                }
                break;
        }
    }

    if (hex_encode)
    {
        linesize = insize * 2;
        if (linesize > *outsize) return 1;
        write_hex(input, insize, output);
        *outsize = linesize;
        *hex = 1;
        return 0;
    }
    else if (escape)
    {
        const uint8_t *c = input;

        if (linesize > *outsize) return 1;

        for (i = 0 ; i != insize; ++i, ++c)
        {
            /* note that we need to double-escape characters so that
             * when the write_line function invokes snprintf, the
             * escapes will not be lost */
            switch (*c)
            {
            case '&':  memcpy(output, "&amp;", 5); output += 5; break;
            case '<':  memcpy(output, "&lt;", 4);  output += 4; break;
            case '>':  memcpy(output, "&gt;", 4);  output += 4; break;
            case '\"': memcpy(output, "\\\"", 2);  output += 2; break;
            case '\\': memcpy(output, "\\\\", 2);  output += 2; break;
            default:  *output++ = *c;                           break;
            }
        }
        *outsize = linesize;
        *hex = 0;
        return 0;
    }
    else
    {
        memcpy(output, input, insize);
        *outsize = insize;
        *hex = 0;
    }
    return 0;
}


/**
 * @brief decode a character string from XML format and store it in array
 */
static inline
int                           /** @return 0 on success, 1 on failure */
read_cdata
    (const char *text         /**< [in] input text to parse, null-terminated */
    ,uint8_t *output          /**< [out] output byte array */
    ,size_t *outsize          /**< [in/out] in - capacity of output byte array,
                                            out - length of decoded data */
    ,pmd_bool hex             /**< [in] 1 if base16 decoding required */
    )
{
    if (hex)
    {
        size_t len = strnlen(text, *outsize*2);
        *outsize = len/2;
        return read_hex(text, len, output);
    }
    else
    {
        size_t len = strlen(text);
        const char *end = text + len;
        uint8_t *outend = output + *outsize;
        uint8_t *out = output;

        while (text < end && out < outend)
        {
            char c = *text++;
            if (c == '&')
            {
                if (!strncmp(text, "amp;", 4))
                {
                    *out++ = '&';
                    text += 4;
                }
                else if (!strncmp(text, "lt;", 3))
                {
                    *out++ = '<';
                    text += 3;
                }
                else if (!strncmp(text, "gt;", 3))
                {
                    *out++ = '>';
                    text += 3;
                }
                else
                {
                    return 1;
                }
            }
            else if (c == '\\')
            {
                c = *text++;
                if (c == '\\' || c == '\"')
                {
                    *out++ = c;
                }
                else
                {
                    return 1;
                }
            }
            else if (c < ' ' || c > '~')
            {
                return 1;
            }
            else
            {
                *out++ = c;
            }
        }
        *outsize = out - output;
    }
    return 0;
}


#endif /* PMD_XML_CDATA_H_ */
