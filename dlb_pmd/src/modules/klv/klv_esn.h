/************************************************************************
 * dlb_pmd
 * Copyright (c) 2017-2020, Dolby Laboratories Inc.
 * Copyright (c) 2017-2020, Dolby International AB.
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
 * @file klv_esn.h
 * @brief defines reading and writing the ED2 Stream Name (ESN) payload
 */

#ifndef KLV_ESN_H_
#define KLV_ESN_H_

#include "pmd_abd_aod.h"
#include "pmd_idmap.h"
#include "klv_writer.h"
#include "klv_reader.h"
#include <assert.h>


/* entries into byte array:                   bitpos    len */
#define ESN_STREAM_IDX                            0,     4
#define ESN_CHARVAL                               4,     8

#define ESN_BITS(len) ((4+((len)+1)*8))
#define ESN_BYTES(len) ((ESN_BITS(len) + 7)/8)

/**
 * @brief write an ED2 stream name to the KLV output stream
 */
static inline
int            /** @return 0 on success, 1 on error */
klv_esn_write
    (klv_writer *w            /**< [in] KLV writer */
    ,dlb_pmd_model *model     /**< [in] PMD model */
    ,unsigned int sindex      /**< [in] stream index */
    )
{
    if (model->esd && klv_write_local_key_opened(w))
    {
        size_t payload_bytes;
        char name[DLB_PMD_TITLE_SIZE];
        uint8_t *wp = w->wp;
        size_t len;
        size_t j;
        
        if (!strcmp((char*)model->title, PMD_UNTITLED_MODEL_TITLE))
        {
            snprintf(name, sizeof(name), "Stream [%u/%u]", sindex+1,
                     model->esd->count);
        }
        else
        {
            snprintf(name, sizeof(name), "%s [%u/%u]", model->title,
                     sindex+1, model->esd->count);
        }
            
        len = strlen(name);
        payload_bytes = ESN_BYTES(len);
        if (klv_writer_space(w) < payload_bytes)
        {
            /* it's not an error to fail to write payload if there is no
             * room in the buffer; it is an error, however, if there was nothing
             * in the buffer to start with, because it means that either the name
             * is too long, or the buffer far too small. */
            return w->wp == w->buffer;
        }
        TRACE(("        ESN: %s\n", name));
        memset(wp, '\0', payload_bytes);
        set_(wp, ESN_STREAM_IDX, sindex);
        for (j = 0; j != len; ++j)
        {
            set_(wp, ESN_CHARVAL, name[j]);
            ++wp;
        }
        set_(wp, ESN_CHARVAL, 0);  /* add null terminator (possibly redundant!) */
        ++wp; 
        model->write_state.esn_written += 1;
        model->write_state.esn_bitmap |= (1ul << sindex);
        w->wp = wp+1; /* make sure final nibble is written */
    }
    return 0;
}


/**
 * @brief extract E stream names from serialized form
 */
static inline
int                                             /** @return 0 on success, 1 on error */
klv_esn_read
    (klv_reader *r                              /**< [in] KLV buffer to read */
    ,int payload_length                         /**< [in] bytes in ESN payload */
    ,dlb_pmd_payload_status_record *read_status /**< [out] read status record, may be NULL */
    )
{
    /**
     * This does not conform to the current PMD spec, it reads but one name...  which
     * is compatible with how we are writing the payload, but is not compliant with the spec.
     * HOWEVER, the effect is to just skip over the remaining stream index records, so we
     * are likely OK.
     */

    uint8_t *rp = r->rp;
    uint8_t *end = rp + payload_length;
    uint8_t *name = NULL;
    uint8_t *name_end = NULL;
    uint8_t c = 1;
    
    (void)read_status;

    memset(r->model->title, '\0', sizeof(r->model->title));
    name = r->model->title;
    name_end = name + sizeof(r->model->title) - 1;
    while (rp < end && c != 0)
    {
        if (name < name_end)    /* Truncate extra-long sADM names */
        {
            c = get_(rp, ESN_CHARVAL);
            *name++ = c;
        }
        ++rp;
    }
    
    if (NULL != name)
    {
        TRACE(("        %s\n", r->model->title));

        /* now walk backwards to remove the [%u/%u] part */
        /* TODO: this looks buggy - what if there is not a [%u/%u] part? */
        while (*name != ' ' && name >= r->model->title)
        {
            --name;
        }
        if (name < r->model->title)
        {
            name = r->model->title;
        }
        *name = '\0';
    }
    
    r->rp = end;    /* skip remainder of the payload */
    return 0;
}


#endif /* KLV_ESN_H_ */
