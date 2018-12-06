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
 * @file klv_apn.h
 * @brief defines reading and writing Audio Presentation Name (APN) payloads
 */

#ifndef KLV_APN_H_
#define KLV_APN_H_

#ifdef _MSC_VER
__pragma(warning(disable:4706))
#endif


#include "pmd_apd.h"
#include "pmd_idmap.h"
#include "klv_writer.h"
#include "klv_reader.h"
#include <assert.h>


/* entries into byte array:                   bitpos    len */
#define APN_PRESENTATION_ID(bo)                 (bo),      9
#define APN_LANGCOD0(bo)                   ((bo)+ 9),      5
#define APN_LANGCOD1(bo)                   ((bo)+14),      5
#define APN_LANGCOD2(bo)                   ((bo)+19),      5
#define APN_CHARVAL(bo,i)            ((bo)+24+(i)*8),      8


/**
 * @def APN_BITS(len)
 * @brief number of bits required for a presentation name
 */
#define APN_BITS(len)  (32 + 8*(len))


/**
 * @brief write an audio presentation to the KLV output stream
 */
static inline
int            /** @return 0 on success, 1 on error */
klv_apn_write
    (klv_writer *w         /**< [in] KLV writer */
    ,dlb_pmd_model *model  /**< [in] source model to write */
    )
{
    if (klv_write_local_key_opened(w))
    {
        pmd_apn *name;
        size_t name_bits;
        unsigned int bo = 0;
        uint8_t *wp = w->wp;
        uint8_t *text;
        size_t len;
        unsigned int j;
        
        while ((name = pmd_apn_list_iterator_get(&model->apni)))
        {
            text = name->text;

            if ((uint8_t)text[0] == 0xff)
            {
                break;
            }

            len = strlen((char*)text);
            name_bits = APN_BITS(len);
            if (klv_writer_space(w) < (bo + name_bits+7)/8)
            {
                break;
            }
            TRACE(("        APN[%u]: %s\n", name->presid, text));
            if (!bo)
            {
                *wp = 0;
            }
            memset(wp+1, 0, (bo + name_bits+7)/8);
            set_(wp, APN_PRESENTATION_ID(bo), name->presid);
            set_(wp, APN_LANGCOD0(bo), klv_encode_langch(name->lang >> 24));
            set_(wp, APN_LANGCOD1(bo), klv_encode_langch(name->lang >> 16));
            set_(wp, APN_LANGCOD2(bo), klv_encode_langch(name->lang >> 8));
                
            for (j = 0; j != len; ++j)
            {
                set_(wp, APN_CHARVAL(bo,j), text[j]);
            }
            set_(wp, APN_CHARVAL(bo,j), 0);
            wp += (bo + name_bits) / 8;
            bo = (bo + name_bits) % 8;
            w->wp = wp;
            pmd_apn_list_iterator_next(&model->apni);
        }
        if (bo)
        {
            w->wp += 1;
        }
    }
    return 0;
}


/**
 * @brief extract presentation names from serialized form
 */
static inline
int                                /** @return 0 on success, 1 on error */
klv_apn_read
    (klv_reader *r                 /**< [in] KLV buffer to read */
    ,int payload_length            /**< [in] bytes in presentation payload */
    )
{
    uint8_t *rp = r->rp;
    uint8_t *end = rp + payload_length;
    pmd_apn *name = NULL;
    pmd_apd *pres = NULL;
    unsigned int bo = 0;
    unsigned int name_bits;
    uint16_t presid;
    uint8_t c;
    unsigned int len;
    uint8_t *text = NULL;
    uint8_t *text_end = NULL;
    uint16_t presidx;
    pmd_langcode lang;

    while (rp < end-3)
    {
        presid = get_(rp, APN_PRESENTATION_ID(bo));
        if (presid == 0)
        {
            /* end of list */
            break;
        }

        lang = 0;
        lang |= klv_decode_langch(get_(rp, APN_LANGCOD0(bo))) << 24;
        lang |= klv_decode_langch(get_(rp, APN_LANGCOD1(bo))) << 16;
        lang |= klv_decode_langch(get_(rp, APN_LANGCOD2(bo))) << 8;

        /* look up presentation name, if it exists */
        name = pmd_apn_list_find(&r->model->apn_list, presid, lang);

        if (!name)
        {
            name = pmd_apn_list_add(&r->model->apn_list);
            if (!name)
            {
                klv_reader_error_at(r, "No space for APN name in model\n");
                return 1;
            }
        }
        pmd_apn_list_mark(&r->model->apn_list, name);

        pres = NULL;
        if (pmd_idmap_lookup(&r->model->apd_ids, presid, &presidx))
        {
            pres = &r->model->apd_list[presidx];
            if (pres->num_names >= DLB_PMD_MAX_PRESENTATION_NAMES)
            {
                /* too many names */
                return 1;
            }
        }

        if (pres)
        {
            unsigned int i;
            for (i = 0; i != pres->num_names; ++i)
            {
                if (pres->names[i] == name->idx)
                {
                    break;
                }
            }

            if (i == pres->num_names)
            {
                pres->names[pres->num_names] = name->idx;
                pres->num_names += 1;
            }
        }
        text = name->text;

        memset(text, '\0', sizeof(name->text));
        text_end = text + sizeof(name->text) - 1;

        name->presid = presid;
        name->lang = lang;
        name->readcount += 1;

        len = 0;
        c = get_(rp, APN_CHARVAL(bo,len));
        while (rp < end && c != 0)
        {
            if (text == text_end)
            {
                /* string too long! error */
                return 1;
            }
            text[len] = c;
            len += 1;
            c = get_(rp, APN_CHARVAL(bo,len));
        }
#ifdef KLV_READ_TRACE
        {
             char tmp[4];

             memset(tmp, '\0', sizeof(tmp));
             pmd_langcode_string(name->lang, &tmp);
             printf("        APN[%u]: %s %s\n", presid, tmp, name->text);
        }
#endif
        name_bits = APN_BITS(len);
        rp += (bo + name_bits) / 8;
        bo = (bo + name_bits) % 8;
        r->rp = rp;
    }

    r->rp = end;
    return 0;
}


#endif /* KLV_APN_H_ */
