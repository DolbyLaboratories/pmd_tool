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

/** @addtogroup DLB_BITBUF_API */
/*@{*/
/**
 * @file
 * @brief  Bit Buffer Management Interface - Write API
 */

#ifndef DLB_BITBUF_WRITE_H
#define DLB_BITBUF_WRITE_H

#include "dlb_bitbuf.h"

#ifdef __cplusplus
extern "C"
{
#endif
/**
 * @brief Write bits from an 'int' type.
 *
 * Write a pattern of `n` right-aligned bits of `data` to the current postion 
 * of bitstream `p_bitbuf`. Note, that bits outside of this pattern have to be
 * zero.
 *
 * For portability, `n` should not be larger than 16 bits (minimum size of 'int'
 * in C90). Use `dlb_bitbuf_write_long()` if `n` is larger.
 *
 * In case an error code is returned,
 * - nothing is written to the buffer and
 * - no internal states are updated.
 *
 * @return 0: success,
 *         1: out of bounds (write over buffer boundary)
 */
int
dlb_bitbuf_write
    (dlb_bitbuf_handle p_bitbuf    /**< [in,out] Handle to allocated and initialized #dlb_bitbuf instance. */
    ,unsigned int      data        /**< [in] Bits to write into bitstream, right-aligned. */
    ,unsigned int      n           /**< [in] Number of bits to write. */
    ); 

/**
 * @brief Write bits from a 'long' type.
 *
 * Write a pattern of `n` right-aligned bits of `data` to the current postion 
 * of bitstream `p_bitbuf`. Note, that bits outside of this pattern have to be
 * zero.
 *
 * For portability, `n` should not be larger than 32 bits (minimum size of
 * 'long' in C90).
 *
 * In case an error code is returned,
 * - nothing is written to the buffer and
 * - no internal states are updated.
 *
 * @return 0: success,
 *         1: out of bounds (write over buffer boundary)
 */
int
dlb_bitbuf_write_long
    (dlb_bitbuf_handle p_bitbuf    /**< [in,out] Handle to allocated and initialized #dlb_bitbuf instance. */
    ,unsigned long     data        /**< [in] Bits to write into bitstream, right-aligned. */
    ,unsigned int      n           /**< [in] Number of bits to write. */
    );

#ifdef __cplusplus
}
#endif

#endif /* DLB_BITBUF_H */
/*@}*/
