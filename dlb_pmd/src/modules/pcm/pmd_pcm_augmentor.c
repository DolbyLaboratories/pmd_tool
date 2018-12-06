/************************************************************************
 * dlb_pmd
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
 * @file pmc_pcm_augmentor.c
 * @brief PMD model PCM augmentor
 *
 * Overwrite final two channels of PCM with SMPTE-337m wrapped KLV
 */

#include "pmd_model.h"
#include "pmd_smpte_337m.h"

#include "dlb_pmd_pcm.h"
#include "dlb_pmd_klv.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>


//#define TRACE_SMPTE
#ifdef TRACE_SMPTE
#  define TRACE(x) printf x
#else
#  define TRACE(x)
#endif


/**
 * @brief internal state of PCM augmentor
 */
struct dlb_pcmpmd_augmentor
{
    dlb_pmd_model *model;       /**< source model */
    dlb_klvpmd_universal_label ul; /**< Universal label */
    uint8_t klvbuf[MAX_DATA_BYTES_PAIR]; /**< current KLV block write buffer */     
    uint8_t *klvp;              /**< current read pointer of klvbuf */
    size_t klvsize;             /**< bytes remaining in block */
    pmd_s337m s337m;            /**< SMPTE 337m wrapping state */
    unsigned int samples;       /**< samples remaining to write in current block */
    dlb_pmd_frame_rate rate;    /**< video frame rate */
    unsigned int numchannels;   /**< number of channels of PCM */
    unsigned int klvchan;       /**< 1st channel for klv */
    unsigned int maxblock;      /**< maximum number of blocks for frame rate */
    unsigned int block;         /**< current block number */
};
     

/**
 * @brief callback for SMPTE 337m wrapper to get next block
 */
static
int
pcm_next_block
    (pmd_s337m *s337m
    )
{
    dlb_pcmpmd_augmentor *aug = (dlb_pcmpmd_augmentor*)s337m->nextarg;
    pmd_bool empty_block = PMD_FALSE;
    int max_bytes = s337m->pair ? MAX_DATA_BYTES_PAIR : MAX_DATA_BYTES_CHAN;
    int byte_size;
    int res = 1;

    memset(aug->klvbuf, '\0', MAX_DATA_BYTES_PAIR);

    aug->block += 1;
    if (aug->block == aug->maxblock)
    {
        dlb_pmd_apply_updates(aug->model);
        aug->block = 0;
        res = 0;
    }

    byte_size = dlb_klvpmd_write_block(aug->model, 0, aug->block, aug->klvbuf,
                                       max_bytes, aug->ul);
    assert(byte_size <= max_bytes);
    empty_block = (byte_size == dlb_klvpmd_min_block_size());

    TRACE(("block %u: klv datasize %u empty? %d\n", aug->block, byte_size, empty_block));

    s337m->framelen = DLB_PCMPMD_BLOCK_SIZE;
    if (empty_block)
    {
        s337m->data = NULL;
        s337m->databits = 0;
    }
    else
    {
        s337m->data = aug->klvbuf;
        s337m->databits = 8 * byte_size;
    }
    return res;
}


size_t
dlb_pcmpmd_augmentor_query_mem
    (void
    )
{
    return sizeof(struct dlb_pcmpmd_augmentor);
}


void
dlb_pcmpmd_augmentor_init
    (dlb_pcmpmd_augmentor **augptr
    ,dlb_pmd_model *model
    ,void *mem
    ,dlb_pmd_frame_rate rate
    ,dlb_klvpmd_universal_label ul
    ,dlb_pmd_bool mark_empty_blocks
    ,unsigned int numchannels
    ,unsigned int stride
    ,dlb_pmd_bool pmd_pair
    ,unsigned int start
    )
{
    dlb_pcmpmd_augmentor *aug = mem;
    unsigned int vfsize;   /** video frame size in samples */

    memset(mem, '\0', sizeof(dlb_pcmpmd_augmentor));

    *augptr = aug;
    aug->model = model;
    aug->rate = rate;
    aug->ul = ul;
    aug->block = ~0u;

    if (start >= stride)
    {
        if (pmd_pair)
        {
            start = stride - 2;
        }
        else
        {
            start = stride - 1;
        }
    }

    vfsize = pmd_s337m_min_frame_size(rate);
    vfsize -= GUARDBAND;
    aug->maxblock = vfsize / DLB_PCMPMD_BLOCK_SIZE;
    aug->numchannels = numchannels;

    if (aug->numchannels < 2) abort();
    if (aug->numchannels & 1) abort();

    aug->klvchan = start;
    pmd_apn_list_iterator_init(&model->apni, &model->apn_list);
    pmd_s337m_init(&aug->s337m, stride, pcm_next_block, aug, pmd_pair, start, mark_empty_blocks);
}

    
void
dlb_pcmpmd_augmentor_finish
    (dlb_pcmpmd_augmentor *aug
    )
{
    (void)aug;
}


void
dlb_pcmpmd_augment
    (dlb_pcmpmd_augmentor *aug
    ,uint32_t *pcm
    ,size_t num_samples
    ,size_t video_sync
    )
{
    uint32_t *end;

    pcm += aug->klvchan;
    end = pcm + num_samples * aug->s337m.stride;

    aug->s337m.vsync_offset = video_sync;
    pmd_s337m_wrap(&aug->s337m, pcm, end);
}

