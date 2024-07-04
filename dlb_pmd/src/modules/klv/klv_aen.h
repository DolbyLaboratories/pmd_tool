/************************************************************************
 * dlb_pmd
 * Copyright (c) 2017-2019, Dolby Laboratories Inc.
 * Copyright (c) 2017-2019, Dolby International AB.
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
 * @file klv_aen.h
 * @brief defines reading and writing Audio Element Name (AEN) payloads
 */


#ifndef KLV_AEN_H_
#define KLV_AEN_H_

#include "pmd_abd_aod.h"
#include "pmd_idmap.h"
#include "klv_writer.h"
#include "klv_reader.h"
#include <assert.h>


/* entries into byte array:                   bitpos    len */
#define AEN_ELEMENT_ID(bo)                      (bo),    12
#define AEN_CHARVAL(bo,i)            ((bo)+12+(i)*8),     8

/**
 * @def AEN_BITS(len)
 * @brief number of bits required for a name of length len
 */
#define AEN_BITS(len)  (20 + 8*(len))


/**
 * @brief write an audio presentation to the KLV output stream
 */
static inline
int            /** @return 0 on success, 1 on error */
klv_aen_write
    (klv_writer *w         /**< [in] KLV writer */
    ,dlb_pmd_model *model  /**< [in] source model to write */
    )
{
    if (klv_write_local_key_opened(w))
    {
        pmd_aen *e = &model->aen_list[model->write_state.aen_written];
        uint8_t *wp = w->wp;
        unsigned int i;
        unsigned int bo = 0;
        size_t name_bits;
        uint8_t *name;
        int len;
        int j;
    
        for (i = model->write_state.aen_written; i != model->num_elements; ++i)
        {
            name = e->name;
            len = (int)strlen((char*)name);

            if ((uint8_t)name[0] == 0xff)
            {
                break;
            }
            else if (len > 0)
            {
                name_bits = AEN_BITS(len);

                if (klv_writer_space(w) < (bo + name_bits+7)/8)
                {
                    break;
                }
                TRACE(("        AEN: %s\n", e->name));
                if (!bo)
                {
                    *wp = 0;
                }
                memset(wp+1, '\0', (bo + name_bits+7)/8 - 1);
                
                set_(wp, AEN_ELEMENT_ID(bo), e->id);
                for (j = 0; j != len; ++j)
                {
                    set_(wp, AEN_CHARVAL(bo, j), name[j]);
                }
                set_(wp, AEN_CHARVAL(bo, j), 0);
                wp += (bo + name_bits) /8;
                bo = (bo + name_bits) % 8;
                w->wp = wp;
            }
            ++e;
        }
        model->write_state.aen_written = i;
        if (bo)
        {
            w->wp += 1;
        }
    }
    return 0;
}


/**
 * @brief extract element names from serialized form
 */
static inline
int                                             /** @return 0 on success, 1 on error */
klv_aen_read
    (klv_reader *r                              /**< [in] KLV buffer to read */
    ,int payload_length                         /**< [in] bytes in presentation payload */
    ,dlb_pmd_payload_status_record *read_status /**< [out] read status record, may be NULL */
    )
{
    uint8_t *rp = r->rp;
    uint8_t *end = rp + payload_length;
    pmd_aen *e = NULL;
    uint16_t eid;
    uint16_t idx;
    uint8_t *name = NULL;
    unsigned int bo = 0;
    unsigned int name_bits;
    unsigned int len;
    unsigned int max_len;
    uint8_t c;
    
    while (rp < end-2)
    {
        eid = (uint16_t)get_(rp, AEN_ELEMENT_ID(bo));
        if (0 == eid)
        {
            /* end of list */
            break;
        }

        if (!pmd_idmap_lookup(r->aen_ids, eid, &idx))
        {
            /* allow as yet unknown names */            
            idx = r->model->num_aen;
            r->model->num_aen += 1;
            pmd_idmap_insert(r->aen_ids, eid, idx);
        }

        e = &r->model->aen_list[idx];
        e->id = eid;
        name = e->name;
        memset(name, '\0', sizeof(e->name));

        len = 0;
        max_len = sizeof(e->name);
        c = (uint8_t)get_(rp, AEN_CHARVAL(bo,len));
        while (rp < end && c != 0)
        {
            if (len >= max_len)
            {
                /* string too long! error */
                klv_reader_error_at(r, DLB_PMD_PAYLOAD_STATUS_OUT_OF_MEMORY, read_status,
                                    "Too many characters (%u) in name for audio element %u\n",
                                    len, (unsigned int)eid);
                return 1;
            }
            name[len] = c;
            len += 1;
            c = (uint8_t)get_(rp, AEN_CHARVAL(bo,len));
        }
        TRACE(("        %s\n", e->name));

        name_bits = AEN_BITS(len);
        rp += (bo + name_bits) / 8;
        bo = (bo + name_bits) % 8;
        r->rp = rp;
    }
    if (bo)
    {
        r->rp += 1;
    }
    r->rp = end;
    return 0;
}


#endif /* KLV_AEN_H_ */
