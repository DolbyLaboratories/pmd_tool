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
 * @file klv_hed.h
 * @brief defines reading and writing the Headphone Element Description (HED) payload
 */

#ifndef KLV_HED_H_
#define KLV_HED_H_

#include "pmd_hed.h"
#include "pmd_idmap.h"
#include "klv_writer.h"
#include "klv_reader.h"


/*                                           bitpos    len */

#define HED_ELEMENT_ID(bo)                     (bo),      12
#define HED_HEAD_TRACK_ENABLED(bo)          (bo)+12,       1
#define HED_RENDER_MODE(bo)                 (bo)+13,       7
#define HED_CHANNEL_MASK(bo)                (bo)+20,      16

#define HED_PAYLOAD_BITS(bed)      ((bed)? 36 : 20)



/**
 * @brief write a headphone element description to the KLV output stream
 *
 * Note that this writes *all* objects at the given update time
 */
static inline
int
klv_hed_write
    (klv_writer *w
    ,dlb_pmd_model *model
    )
{
    if (klv_write_local_key_opened(w))
    {
        pmd_element *elements = model->element_list;
        pmd_hed *hed;
        uint8_t *wp = w->wp;
        unsigned int i = 0;
        unsigned int bo = 0;  /* start bit offset for current bed */

        memset(wp, '\0', klv_writer_space(w));

        hed = &model->hed_list[model->write_state.hed_written];
        for (i = model->write_state.hed_written; i != model->num_hed; ++i)
        {
            pmd_element *e = &elements[hed->audio_element_id];
            size_t payload_bits = HED_PAYLOAD_BITS(e->mode == PMD_MODE_CHANNEL);

            if (klv_writer_space(w) < (bo+payload_bits + 7)/8)
            {
                break;
            }
            else
            {
                uint16_t id = e->id;
                if (!bo)
                {
                   *wp = 0;
                }
                memset(wp+1, '\0', (bo + payload_bits + 7)/8-1);

                TRACE(("        HED: %u\n", id));
                set_(wp, HED_ELEMENT_ID(bo),         id);
                set_(wp, HED_HEAD_TRACK_ENABLED(bo), hed->head_tracking_enabled);
                set_(wp, HED_RENDER_MODE(bo),        hed->render_mode);
                if (e->mode == PMD_MODE_CHANNEL)
                {
                    set_(wp, HED_CHANNEL_MASK(bo), hed->channel_mask);
                }
            }
            wp += (bo + payload_bits) / 8;
            bo = (bo + payload_bits) % 8;
            w->wp = wp;
            ++hed;
        }
        if (bo)
        {
            ++wp;
        }
        w->wp = wp;
        model->write_state.hed_written = i;
    }
    return 0;
}


/**
 * @brief extract a headphone element description from serialized form
 */
static inline
int                                             /** @return 0 on success, 1 on error */
klv_hed_read
    (klv_reader *r                              /**< [in] KLV buffer to read */
    ,int payload_length                         /**< [in] bytes in presentation payload */
    ,dlb_pmd_payload_status_record *read_status /**< [out] read status record, may be NULL */
    )
{
    dlb_pmd_model *model = r->model;
    pmd_element *e;
    pmd_hed *hed;
    uint8_t *rp = r->rp;
    uint8_t *end = rp + payload_length;
    pmd_element_id id;
    size_t payload_bits;
    unsigned int bo = 0;
    uint16_t idx;

    while (rp < end - 2)
    {
        dlb_pmd_payload_status ps;

        id = (pmd_element_id)get_(rp, HED_ELEMENT_ID(bo));
        ps = pmd_validate_audio_element_id(id);
        if (ps != DLB_PMD_PAYLOAD_STATUS_OK)
        {
            klv_reader_error_at(r, ps, read_status,
                                "Invalid audio element id %u for headphone element description\n",
                                (unsigned int)id);
            return 1;
        }
        if (!pmd_idmap_lookup(r->element_ids, id, &idx))
        {
            klv_reader_error_at(r, DLB_PMD_PAYLOAD_STATUS_INCORRECT_STRUCTURE, read_status,
                                "HED payload refers to unknown audio element %u\n",
                                (unsigned int)id);
            return 1; 
        }                    
        e = &model->element_list[idx];

        if (model->num_hed >= model->profile.constraints.max.num_headphone_desc)
        {
            klv_reader_error_at(r, DLB_PMD_PAYLOAD_STATUS_OUT_OF_MEMORY, read_status, "No space for HED payload in model\n");
            return 1;
        }

        TRACE(("        HeadphoneElement %u\n", e->id));
        hed = &model->hed_list[model->num_hed];
        hed->audio_element_id      = idx;
        hed->head_tracking_enabled = (pmd_bool)get_(rp, HED_HEAD_TRACK_ENABLED(bo));
        hed->render_mode           = (dlb_pmd_render_mode)get_(rp, HED_RENDER_MODE(bo));    /* No validation needed */
        hed->channel_mask          = 0xffff;
        e->hed_idx = model->num_hed;
        model->num_hed += 1;
        if (e->mode == PMD_MODE_CHANNEL)
        {
            hed->channel_mask = (dlb_pmd_channel_mask)get_(rp, HED_CHANNEL_MASK(bo));
        }

        payload_bits = HED_PAYLOAD_BITS(e->mode == PMD_MODE_CHANNEL);
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


#endif /* KLV_HED_H_ */
