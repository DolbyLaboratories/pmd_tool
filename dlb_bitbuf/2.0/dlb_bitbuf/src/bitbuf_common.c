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

/** @addtogroup DLB_BITBUF_SRC */
/*@{*/
/**
  @file
  @brief  Bit Buffer Management - Common Functions
*/
#include "stdio.h"
#include "assert.h"
#include "dlb_bitbuf.h"
#include "bitbuf_pvt.h"


static const dlb_bitbuf_version_info v =
{
    DLB_BITBUF_V_API,
    DLB_BITBUF_V_FCT,
    DLB_BITBUF_V_MTNC,
    NULL
};

/*
 * Description provided in bitbuf_pvt.h.
 */ 
const dlb_bitbuf_version_info *
dlb_bitbuf_get_version
    (void
    )
{
    return &v;
}

/*
 * Description provided in bitbuf_pvt.h.
 */ 
void 
skip_bits_forward
    (dlb_bitbuf_handle p_bitbuf
    ,long              num_bits
    )
{
    const long bit_mask = BIT_MASK(DLB_BITBUF_WIDTH_SHIFT);

    long abs_words_to_skip;
    int abs_bits_to_skip, new_bit_pos;

    /* Exit early for trivial case */
    if (num_bits == 0) return;

    abs_words_to_skip  = num_bits >> DLB_BITBUF_WIDTH_SHIFT;
    abs_bits_to_skip  = (int) (num_bits & bit_mask);

    new_bit_pos   = p_bitbuf->bit_pos + abs_bits_to_skip;

    /* Check for carry in new bit position */
    if (new_bit_pos >= DLB_BITBUF_WIDTH) 
    {
        abs_words_to_skip++;
        new_bit_pos -= DLB_BITBUF_WIDTH;
    }

    /* Sanity check */
    assert(new_bit_pos < DLB_BITBUF_WIDTH);

    /* Update state */
    p_bitbuf->p_cur     += abs_words_to_skip;
    p_bitbuf->bit_pos    = (unsigned int) new_bit_pos;
    p_bitbuf->bits_left -= num_bits;
}

/*
 * Description provided in dlb_bitbuf.h.
 */
void
dlb_bitbuf_init( dlb_bitbuf_handle    p_bitbuf     
                ,DLB_BITBUF_DATATYPE *p_base      
                ,unsigned long        bitbuf_size
                )
{
    /* Parameter consistency checks */
    assert(p_bitbuf != 0);
    assert(p_base   != 0);

    assert(bitbuf_size <= DLB_BITBUF_MAX_BUFFER_SIZE);
    
    /* Initialize struct members */
    p_bitbuf->p_base = p_base;
    p_bitbuf->p_cur = p_base;
    p_bitbuf->bit_pos = 0;
    p_bitbuf->bits_left = bitbuf_size;
}

/*
 * Description provided in dlb_bitbuf.h.
 */
int
dlb_bitbuf_skip( dlb_bitbuf_handle p_bitbuf 
                ,long              num_bits
                )
{
    const long bit_mask = BIT_MASK(DLB_BITBUF_WIDTH_SHIFT);

    /* Parameter consistency checks */
    assert(p_bitbuf);
    assert(p_bitbuf->p_cur >= p_bitbuf->p_base);

    if (num_bits >= 0) 
    {
        /* Bail out if we would skip over buffer end */
        if (num_bits > p_bitbuf->bits_left)
            return DLB_BITBUF_ERROR_OUT_OF_BOUNDS;

        skip_bits_forward(p_bitbuf, num_bits);
    } 
    else /* num_bits < 0 */
    {
        int old_bit_pos, new_bit_pos, abs_bits_to_skip;
        long  abs_words_to_skip;

        old_bit_pos       = (int) p_bitbuf->bit_pos;
        abs_words_to_skip = -num_bits >> DLB_BITBUF_WIDTH_SHIFT;
        abs_bits_to_skip  = -num_bits & bit_mask;
        
        new_bit_pos = old_bit_pos - abs_bits_to_skip;

        /* Check for borrow in new bit position */
        if (new_bit_pos < 0) 
        {
            abs_words_to_skip++;
            new_bit_pos += DLB_BITBUF_WIDTH;
        }

        /* Bail out if we would skip reverse over buffer begin */
        if (p_bitbuf->p_cur - abs_words_to_skip < p_bitbuf->p_base)
            return DLB_BITBUF_ERROR_OUT_OF_BOUNDS;

        /* Sanity check */
        assert(new_bit_pos < DLB_BITBUF_WIDTH);

        /* Update state */
        p_bitbuf->p_cur     -= abs_words_to_skip;
        p_bitbuf->bit_pos    = (unsigned int) new_bit_pos;
        p_bitbuf->bits_left -= num_bits;
    }
    return DLB_BITBUF_NO_ERROR;
}

/*
 * Description provided in dlb_bitbuf.h.
 */
int
dlb_bitbuf_set_abs_pos( dlb_bitbuf_handle p_bitbuf  
                       ,unsigned long     abs_bit_pos  
                      )
{
    DLB_BITBUF_DATATYPE *p_cur_new;
    int bit_pos_diff, bit_pos_new;

    long word_diff, word_pos_new, bits_left_new;

    const unsigned long bit_mask = ((1 << DLB_BITBUF_WIDTH_SHIFT) -1);

    /* Parameter consistency checks */
    assert(p_bitbuf);
    assert(p_bitbuf->p_cur >= p_bitbuf->p_base);

    /* Transform abs_bit_pos into ptr+bit_pos */
    word_pos_new   = (long) (abs_bit_pos >> DLB_BITBUF_WIDTH_SHIFT);
    bit_pos_new    = abs_bit_pos & bit_mask;
    p_cur_new      = p_bitbuf->p_base + word_pos_new;
    
    /* Calculate bits_left */
    word_diff     = (long) (p_cur_new - p_bitbuf->p_cur);
    bit_pos_diff  = bit_pos_new - p_bitbuf->bit_pos;

    bits_left_new  = p_bitbuf->bits_left - ((word_diff << DLB_BITBUF_WIDTH_SHIFT) + bit_pos_diff);

    /* Check if new position is within buffer boundaries */
    if (bits_left_new < 0)
        return DLB_BITBUF_ERROR_OUT_OF_BOUNDS;

    /* Sanity check */
    assert(bit_pos_new < DLB_BITBUF_WIDTH);

    /* Update states */
    p_bitbuf->p_cur     = p_cur_new;
    p_bitbuf->bit_pos   = (unsigned int) bit_pos_new;
    p_bitbuf->bits_left = bits_left_new;

    return DLB_BITBUF_NO_ERROR;
}

/*
 * Description provided in dlb_bitbuf.h.
 */
int
dlb_bitbuf_align( dlb_bitbuf_handle p_bitbuf  
                )
{
    unsigned int alignment_bits; 

    /* Parameter consistency checks */
    assert(p_bitbuf);
    assert(p_bitbuf->p_cur >= p_bitbuf->p_base);

    /* Get bits after last byte alignment */
    alignment_bits = dlb_bitbuf_get_alignment_bits(p_bitbuf);

    /* Skip bits and check for out of bounds */
    return dlb_bitbuf_skip(p_bitbuf, (long) alignment_bits);
}

/*
 * Description provided in dlb_bitbuf.h.
 */
unsigned int
dlb_bitbuf_get_alignment_bits( dlb_bitbuf_handle p_bitbuf  
                             )
{
    unsigned int bit_pos_fraction;

    /* Parameter consistency checks */
    assert(p_bitbuf);
    assert(p_bitbuf->p_cur >= p_bitbuf->p_base);

    /* Get bits after last byte alignment */
    bit_pos_fraction = p_bitbuf->bit_pos & BIT_MASK(3);

    /* Sanity check */
    assert(bit_pos_fraction < DLB_BITBUF_WIDTH);

    /* Return alignment bits in case we're not aligned */
    if (bit_pos_fraction)
        return (unsigned int) (8 /* byte size in bits */ - bit_pos_fraction);

    return 0; 
}

/*
 * Description provided in dlb_bitbuf.h.
 */
unsigned long 
dlb_bitbuf_get_abs_pos( dlb_bitbuf_handle p_bitbuf  
                       )
{
    /* Parameter consistency checks */
    assert(p_bitbuf);
    assert(p_bitbuf->p_cur >= p_bitbuf->p_base);

    return ((unsigned long) (p_bitbuf->p_cur - p_bitbuf->p_base) << DLB_BITBUF_WIDTH_SHIFT) + p_bitbuf->bit_pos;
}

/*
 * Description provided in dlb_bitbuf.h.
 */
long
dlb_bitbuf_get_bits_left( dlb_bitbuf_handle p_bitbuf   
                         )
{
    /* Parameter consistency checks */
    assert(p_bitbuf);

    return (p_bitbuf->bits_left);
}
/*@}*/
