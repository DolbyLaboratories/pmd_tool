/************************************************************************
 * dlb_pmd
 * Copyright (c) 2020 - 2022, Dolby Laboratories Inc.
 * Copyright (c) 2022, Dolby International AB.
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
#include "dlb_pmd_model_combo.h"
#include "dlb_pmd_klv.h"
#include "pmd_model.h"
#include "pmd_error_helper.h"
#include "pmd_smpte_337m.h"
#include "sadm_bitstream_decoder.h"

#ifdef _MSC_VER
#  define PRIu64 "I64u"
#  define PRId64 "I64"
#else
#  include <inttypes.h>
#endif

/*#define TRACE_BLOCKS*/
#ifdef TRACE_BLOCKS
#  define TRACE(x) printf x
#else
#  define TRACE(x)
#endif

#define EXTRACT_BLOCK_SIZE (32)

/**
 * @brief internal state of PCM extractor
 */
struct dlb_pcmpmd_extractor
{
    dlb_pmd_model_combo *model;                     /**< source model */
    uint8_t              klv_buf[MAX_DATA_BYTES];   /**< current KLV block */

    pmd_s337m            s337m;                     /**< SMPTE 337m wrapping state */

    dlb_pmd_frame_rate   rate;                      /**< video frame rate */
    size_t               channel_count;             /**< number of channels of PCM */
    unsigned int         klv_chan;                  /**< channel to extract klv from */
    dlb_pmd_bool         klv_pair;                  /**< KLV is a pair! */
    unsigned int         block_count;               /**< maximum number of blocks for frame rate */
    unsigned int         max_block_num;             /**< block_count - 1 */
    unsigned int         cur_block_num;             /**< current block number */
    dlb_pmd_bool         new_frame;                 /**< next block to be decoded is a new frame */
    dlb_pmd_bool         waiting;                   /**< waiting to find first frame */
    dlb_pmd_bool         no_vsync;                  /**< no external vsync given; try to work it out */
    size_t               sample_count;              /**< total number of samples decoded */
    size_t               prev_pa;                   /**< sample count of previously found PA, or NO_PA_FOUND */
    size_t               frame_start;               /**< sample count of start of current frame, or NO_PA_FOUND */

    dlb_pcmpmd_new_frame         callback;
    dlb_pcmpmd_new_sadm          sadm_callback;
    char                        *sadm_xml_buf;
    void                        *cbarg;
    dlb_pmd_bool                 sadm;
    sadm_bitstream_decoder      *sdec;

    dlb_pmd_payload_set_status  *payload_set_status;

    dlb_pmd_bool     error_flag;                    /**< was there an error? */
    char             error_msg[PMD_ERROR_SIZE];     /**< error string, if any */
};


static
void
reset_extractor_error
    (dlb_pcmpmd_extractor   *extractor
    )
{
    const dlb_pmd_model *pmd_model;

    if (!dlb_pmd_model_combo_get_readable_pmd_model(extractor->model, &pmd_model, PMD_FALSE))
    {
        error_reset(pmd_model);
    }
    memset(extractor->error_msg, 0, sizeof(extractor->error_msg));
    extractor->error_flag = PMD_FALSE;
}


/**
 * @brief callback function to handle PA found from s337m unwrapper
 *
 * We keep track of PA offsets: whenever the difference between
 * successive PA sample positions exceeds the start of the PCM+PMD
 * block size (160 samples), then the second PA must be the PA
 * position of the first PMD block of a new frame.
 */
static
void
found_pa
    (void   *cb_arg
    ,size_t  pa_found
    )
{
    if (cb_arg != NULL)
    {
        dlb_pcmpmd_extractor *ext = (dlb_pcmpmd_extractor *)cb_arg;
        size_t pa_pos = ext->sample_count + pa_found;
        size_t pa_diff = pa_pos - ext->prev_pa;
        dlb_pmd_bool long_block = pa_diff > DLB_PCMPMD_BLOCK_SIZE;

        TRACE(("pa_pos = %" PRIu64
               " (prev_pa: %" PRIu64 " pa_diff: %" PRIu64 ")\n",
               (uint64_t)pa_pos, (uint64_t)ext->prev_pa, (uint64_t)pa_diff));

        if (long_block)
        {
            ext->frame_start = pa_pos - GUARDBAND;
            TRACE(("new frame detected....%" PRIu64 "\n", (uint64_t)ext->frame_start));
            ext->new_frame = PMD_TRUE;
            if (ext->no_vsync && ext->callback)
            {
                ext->cur_block_num = ext->max_block_num;
                ext->callback(ext->cbarg);
            }
            ext->cur_block_num = 0;
        }
        else
        {
            ext->cur_block_num++;
        }
        ext->prev_pa = pa_pos;
    }
}


static
void
sadm_dec_callback
    (void           *user_data
    ,sadm_dec_state  state
    )
{
    dlb_pcmpmd_extractor *ext = (dlb_pcmpmd_extractor*)user_data;

    if (ext->sadm_callback)
    {
        if (state != SADM_DECOMPRESS_ERR)
        {
            memcpy(ext->sadm_xml_buf, ext->sdec->xmlbuf, ext->sdec->size);
        }
        ext->sadm_callback(ext->cbarg, ext->sadm_xml_buf, ext->sdec->size, (dlb_pcmsadm_status)state);
    }
}


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
    
    if (0 != s337m->data)       /* neither init time nor callback with 0 data */
    {
        /* we have extracted data from SMPTE 337m stream. Now decode it */
        size_t           datasize = s337m->data - ext->klv_buf;
        int              new_frame = ext->new_frame;
        dlb_pmd_bool     ok = 0;

        ext->new_frame = 0;
        res = !new_frame;

        if (s337m->sadm)
        {
            if (ext->sdec != NULL)
            {
                dlb_adm_core_model *core_model;

                if (dlb_pmd_model_combo_get_writable_core_model(ext->model, &core_model))
                {
                    TRACE(("Serial ADM detected but could not get a writable core model!\n"));
                    ok = PMD_FALSE;
                } 
                else
                {
                    ok = !sadm_bitstream_decoder_decode(ext->sdec, ext->klv_buf, datasize, core_model, PMD_FALSE, sadm_dec_callback, s337m->nextarg);
                }
            } 
            else
            {
                TRACE(("Serial ADM detected but there is no sADM decoder!\n"));
                ok = PMD_FALSE;
            }
            s337m->framelen = pmd_s337m_min_frame_size(ext->rate);
        }
        else 
        {
            dlb_pmd_model *pmd_model;

            if (dlb_pmd_model_combo_get_writable_pmd_model(ext->model, &pmd_model, PMD_FALSE))
            {
                TRACE(("PMD detected but could not get a writable PMD model!\n"));
                ok = PMD_FALSE;
            } 
            else
            {
                ok = !dlb_klvpmd_read_payload(ext->klv_buf, datasize, pmd_model, new_frame, NULL, ext->payload_set_status);
                if ((!ok) && (pmd_model->error[0] != '\0'))
                {
                    strncpy(ext->error_msg, pmd_model->error, sizeof(ext->error_msg));
                }
            }
            s337m->framelen = DLB_PCMPMD_BLOCK_SIZE;
        }

        if (!ok)
        {
            ext->error_flag = PMD_TRUE;
        }
        TRACE(("block %s\n", ok ? "decoded OK" : "failed to decode"));
    }
    else
    {
        /* The bitstream may be either sADM or PMD, so default for larger */
        s337m->framelen = pmd_s337m_min_frame_size(ext->rate);
    }

    memset(ext->klv_buf, '\0', sizeof(ext->klv_buf));
    s337m->databits = 8 * sizeof(ext->klv_buf);
    s337m->data = ext->klv_buf;
    return res;
}


size_t
dlb_pcmpmd_extractor_query_mem
    (dlb_pmd_bool sadm
    )
{
    size_t sz = sizeof(struct dlb_pcmpmd_extractor);

    if (sadm)
    {
        sz += sadm_bitstream_decoder_query_mem();
    }

    return sz;
}


void
dlb_pcmpmd_extractor_init3
    (dlb_pcmpmd_extractor          **extptr
    ,void                           *mem
    ,unsigned int                    wrap_depth
    ,dlb_pmd_frame_rate              rate
    ,unsigned int                    chan
    ,unsigned int                    stride
    ,dlb_pmd_bool                    ispair
    ,dlb_pmd_model_combo            *model
    ,dlb_pmd_payload_set_status     *status
    ,dlb_pmd_bool                    sadm
    )
{
    dlb_pcmpmd_extractor    *ext = mem;
    unsigned int             wd;        /** s337m wrapping bit depth */
    unsigned int             vfsize;    /** video frame size in samples */

    memset(mem, 0, sizeof(dlb_pcmpmd_extractor));

    switch (wrap_depth)
    {
    case 16:
    case 20:
    case 24:
        wd = wrap_depth;
        break;
    default:
        wd = (sadm ? 24 : 20);
        break;
    }

    *extptr = ext;
    ext->model = model;
    ext->rate = rate;
    ext->new_frame = PMD_TRUE;
    ext->waiting = PMD_TRUE;

    vfsize = pmd_s337m_min_frame_size(rate);

    ext->block_count = vfsize / DLB_PCMPMD_BLOCK_SIZE;
    ext->max_block_num = ext->block_count - 1;
    ext->channel_count = stride;
    ext->prev_pa = NO_PA_FOUND;
    ext->payload_set_status = status;
    ext->sadm = sadm;

    if (ispair && ((ext->channel_count < 2) || (ext->channel_count & 1)))
    {
        abort();
    }

    ext->klv_pair = ispair;
    ext->klv_chan = chan;
    if (chan >= stride)
    {
        if (ispair)
        {
            ext->klv_chan = stride - 2;
        }
        else
        {
            ext->klv_chan = stride - 1;
        }
    }

    if (sadm)
    {
        uint8_t *p = (uint8_t *)ext;
        p += sizeof(*ext);
        sadm_bitstream_decoder_init(p, &ext->sdec);
    }

    pmd_s337m_init(&ext->s337m, wd, stride, pcm_next_block, ext, ext->klv_pair, ext->klv_chan, 0, sadm);
    ext->s337m.pa_found_cb = found_pa;
    ext->s337m.pa_found_cb_arg = ext;
}


void
dlb_pcmpmd_extractor_init2
    (dlb_pcmpmd_extractor          **extptr
    ,void                           *mem
    ,dlb_pmd_frame_rate              rate
    ,unsigned int                    chan
    ,unsigned int                    stride
    ,dlb_pmd_bool                    ispair
    ,dlb_pmd_model_combo            *model
    ,dlb_pmd_payload_set_status     *status
    ,dlb_pmd_bool                    sadm
    )
{
    dlb_pcmpmd_extractor_init3(extptr, mem, 0, rate, chan, stride, ispair, model, status, sadm);
}


void
dlb_pcmpmd_extractor_init
    (dlb_pcmpmd_extractor          **extptr
    ,void                           *mem
    ,dlb_pmd_frame_rate              rate
    ,unsigned int                    chan
    ,unsigned int                    stride
    ,dlb_pmd_bool                    ispair
    ,dlb_pmd_model_combo            *model
    ,dlb_pmd_payload_set_status     *status
    )
{
    dlb_pcmpmd_extractor_init2(extptr, mem, rate, chan, stride, ispair, model, status, 0);
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
dlb_pmd_bool                     /* @return 1 if a new frame was encountered, else 0*/
extract_block
    (dlb_pcmpmd_extractor *ext
    ,uint32_t **pcm
    ,size_t num_samples
    ,size_t *video_sync
    )
{
    uint32_t *end = *pcm + num_samples * ext->s337m.stride;

    TRACE(("EXTRACTOR: read block @%u %u/%u (%u/%u)\n",
           (unsigned)ext->sample_count, ext->cur_block_num, ext->max_block_num,
           (unsigned)num_samples, (unsigned)DLB_PCMPMD_BLOCK_SIZE));

    ext->s337m.vsync_offset = *video_sync;
    ext->s337m.pa_found = NO_PA_FOUND;
    pmd_s337m_unwrap(&ext->s337m, *pcm, end);
    
    if (ext->no_vsync)
    {
        /* This is communicating Pa position with respect to the start of the block of samples,
           which is not quite the same as frame start */
        *video_sync = ext->s337m.pa_found;
    }
    else
    {
        *video_sync -= num_samples;
    }
    ext->sample_count += num_samples;
    *pcm = end;
    return ext->new_frame;
}


dlb_pmd_success
dlb_pcmpmd_extract
    (dlb_pcmpmd_extractor *ext
    ,uint32_t             *pcm
    ,size_t                num_samples
    ,size_t                video_sync
    )
{
    size_t remaining = num_samples;

    reset_extractor_error(ext);

    ext->callback = NULL;
    ext->sadm_callback = NULL;
    ext->sadm_xml_buf = NULL;
    ext->cbarg = NULL;
    ext->no_vsync = 0;
    pcm += ext->klv_chan;
    if (ext->waiting)
    {
        if (video_sync > remaining)
        {
            return PMD_SUCCESS;
        }
        else
        {
            ext->waiting = 0;
            pcm += video_sync * ext->channel_count;
            remaining -= video_sync;
            video_sync = 0;
        }
    }

    while (remaining > EXTRACT_BLOCK_SIZE)
    {
        (void)extract_block(ext, &pcm, EXTRACT_BLOCK_SIZE, &video_sync);
        remaining -= EXTRACT_BLOCK_SIZE;
    }
    
    if (remaining)
    {
        (void)extract_block(ext, &pcm, remaining, &video_sync);
        /* remaining = 0; */
    }

    /* return 0 (success) if there is no error */
    return (ext->error_flag ? PMD_FAIL : PMD_SUCCESS);
}


dlb_pmd_success
dlb_pcmpmd_extract3
    (dlb_pcmpmd_extractor *ext
    ,uint32_t             *pcm
    ,size_t                num_samples
    ,dlb_pcmpmd_new_frame  callback
    ,dlb_pcmpmd_new_sadm   sadm_callback
    ,char                 *sadm_xml_buf
    ,void                 *cbarg
    ,size_t               *video_sync
    )
{
    size_t processed = 0;
    size_t remaining = num_samples;
    size_t vs;

    ext->callback = callback;
    ext->cbarg = cbarg;

    if (ext->sadm)
    {
        ext->sadm_callback = sadm_callback;
        ext->sadm_xml_buf = sadm_xml_buf;
    }
    else
    {
        ext->sadm_callback = NULL;
        ext->sadm_xml_buf = NULL;
    }

    vs = (ext->prev_pa == NO_PA_FOUND) ? 0 : NO_PA_FOUND;
    reset_extractor_error(ext);
    pcm += ext->klv_chan;
    ext->no_vsync = 1;

    /* add a block at a time to keep track of where in the frame we are */
    while (remaining > EXTRACT_BLOCK_SIZE)
    {
        remaining -= EXTRACT_BLOCK_SIZE;
        if (extract_block(ext, &pcm, EXTRACT_BLOCK_SIZE, &vs))
        {
            if (vs != NO_PA_FOUND && video_sync != NULL)
            {
                *video_sync = vs + processed;
            }
        }
        processed += EXTRACT_BLOCK_SIZE;
    }
    
    if (remaining)
    {
        if (extract_block(ext, &pcm, remaining, &vs))
        {
            if (vs != NO_PA_FOUND && video_sync != NULL)
            {
                *video_sync = vs + processed;
            }
            /* processed += remaining; */
            /* remaining = 0; */
        }
    }

    /* return 0 (success) if there is no error */
    return (ext->error_flag ? PMD_FAIL : PMD_SUCCESS);
}


dlb_pmd_success
dlb_pcmpmd_extract2
    (dlb_pcmpmd_extractor *ext
    ,uint32_t             *pcm
    ,size_t                num_samples
    ,dlb_pcmpmd_new_frame  callback
    ,void                 *cbarg
    ,size_t               *video_sync
    )
{
    return dlb_pcmpmd_extract3(ext, pcm, num_samples, callback, NULL, NULL, cbarg, video_sync);
}


dlb_pmd_bool
dlb_pcmpmd_extractor_error_flag
    (dlb_pcmpmd_extractor   *ext            /**< [in]  PCM extractor struct */
    )
{
    dlb_pmd_bool flag = PMD_FALSE;

    if (ext)
    {
        flag = ext->error_flag;
    }

    return flag;
}


const char *
dlb_pcmpmd_extractor_error_msg
    (dlb_pcmpmd_extractor   *ext            /**< [in]  PCM extractor struct */
    )
{
    const char *msg = "";

    if (ext)
    {
        msg = ext->error_msg;   /* TODO: not exactly memory-safe... */
    }

    return msg;
}
