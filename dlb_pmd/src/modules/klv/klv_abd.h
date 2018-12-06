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
 * @file klv_abd.h
 * @brief defines reading and writing Audio Bed Description (ABD) payloads
 */

#ifndef KLV_ABD_H_
#define KLV_ABD_H_

#include "src/model/pmd_abd_aod.h"
#include "klv_speaker_config.h"
#include "klv_writer.h"
#include "klv_reader.h"



/* Audio Bed Description (ABD)      bitpos,    length */
#define ABD_ID(bo)                     (bo),        12
#define ABD_SPKRCFG(bo)           ((bo)+12),         5
#define ABD_TYPE(bo)              ((bo)+17),         1
#define ABD_SOURCE(bo)            ((bo)+18),        12

#define OPT1(bo,t,x) ((bo) + (x) + ((t)?12:0))
#define ABD_TARGET(bo,t,i)         OPT1(bo,t,(18+20*i)), 6
#define ABD_SRC(bo,t,i)            OPT1(bo,t,(24+20*i)), 8
#define ABD_GAIN(bo,t,i)           OPT1(bo,t,(32+20*i)), 6

#define ABD_PAYLOAD_BITS(n,d)      (18 + ((n)+1) * 20 + ((d)?12:0))


/**
 * @brief write an audio bed to the KLV output stream
 */
static inline
int                           /** @return 0 on success, 1 on error */
klv_abd_write
    (klv_writer *w            /**< [in] KLV writer state */
    ,dlb_pmd_model *model     /**< [in] PMD model to write */
    )
{
    if (klv_write_local_key_opened(w))
    {
        pmd_element *e;
        unsigned int i;
        unsigned int ei;
        uint8_t *wp = w->wp;
        unsigned int bo = 0;  /* start bit offset for current bed */
        pmd_channel_metadata *cmd;
        pmd_track_metadata *tmd;
        size_t payload_bits;

        if (model->abd_written >= model->num_abd)
        {
            return 0;
        }

        e = &model->element_list[model->bed_write_index];
        for (ei = model->bed_write_index; ei < model->num_elements; ++ei, ++e)
        {
            if (PMD_MODE_CHANNEL == e->mode)
            {
                cmd = &e->md.channel;
                tmd = cmd->metadata;
                payload_bits = ABD_PAYLOAD_BITS(cmd->num_tracks, cmd->derived);
                if (klv_writer_space(w) < (bo + payload_bits+7)/8)
                {
                    break;
                }
                
                if (!bo)
                {
                    *wp = 0;
                }
                memset(wp+1, '\0', (bo+payload_bits+7)/8);
                
                set_(wp, ABD_ID(bo),       e->id);
                set_(wp, ABD_SPKRCFG(bo),  klv_encode_speaker_config(cmd->config));
                set_(wp, ABD_TYPE(bo),     cmd->derived);
                if (cmd->derived)
                {
                    set_(wp, ABD_SOURCE(bo),  cmd->origin);
                }
                
                for (i = 0; i != cmd->num_tracks; ++i)
                {
                    set_(wp, ABD_TARGET(bo,cmd->derived,i), tmd->target);
                    set_(wp, ABD_SRC(bo,cmd->derived,i),    tmd->source+1);
                    set_(wp, ABD_GAIN(bo,cmd->derived,i),   tmd->gain);
                    ++tmd;
                }
                /* nope, need to write another track, with target 0 */
                set_(wp, ABD_TARGET(bo,cmd->derived,i), 0); /* indicate end of tracks */
                set_(wp, ABD_SRC(bo,cmd->derived,i),    0);
                set_(wp, ABD_GAIN(bo,cmd->derived,i),   0);
            
                wp += (bo + payload_bits) / 8;
                bo = (bo + payload_bits) % 8;
                w->wp = wp;
                model->abd_written += 1;

                TRACE(("        ABD: %u\n", e->id));
            }
        }
        model->bed_write_index = ei;
        if (bo)
        {
            w->wp += 1;
        }
    }
    return 0;
}


/**
 * @brief extract an audio bed from serialized form
 */
static inline
int                          /** @return 0 on success, 1 on error */
klv_abd_read
    (klv_reader *r           /**< [in] KLV buffer to read */
    ,int payload_length      /**< [in] bytes in audio objects payload */
    )
{
    dlb_pmd_model *model = r->model;
    pmd_channel_metadata *cmd;
    pmd_track_metadata *tmd;
    pmd_element *e;
    uint8_t *rp = r->rp;
    uint8_t *end = rp + payload_length;
    unsigned int src;
    pmd_bool d = 0;
    pmd_element_id id;
    unsigned int bo = 0;
    unsigned int payload_bits;
    uint16_t idx;

    while (rp < end-4)
    {
        id = (pmd_element_id)get_(rp, ABD_ID(bo));
        
        if (!pmd_idmap_lookup(r->element_ids, id, &idx))
        {
            if (model->num_elements == MAX_AUDIO_ELEMENTS)
            {
                klv_reader_error_at(r, "No space for ABD element in model\n");
                return 1; 
            }            

            idx = (uint16_t)model->num_elements;
            pmd_idmap_insert(r->element_ids, id, idx);
            model->num_elements += 1;
            model->num_abd += 1;
        }
        TRACE(("        AudioBed %u\n", id));

        e = &model->element_list[idx];
        e->id = id;
        e->mode = PMD_MODE_CHANNEL;
        e->hed_idx = 0xffff;
        cmd = &e->md.channel;
        cmd->num_tracks = 0;

        if (!klv_decode_speaker_config((unsigned int)get_(rp, ABD_SPKRCFG(bo)), &cmd->config))
        {
            klv_reader_error_at(r, "Illegal speaker config value, reading ABD\n");
            return 1;
        }
        cmd->derived        = (pmd_bool)get_(rp, ABD_TYPE(bo));
        d = 0;
        if (cmd->derived)
        {
            cmd->origin     = (pmd_element_id)get_(rp, ABD_SOURCE(bo));
            d = 1;
        }
    
        tmd = cmd->metadata;
        do
        {
            /* todo: check for consistency */
            tmd->target        = (pmd_speaker) get_(rp, ABD_TARGET(bo,d,cmd->num_tracks));
            src                = (unsigned int)get_(rp, ABD_SRC(bo,d,cmd->num_tracks));
            tmd->gain          = (pmd_gain)    get_(rp, ABD_GAIN(bo,d, cmd->num_tracks));
            tmd->source        = (uint8_t)src - 1;

            if (tmd->target == 0)
            {
                break;
            }
            if (src > 0 && !pmd_signals_test(&r->signals, tmd->source))
            {
                pmd_signals_add(&r->signals, tmd->source);
                r->num_signals += 1;
            }
            cmd->num_tracks += 1;
            ++tmd;
        }
        while (cmd->num_tracks <= MAX_PMD_CHANNEL_METADATA && src > 0);
        pmd_bed_set_normal_form(cmd);

        payload_bits = ABD_PAYLOAD_BITS(cmd->num_tracks, cmd->derived);
        rp += (bo + payload_bits) / 8;
        bo = (bo + payload_bits) % 8;
        r->rp = rp;
    }
    if (bo)
    {
        r->rp += 1;
    }
    return 0;
}


#undef OPT1

#endif /* KLV_ABD_ */
