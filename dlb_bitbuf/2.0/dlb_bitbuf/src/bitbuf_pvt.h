/************************************************************************
 * dlb_bitbuf
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

/**
 * @defgroup DLB_BITBUF_SRC Bit Buffer Management Source Code
 *
 * This is a detailed description of the components source code,
 * targeted to facilitate porting efforts of the module. 
 * 
 * If you are solely using the component in your code, please have a
 * look ath the API Reference.
 */
/*@{*/
/**
 * @file
 * @brief  Bit Buffer Management - Private Defines and Declarations
 */

#ifndef DLB_BITBUF_PVT_H
#define DLB_BITBUF_PVT_H

#include "dlb_bitbuf.h"

/* Retrieve log2(DLB_BITBUF_WIDTH) - does anyone know a smarter way? */
#if (DLB_BITBUF_WIDTH == 8)
#define DLB_BITBUF_WIDTH_SHIFT 3 
#else
#if (DLB_BITBUF_WIDTH == 16)
#define DLB_BITBUF_WIDTH_SHIFT 4
#else
#if (DLB_BITBUF_WIDTH == 32)
#define DLB_BITBUF_WIDTH_SHIFT 5
#else /* no match */
#error "Machine type not supported, CHAR_BIT none of 8, 16 or 32!"
#endif /* 32 */
#endif /* 16 */
#endif /* 8 */

#define BIT_MASK(N)        ((1 << (N)) - 1)   /**< 0b1...1 (N 1s) */

#define ASSERT_BITBUF_SANITY(p)                                                     \
do                                                                                  \
{                                                                                   \
    assert((p));                             /* Null pointer check */               \
    assert((p)->p_cur >= (p)->p_base);       /* Current position > buffer begin */  \
    assert((p)->bit_pos < DLB_BITBUF_WIDTH); /* Bit position within current word */ \
} while((void)0, 0)

#ifdef _MSC_VER
#define inline __forceinline
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * Blindly skip 'num_bits' forward in p_bitbuf 
 */
void 
skip_bits_forward
    (dlb_bitbuf_handle p_bitbuf
    ,long              num_bits
    );


#ifdef __cplusplus
}
#endif

#endif /* DLB_BITBUF_H */
/*@}*/
