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

/** @addtogroup DLB_BITBUF_SRC */
/*@{*/
/**
  @file
  @brief  Bit Buffer Management - Read and Peek Functions
*/

#include "stdio.h"
#include "assert.h"
#include "dlb_bitbuf_read.h"
#include "bitbuf_pvt.h"

static inline unsigned long
dlb_bitbuf_fast_read_long_core( dlb_bitbuf_handle p_bitbuf      
                          ,unsigned int      n            
                          )
{
    unsigned int bit_pos;
    unsigned int top_bit, bottom_bit;
    DLB_BITBUF_DATATYPE * p_data;
    unsigned long data;

    ASSERT_BITBUF_SANITY(p_bitbuf);
    assert(n <= 32);
    assert(n <= (unsigned int) p_bitbuf->bits_left);

    p_data = p_bitbuf->p_cur;
    bit_pos = p_bitbuf->bit_pos;

    p_bitbuf->bits_left -= n;
    p_bitbuf->bit_pos += n;
    p_bitbuf->p_cur += p_bitbuf->bit_pos / DLB_BITBUF_WIDTH;
    p_bitbuf->bit_pos %= DLB_BITBUF_WIDTH;

    top_bit = DLB_BITBUF_WIDTH - bit_pos;
    data = (unsigned long) *p_data;
    data &= BIT_MASK(top_bit);
    bottom_bit = top_bit > n ? top_bit - n : 0;
    data >>= bottom_bit;
    n -= top_bit - bottom_bit;

    while (n >= DLB_BITBUF_WIDTH)
    {
        p_data++;
        data <<= DLB_BITBUF_WIDTH;
        data |= (unsigned long) *p_data;
        n -= DLB_BITBUF_WIDTH;
    }

    if (n)
    {
        p_data++;
        data <<= n;
        data |= (unsigned long) (*p_data >> (DLB_BITBUF_WIDTH - n));
    }


    return data;
}

/*
 * Description provided in dlb_bitbuf.h.
 */
unsigned int
dlb_bitbuf_fast_read( dlb_bitbuf_handle p_bitbuf      
                     ,unsigned int      n            
                    )
{
    assert(n <= 16);
    return (unsigned int) dlb_bitbuf_fast_read_long_core(p_bitbuf, n);
}


/*
 * Description provided in bitbuf_pvt.h.
 */
static unsigned int
read_and_zero_fill( dlb_bitbuf_handle p_bitbuf      
                   ,unsigned int      n            
                  )
{
    long bits_left;
    unsigned int bit_pos;
    int valid_bits, zero_fill, bits_to_read;
    long data_long;

    DLB_BITBUF_DATATYPE * p_data;

    /* Parameter consistency checks */
    ASSERT_BITBUF_SANITY(p_bitbuf);
    assert(n <= 16);

    p_data    = p_bitbuf->p_cur;
    bit_pos   = p_bitbuf->bit_pos;
    bits_left = p_bitbuf->bits_left;

    bits_to_read = (int) bits_left;
    zero_fill    = n - (int) bits_left;
    data_long    = 0L;
    valid_bits   = 0;

    /* Reading remaining bits from current word if there are any */
    if ( bits_to_read > 0)
    {
        valid_bits = DLB_BITBUF_WIDTH - bit_pos;
        data_long  = (unsigned long) (*(p_data++) & BIT_MASK(valid_bits)); 
    }

    /* Continue reading words from bitstream until we have enough bits */
    while ( bits_to_read > valid_bits )
    {
        data_long <<= DLB_BITBUF_WIDTH;
        data_long |= (unsigned long) (*p_data++);
        valid_bits += DLB_BITBUF_WIDTH;
    }

    /* Shift away superfluous bits */
    data_long >>= valid_bits - bits_to_read;
        
    /* Adjust read position to the proper position outside of buffer boundary */
    p_data  = p_bitbuf->p_cur + (((bit_pos + n) & ~BIT_MASK(DLB_BITBUF_WIDTH_SHIFT)) >> DLB_BITBUF_WIDTH_SHIFT);
    bit_pos = (bit_pos + n) & BIT_MASK(DLB_BITBUF_WIDTH_SHIFT);

    /* Update states */
    p_bitbuf->p_cur      = p_data;
    p_bitbuf->bits_left -= (long) n;
    p_bitbuf->bit_pos    = bit_pos;

    /* Fill up bit pattern with zeros for out-of-bounds values */
    return (unsigned int) (data_long << zero_fill);
}

/*
 * Description provided in dlb_bitbuf.h.
 */
unsigned int
dlb_bitbuf_read( dlb_bitbuf_handle p_bitbuf      
                ,unsigned int      n            
                )
{
    /* Parameter consistency checks */
    ASSERT_BITBUF_SANITY(p_bitbuf);
    assert(n <= 16);

    /* Take care to not read over buffer boundary */
    if ((long) n <= p_bitbuf->bits_left)
    {
        /* 
         * Fast path in case we're not trying to read over the buffer boundary. 
         */
        return dlb_bitbuf_fast_read(p_bitbuf, n);
    } else {
        /* 
         * Carefully read last valid bits and fill the read pattern with zeros. 
         * Update the buffer states as if we had read properly to the position 
         * behind the buffer boundary. 
         * This would even allow for a defined "skip back" to a valid position.
         */
        return read_and_zero_fill(p_bitbuf, n);
    }
}

/*
 * Description provided in dlb_bitbuf.h.
 */
unsigned long
dlb_bitbuf_fast_read_long( dlb_bitbuf_handle p_bitbuf      
                          ,unsigned int      n            
                          )
{
    return dlb_bitbuf_fast_read_long_core(p_bitbuf, n);
}
/*
 * Description provided in bitbuf_pvt.h.
 */
static unsigned long
read_and_zero_fill_long( dlb_bitbuf_handle p_bitbuf      
                        ,unsigned int      n            
                       )
{
    unsigned long data;
    long bits_left;
    int valid_bits, bit_pos, bits_to_read, zero_fill;

    DLB_BITBUF_DATATYPE * p_data;

    /* Parameter consistency checks */
    ASSERT_BITBUF_SANITY(p_bitbuf);
    assert(n <= 32);

    p_data    = p_bitbuf->p_cur;
    bit_pos   = p_bitbuf->bit_pos;
    bits_left = p_bitbuf->bits_left;

    bits_to_read = (int) bits_left;
    zero_fill    = n - (int) bits_left;

    data = 0L;
    valid_bits = 0;

    if (bits_left > 0) {
        /* Extract valid bits from current word */
        valid_bits = DLB_BITBUF_WIDTH - bit_pos;
        data = (unsigned long) (*(p_data) & BIT_MASK(valid_bits)); 
    }
    bits_to_read -= valid_bits;

    /* Continue reading words from bitstream until less than a word is left */
    while ( bits_to_read >= DLB_BITBUF_WIDTH )
    {
        /* read next word and add to data */
        p_data++;
        
        data <<= DLB_BITBUF_WIDTH;
        data |= (unsigned long) (*p_data);

        bits_to_read -= DLB_BITBUF_WIDTH; 
    }

    if ( bits_to_read > 0 )
    {
        DLB_BITBUF_DATATYPE last_word;

        /* read 1 more word */
        p_data++;

        /* Read 0-7 bits from last word */
        data <<= bits_to_read;

        last_word = *p_data;
        data |= (unsigned long) (last_word >> (DLB_BITBUF_WIDTH - bits_to_read));

    } else  {
        /* We've read too many bits, so shift away superfluous bits */
        data >>= -bits_to_read;
    }

    /* Adjust read position to the proper position outside of buffer boundary */
    p_data  = p_bitbuf->p_cur + (((bit_pos + n) & ~BIT_MASK(DLB_BITBUF_WIDTH_SHIFT)) >> DLB_BITBUF_WIDTH_SHIFT);
    bit_pos = (bit_pos + n) & BIT_MASK(DLB_BITBUF_WIDTH_SHIFT);

    /* Update states */
    p_bitbuf->p_cur      = p_data;
    p_bitbuf->bits_left -= (long) n;
    p_bitbuf->bit_pos    = (unsigned int) bit_pos;

    /* Fill up bit pattern with zeros for out-of-bounds values */
    return data << zero_fill;
}

/*
 * Description provided in dlb_bitbuf.h.
 */
unsigned long
dlb_bitbuf_read_long( dlb_bitbuf_handle p_bitbuf      
                     ,unsigned int      n            
                     )
{
    /* Parameter consistency checks */
    ASSERT_BITBUF_SANITY(p_bitbuf);
    assert(n <= 32);

    /* Take care to not read over buffer boundary */
    if ((long) n <= p_bitbuf->bits_left)
    {
        /* 
         * Fast path in case we're not trying to read over the buffer boundary. 
         */
        return dlb_bitbuf_fast_read_long(p_bitbuf, n);
    } else  {
        /* 
         * Carefully read last valid bits and fill the read pattern with zeros. 
         * Update the buffer states as if we had read properly to the position 
         * behind the buffer boundary. 
         * This would even allow for a defined "skip back" to a valid position.
         */
        return read_and_zero_fill_long(p_bitbuf, n);
    }     
}

/*
 * Description provided in dlb_bitbuf.h.
 */
int
dlb_bitbuf_safe_read
    (dlb_bitbuf_handle p_bitbuf    
    ,unsigned int      n          
    ,unsigned int     *p_data    
    )
{
    /* Parameter consistency checks */
    ASSERT_BITBUF_SANITY(p_bitbuf);
    assert(n <= 16); 
    assert(p_data);

    /* Check if we have enough bits left to peek */
    if (p_bitbuf->bits_left < (long) n)
        return DLB_BITBUF_ERROR_OUT_OF_BOUNDS;

    /* Read n bits ahead */
    *p_data = dlb_bitbuf_fast_read(p_bitbuf, n);

    return DLB_BITBUF_NO_ERROR;
}

/*
 * Description provided in dlb_bitbuf.h.
 */
int
dlb_bitbuf_safe_read_long
    (dlb_bitbuf_handle p_bitbuf 
    ,unsigned int      n       
    ,unsigned long    *p_data 
    )
{
    /* Parameter consistency checks */
    ASSERT_BITBUF_SANITY(p_bitbuf);
    assert(n <= 32);

    /* Check if we have enough bits left to peek */
    if (p_bitbuf->bits_left < (long) n)
        return DLB_BITBUF_ERROR_OUT_OF_BOUNDS;

    /* Peek n bits ahead */
    *p_data = dlb_bitbuf_fast_read_long(p_bitbuf, n);

    return DLB_BITBUF_NO_ERROR;
}

/*
 * Description provided in dlb_bitbuf.h.
 */
unsigned int
dlb_bitbuf_fast_peek( dlb_bitbuf_handle p_bitbuf      
                     ,unsigned int      n            
                     )
{
    DLB_BITBUF_DATATYPE * p_cur;
    unsigned int bit_pos;
    long bits_left;
    unsigned int data;
    
    /* Parameter consistency checks */
    ASSERT_BITBUF_SANITY(p_bitbuf);
    assert(n <= 16);

    /* Save bitbuf states */
    p_cur     = p_bitbuf->p_cur;
    bit_pos   = p_bitbuf->bit_pos; 
    bits_left = p_bitbuf->bits_left;
    
    /* Read bits */
    data = dlb_bitbuf_fast_read(p_bitbuf, n);

    /* Restore states as if we've never read anything */
    p_bitbuf->p_cur     = p_cur;
    p_bitbuf->bit_pos   = bit_pos; 
    p_bitbuf->bits_left = bits_left;
    
    return data;
}

/*
 * Description provided in dlb_bitbuf.h.
 */
unsigned long
dlb_bitbuf_fast_peek_long( dlb_bitbuf_handle p_bitbuf      
                          ,unsigned int      n            
                          )
{
    DLB_BITBUF_DATATYPE * p_cur;
    unsigned int bit_pos;
    long bits_left;
    unsigned long data;
    
    /* Parameter consistency checks */
    ASSERT_BITBUF_SANITY(p_bitbuf);
    assert(n <= 32);

    /* Save bitbuf states */
    p_cur     = p_bitbuf->p_cur;
    bit_pos   = p_bitbuf->bit_pos; 
    bits_left = p_bitbuf->bits_left;
    
    /* Read bits */
    data = dlb_bitbuf_fast_read_long(p_bitbuf, n);

    /* Restore states as if we've never read anything */
    p_bitbuf->p_cur     = p_cur;
    p_bitbuf->bit_pos   = bit_pos; 
    p_bitbuf->bits_left = bits_left;
    
    return data;
}

/*
 * Description provided in dlb_bitbuf.h.
 */
unsigned int
dlb_bitbuf_peek( dlb_bitbuf_handle p_bitbuf      
                ,unsigned int      n            
                )
{
    DLB_BITBUF_DATATYPE * p_cur;
    unsigned int bit_pos;
    long bits_left;
    unsigned int data;
    
    /* Parameter consistency checks */
    ASSERT_BITBUF_SANITY(p_bitbuf);
    assert(n <= 16);

    /* Save bitbuf states */
    p_cur     = p_bitbuf->p_cur;
    bit_pos   = p_bitbuf->bit_pos; 
    bits_left = p_bitbuf->bits_left;
    
    /* Read bits */
    data = dlb_bitbuf_read(p_bitbuf, n);

    /* Restore states as if we've never read anything */
    p_bitbuf->p_cur     = p_cur;
    p_bitbuf->bit_pos   = bit_pos; 
    p_bitbuf->bits_left = bits_left;
    
    return data;
}

/*
 * Description provided in dlb_bitbuf.h.
 */
unsigned long
dlb_bitbuf_peek_long( dlb_bitbuf_handle p_bitbuf      
                     ,unsigned int      n            
                     )
{
    DLB_BITBUF_DATATYPE * p_cur;
    unsigned int bit_pos;
    long bits_left;
    unsigned long data;
    
    /* Parameter consistency checks */
    ASSERT_BITBUF_SANITY(p_bitbuf);
    assert(n <= 32);

    /* Save bitbuf states */
    p_cur     = p_bitbuf->p_cur;
    bit_pos   = p_bitbuf->bit_pos; 
    bits_left = p_bitbuf->bits_left;
    
    /* Read bits */
    data = dlb_bitbuf_read_long(p_bitbuf, n);

    /* Restore states as if we've never read anything */
    p_bitbuf->p_cur     = p_cur;
    p_bitbuf->bit_pos   = bit_pos; 
    p_bitbuf->bits_left = bits_left;
    
    return data;
}

int
dlb_bitbuf_safe_peek
    (dlb_bitbuf_handle p_bitbuf   
    ,unsigned int      n         
    ,unsigned int     *p_data   
    )
{
    /* Parameter consistency checks */
    ASSERT_BITBUF_SANITY(p_bitbuf);
    assert(n <= 16); 
    assert(p_data);

    /* Check if we have enough bits left to peek */
    if (p_bitbuf->bits_left < (long) n)
        return DLB_BITBUF_ERROR_OUT_OF_BOUNDS;

    /* Peek n bits ahead */
    *p_data = dlb_bitbuf_fast_peek(p_bitbuf, n);

    return DLB_BITBUF_NO_ERROR;
}

int
dlb_bitbuf_safe_peek_long
    (dlb_bitbuf_handle p_bitbuf    /**< [in] Handle to allocated and initialized `dlb_bitbuf` instance. */
    ,unsigned int      n           /**< [in] Number of bits to read. */
    ,unsigned long    *p_data      /**< [out] Right-aligned bits read from bitstream. */
    )
{
    /* Parameter consistency checks */
    ASSERT_BITBUF_SANITY(p_bitbuf);
    assert(n <= 32); 
    assert(p_data);

    /* Check if we have enough bits left to peek */
    if (p_bitbuf->bits_left < (long) n)
        return DLB_BITBUF_ERROR_OUT_OF_BOUNDS;

    /* Peek n bits ahead */
    *p_data = dlb_bitbuf_fast_peek_long(p_bitbuf, n);

    return DLB_BITBUF_NO_ERROR;
}
/*@}*/
