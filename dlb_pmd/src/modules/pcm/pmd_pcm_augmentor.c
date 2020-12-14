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

/**
 * @file pmc_pcm_augmentor.c
 * @brief PMD model PCM augmentor
 *
 * Overwrite final two channels of PCM with SMPTE-337m wrapped KLV
 */

#include "pmd_model.h"
#include "pmd_smpte_337m.h"
#include "dlb_pmd_pcm.h"

#include "sadm_bitstream_encoder.h"
#include "pmd_bitstream.h"

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
    dlb_pmd_model               *model;                     /**< source model */
    dlb_klvpmd_universal_label   ul;                        /**< Universal label */
    uint8_t                      klvbuf[MAX_DATA_BYTES];    /**< current KLV block write buffer */     
    uint8_t                     *klvp;                      /**< current read pointer of klvbuf */
    size_t                       klvsize;                   /**< bytes remaining in block */
    pmd_s337m                    s337m;                     /**< SMPTE 337m wrapping state */
    unsigned int                 samples;                   /**< samples remaining to write in current block */
    dlb_pmd_frame_rate           rate;                      /**< video frame rate */
    unsigned int                 numchannels;               /**< number of channels of PCM */
    unsigned int                 klvchan;                   /**< 1st channel for klv */
    unsigned int                 maxblock;                  /**< maximum number of blocks for frame rate */
    unsigned int                 block;                     /**< current block number */

    dlb_pcmpmd_new_frame         callback;  /**< client callback to invoke before beginning new frame */
    void                        *cbarg;     /**< client parameter for client callback */
    dlb_pmd_bool                 sadm;      /**< generate sADM instead of PMD? */
    sadm_bitstream_encoder      *senc;      /**< abstracts the creation of sADM bitstreams */
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
    int byte_size;
    int res = 1;

    aug->block += 1;
    memset(aug->klvbuf, '\0', MAX_DATA_BYTES);
    if (aug->block == aug->maxblock)
    {
        dlb_pmd_apply_updates(aug->model, aug->rate);
        aug->block = 0;
        res = 0;
        if (aug->callback)
        {
            aug->callback(aug->cbarg);
        }
    }

    byte_size = (s337m->sadm)
        ? sadm_bitstream_encoder_encode(s337m, aug->senc, aug->model, aug->rate, aug->klvbuf)
        : generate_pmd_bitstream(s337m, aug->model, aug->block, aug->maxblock,
                                 aug->ul, aug->klvbuf);
    s337m->data = aug->klvbuf;
    s337m->databits = 8 * byte_size;
    if (0 == byte_size)
    {
        s337m->data = NULL;
    }
    return res;
}


unsigned int
dlb_pcmpmd_min_frame_size
   (dlb_pmd_frame_rate frame_rate
   )
{
    if (frame_rate > DLB_PMD_FRAMERATE_LAST)
    {
        return 0;
    }

    return pmd_s337m_min_frame_size(frame_rate);
}


size_t
dlb_pcmpmd_augmentor_query_mem2
    (dlb_pmd_bool sadm
    ,dlb_pmd_model_constraints *limits
    )
{
    size_t sz = sizeof(struct dlb_pcmpmd_augmentor);

    if (sadm)
    {
        if (limits == NULL)
        {
            sz = 0;
        }
        else
        {
            sz += sadm_bitstream_encoder_query_mem(limits);
        }
    }

    return sz;
}


void
dlb_pcmpmd_augmentor_init3
    (dlb_pcmpmd_augmentor **augptr
    ,dlb_pmd_model *model
    ,void *mem
    ,unsigned int wrap_depth
    ,dlb_pmd_frame_rate rate
    ,dlb_klvpmd_universal_label ul
    ,dlb_pmd_bool mark_empty_blocks
    ,unsigned int numchannels
    ,unsigned int stride
    ,dlb_pmd_bool ispair
    ,unsigned int start
    ,dlb_pmd_bool sadm
    )
{
    dlb_pcmpmd_augmentor *aug = mem;
    unsigned int wd;       /** s337m wrapping bit depth */
    unsigned int vfsize;   /** video frame size in samples */

    memset(mem, '\0', sizeof(dlb_pcmpmd_augmentor));

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

    *augptr    = aug;
    aug->model = model;
    aug->rate  = rate;
    aug->ul    = ul;
    aug->block = ~0u;
    aug->sadm  = sadm;

    if (start >= stride)
    {
        if (ispair)
        {
            start = stride - 2;
        }
        else
        {
            start = stride - 1;
        }
    }

    vfsize = pmd_s337m_min_frame_size(rate);
    aug->numchannels = numchannels;
    aug->maxblock = sadm ? 1 : (vfsize / DLB_PCMPMD_BLOCK_SIZE);

    if (ispair && ((aug->numchannels < 2) || (aug->numchannels & 1)))
    {
        abort();
    }

    aug->klvchan = start;
    pmd_apn_list_iterator_init(&model->write_state.apni, &model->apn_list);
    if (sadm)
    {
        dlb_pmd_model_constraints limits;
        dlb_pmd_get_constraints(model, &limits);
        sadm_bitstream_encoder_init(&limits, (void*)(aug+1), &aug->senc);
    }
    pmd_s337m_init(&aug->s337m, wd, stride, pcm_next_block, aug, ispair, start, mark_empty_blocks, sadm);
}


void
dlb_pcmpmd_augmentor_init2
    (dlb_pcmpmd_augmentor **aug
    ,dlb_pmd_model *model
    ,void *mem
    ,dlb_pmd_frame_rate rate
    ,dlb_klvpmd_universal_label ul
    ,dlb_pmd_bool mark_empty_blocks
    ,unsigned int numchannels
    ,unsigned int stride
    ,dlb_pmd_bool is_pair
    ,unsigned int start
    ,dlb_pmd_bool sadm
    )
{
    dlb_pcmpmd_augmentor_init3(aug, model, mem, 0, rate, ul, mark_empty_blocks,
                               numchannels, stride, is_pair, start,
                               sadm);
}


void
dlb_pcmpmd_augmentor_init
    (dlb_pcmpmd_augmentor **aug
    ,dlb_pmd_model *model
    ,void *mem
    ,dlb_pmd_frame_rate rate
    ,dlb_klvpmd_universal_label ul
    ,dlb_pmd_bool mark_empty_blocks
    ,unsigned int numchannels
    ,unsigned int stride
    ,dlb_pmd_bool is_pair
    ,unsigned int start
    )
{
    dlb_pcmpmd_augmentor_init2(aug, model, mem, rate, ul, mark_empty_blocks,
                               numchannels, stride, is_pair, start,
                               0);
}


void
dlb_pcmpmd_augmentor_finish
    (dlb_pcmpmd_augmentor *aug
    )
{
    (void)aug;
}


size_t
dlb_pcmpmd_augmentor_try_frame_query_mem
    (dlb_pcmpmd_augmentor *aug
    )
{
    size_t sz = 0;

    if (aug != NULL)
    {
        sz = dlb_pcmpmd_augmentor_model_try_frame_query_mem(aug->model, aug->sadm);
    }

    return sz;
}


size_t
dlb_pcmpmd_augmentor_model_try_frame_query_mem
    (dlb_pmd_model        *model
    ,dlb_pmd_bool          sadm
    )
{
    size_t sz = 0;

    if (model != NULL)
    {
        dlb_pmd_model_constraints limits;
        dlb_pmd_get_constraints(model, &limits);
        sz = dlb_pcmpmd_augmentor_query_mem2(sadm, &limits);
    }

    return sz;
}


typedef struct 
{
    dlb_pmd_model           *model;
    pmd_model_write_state    final_state;
} try_frame_cbarg;


static
void
try_frame_callback(void* arg)
{
    if (arg != NULL)
    {
        try_frame_cbarg *cbarg = (try_frame_cbarg *)arg;
        memcpy(&cbarg->final_state, &cbarg->model->write_state, sizeof(cbarg->final_state));
    }
}


static
dlb_pmd_bool
try_frame_test_xyz
    (uint16_t        num_xyz
    ,pmd_xyz_set    *xyz_set
    )
{
    unsigned int i;

    for (i = 0; i < num_xyz; i++)
    {
        if (!pmd_xyz_set_test(xyz_set, i))
        {
            return PMD_FALSE;
        }
    }
    return PMD_TRUE;
}


dlb_pcmpmd_write_status
dlb_pcmpmd_augmentor_model_try_frame
    (dlb_pmd_model        *model
    ,void                 *mem
    ,uint32_t             *buf
    ,unsigned int          num_channels
    ,unsigned int          num_samples
    ,dlb_pmd_frame_rate    rate
    ,dlb_pmd_bool          pair
    ,dlb_pmd_bool          sadm
)
{
    dlb_pcmpmd_write_status s;
    pmd_model_write_state write_state;
    unsigned int n_pmd, min_frame;
    dlb_pcmpmd_augmentor *faux = (dlb_pcmpmd_augmentor *)mem;
    try_frame_cbarg cbarg;

    /* Check argument values */
    if (model == NULL || buf == NULL || mem == NULL || rate > DLB_PMD_FRAMERATE_LAST)
    {
        return DLB_PCMPMD_WRITE_STATUS_ERROR;
    }

    n_pmd = pair ? 2 : 1;
    min_frame = pmd_s337m_min_frame_size(rate);
    if (num_channels < n_pmd || num_samples < min_frame)
    {
        return DLB_PCMPMD_WRITE_STATUS_ERROR;
    }

    /* Save and reset the model's write state */
    memcpy(&write_state, &model->write_state, sizeof(write_state));
    memset(&model->write_state, 0, sizeof(model->write_state));
    pmd_apn_list_iterator_init(&model->write_state.apni, &model->apn_list);

    /* Set up a faux augmentor, initialized to beginning of a frame */
    memset(faux, 0, sizeof(*faux));
    faux->model = model;
    faux->rate = rate;
    faux->ul = DLB_PMD_KLV_UL_ST2109;
    faux->block = ~0u;
    faux->sadm = sadm;
    faux->numchannels = num_channels;
    faux->maxblock = sadm ? 1 : (min_frame / DLB_PCMPMD_BLOCK_SIZE);
    faux->klvchan = 0;
    if (sadm)
    {
        dlb_pmd_model_constraints limits;
        dlb_pmd_get_constraints(model, &limits);
        sadm_bitstream_encoder_init(&limits, (void *)(faux + 1), &faux->senc);
    }
    pmd_s337m_init(&faux->s337m, (sadm ? 24 : 20), num_channels, pcm_next_block, faux, pair, 0, PMD_FALSE, sadm);

    s = DLB_PCMPMD_WRITE_STATUS_RED;
    if (sadm)
    {
        if (faux->s337m.databits > 0)   /* If the encoded model is too big, the sADM encoder sets databits to 0 */
        {
            s = DLB_PCMPMD_WRITE_STATUS_GREEN;
        }
    } 
    else
    {
        /* Set up callback data */
        cbarg.model = model;
        memset(&cbarg.final_state, 0, sizeof(cbarg.final_state));

        /* Augment a full frame */
        dlb_pcmpmd_augment2(faux, buf, min_frame, 0, try_frame_callback, &cbarg);

        /* How much did we write? */
        if ((!model->esd_present || (model->esd_present && cbarg.final_state.esd_written)) &&
            (cbarg.final_state.abd_written == model->num_abd) &&
            (cbarg.final_state.aod_written == model->num_elements - cbarg.final_state.abd_written) && /* TODO: other kinds of elements? */
            (cbarg.final_state.apd_written == model->num_apd) &&
            (cbarg.final_state.hed_written == model->num_hed) &&
            (model->iat == NULL || !(model->iat->options & PMD_IAT_PRESENT) || (cbarg.final_state.iat_written)) &&
            (try_frame_test_xyz(model->num_xyz, &cbarg.final_state.xyz_written)) &&
            (cbarg.final_state.eep_written == model->num_eep) &&
            (cbarg.final_state.etd_written == model->num_etd) &&
            (cbarg.final_state.pld_written == model->num_pld)
            )
        {
            s = DLB_PCMPMD_WRITE_STATUS_YELLOW;
            if ((cbarg.final_state.apn_written == model->apn_list.num) &&
                (cbarg.final_state.aen_written == model->num_elements) &&
                (!model->esd || (cbarg.final_state.esn_written == model->esd->count))
                )
            {
                s = DLB_PCMPMD_WRITE_STATUS_GREEN;
            }
        }

        /* Restore the model's write state */
        memcpy(&model->write_state, &write_state, sizeof(model->write_state));
    }

    return s;
}


dlb_pcmpmd_write_status
dlb_pcmpmd_augmentor_try_frame
    (dlb_pcmpmd_augmentor *aug
    ,void                 *mem
    ,uint32_t             *buf
    ,unsigned int          num_channels
    ,unsigned int          num_samples
    )
{
    if (aug == NULL || buf == NULL || mem == NULL || num_channels == 0 || num_samples == 0)
    {
        return DLB_PCMPMD_WRITE_STATUS_ERROR;
    }

    return dlb_pcmpmd_augmentor_model_try_frame(aug->model, mem, buf, num_channels, num_samples, aug->rate, aug->s337m.pair, aug->sadm);
}


void
dlb_pcmpmd_augment2
    (dlb_pcmpmd_augmentor *aug
    ,uint32_t *pcm
    ,size_t num_samples
    ,size_t video_sync
    ,dlb_pcmpmd_new_frame callback
    ,void                *cbarg
    )
{
    uint32_t *end;

    pcm += aug->klvchan;
    end = pcm + num_samples * aug->s337m.stride;

    aug->s337m.vsync_offset = video_sync;
    aug->callback = callback;
    aug->cbarg = cbarg;
    pmd_s337m_wrap(&aug->s337m, pcm, end);
}


void
dlb_pcmpmd_augment
    (dlb_pcmpmd_augmentor *aug
    ,uint32_t *pcm
    ,size_t num_samples
    ,size_t video_sync
    )
{
    dlb_pcmpmd_augment2(aug, pcm, num_samples, video_sync, NULL, NULL);
}

