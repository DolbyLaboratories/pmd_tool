/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2020 by Dolby Laboratories,
 *                Copyright (C) 2020 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

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
