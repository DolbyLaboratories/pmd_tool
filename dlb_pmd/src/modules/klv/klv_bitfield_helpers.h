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

/**
 * @file klv_bitfield_helpers.h
 * @brief common definitions for manipulating and reading groups of bits
 */


#ifndef KLV_BITFIELD_HELPERS_INC_H
#define KLV_BITFIELD_HELPERS_INC_H

#include <stdlib.h>
#include <ctype.h>


#ifdef _MSC_VER
#  define PRIu64 "I64u"
#  define PRId64 "I64"
__pragma(warning(disable:4244))
#else
#  include <inttypes.h>
#endif

//#define TRACE_BITS
#ifdef TRACE_BITS
#  define TRACEBITS(x) printf x
#else
#  define TRACEBITS(x)
#endif


/**
 * @brief set variable length bitfields inside bitarray
 */
static inline
void
set_
    (uint8_t     *bytes  /**< byte array to modify */
    ,int          bit    /**< bit position from start of array (msb) */
    ,int          len    /**< bitfield length */
    ,uint64_t     val    /**< value to set */
    )
{
    int intra_byte_shift;
    int intra_byte_len;
    int intra_byte_mask;
    uint8_t *byte;

    TRACEBITS(("            set_ (%d, %d) %" PRIu64 "\n", bit, len, val));

    assert(val < (1ull<<len));

    byte = bytes + bit/8;
    while (len > 0)
    {
        intra_byte_len   = (8 - (bit&7));        /**< space available in output byte */
        if (intra_byte_len > len)
	{
	    intra_byte_len = len;
        }
        intra_byte_shift = 8 - intra_byte_len - (bit&7);   /**< bit position within output byte */
        intra_byte_mask  = (1ul << intra_byte_len)-1; /**< value mask to fit space available */

	*byte |= ((val >> (len - intra_byte_len)) & intra_byte_mask) << intra_byte_shift;
	++byte;
	bit += intra_byte_len;
        len -= intra_byte_len;
    }
}


/**
 * @brief helper function to read variable-bit bitfield values from bitarray
 */
static inline
uint64_t                 /** @return bitfield value */
get_
    (uint8_t     *bytes  /**< 16-byte array to read */
    ,int          bit    /**< bit position from bit 0 (lsb) */
    ,int          len    /**< bitfield length */
    )
{
    uint64_t val = 0;
    int intra_byte_len;
    int intra_byte_shift;
    int intra_byte_mask;
    uint8_t *byte;

    TRACEBITS(("            get_ (%d, %d) ", bit, len));

    byte = bytes + bit/8;
    while (len > 0)
    {
        intra_byte_len   = (8 - (bit&7));        /**< space available in output byte */
        if (intra_byte_len > len)
        {
            intra_byte_len = len;
        }
        intra_byte_shift = 8 - intra_byte_len - (bit&7);   /**< bit position within output byte */
        intra_byte_mask  = (1ul << intra_byte_len)-1; /**< value mask to fit space available */

        val = (val << intra_byte_len) | ((*byte >> intra_byte_shift) & intra_byte_mask);
	++byte;
	bit += intra_byte_len;
        len -= intra_byte_len;
    }

    TRACEBITS(("%" PRId64 "\n", val));
    return val;
}
    

/**
 * @brief copy a bitarray into output bitarray
 */
static inline
void
seta_
    (uint8_t     *output /**< bitarray to modify */
    ,int          bit    /**< bit position to write from msb */
    ,int          len    /**< bitfield length */
    ,uint8_t     *input  /**< array to copy */
    )
{
    while (len > 0)
    {
        int bytelen = len > 8 ? 8 : len;
        set_(output, bit, bytelen, *input);
        ++input;
        len -= bytelen;
        bit += bytelen;
    }
}


/**
 * @brief helper function to read variable-bit array out from bitarray
 */
static inline
void
geta_
    (uint8_t     *input  /**< bitarray to read */
    ,int          bit    /**< bit position from 0 (msb) to read */
    ,int          len    /**< bitfield length */
    ,uint8_t     *output /**< output array, big enough to hold len bits */
    )
{
    while (len > 0)
    {
        int bytelen = len > 8 ? 8 : len;
        uint64_t val = get_(input, bit, bytelen);
        *output++ = (uint8_t)val;
        len -= bytelen;
        bit += bytelen;
    }
}


/**
 * @brief convert ASCII char in 8 bits to 1-27 (or 0 if terminator)
 */
static inline
uint8_t
klv_encode_langch
   (pmd_langcode byte
   )
{
    uint8_t b = (uint8_t)byte;
    if (b)
    {
        return tolower(b & 0x7f) - 'a' + 1;
    }
    return 0;
}


/**
 * @brief reconstruct ASCII char in 8 bits from 1-17 (or 0 if terminator)
 */
static inline
uint8_t
klv_decode_langch
   (uint64_t bits
   )
{
    uint8_t byte = (uint8_t)bits;
    return byte ? (byte + 'a' - 1) : 0;
}


#endif /* KLV_BITFIELD_HELPERS_INC_H */
