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
  @brief  Bit Buffer Management - Write Functions
*/

#include "stdio.h"
#include "assert.h"
#include "dlb_bitbuf_write.h"
#include "bitbuf_pvt.h"

/*
 * Description provided in dlb_bitbuf.h.
 */
int
dlb_bitbuf_write( dlb_bitbuf_handle p_bitbuf      
                 ,unsigned int      data         
                 ,unsigned int      n           
                 )
{
    DLB_BITBUF_DATATYPE *p_data;
    unsigned int         bit;

    /* Parameter consistency checks */
    ASSERT_BITBUF_SANITY(p_bitbuf);
    assert(n <= 16);            /* Check max pattern size */
    assert(data >> (n-1) <= 1);     /* Check for out-of-pattern bits */

    p_data  = p_bitbuf->p_cur;
    bit     = DLB_BITBUF_WIDTH - p_bitbuf->bit_pos;

    /* Enough bits left in buffer? */
    if ((long) n > p_bitbuf->bits_left)
    {
        return 1;
    }
    /* Exit early for trivial case */
    if (n == 0)         return 0;
    
    /* Update bits_left */
    p_bitbuf->bits_left -= n;

    /* Write to the end of the current word */
    while (n >= bit)
    {
        int bs_word; 

        bs_word  = *p_data;
        bs_word &= ~BIT_MASK(bit);
        bs_word |= (data >> (n - bit) & BIT_MASK(bit));

        *p_data++ = (DLB_BITBUF_DATATYPE) bs_word;
        n -= bit;
        bit = DLB_BITBUF_WIDTH;
    }

    /* Write remaing bits */
    if (n > 0)
    {
        int mask = BIT_MASK(n) << (bit - n);

        *p_data  = (DLB_BITBUF_DATATYPE) (*p_data & ~mask);
        *p_data |= (DLB_BITBUF_DATATYPE) (data << (bit - n)); 
        bit -= n;
    }

    /* Update remaining states */
    p_bitbuf->p_cur   = p_data;
    p_bitbuf->bit_pos = DLB_BITBUF_WIDTH - bit;

    return 0;
}

/*
 * Description provided in dlb_bitbuf.h.
 */
int
dlb_bitbuf_write_long( dlb_bitbuf_handle p_bitbuf      
                      ,unsigned long     data         
                      ,unsigned int      n           
                      )
{ 
    DLB_BITBUF_DATATYPE *p_data;
    unsigned int         bit;

    /* Parameter consistency checks */
    ASSERT_BITBUF_SANITY(p_bitbuf);
    assert(n <= 32);            /* Check max pattern size */ 
    assert(data >> (n-1) <= 1);     /* Check for out-of-pattern bits */
    
    p_data  = p_bitbuf->p_cur;
    bit     = DLB_BITBUF_WIDTH - p_bitbuf->bit_pos;

    /* Enough bits left in buffer? */
    if ((long) n > p_bitbuf->bits_left)
    {
        return 1;
    }
    /* Exit early for trivial case */
    if (n == 0)         return 0;
    
    /* Update bits_left */
    p_bitbuf->bits_left -= n;

    /* Write to the end of the current word */
    while (n >= bit)
    {
        long bs_word; 

        bs_word  = *p_data;
        bs_word &= ~BIT_MASK(bit);
        bs_word |= (data >> (n - bit) & BIT_MASK(bit));

        *p_data++  = (unsigned char) bs_word;

        n  -= bit;
        bit = DLB_BITBUF_WIDTH;
    }

    /* Write remaing bits */
    if (n > 0)
    {
        int mask = BIT_MASK(n) << (bit - n);

        *p_data  = (DLB_BITBUF_DATATYPE) (*p_data & ~mask);
        *p_data |= (DLB_BITBUF_DATATYPE) (data << (bit - n)); 

        bit -= n;
    }

    /* Update remaining states */
    p_bitbuf->p_cur   = p_data;
    p_bitbuf->bit_pos = DLB_BITBUF_WIDTH - bit;

    return 0;
}
/*@}*/
