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
 * @defgroup DLB_BITBUF_API Bit Buffer Management API Reference
 *
 * Interface description of the dlb_bitbuf component, a component which
 * provides sequential access to single or multiple bits in a bitstream.
 *
 * ## Functionality of dlb_bitbuf Library
 *
 * The functionality of the library includes
 * - Common Functions
 *     - Set up a bitstream instance with dlb_bitbuf_init().
 *     - Access status information from the instance with
 *       dlb_bitbuf_get_abs_pos() and dlb_bitbuf_get_bits_left().
 *     - Take care of alignment with dlb_bitbuf_align() and
 *       dlb_bitbuf_get_alignment_bits().
 *     - Jump to an absolute or relative position in the bistream with
 *       dlb_bitbuf_set_bitpos() and dlb_bitbuf_skip(), respectively.
 * - Bitstream Reading Functions
 *     - Read bits from the bitstream with dlb_bitbuf_read(), dlb_bitbuf_peek()
 *       or their _long() equivalents. Note, that these functions only provide
 *       a passive boundary check, i.e. they
 *         -  do not access memory beyond the end of the buffer, but
 *         -  fill the read pattern with zeros instead.
 *       They do not provide an error code.
 *     - Read 'safely' from the bitstream, i.e. including boundary checks,
 *       with dlb_bitbuf_safe_read() or dlb_bitbuf_safe_peek(), and their
 *       _long() equivalents.
 *     - Read from the bitstream in a 'quick and dirty' manner without
 *       boundary checks, using dlb_bitbuf_fast_read(), dlb_bitbuf_fast_peek()
 *       or their _long() equivalents. These functions are optimized for
 *       high performance.
  * - Bitstream Writing  Functions
 *     - Write bits to the bitstream with dlb_bitbuf_write() or
 *       dlb_bitbuf_write_long(), including boundary checks.
 *
 * Note that reading and writing functionality has to be activated by a
 * the hash defines DLB_BITBUF_USE_READ_API and DLB_BITBUF_USE_WRITE_API,
 * respectively. Further information is provided in the component's README.txt.
 *
 * ## Accessing the Bitstream
 *
 * The bitstream is stored in memory as an array of bytes. Access is provided
 * by using pointers of type #DLB_BITBUF_DATATYPE, which represent a 'word' of
 * the bitstream.
 *
 * On a typical platform, #DLB_BITBUF_DATATYPE is mapped to an unsigned char,
 * and an unsigned char has 8 bits. Hence a 'word' is an unsigned char of 8
 * bits, and everyone is happy.
 *
 * ## Memory Optimization On Non-8-bit-char Machines
 *
 * On platforms where no 8-bit data type is available (i.e. no single byte
 * access supported), mapping #DLB_BITBUF_DATATYPE to a natively supported type
 * (e.g. unsigned short) may be required to prevent a waste of memory. Hence, a
 * 'word' equals the smallest natively supported type.
 *
 * This potential memory optimization is still in experimental state.
 *
 * ## Bitstream Buffer Size Limit
 * Bitstream buffer sizes up to 256 MB (2^31 bits) are supported.
 *
 * ## Access Restrictions of dlb_bitbuf Structure
 * To be future-proof, the #dlb_bitbuf struct shall be treated as an opaque
 * data type. That means, its members are considered 'private' and shall not
 * be directly accessed by external functions. Instead, merely API functions
 * shall be used.
 *
 * Memory allocation and copying of the structure mark exceptions to this rule
 * (see below).
 *
 * ## Usage Recommendations
 * The component does not provide any functions for querying the memory size
 * needed to allocate an instance of #dlb_bitbuf. Neither does it provide a
 * function to duplicate an instance. Alternative solutions are provided below.
 *
 * ### Memory Allocation
 * Memory may be allocated
 *     - on the heap by, e.g.,
 *         \code
 *         dlb_bitbuf_handle h_bitbuf;
 *         h_bitbuf = malloc(sizeof(dlb_bitbuf));
 *         \endcode
 *     - or on the stack by, e.g.,
 *         \code
 *         dlb_bitbuf bitbuf;
 *         dlb_bitbuf_handle h_bitbuf;
 *         h_bitbuf = &bitbuf;
 *         \endcode
 *
 * ### Duplicating a #dlb_bitbuf Instance
 * To copy an instance of #dlb_bitbuf, you can use
 *     - memcpy() for heap allocation
 *         \code
 *         dlb_bitbuf_handle src, dst;
 *         ...
 *         memcpy(dst,src,sizeof(dlb_bitbuf))
 *         \endcode
 *     - struct assignment for stack allocation
 *         \code
 *         dlb_bitbuf src, dst;
 *         ...
 *         dst = src;
 *         \endcode
 */
/*@{*/
/**
 * @file
 * @brief  Bit Buffer Management Interface
 */

#ifndef DLB_BITBUF_H
#define DLB_BITBUF_H

#include <limits.h>  /* for CHAR_BIT */

#include "bitbuf_symbol_rename.inc"

#ifdef __cplusplus
extern "C"
{
#endif
/* @{ */
/**
 * @name Library version info
 */
#define DLB_BITBUF_V_API  2    /**< @brief API version. */
#define DLB_BITBUF_V_FCT  0    /**< @brief Functional version. */
#define DLB_BITBUF_V_MTNC 6    /**< @brief Maintenance version. */
/* @} */

/* @{ */
#ifndef DLB_BITBUF_DATATYPE
/**
 * @name Potential modification of the bitbuf datatype
 * Modification of #DLB_BITBUF_DATATYPE could result in a memory optimization on
 * non-8-bit-char machines.
 * See API description for details.
 */
#define DLB_BITBUF_DATATYPE unsigned char    /**< @brief Data type of bitstream buffer. */
#define DLB_BITBUF_WIDTH    CHAR_BIT         /**< @brief Width of bitstream buffer data type. */
#endif
/* @} */

#if !defined(DLB_BITBUF_DATATYPE) || !defined(DLB_BITBUF_WIDTH)
#error "Both DLB_BITBUF_DATATYPE and DLB_BITBUF_WIDTH need to be defined!"
#endif

#define DLB_BITBUF_MAX_BUFFER_SIZE 0x7FFFFFFFUL    /**< @brief Maximum allowed buffer size (256 MB) in bits. */

/* @{ */
/**
 * @name Error codes
 * The API functions of the dlb_bitbuf library partially return error codes,
 * which need to be handled at a higher level.
 *
 * For performance reasons, bad input parameters (e.g., NULL pointers) do
 * generally not result in error codes. Instead, assert()s are used.
 */
#define DLB_BITBUF_NO_ERROR            0x0000    /**< @brief No error occurred. */
#define DLB_BITBUF_ERROR_OUT_OF_BOUNDS 0x0001    /**< @brief Tried to write or jump to memory outside of the bitstream buffer
                                                  * boundaries. */
/* @} */

/**
 * @brief Version definition structure for component dlb_bitbuf.
 *
 * Assigning the defined macro values.
 */
typedef struct dlb_bitbuf_version_info_s
{
    int         v_api;     /**< API version. */
    int         v_fct;     /**< Functional change. */
    int         v_mtnc;    /**< Maintenance release. */
    const char *text;      /**< Optional, set to NULL if unused. */
} dlb_bitbuf_version_info;
/*@}*/

/** @addtogroup DLB_BITBUF_SRC */
/*@{*/
/**
 * @brief Bitstream Buffer Object
 *
 * The information below is only relevant for porting purposes.
 *
 * **Client code shall NOT access individual struct members directly.**
 *
 * The pointer `p_base` points to the first word in the bitstream buffer. The
 * bitstream is assumed to start word-aligned in `p_base`, i.e. without a bit
 * offset.
 *
 * The current position in the bistream is precisely described by `p_cur` and
 * `bit_pos`. The pointer `p_cur` points to the current word, and `bit_pos`
 * indicates the next bit, where
 *     - data is read from or
 *     - data will be written to.
 *
 * Hereby the read/write direction is assumed to be from MSB to LSB, i.e.
 * `bit_pos = 0` adresses the bit with the value `1<<(DLB_BITBUF_WIDTH-1)` in
 * the word `*p_cur`.
 *
 * The element `bits_left` always contains the number of bits which are left
 * between the current position and the end of the bitstream buffer. At
 * initialization, it is set to the buffer size. It is continuously updated
 * whenever the current position changes, thus
 * enables boundary checks.
 */
typedef struct dlb_bitbuf_s
{
    DLB_BITBUF_DATATYPE *p_base;       /**< Pointer to first word in bitstream buffer. */
    DLB_BITBUF_DATATYPE *p_cur;        /**< Pointer to current word in bitstream buffer. */
    unsigned int         bit_pos;      /**< Position of next bit to be accessed, `0 <= bit_pos < DLB_BITBUF_WIDTH`. */
    long                 bits_left;    /**< Number of bits left between current position and end of buffer. A negative value in
                                        * `bits_left` indicates a buffer underrun when reading, or overflow when writing. */
} dlb_bitbuf;
/*@}*/
/** @addtogroup DLB_BITBUF_API */
/*@{*/

/**
 * @brief Pointer to bit buffer structure
 */
typedef struct dlb_bitbuf_s *dlb_bitbuf_handle;

/**
 * @brief Get component dlb_bitbuf version.
 *
 * @return Version definition structure.
 */
const dlb_bitbuf_version_info *
dlb_bitbuf_get_version
    (void
    );

/**
 * @brief Initialize a bitstream buffer.
 *
 * Initialize internal pointers and states of a bistream buffer instance. The
 * current read/write position is set to `p_base`, i.e., to the beginning of
 * the bitstream.
 *
 * The parameter `bitbuf_size` should be set to the size of the bitstream (if
 * known in advance). If unknown, `bitbuf_size` should be set to the size of
 * the memory allocated for `p_base`.
 *
 * Note: buffer size must be < 256 MB (2^31 bits).
 */
void
dlb_bitbuf_init
    (dlb_bitbuf_handle    p_bitbuf       /**< [in,out] Handle to previously allocated #dlb_bitbuf instance. */
    ,DLB_BITBUF_DATATYPE *p_base         /**< [in] Pointer to the beginning of the bitstream buffer. */
    ,unsigned long        bitbuf_size    /**< [in] Size of bitstream buffer in bits, `0 <= bitbuf_size < 2^31`. */
    );

/**
 * @brief Skip bits forward or reverse.
 *
 * Moves the current bitstream position by `num_bits` forward (if num_bits > 0)
 * or backward (if num_bits < 0), unless the move would go *out of* the buffer
 * boundaries.
 *
 * Note, that it _is_ possible to skip back into the buffer boundaries if you
 * previously have read over the buffer boundaries using dlb_bitbuf_read() or
 * dlb_bitbuf_read_long().
 *
 * @return 0: success,
 *         1: out of bounds (skip over buffer boundaries)
 */
int
dlb_bitbuf_skip
    (dlb_bitbuf_handle p_bitbuf    /**< [in,out] Handle to allocated and initialized #dlb_bitbuf instance. */
    ,long              num_bits    /**< [in] Number of bits to skip forward (if positive) or backward (if negative),
                                    * `-2^31 <= num_bits <= 2^31-1`.*/
    );

/**
 * @brief Jump to an absolute bit position.
 *
 * Set the current position in p_bitbuf to bit_pos, unless it is not within
 * the buffer boundaries.
 *
 * @return 0: success,
 *         1: out of bounds (`abs_bit_pos` > buffer size)
 */
int
dlb_bitbuf_set_abs_pos
    (dlb_bitbuf_handle p_bitbuf       /**< [in,out] Handle to allocated and initialized #dlb_bitbuf instance. */
    ,unsigned long     abs_bit_pos    /**< [in] Bit position relative to beginning of p_bitbuf, `0 <= abs_bit_pos <= buffer size. */
    );

/**
 * @brief Align current bitstream position to next byte border.
 *
 * Move the current position of `p_bitbuf` to next byte border to achieve 8-bit
 * alignment, unless
 * - we're already aligned to byte border (=> nothing happens) or
 * - this move would go over the buffer boundaries (=> return error code 'out
 * of bounds').
 *
 * @return 0: success,
 *         1: out of bounds (alignment went over buffer boundaries)
 */
int
dlb_bitbuf_align
    (dlb_bitbuf_handle p_bitbuf    /**< [in,out] Handle to allocated and initialized #dlb_bitbuf instance. */
    );

/**
 * @brief Get the bits needed for alignment to next byte border.
 *
 * Get the number of bits that are needed to align to the next byte border,
 * regardless of #DLB_BITBUF_WIDTH.
 *
 * Examples:
 *    - `bit_pos=3` => return value is 5
 *    - `bit_pos=14` => return value is 2.
 *
 * @return 0 <= alignment_bits <= 7.
 */
unsigned int
dlb_bitbuf_get_alignment_bits
    (dlb_bitbuf_handle p_bitbuf    /**< [in] Handle to allocated and initialized #dlb_bitbuf instance. */
    );

/**
 * @brief Get the current absolute position in bits.
 *
 * Get the number of bits between the beginning of the bitstream and the current
 * position in `p_bitbuf`.
 *
 * @return Absolute position in bits.
 */
unsigned long
dlb_bitbuf_get_abs_pos
    (dlb_bitbuf_handle p_bitbuf    /**< [in] Handle to allocated and initialized #dlb_bitbuf instance. */
    );

/**
 * @brief Return number of bits left.
 *
 * Get the number of bits left in `p_bitbuf`, i.e. the number of bits between
 * current position and the end of the buffer.
 * If the return value is negative, we have read or skipped over the end of the
 * buffer.
 *
 * @return Number of bits left in buffer.
 */
long
dlb_bitbuf_get_bits_left
    (dlb_bitbuf_handle p_bitbuf    /**< [in] Handle to allocated and initialized #dlb_bitbuf instance. */
    );

/* Include bitstream read() and peek() functionality */
#ifdef DLB_BITBUF_USE_READ_API
#include "dlb_bitbuf_read.h"
#endif

/* Include bitstream write() functionality */
#ifdef DLB_BITBUF_USE_WRITE_API
#include "dlb_bitbuf_write.h"
#endif

#ifdef __cplusplus
}
#endif

#endif /* DLB_BITBUF_H */
/*@}*/
