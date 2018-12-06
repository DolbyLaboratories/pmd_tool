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
 * @file pmc_pcm_extractor.c
 * @brief PMD model PCM extractor
 *
 * Extract PMD model from SMPTE-337m encoded KLV metadata
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include "dlb_pmd_pcm.h"
#include "dlb_pmd_klv.h"
#include "pmd_model.h"
#include "pmd_error_helper.h"
#include "pmd_smpte_337m.h"

//#define TRACE_BLOCKS
#ifdef TRACE_BLOCKS
#  define TRACE(x) printf x
#else
#  define TRACE(x)
#endif


/**
 * @brief internal state of PCM augmentor
 */
struct dlb_pcmpmd_extractor
{
    dlb_pmd_model *model;       /**< source model */
    uint8_t  klvbuf[MAX_DATA_BYTES_PAIR+6]; /**< current KLV block */
    uint8_t *klvp;              /**< current read pointer of klvbuf */
    size_t klvsize;             /**< bytes remaining in block */

    pmd_s337m s337m;            /**< SMPTE 337m wrapping state */

    dlb_pmd_frame_rate rate;    /**< video frame rate */
    size_t numchannels;         /**< number of channels of PCM */
    unsigned int klvchan;       /**< channel to extract klv from */
    dlb_pmd_bool klvpair;       /**< KLV is a pair! */
    unsigned int maxblock;      /**< maximum number of blocks for frame rate */
    unsigned int block;         /**< current block number */
    unsigned int unused;        /**< number of samples unused at end of frame */
    size_t remaining;           /**< samples remaining in block */
    dlb_pmd_bool new_frame;     /**< next block to be decoded is a new frame */
    dlb_pmd_bool sync;          /**< waiting to find first frame */
    size_t sample_count;        /**< total number of samples decoded */
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
    dlb_pcmpmd_extractor *ext = (dlb_pcmpmd_extractor*)s337m->nextarg;
    int res = 1;

    if (0 != s337m->data)
    {
        /* we have extracted data from SMPTE 337m stream. Now decode it */
        size_t datasize = s337m->data - ext->klvbuf;
        int new_frame = ext->new_frame;
        ext->new_frame = 0;
        res = !new_frame;

        /* neither init time nor callback with 0 data */
        if (!dlb_klvpmd_read_payload(ext->klvbuf, datasize, ext->model, new_frame, NULL))
        {
            /* somehow raise a callback if something changes */
        }
    }
    
    memset(ext->klvbuf, '\0', sizeof(ext->klvbuf));
    s337m->databits = 8 * sizeof(ext->klvbuf);
    s337m->data = ext->klvbuf;
    s337m->framelen = DLB_PCMPMD_BLOCK_SIZE;
    return res;
}


size_t
dlb_pcmpmd_extractor_query_mem
    (void
    )
{
    return sizeof(struct dlb_pcmpmd_extractor);
}


void
dlb_pcmpmd_extractor_init
    (dlb_pcmpmd_extractor **extptr
    ,void *mem
    ,dlb_pmd_frame_rate rate
    ,unsigned int chan
    ,unsigned int stride
    ,dlb_pmd_bool ispair
    ,dlb_pmd_model *model
    )
{
    dlb_pcmpmd_extractor *ext = mem;
    unsigned int vfsize;   /** video frame size in samples */

    memset(mem, '\0', sizeof(dlb_pcmpmd_extractor));

    *extptr = ext;
    ext->model = model;
    ext->rate = rate;
    ext->block = ~0u;
    ext->new_frame = 1;
    ext->sync = 1;

    vfsize = pmd_s337m_min_frame_size(rate);
    vfsize -= GUARDBAND;

    ext->maxblock = vfsize / DLB_PCMPMD_BLOCK_SIZE;
    ext->numchannels = stride;
    ext->block = 0;
    ext->remaining = DLB_PCMPMD_BLOCK_SIZE + GUARDBAND;
    /* compute unused space at end of frame */
    ext->unused = vfsize - (ext->maxblock * DLB_PCMPMD_BLOCK_SIZE);

    if (ext->numchannels < 2) abort();
    if (ext->numchannels & 1) abort();

    ext->klvpair = ispair;
    ext->klvchan = chan;
    if (chan >= stride)
    {
        if (ispair)
        {
            ext->klvchan = stride - 2;
        }
        else
        {
            ext->klvchan = stride - 1;
        }
    }

    pmd_s337m_init(&ext->s337m, stride, pcm_next_block, ext, ext->klvpair, ext->klvchan, 0);
}

    
void
dlb_pcmpmd_extractor_finish
    (dlb_pcmpmd_extractor *ext
    )
{
    (void)ext;
}


/**
 * @brief helper function to extract a single block, maintaining
 * accurate track of how long before a new frame is due
 */
static inline
void
extract_block
    (dlb_pcmpmd_extractor *ext
    ,uint32_t **pcm
    ,size_t num_samples
    ,size_t *video_sync
    )
{
    uint32_t *end = *pcm + num_samples * ext->s337m.stride;

    TRACE(("EXTRACTOR: read block @%u %u/%u (%u/%u)\n",
           ext->sample_count, ext->block, ext->maxblock,
           num_samples, ext->remaining));

    ext->sample_count += num_samples;
    ext->s337m.vsync_offset = *video_sync;
    pmd_s337m_unwrap(&ext->s337m, *pcm, end);
    *video_sync -= ext->remaining;
    *pcm = end;

    if (num_samples >= ext->remaining)
    {
        ext->remaining = DLB_PCMPMD_BLOCK_SIZE;
        ext->block += 1;
        if (ext->block == ext->maxblock)
        {
            TRACE((".... new frame expected .... \n"));
            ext->block = 0;
            ext->new_frame = 1;
        }
        else if (ext->block == ext->maxblock - 1)
        {
            ext->remaining += ext->unused + GUARDBAND;
        }
    }
    else
    {
        ext->remaining -= num_samples;
    }
}


dlb_pmd_success
dlb_pcmpmd_extract
    (dlb_pcmpmd_extractor *ext
    ,uint32_t *pcm
    ,size_t num_samples
    ,size_t video_sync
    )
{
    error_reset(ext->model);

    pcm += ext->klvchan;
    if (ext->sync)
    {
        if (video_sync > num_samples)
        {
            return 0;
        }
        else
        {
            ext->sync = 0;
            pcm += video_sync * ext->numchannels;
            num_samples -= video_sync;
            video_sync = 0;
        }
    }

    /* add a block at a time to keep track of where in the frame we are */
    while (num_samples > ext->remaining)
    {
        num_samples -= ext->remaining;
        extract_block(ext, &pcm, ext->remaining, &video_sync);
    }
    
    if (num_samples)
    {
        extract_block(ext, &pcm, num_samples, &video_sync);
    }

    /* return 0 (success) if there is no error */
    return ext->model->error[0] != '\0';
}

