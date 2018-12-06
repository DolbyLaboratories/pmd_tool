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
 * @file pcm.c
 * @brief SMPTE-337m wrapped SMPTE 336-KLV/SMPTE 2109 reader/writer functionality for pmd tool
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>

#include "dlb_buffer/include/dlb_buffer.h"
#include "dlb_wave/include/dlb_wave_int.h"

#include "dlb_pmd_api.h"
#include "dlb_pmd_pcm.h"
#include "pcm_vsync_timer.h"
#include "pcm.h"

#ifdef _MSC_VER
#  define _snprintf snprintf
#endif

//#define TRACE_VSYNC
#ifdef TRACE_VSYNC
#  define TRACE(x) printf x
#else
#  define TRACE(x)
#endif


#define MAX_CHANNELS (256)
#define BLOCK_SIZE (256)
#define OUTPUT_PCM_BIT_DEPTH (24)

static uint32_t channeldata[MAX_CHANNELS * BLOCK_SIZE];
static uint32_t *ppdata[MAX_CHANNELS];


static inline
void
buffer_init
    (dlb_buffer *buf
    ,unsigned nchannel
    )
{
    unsigned int i;

    buf->nchannel = nchannel;
    buf->nstride = nchannel;
    buf->data_type = DLB_BUFFER_INT_LEFT;
    buf->ppdata = (void**)ppdata;

    for (i = 0; i != nchannel; ++i)
    {
        ppdata[i] = &channeldata[i];
    }
}


static inline
void
buffer_shift
    (dlb_buffer *buf
    ,size_t skip
    ,size_t amount
    )
{
    size_t sample_set_size = sizeof(uint32_t) * buf->nstride;
    memmove(channeldata, channeldata + skip * sample_set_size, sample_set_size * amount);
}



static inline
int
open_files
    (const char *infile
    ,const char *outfile
    ,dlb_wave_file *source
    ,dlb_wave_file *sink
    ,unsigned int *nchans
    )
{
    int res;

    res = dlb_wave_open_read(source, infile, NULL);
    if (DLB_RIFF_OK != res)
    {
        printf("ERROR: could not open file \"%s\"\n", infile);
        return 1;
    }

    if (source->format.sample_rate != 48000)
    {
        printf("ERROR: only 48 kHz sample rate supported\n");
        dlb_wave_close(source);
        return 1;
    }

    *nchans = source->format.channel_count;

    if (NULL != sink)
    {
        res = dlb_wave_open_write(sink, outfile, 0,
                                  source->format.sample_rate,
                                  *nchans, 0, OUTPUT_PCM_BIT_DEPTH);
        if (DLB_RIFF_OK != res)
        {
            printf("ERROR: could not open file \"%s\"\n", outfile);
            dlb_wave_close(source);
            return 1;
        }
        
        res = dlb_wave_begin_data(sink);
        if (res != DLB_RIFF_OK)
        {
            printf("ERROR: failed to begin output file's data chunk \"%s\"", outfile);
            dlb_wave_close(source);
            dlb_wave_close(sink);
            return 1;
        }
    }
    return 0;
}


int
pcm_read
    (const char *infile
    ,dlb_pmd_frame_rate rate
    ,unsigned int chan
    ,dlb_pmd_bool ispair
    ,size_t vsync
    ,size_t skip
    ,dlb_pmd_model *model
    )
{
    dlb_pcmpmd_extractor *ext;
    dlb_wave_file source;
    dlb_buffer buffer;
    unsigned int nchans;
    unsigned int block_count;
    unsigned int frame_count;
    unsigned int error_count;
    size_t read;
    size_t sz;
    void *mem;
    vsync_timer vt;
    size_t video_sync;
    int res;

    sz = dlb_pcmpmd_extractor_query_mem();
    mem = malloc(sz);
    if (NULL == mem)
    {
        printf("ERROR: could not allocate memory\n");
        return 1;
    }

    if (open_files(infile, NULL, &source, NULL, &nchans))
    {
        return 1;
    }
    
    dlb_pcmpmd_extractor_init(&ext, mem, rate, chan, nchans, ispair, model);

    buffer_init(&buffer, nchans);
    vsync_timer_init(&vt, rate, vsync);
    frame_count = 0;
    block_count = 0;
    error_count = 0;

    res = 0;
    while (0 == res)
    {
        res = dlb_wave_int_read(&source, &buffer, BLOCK_SIZE, &read);
        if (0 < read)
        {
            if (skip)
            {
                if (skip > read)
                {
                    video_sync = vsync_timer_add_samples(&vt, read);
                    skip -= read;
                    continue;
                }
                video_sync = vsync_timer_add_samples(&vt, skip);
                read -= skip;
                buffer_shift(&buffer, skip, read);
                skip = 0;
            }

            video_sync = vsync_timer_add_samples(&vt, read);
            if (video_sync != DLB_PMD_VSYNC_NONE) { TRACE(("video sync in %u\n", video_sync)); }
            if (dlb_pcmpmd_extract(ext, channeldata, read, video_sync))
            {
                printf("%s", dlb_pmd_error(model));
                printf("    at block %u of frame %u\n", block_count, frame_count);
                error_count += 1;
            }
            else if (block_count == 0)
            {
                /* we got a good first frame */
                error_count = 0;
            }
            block_count += 1;
            if (video_sync)
            {
                block_count = 0;
                frame_count += 1;
            }
        }
    }
    
    dlb_pcmpmd_extractor_finish(ext);
    dlb_wave_close(&source);
    free(mem);
    return error_count != 0;
}


int
pcm_write
    (const char *infile
    ,const char *outfile
    ,dlb_pmd_frame_rate rate
    ,unsigned int chan
    ,dlb_pmd_bool ispair
    ,dlb_klvpmd_universal_label ul
    ,dlb_pmd_bool mark_empty_blocks
    ,dlb_pmd_model *model
    )
{
    dlb_pcmpmd_augmentor *aug;
    dlb_wave_file source;
    dlb_wave_file sink;
    dlb_buffer buffer;
    unsigned int nchans;
    size_t read;
    size_t sz;
    void *mem;
    vsync_timer vt;
    size_t video_sync;
    int res;

    sz = dlb_pcmpmd_augmentor_query_mem();
    mem = malloc(sz);
    if (NULL == mem)
    {
        printf("ERROR: could not allocate memory\n");
        return 1;
    }

    if (open_files(infile, outfile, &source, &sink, &nchans))
    {
        return 1;
    }
    
    dlb_pcmpmd_augmentor_init(&aug, model, mem, rate, ul, mark_empty_blocks,
                              nchans, nchans, ispair, chan);

    buffer_init(&buffer, nchans);
    vsync_timer_init(&vt, rate, 0);

    res = 0;
    video_sync = 0;
    while (0 == res)
    {
        res = dlb_wave_int_read(&source, &buffer, BLOCK_SIZE, &read);
        if (0 < read)
        {
            if (video_sync != DLB_PMD_VSYNC_NONE) { TRACE(("video sync in %u\n", video_sync)); }
            dlb_pcmpmd_augment(aug, channeldata, read, video_sync);
            video_sync = vsync_timer_add_samples(&vt, read);
            res = dlb_wave_int_write(&sink, &buffer, read);
            if (res)
            {
                printf("ERROR: could not write: %d\n", res);
                break;
            }
        }
    }
    
    dlb_pcmpmd_augmentor_finish(aug);

    dlb_wave_end_data(&sink);
    dlb_wave_close(&sink);
    dlb_wave_close(&source);

    free(mem);

    return 0;
}

