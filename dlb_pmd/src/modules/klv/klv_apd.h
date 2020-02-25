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
 * @file klv_apd.h
 * @brief defines reading and writing Audio Presentation Description (APD) payloads
 */


#ifndef KLV_APD_H_
#define KLV_APD_H_

#include "pmd_apd.h"
#include "pmd_idmap.h"
#include "klv_writer.h"
#include "klv_reader.h"


/* entries into byte array:                           bitpos    len */
#define AUDIO_PRESENTATION_ID(bo)                       (bo),     9
#define AUDIO_PRESENTATION_CONFIG(bo)              ((bo)+ 9),     5
#define AUDIO_PRESENTATION_LANGCOD0(bo)            ((bo)+14),     5
#define AUDIO_PRESENTATION_LANGCOD1(bo)            ((bo)+19),     5
#define AUDIO_PRESENTATION_LANGCOD2(bo)            ((bo)+24),     5
#define AUDIO_PRESENTATION_ELEMENT(bo,i)    ((bo)+29+12*(i)),    12

#define AUDIO_PRESENTATION_PAYLOAD_BITS(n) (29+12*((n)+1))


/**
 * @brief write audio presentations to the KLV output stream
 */
static inline
int
klv_apd_write
    (klv_writer *w
    ,dlb_pmd_model *model
    )
{
    if (klv_write_local_key_opened(w))
    {
        pmd_apd *pres;
        uint8_t *wp = w->wp;
        unsigned int n;
        unsigned int payload_bits;
        unsigned int bo = 0;
        unsigned int i;
        unsigned int j;
        pmd_apd_iterator pi;
        pmd_element *elements = w->model->element_list;
        unsigned int idx;
        
        pres = &model->apd_list[model->write_state.apd_written];
        for (j = model->write_state.apd_written; j != model->num_apd; ++j, ++pres)
        {
            n = pres->num_elements;
            payload_bits = AUDIO_PRESENTATION_PAYLOAD_BITS(n);

            if (klv_writer_space(w) < (bo + payload_bits+7)/8)
            {
                break;
            }

            if (!bo)
            {
                *wp = 0;
            }
            memset(wp+1, '\0', (bo+payload_bits+7)/8);
            set_(wp, AUDIO_PRESENTATION_ID(bo),       pres->id);
            set_(wp, AUDIO_PRESENTATION_CONFIG(bo),   klv_encode_speaker_config(pres->config));
            set_(wp, AUDIO_PRESENTATION_LANGCOD0(bo), klv_encode_langch(pres->pres_lang >> 24));
            set_(wp, AUDIO_PRESENTATION_LANGCOD1(bo), klv_encode_langch(pres->pres_lang >> 16));
            set_(wp, AUDIO_PRESENTATION_LANGCOD2(bo), klv_encode_langch(pres->pres_lang >> 8));

            i = 0;
            pmd_apd_iterator_init(&pi, pres);
            while (pmd_apd_iterator_next(&pi, &idx))
            {
                if (idx >= model->write_state.abd_written + model->write_state.aod_written)
                {
                    /* don't write presentation if it refers to an element
                     * that hasn't been written yet
                     */
                     model->write_state.apd_written = j;
                     if (bo)
                     {
                         w->wp += 1;
                     }
                     return 0;
                }

                set_(wp, AUDIO_PRESENTATION_ELEMENT(bo,i), elements[idx].id);
                ++i;
            }
            set_(wp, AUDIO_PRESENTATION_ELEMENT(bo,i), 0);  /* terminating 0 id */
            wp += (bo + payload_bits) / 8;
            bo = (bo + payload_bits) % 8;
            w->wp = wp;

            TRACE(("        APD: %u\n", pres->id));
        }
        model->write_state.apd_written = j;
        if (bo)
        {
            w->wp += 1;
        }
    }
    return 0;
}


/**
 * @brief extract audio presentations from serialized form
 */
static inline
int                                             /** @return 0 on success, 1 on error */
klv_apd_read
    (klv_reader *r                              /**< [in] KLV buffer to read */
    ,int payload_length                         /**< [in] bytes in presentation payload */
    ,dlb_pmd_payload_status_record *read_status /**< [out] read status record, may be NULL */
    )
{
    dlb_pmd_model *model = r->model;
    pmd_presentation_id id;
    pmd_element_id eid;
    pmd_apd *pres;
    uint8_t *rp = r->rp;
    uint8_t *end = rp + payload_length;
    unsigned int bo = 0;
    unsigned int payload_bits;
    size_t max_elements;
    uint16_t idx;
        
    while (rp < end - 4)
    {
        dlb_pmd_payload_status ps;
        uint8_t langch0;
        uint8_t langch1;
        uint8_t langch2;

        id = (pmd_presentation_id)get_(rp, AUDIO_PRESENTATION_ID(bo));
        if (id == DLB_PMD_RESERVED_PRESENTATION_ID)
        {
            break;
        }
        if (!pmd_idmap_lookup(r->apd_ids, id, &idx))
        {
            if (model->num_apd == r->model->profile.constraints.max.num_presentations)
            {
                klv_reader_error_at(r, DLB_PMD_PAYLOAD_STATUS_OUT_OF_MEMORY, read_status, "No space for APD presentation in model\n");
                return 1; 
            }            

            idx = (uint16_t)model->num_apd;
            pmd_idmap_insert(r->apd_ids, id, idx);
            model->num_apd += 1;
        }
      
        TRACE(("        Presentation %u -> %u\n", id, idx));
        pres = &model->apd_list[idx];
        pres->id = id;        
        pres->num_elements = 0;
        pres->pres_lang = 0;
        pres->num_names = 0;
        pmd_elements_init(&pres->elements);

        pres->config = (dlb_pmd_speaker_config)get_(rp, AUDIO_PRESENTATION_CONFIG(bo));
        ps = klv_speaker_config_validate(pres->config);
        if (ps != DLB_PMD_PAYLOAD_STATUS_OK)
        {
            klv_reader_error_at(r, ps, read_status, "Presentation %d has invalid speaker configuration %d\n",
                                id, pres->config);
            return 1;
        }
        if (!klv_decode_speaker_config((unsigned int)pres->config, &pres->config))
        {
            klv_reader_error_at(r, DLB_PMD_PAYLOAD_STATUS_VALUE_OUT_OF_RANGE, read_status,
                                "Can't decode speaker config value %u, reading APD\n",
                                (unsigned int)pres->config);
            return 1;
        }

        langch0 = get_(rp, AUDIO_PRESENTATION_LANGCOD0(bo));
        ps = klv_reader_validate_langcod_char(langch0);
        if (ps != DLB_PMD_PAYLOAD_STATUS_OK)
        {
            klv_reader_error_at(r, ps, read_status, "Presentation %d has invalid language code character [0] %u\n",
                                id, langch0);
            return 1;
        }

        langch1 = get_(rp, AUDIO_PRESENTATION_LANGCOD1(bo));
        ps = klv_reader_validate_langcod_char(langch1);
        if (ps != DLB_PMD_PAYLOAD_STATUS_OK)
        {
            klv_reader_error_at(r, ps, read_status, "Presentation %d has invalid language code character [1] %u\n",
                                id, langch1);
            return 1;
        }

        langch2 = get_(rp, AUDIO_PRESENTATION_LANGCOD2(bo));
        ps = klv_reader_validate_langcod_char(langch2);
        if (ps != DLB_PMD_PAYLOAD_STATUS_OK)
        {
            klv_reader_error_at(r, ps, read_status, "Presentation %d has invalid language code character [2] %u\n",
                                id, langch2);
            return 1;
        }

        pres->pres_lang |= klv_decode_langch(langch0) << 24;
        pres->pres_lang |= klv_decode_langch(langch1) << 16;
        pres->pres_lang |= klv_decode_langch(langch2) << 8;

        /* compute max possible number of elements remaining in payload */
        max_elements = ((end - rp) * 8 - (bo + 26)) / 12;
        do
        {
            eid = (pmd_element_id)get_(rp, AUDIO_PRESENTATION_ELEMENT(bo,pres->num_elements));
            if (eid)
            {
                if (!pmd_idmap_lookup(r->element_ids, eid, &idx))
                {
                    klv_reader_error_at(r, DLB_PMD_PAYLOAD_STATUS_MISSING_AUDIO_ELEMENT, read_status, "Presentation %d refers to non-existent element %d\n",
                                        id, (int)eid);
                    return 1;
                }
                pmd_elements_add(&pres->elements, idx);
                pres->num_elements += 1;
            }
        }
        while (eid && pres->num_elements < max_elements);
        payload_bits = AUDIO_PRESENTATION_PAYLOAD_BITS(pres->num_elements);
        rp += (bo + payload_bits) / 8;
        bo = (bo + payload_bits) % 8;
        r->rp = rp;

        /* now look through presentation names and see if any can be added */
        pmd_apn_list_isolate(&model->apn_list, pres);
    }
    if (bo)
    {
        r->rp += 1;
    }
    return 0;
}


#endif /* KLV_PRESENTATION_INC_H */
