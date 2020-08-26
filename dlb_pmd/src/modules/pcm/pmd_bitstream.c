/************************************************************************
 * dlb_pmd
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

#include "pmd_bitstream.h"

#include <assert.h>


int
generate_pmd_bitstream
    (pmd_s337m *s337m
    ,const dlb_pmd_model *model
    ,unsigned int block
    ,unsigned int block_count
    ,dlb_klvpmd_universal_label ul
    ,uint8_t *klvbuf
    )
{
    int max_bytes = pmd_s337m_pmd_data_bytes(s337m, block, block_count);
    int byte_size;

    s337m->framelen = DLB_PCMPMD_BLOCK_SIZE;
    if (0 == block || block_count-1 == block)
    {
        s337m->framelen -= GUARDBAND;
    }
    
    byte_size = dlb_klvpmd_write_block((dlb_pmd_model*)model, DLB_PMD_NO_ED2_STREAM_INDEX,
                                       block, klvbuf, max_bytes, ul);
    assert(byte_size <= max_bytes);
    return (byte_size == (int)dlb_klvpmd_min_block_size())
        ? 0
        : byte_size;
}
