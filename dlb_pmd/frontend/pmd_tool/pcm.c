/************************************************************************
 * dlb_pmd
 * Copyright (c) 2016-2021, Dolby Laboratories Inc.
 * Copyright (c) 2016-2021, Dolby International AB.
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
#  define PRIu64 "I64u"
#  define PRId64 "I64"
#else
#  include <inttypes.h>
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
    ,unsigned    nchannel
    )
{
    unsigned int i;

    buf->nchannel  = nchannel;
    buf->nstride   = nchannel;
    buf->data_type = DLB_BUFFER_INT_LEFT;
    buf->ppdata    = (void**)ppdata;

    for (i = 0; i != nchannel; ++i)
    {
        ppdata[i] = &channeldata[i];
    }
}


static inline
void
buffer_shift
    (dlb_buffer *buf
    ,size_t      skip
    ,size_t      amount
    )
{
    size_t sample_set_size = sizeof(uint32_t) * buf->nstride;
    memmove(channeldata, (channeldata + (skip * sample_set_size)), (sample_set_size * amount));
}


static inline
int
open_files
    (const char          *infile
    ,const char          *outfile
    ,      dlb_wave_file *source
    ,      dlb_wave_file *sink
    ,      unsigned int  *nchans
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

static
void
show_payload_status
    (FILE                           *f
    ,const char                     *payload_name
    ,int                             payload_index
    ,dlb_pmd_payload_status_record  *status
    )
{
    fprintf(f, "%s", payload_name);
    if (payload_index >= 0)
    {
        fprintf(f, "[%d]", payload_index);
    }
    if (status->payload_status != DLB_PMD_PAYLOAD_STATUS_OK)
    {
        fprintf(f, "\t%s", status->error_description);
    }
    fprintf(f, "\n");
}

static
int
payload_set_status_callback
    (dlb_pmd_payload_set_status *status
    )
{
    if (status != NULL)
    {
        FILE *f = (FILE *)status->callback_arg;

        if (f != NULL)
        {
            int index, number;

            if (status->new_frame)
            {
                if (status->count_frames)
                {
                    fprintf(f, "------------ Frame %" PRIu64 " ------------\n", ++status->frame_count);
                } 
                else
                {
                    fprintf(f, "------------ Frame ------------\n");
                }
                status->burst_count = 0;
            }
            fprintf(f, "------------ Burst %" PRIu64 " ------------\n", ++status->burst_count);

            if (status->payload_set_status.payload_status != DLB_PMD_PAYLOAD_STATUS_OK)
            {
                fprintf(f, "Error %d in payload set: %s\n", status->payload_set_status.payload_status, status->payload_set_status.error_description);
            }

            if (status->has_ver_payload)
            {
                show_payload_status(f, "VER", -1, &status->ver_payload_status);
            }
            if (status->has_abd_payload)
            {
                show_payload_status(f, "ABD", -1, &status->abd_payload_status);
            }
            if (status->has_aod_payload)
            {
                show_payload_status(f, "AOD", -1, &status->aod_payload_status);
            }
            if (status->has_apd_payload)
            {
                show_payload_status(f, "APD", -1, &status->apd_payload_status);
            }
            if (status->has_hed_payload)
            {
                show_payload_status(f, "HED", -1, &status->hed_payload_status);
            }
            if (status->has_iat_payload)
            {
                show_payload_status(f, "IAT", -1, &status->iat_payload_status);
            }
            if (status->has_apn_payload)
            {
                show_payload_status(f, "APN", -1, &status->apn_payload_status);
            }
            if (status->has_aen_payload)
            {
                show_payload_status(f, "AEN", -1, &status->aen_payload_status);
            }
            if (status->has_esd_payload)
            {
                show_payload_status(f, "ESD", -1, &status->esd_payload_status);
            }
            if (status->has_esn_payload)
            {
                show_payload_status(f, "ESN", -1, &status->esn_payload_status);
            }
            if (status->has_eep_payload)
            {
                show_payload_status(f, "EEP", -1, &status->eep_payload_status);
            }
            for (index = 0, number = 1; index < (int)status->xyz_payload_count; index++, number++)
            {
                show_payload_status(f, "XYZ", number, &status->xyz_payload_status[index]);
            }
            if (status->has_pld_payload)
            {
                show_payload_status(f, "PLD", -1, &status->pld_payload_status);
            }
            if (status->has_etd_payload)
            {
                show_payload_status(f, "ETD", -1, &status->etd_payload_status);
            }
            if (status->has_crc_payload)
            {
                show_payload_status(f, "CRC", -1, &status->crc_payload_status);
            }
            fflush(f);
        }
    }

    return 0;   /* This callback just prints debug info and should never signal failure */
}

int
pcm_read
    (const char             *infile
    ,const char             *logfile
    ,dlb_pmd_frame_rate      rate
    ,unsigned int            chan
    ,dlb_pmd_bool            is_pair
    ,size_t                  vsync
    ,size_t                  skip
    ,dlb_pmd_model_combo    *model
    )
{
    dlb_pcmpmd_extractor    *ext;
    dlb_wave_file            source;
    dlb_buffer               buffer;
    unsigned int             nchans;
    unsigned int             block_count;
    unsigned int             frame_count;
    unsigned int             error_count;
    size_t                   read;
    size_t                   sz;
    void                    *mem;
    vsync_timer              vt;
    size_t                   video_sync;
    FILE                    *filelog = NULL;
    dlb_pmd_bool             close_log_file = PMD_FALSE;
    dlb_pmd_success          success;
    int                      res;

    dlb_pmd_payload_status_record    update_array[DLB_PMD_MAX_UPDATES];
    dlb_pmd_payload_set_status       payload_set_status;

    memset(&update_array, 0, sizeof(update_array));
    memset(&payload_set_status, 0, sizeof(payload_set_status));

    sz  = dlb_pcmpmd_extractor_query_mem(PMD_TRUE);
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

    if (logfile != NULL && logfile[0] != '\0')
    {
        if (strcmp(logfile, "stdout") == 0)
        {
            filelog = stdout;
        }
        else if (strcmp(logfile, "stderr") == 0)
        {
            filelog = stderr;
        } 
        else
        {
            filelog = fopen(logfile, "w");
            if (filelog == NULL)
            {
                return 1;
            }
            close_log_file = PMD_TRUE;
        }
    }

    if (filelog != NULL)
    {
        success = dlb_pmd_initialize_payload_set_status_with_callback(&payload_set_status, update_array, DLB_PMD_MAX_UPDATES, (void *)filelog, payload_set_status_callback);
        payload_set_status.count_frames = PMD_TRUE;
    } 
    else
    {
        success = dlb_pmd_initialize_payload_set_status(&payload_set_status, update_array, DLB_PMD_MAX_UPDATES);
    }
    if (success != PMD_SUCCESS)
    {
        return 1;
    }

    dlb_pcmpmd_extractor_init2(&ext, mem, rate, chan, nchans, is_pair, model, &payload_set_status, 1);

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
                    (void)vsync_timer_add_samples(&vt, read);
                    skip -= read;
                    continue;
                }
                (void)vsync_timer_add_samples(&vt, skip);
                read -= skip;
                buffer_shift(&buffer, skip, read);
                skip = 0;
            }

            video_sync = vsync_timer_add_samples(&vt, read);
            if (video_sync != DLB_PMD_VSYNC_NONE) { TRACE(("video sync in %u\n", video_sync)); }
            if (dlb_pcmpmd_extract(ext, channeldata, read, video_sync))
            {
                const char *msg = dlb_pcmpmd_extractor_error_msg(ext);

                if (msg[0] == '\0')
                {
                    msg = "error";
                }
                printf("%s", msg);
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
    if (close_log_file)
    {
        fclose(filelog);
    }
    free(mem);
    return (error_count != 0);
}


int
pcm_write
    (const char                 *infile
    ,const char                 *outfile
    ,dlb_pmd_frame_rate          rate
    ,unsigned int                chan
    ,dlb_pmd_bool                is_pair
    ,dlb_klvpmd_universal_label  ul
    ,dlb_pmd_bool                mark_empty_blocks
    ,dlb_pmd_bool                sadm
    ,dlb_pmd_model_combo        *model
    )
{
    dlb_pcmpmd_augmentor      *aug;
    dlb_wave_file              source;
    dlb_wave_file              sink;
    dlb_buffer                 buffer;
    unsigned int               nchans;
    size_t                     read;
    size_t                     sz;
    void                      *mem;
    vsync_timer                vt;
    size_t                     video_sync;
    int                        res;

    sz  = dlb_pcmpmd_augmentor_query_mem(sadm);
    mem = malloc(sz);
    if (NULL == mem)
    {
        printf("ERROR: could not allocate memory\n");
        return 1;
    }

    if (open_files(infile, outfile, &source, &sink, &nchans))
    {
        free(mem);
        return 1;
    }

    dlb_pcmpmd_augmentor_init2(&aug, model, mem, rate, ul, mark_empty_blocks,
                               nchans, nchans, is_pair, chan, sadm);

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
            video_sync = vsync_timer_add_samples(&vt, read);
            dlb_pcmpmd_augment(aug, channeldata, read, video_sync);
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
