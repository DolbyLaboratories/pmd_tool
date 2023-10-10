/************************************************************************
 * dlb_pmd
 * Copyright (c) 2023, Dolby Laboratories Inc.
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

#ifndef DLB_PMD_BITSET_H
#define DLB_PMD_BITSET_H

/**
 * @def BYTE_BITS
 * @brief number of bits in a byte
 */
#define BYTE_BITS (8)

static
void
set_element_bit
    (uint8_t            *element_set
    ,dlb_pmd_element_id  element_id
    )
{
    while (element_id >= BYTE_BITS)
    {
        element_set++;
        element_id -= BYTE_BITS;
    }
    *element_set |= (uint8_t)(1 << element_id);
}

static
dlb_pmd_bool
get_element_bit
    (uint8_t            *element_set
    ,dlb_pmd_element_id  element_id
    )
{
    while (element_id >= BYTE_BITS)
    {
        element_set++;
        element_id -= BYTE_BITS;
    }
    return (dlb_pmd_bool)((*element_set & (uint8_t)(1 << element_id)) ? PMD_TRUE : PMD_FALSE);
}

#endif  // DLB_PMD_BITSET_H
