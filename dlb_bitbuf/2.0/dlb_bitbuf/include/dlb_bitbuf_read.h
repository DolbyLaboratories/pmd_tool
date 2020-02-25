/************************************************************************
 * dlb_bitbuf
 * Copyright (c) 2020, Dolby Laboratories Inc.
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
 * @brief  Bit Buffer Management Interface - Read and peek API
 */

#ifndef DLB_BITBUF_READ_H
#define DLB_BITBUF_READ_H

#include "dlb_bitbuf.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Read bits into 'unsigned int' data type.
 *
 * Read `n` bits from the current position of bitstream `p_bitbuf` and return
 * them right-aligned in an 'unsigned int' type.
 *
 * Note, that reading over the end of the bitstream will not cause any memory
 * access outside of the buffer boundary. Instead, remaining bits will be filled
 * with zeros. Moreover,
 * - a subsequent call of dlb_bitbuf_get_bits_left() returns a negative number,
 * - a subsequent call of dlb_bitbuf_get_abs_pos() returns a number > bitbuf_size,
 * - subsequent calls of dlb_bitbuf_write(), dlb_bitbuf_write_long() or 
 *   dlb_bitbuf_align() return out-of-bounds errors.
 * 
 * If you instantly have to check for out of bounds errors while reading, use 
 * dlb_bitbuf_safe_read() instead.
 *
 * For portability, `n` should not be larger than 16 bits (minimum size of 'int'
 * in C90). Use dlb_bitbuf_read_long() if `n` is larger.
 *
 * @return Right-aligned bits read from bitstream.
 */
unsigned int
dlb_bitbuf_read
    (dlb_bitbuf_handle p_bitbuf    /**< [in,out] Handle to allocated and initialized #dlb_bitbuf instance. */
    ,unsigned int      n           /**< [in] Number of bits to read. */
    );

/**
 * @brief Read bits into 'unsigned long' data type.
 *
 * Read `n` bits from the current position of bitstream `p_bitbuf` and return
 * them right-aligned in a 'unsigned long' type.
 * 
 * Note, that reading over the end of the bitstream will not cause any memory
 * access outside of the buffer boundary.
 * Instead, remaining bits will be filled with zeros. Moreover,
 * - a subsequent call of dlb_bitbuf_get_bits_left() returns a negative number,
 * - a subsequent call of dlb_bitbuf_get_abs_pos() returns a number >
 * bitbuf_size,
 * - subsequent calls of dlb_bitbuf_write(), dlb_bitbuf_write_long() or 
 *   dlb_bitbuf_align() return out-of-bounds errors.
 *
 * If you instantly have to check for out of bounds errors while reading, use 
 * dlb_bitbuf_safe_read_long() instead.
 *
 * For portability, `n` should not be larger than 32 bits (minimum size of
 * 'long' in C90).
 *
 * @return Right-aligned bits read from bitstream.
 */
unsigned long
dlb_bitbuf_read_long
    (dlb_bitbuf_handle p_bitbuf    /**< [in,out] Handle to allocated and initialized #dlb_bitbuf instance. */
    ,unsigned int      n           /**< [in] Number of bits to read. */
    );

/**
 * @brief Read bits into 'unsigned int' data type without bounds check.
 *
 * Read `n` bits from the current position of bitstream `p_bitbuf` and return
 * them right-aligned in an 'unsigned int' type.
 *
 * Beware: Reading over the end of the bitstream causes a rogue memory access
 * outside of the buffer boundary! Boundary check has to be implemented in the
 * client code!
 * 
 * If you want to ensure you're not reading out of bounds, 
 * use dlb_bitbuf_read() instead, which implements an implicit bounds 
 * check.
 * 
 * If you instantly have to check for out of bounds errors while reading, use 
 * dlb_bitbuf_safe_read() instead.
 *
 * For portability, `n` should not be larger than 16 bits (minimum size of 'int'
 * in C90). Use dlb_bitbuf_read_long() if `n` is larger.
 *
 * @return Right-aligned bits read from bitstream.
 */
unsigned int
dlb_bitbuf_fast_read
    (dlb_bitbuf_handle p_bitbuf    /**< [in,out] Handle to allocated and initialized #dlb_bitbuf instance. */
    ,unsigned int      n           /**< [in] Number of bits to read. */
    );

/**
 * @brief Read bits into 'unsigned long' data type without bounds check.
 *
 * Read `n` bits from the current position of bitstream `p_bitbuf` and return
 * them right-aligned in a 'unsigned long' type.
 * 
 * Beware: Reading over the end of the bitstream causes a rogue memory access
 * outside of the buffer boundary! Boundary check has to be implemented in the
 * client code!
 * 
 * If you want to ensure you're not reading out of bounds, 
 * use dlb_bitbuf_read_long() instead, which implements an implicit bounds 
 * check.
 * 
 * If you instantly have to check for out of bounds errors while reading, use 
 * dlb_bitbuf_safe_read_long() instead.
 *
 * For portability, `n` should not be larger than 32 bits (minimum size of
 * 'long' in C90).
 *
 * @return Right-aligned bits read from bitstream.
 */
unsigned long
dlb_bitbuf_fast_read_long
    (dlb_bitbuf_handle p_bitbuf    /**< [in,out] Handle to allocated and initialized #dlb_bitbuf instance. */
    ,unsigned int      n           /**< [in] Number of bits to read. */
    );

/**
 * @brief Safely read bits into 'unsigned int' data type.
 *
 * Read `n` bits from the current position of bitstream `p_bitbuf` and write
 * them right-aligned into the memory pointed to by `p_data`.
 *
 * For portability, `n` should not be larger than 16 bits (minimum size of 
 * 'int' in C90). Use dlb_bitbuf_safe_read_long() if `n` is larger.
 *
 * In contrast to dlb_bitbuf_read(), this function provides an 'active' 
 * boundary check, i.e. it instantly returns an 'out of bounds' error if 
 * there are not enough bits left to read in the bitstream buffer.
 * 
 * In case this error code is returned,
 * - nothing is read from the buffer and
 * - no internal states are updated.
 *
 * @return 0: success,
 *         1: out of bounds (read over buffer boundary)
 */
int
dlb_bitbuf_safe_read
    (dlb_bitbuf_handle p_bitbuf    /**< [in,out] Handle to allocated and initialized #dlb_bitbuf instance. */
    ,unsigned int      n           /**< [in] Number of bits to read. */
    ,unsigned int     *p_data      /**< [out] Right-aligned bits read from bitstream. */
    );

/**
 * @brief Safely read bits into 'unsigned long' data type.
 *
 * Read `n` bits from the current position of bitstream `p_bitbuf` and write
 * them right-aligned into the memory pointed to by `p_data`.
 *
 * For portability, `n` should not be larger than 32 bits (minimum size of
 * 'long' in C90).
 *
 * In contrast to dlb_bitbuf_read_long(), this function provides an 'active' 
 * boundary check, i.e. it instantly returns an 'out of bounds' error if 
 * there are not enough bits left to read in the bitstream buffer.
 * 
 * In case this error code is returned,
 * - nothing is read from the buffer and
 * - no internal states are updated.
 *
 * @return 0: success,
 *         1: out of bounds (read over buffer boundary)
 */
int
dlb_bitbuf_safe_read_long
    (dlb_bitbuf_handle p_bitbuf    /**< [in,out] Handle to allocated and initialized #dlb_bitbuf instance. */
    ,unsigned int      n           /**< [in] Number of bits to read. */
    ,unsigned long    *p_data      /**< [out] Right-aligned bits read from bitstream. */
    );

/**
 * @brief Peek into bitstream and return 'unsigned int' type.
 *
 * Same functionality as dlb_bitbuf_read(), but no updates to the current
 * position in `p_bitbuf`.
 *
 * If you instantly have to check for out of bounds errors while peeking, use 
 * dlb_bitbuf_safe_peek() instead.
 *
 * @return Right-aligned bits read from bitstream.
 */
unsigned int
dlb_bitbuf_peek
    (dlb_bitbuf_handle p_bitbuf    /**< [in] Handle to allocated and initialized ` dlb_bitbuf ` instance. */
    ,unsigned int      n           /**< [in] Number of bits to read. */
    );

/**
 * @brief Peek into bitstream and return 'unsigned long' type.
 *
 * Same functionality as dlb_bitbuf_read_long(), but no updates to the 
 * current position in `p_bitbuf`.
 *
 * If you instantly have to check for out of bounds errors while peeking, use 
 * dlb_bitbuf_safe_peek_long() instead.
 *
 * @return Right-aligned bits read from bitstream.
 */
unsigned long
dlb_bitbuf_peek_long
    (dlb_bitbuf_handle p_bitbuf    /**< [in] Handle to allocated and initialized #dlb_bitbuf instance. */
    ,unsigned int      n           /**< [in] Number of bits to read. */
    );

/**
 * @brief Peek into bitstream and return 'unsigned int' type without bounds check.
 *
 * Same functionality as dlb_bitbuf_read(), but no updates to the current
 * position in `p_bitbuf`.
 *
 * Beware: Peeking over the end of the bitstream causes a rogue memory access
 * outside of the buffer boundary! Boundary check has to be implemented in the
 * client code!
 * 
 * If you want to ensure you're not peeking out of bounds, 
 * use dlb_bitbuf_peek() instead, which implements an implicit bounds 
 * check.
 * 
 * If you instantly have to check for out of bounds errors while peeking, use 
 * dlb_bitbuf_safe_peek() instead.
 *
 * @return Right-aligned bits read from bitstream.
 */
unsigned int
dlb_bitbuf_fast_peek
    (dlb_bitbuf_handle p_bitbuf    /**< [in] Handle to allocated and initialized ` dlb_bitbuf ` instance. */
    ,unsigned int      n           /**< [in] Number of bits to read. */
    );

/**
 * @brief Peek into bitstream and return 'unsigned long' type without bounds check.
 *
 * Same functionality as dlb_bitbuf_read_long(), but no updates to the 
 * current position in `p_bitbuf`.
 *
 * Beware: Peeking over the end of the bitstream causes a rogue memory access
 * outside of the buffer boundary! Boundary check has to be implemented in the
 * client code!
 * 
 * If you want to ensure you're not peeking out of bounds, 
 * use dlb_bitbuf_peek_long() instead, which implements an implicit bounds 
 * check.
 * 
 * If you instantly have to check for out of bounds errors while peeking, use 
 * dlb_bitbuf_safe_peek_long() instead.
 *
 * @return Right-aligned bits read from bitstream.
 */
unsigned long
dlb_bitbuf_fast_peek_long
    (dlb_bitbuf_handle p_bitbuf    /**< [in] Handle to allocated and initialized #dlb_bitbuf instance. */
    ,unsigned int      n           /**< [in] Number of bits to read. */
    );

/**
 * @brief Peek into bitstream and return 'unsigned int' type.
 *
 * Same functionality as dlb_bitbuf_safe_read(), but no updates to the 
 * current position in `p_bitbuf`.
 *
 * @return 0: success,
 *         1: out of bounds (read over buffer boundary)
 */
int
dlb_bitbuf_safe_peek
    (dlb_bitbuf_handle p_bitbuf    /**< [in] Handle to allocated and initialized #dlb_bitbuf instance. */
    ,unsigned int      n           /**< [in] Number of bits to read. */
    ,unsigned int     *p_data      /**< [out] Right-aligned bits read from bitstream. */
    );

/**
 * @brief Peek into bitstream and return 'unsigned long' type.
 *
 * Same functionality as dlb_bitbuf_safe_read_long(), but no updates to the
 * current position in `p_bitbuf`.
 *
 * @return 0: success,
 *         1: out of bounds (read over buffer boundary)
 */
int
dlb_bitbuf_safe_peek_long
    (dlb_bitbuf_handle p_bitbuf    /**< [in] Handle to allocated and initialized #dlb_bitbuf instance. */
    ,unsigned int      n           /**< [in] Number of bits to read. */
    ,unsigned long    *p_data      /**< [out] Right-aligned bits read from bitstream. */
    );

#ifdef __cplusplus
}
#endif
#endif /* DLB_BITBUF_READ_H */
/*@}*/
