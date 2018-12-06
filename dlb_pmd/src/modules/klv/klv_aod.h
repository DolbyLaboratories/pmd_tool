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
 * @file klv_aod.h
 * @brief defines reading and writing Audio Object Description (AOD) payloads
 */

#ifndef KLV_AOD_H_
#define KLV_AOD_H_

#include "pmd_abd_aod.h"
#include "klv_writer.h"
#include "klv_reader.h"



/* Audio Object Description (AOD) bitpos,   length*/
#define AOD_ID(bo)                    (bo),        12
#define AOD_CLASS(bo)            ((bo)+12),         4
#define AOD_DYNUP(bo)            ((bo)+16),         1
#define AOD_XPOS(bo)             ((bo)+17),        10
#define AOD_YPOS(bo)             ((bo)+27),        10
#define AOD_ZPOS(bo)             ((bo)+37),        10
#define AOD_SIZE(bo)             ((bo)+47),         5
#define AOD_SZVR(bo)             ((bo)+52),         1
#define AOD_DIVERGE(bo)          ((bo)+53),         1
#define AOD_SRC(bo)              ((bo)+54),         8
#define AOD_GAIN(bo)             ((bo)+62),         6

/**
 * @def AOD_PAYLOAD_BITS
 * @brief size of an audio object description in bytes
 */
#define AOD_PAYLOAD_BITS  (68)

/**
 * @brief write an audio object to the KLV output stream
 */
static inline
int
klv_aod_write
    (klv_writer *w
    ,dlb_pmd_model *model
    )
{
    if (klv_write_local_key_opened(w))
    {
        pmd_element *e;
        unsigned int ei;
        uint8_t *wp = w->wp;
        unsigned int bo = 0;  /* start bit offset for current bed */
        pmd_object_metadata *omd;
        
        e = &model->element_list[model->obj_write_index];
        for (ei = model->obj_write_index; ei < model->num_elements; ++ei, ++e)
        {
            if (PMD_MODE_OBJECT == e->mode)
            {
                if (klv_writer_space(w) < (bo+AOD_PAYLOAD_BITS+7)/8)
                {
                    break;
                }
                
                if (!bo)
                {
                    *wp = 0;
                }
                memset(wp+1, '\0', (bo+AOD_PAYLOAD_BITS+7)/8);
                omd = &e->md.object;
                set_(wp, AOD_ID(bo),      e->id);
                set_(wp, AOD_CLASS(bo),   omd->oclass);
                set_(wp, AOD_DYNUP(bo),   omd->dynamic_updates != 0);
                set_(wp, AOD_XPOS(bo),    omd->x);
                set_(wp, AOD_YPOS(bo),    omd->y);
                set_(wp, AOD_ZPOS(bo),    omd->z);
                set_(wp, AOD_SIZE(bo),    omd->size);
                set_(wp, AOD_SZVR(bo),    omd->size_vertical);
                set_(wp, AOD_DIVERGE(bo), omd->diverge);
                set_(wp, AOD_GAIN(bo),    omd->gain);
                set_(wp, AOD_SRC(bo),     omd->source+1);

                wp += (bo + AOD_PAYLOAD_BITS) / 8;
                bo = (bo + AOD_PAYLOAD_BITS) % 8;
                w->wp = wp;
                model->aod_written += 1;

                TRACE(("        AOD: %u\n", e->id));
            }
        }
        model->obj_write_index = ei;
        if (bo)
        {
            w->wp += 1;
        }
    }
    return 0;
}


/**
 * @brief extract an audio object from serialized form
 */
static inline
int                          /** @return 0 on success, 1 on error */
klv_aod_read
    (klv_reader *r           /**< [in] KLV buffer to read */
    ,int payload_length      /**< [in] bytes in audio objects payload */
    )
{
    dlb_pmd_model *model = r->model;
    pmd_object_metadata *omd;
    pmd_element *e;
    uint8_t *rp = r->rp;
    uint8_t *end = rp + payload_length;
    pmd_element_id id;
    unsigned int bo = 0;
    unsigned int src;
    uint16_t idx;

    while (rp < end - 8)
    {
        id = (pmd_element_id)get_(rp, AOD_ID(bo));
        if (!pmd_idmap_lookup(r->element_ids, id, &idx))
        {
            if (model->num_elements == MAX_AUDIO_ELEMENTS)
            {
                klv_reader_error_at(r, "No space for AOD element in model\n");
                return 1; 
            }            

            idx = (uint16_t)model->num_elements;
            pmd_idmap_insert(r->element_ids, id, idx);
            model->num_elements += 1;
        }

        TRACE(("        AudioObject %u\n", id));
        e = &model->element_list[idx];
        e->id = id;
        e->mode = PMD_MODE_OBJECT;
        e->hed_idx = 0xffff;
        omd = &e->md.object;

        omd->oclass          = get_(rp, AOD_CLASS(bo));
        omd->dynamic_updates = (pmd_bool)    get_(rp, AOD_DYNUP(bo));
        omd->x               = (pmd_position)get_(rp, AOD_XPOS(bo));
        omd->y               = (pmd_position)get_(rp, AOD_YPOS(bo));
        omd->z               = (pmd_position)get_(rp, AOD_ZPOS(bo));
        omd->size            = (pmd_size)    get_(rp, AOD_SIZE(bo));
        omd->size_vertical   = (pmd_bool)    get_(rp, AOD_SZVR(bo));
        omd->diverge         = (pmd_bool)    get_(rp, AOD_DIVERGE(bo));
        omd->gain            = (pmd_gain)    get_(rp, AOD_GAIN(bo));
        src                  = (unsigned int)get_(rp, AOD_SRC(bo));
        omd->source          = (uint8_t)src - 1;

        if (src > 0 && !pmd_signals_test(&r->signals, omd->source))
        {
            pmd_signals_add(&r->signals, omd->source);
            r->num_signals += 1;
        }

        rp += (bo + AOD_PAYLOAD_BITS) / 8;
        bo = (bo + AOD_PAYLOAD_BITS) % 8;
        r->rp = rp;
    }
    if (bo)
    {
        r->rp += 1;
    }
    return 0;
}


#endif /* KLV_AOD_H_ */
