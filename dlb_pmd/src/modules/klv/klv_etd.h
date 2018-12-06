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
 * @file klv_etd.h
 * @brief defines reading and writing the ED2 Turnaround Description (ETD) payload
 */

#ifndef KLV_ETD_H_
#define KLV_ETD_H_

#include "pmd_etd.h"
#include "pmd_idmap.h"
#include "klv_writer.h"
#include "klv_reader.h"


/* Define offsets within the structure for each field.  Note that
 * because this structure has 3 optional chunks (BSI, ExtBSI and AC4
 * metadata, we have to write in stages, adjusting offsets to take
 * into account variations.
 */

/* 1st chunk, the header, and the 1st optional payload (BSI) */
#define ETD1_ID(bo)                            (bo),      8
#define ETD1_B_ED2(bo)                    ((bo)+ 8),      1

/* The following bitfields are used when the 1st optional payload (BSI) is present */
#define ETD1_ED2_FRAME_RATE(bo)           ((bo)+ 9),      4
#define ETD1_ED2_PRESID(bo,i)    ((bo)+(13+(i)*17)),      9
#define ETD1_ED2_EEPID(bo,i)     ((bo)+(22+(i)*17)),      8


/**
 * @def ETD_ED2_BITS(n)
 * @brief size of optional ED2 payload chunk with n presentations
 */
#define ETD_ED2_BITS(n) ((n)?(((n)+1)*17 + 4):0)

/**
 * @def OPT1(bo,n,off)
 * @brief calculate bit-offset for 2nd chunk
 *
 * The bit-offset for the Dolby E chunk of the payload (if present)
 * depends on whether or not the ED2 payload is present.  This macro
 * computes the correct offset accordingly.
 */
#define OPT1(bo,ed2,off) ((bo) + (off) + ETD_ED2_BITS(ed2))


/* 2st chunk, optional Dolby E turnaround chunk
 * These bitfields assume that the ED2 payload is not present.  If it
 * is present, then they must be offset according to presence flag 
 * entries into a nominal 128-bit int:  shift    len */

#define ETD2_B_DE(bo,ed2)               OPT1(bo,ed2,9),   1
#define ETD2_DE_FRAME_RATE(bo,ed2)      OPT1(bo,ed2,10),  4
#define ETD2_DE_PGM_CONFIG(bo,ed2)      OPT1(bo,ed2,14),  5
#define ETD2_DE_PRESID(bo,ed2,i)        OPT1(bo,ed2,19+(i)*17), 9
#define ETD2_DE_EEPID(bo,ed2,i)         OPT1(bo,ed2,28+(i)*17), 8


/**
 * @def ETD_DE_BITS(n)
 * @brief size of optional Dolby E payload chunk with n presentations
 */
#define ETD_DE_BITS(n) ((n)?(9 + ((n)+1)*17):0)


/**
 * @def ETD_PAYLOAD_BITS(ed2,de)
 * @brief compute size of encoder config payload (in bytes) according
 * to the presence or absence of bsi (boolean), extended bsi (boolean), DE
 * info, AC-4 info and number of presentations.
 */
#define ETD_PAYLOAD_BITS(ed2,de) (10 + ETD_ED2_BITS(ed2) + ETD_DE_BITS(de))


/**
 * @brief encode KLV frame rate
 *    
 * convert dlb_pmd_frame_rate enum to the KLV enum:
 *  1:23.98, 2:24, 3:25, 4:29.97, 5:30
 */
static inline
uint8_t                         /** @return encoded value */
klv_encode_frame_rate
    (dlb_pmd_frame_rate rate    /**< [in] frame rate enum to encode */
    )
{
    return rate + 1;
}


/**
 * @brief decode KLV frame rate
 *    
 * convert encoded KLV value to dlb_pmd_frame_rate enum
 *  1:23.98, 2:24, 3:25, 4:29.97, 5:30
 */
static inline
dlb_pmd_frame_rate         /** @return frame rate enum */
klv_decode_frame_rate
    (uint8_t rate          /**< [in] encoded value */
    )
{
    return (dlb_pmd_frame_rate)(rate-1);
}


/**
 * @brief write as many ED2 Turnaround descriptions to the KLV output stream
 * that will fit
 */
static inline
int                                /** @return 0 on success, 1 on error */
klv_etd_write
    (klv_writer *w                 /**< [in] KLV writer */
    ,dlb_pmd_model *model          /**< [in] model to write */
    )
{
    if (klv_write_local_key_opened(w))
    {
        pmd_apd *apd_list = model->apd_list;
        pmd_eep *eep_list = model->eep_list;
        pmd_etd *etd;
        turnaround *turnaround;
        unsigned int bo = 0;
        unsigned int i;
        uint8_t *wp = w->wp;
        
        etd = &model->etd_list[model->etd_written];
        for (i = model->etd_written; i != model->num_etd; ++i)
        {
            unsigned int ed2    = etd->ed2_presentations;
            unsigned int de     = etd->de_presentations;
            size_t payload_bits = ETD_PAYLOAD_BITS(ed2,de);
            unsigned int j;
            
            if (klv_writer_space(w) < (bo+payload_bits + 7)/8)
            {
                break;
            }
            else
            {
                TRACE(("        ED2 turnaround %u\n", etd->id));
                if (!bo)
                {
                   *wp = 0;
                }
                memset(wp+1, '\0', (bo + payload_bits + 7)/8-1);

                set_(wp, ETD1_ID(bo),    etd->id);
                set_(wp, ETD1_B_ED2(bo), ed2>0);
                if (ed2)
                {
                    turnaround = etd->ed2_turnaround;
                    set_(wp, ETD1_ED2_FRAME_RATE(bo), klv_encode_frame_rate(etd->ed2_framerate));
                    for (j = 0; j != ed2; ++j)
                    {
                        unsigned int pidx = turnaround->presid;
                        unsigned int aidx = turnaround->eepid;
                        if (pidx >= w->model->num_apd || aidx >= w->model->num_eep)
                        {
                            /* invalid presentation id or EAC3 encoding parameters id*/
                            return 1;
                        }
                        set_(wp, ETD1_ED2_PRESID(bo,j), apd_list[pidx].id);
                        set_(wp, ETD1_ED2_EEPID(bo,j),  eep_list[aidx].id);
                        ++turnaround;
                    }
                    /* write end-of-list marker */
                    set_(wp, ETD1_ED2_PRESID(bo,j), 0);
                    set_(wp, ETD1_ED2_EEPID(bo,j), 0);
                }
                set_(wp, ETD2_B_DE(bo,ed2), de>0);
                if (de)
                {
                    turnaround = etd->de_turnaround;
                    set_(wp, ETD2_DE_FRAME_RATE(bo,ed2), klv_encode_frame_rate(etd->de_framerate));
                    set_(wp, ETD2_DE_PGM_CONFIG(bo,ed2), etd->pgm_config);
                    turnaround = etd->de_turnaround;
                    for (j = 0; j != de; ++j)
                    {
                        unsigned int pidx = turnaround->presid;
                        unsigned int aidx = turnaround->eepid;
                        if (pidx >= w->model->num_apd || aidx >= w->model->num_eep)
                        {
                            /* invalid presentation id or EAC3 encoding parameters id*/
                            return 1;
                        }
                        set_(wp, ETD2_DE_PRESID(bo,ed2,j), apd_list[pidx].id);
                        set_(wp, ETD2_DE_EEPID(bo,ed2,j),  eep_list[aidx].id);
                        ++turnaround;
                    }
                    /* write end-of-list marker */
                    set_(wp, ETD2_DE_PRESID(bo,ed2,j), 0);
                    set_(wp, ETD2_DE_EEPID(bo,ed2,j),  0);
                }
                wp += (bo + payload_bits) / 8;
                bo = (bo + payload_bits) % 8;
            }
            w->wp = wp;
            ++etd;
        }
        if (bo)
        {
            w->wp += 1;
        }
        model->etd_written = i;
        return w->wp == w->buffer;
    }
    return 0;
}


/**
 * @brief extract an ED2 Turnaround from serialized form
 */
static inline
int                              /** @return 0 on success, 1 on error */
klv_etd_read
    (klv_reader *r               /**< [in] KLV buffer to read */
    ,int payload_length          /**< [in] bytes in presentation payload */
    )
{
    dlb_pmd_model *model = r->model;
    pmd_etd *etd;
    unsigned int ed2 = 0;
    unsigned int de = 0;
    uint8_t *rp = r->rp;
    uint8_t *end = rp + payload_length;
    unsigned int presid;
    unsigned int eepid;
    unsigned int id;
    unsigned int bo = 0;
    unsigned int payload_bits;
    uint16_t idx;
    
    while (r->rp < end-1)
    {
        id = get_(rp, ETD1_ID(bo));
        if (!pmd_idmap_lookup(r->etd_ids, id, &idx))
        {
            if (model->num_etd == MAX_ED2_TURNAROUNDS)
            {
                klv_reader_error_at(r, "No space for ETD payload in model\n");
                return 1;
            }

            idx = (uint16_t)model->num_etd;
            pmd_idmap_insert(r->etd_ids, id, idx);
            model->num_etd += 1;
        }
        TRACE(("        %u\n", id));
        etd = &model->etd_list[idx];

        etd->id = id;
        etd->ed2_presentations = 0;
        etd->de_presentations = 0;

        ed2 = 0;
        de = 0;

        if (get_(rp, ETD1_B_ED2(bo)))
        {
            etd->ed2_framerate = klv_decode_frame_rate(get_(rp, ETD1_ED2_FRAME_RATE(bo)));
            presid = get_(rp, ETD1_ED2_PRESID(bo, ed2));
            eepid = get_(rp, ETD1_ED2_EEPID(bo, ed2));
            while (0 != presid)
            {
                if (!pmd_idmap_lookup(r->apd_ids, presid, &idx))
                {
                    klv_reader_error_at(r, "ETD %d refers to uknown presentation %d "
                                        "parsing ED2 turnaround\n",
                                        id, presid);
                    return 1;
                }
                etd->ed2_turnaround[ed2].presid = idx;
                if (!pmd_idmap_lookup(r->eep_ids, eepid, &idx))
                {
                    klv_reader_error_at(r, "ETD %d refers to uknown EEP payload %d "
                                        "parsing ED2 turnaround\n",
                                        id, eepid);
                    return 1;
                }
                etd->ed2_turnaround[ed2].eepid = idx;

                ++ed2;
                presid = get_(rp, ETD1_ED2_PRESID(bo,ed2));
                eepid = get_(rp, ETD1_ED2_EEPID(bo,ed2));
            }
        }
        etd->ed2_presentations = ed2;

        if (get_(rp, ETD2_B_DE(bo,ed2)))
        {
            etd->de_framerate = klv_decode_frame_rate(get_(rp, ETD2_DE_FRAME_RATE(bo,ed2)));
            etd->pgm_config = get_(rp, ETD2_DE_PGM_CONFIG(bo,ed2));
            presid = get_(rp, ETD2_DE_PRESID(bo,ed2,de));
            eepid = get_(rp, ETD2_DE_EEPID(bo,ed2,de));
            while (0 != presid)
            {
                if (!pmd_idmap_lookup(r->apd_ids, presid, &idx))
                {
                    klv_reader_error_at(r, "ETD %d refers to uknown presentation %d "
                                        "parsing DE turnaround\n",
                                        id, presid);
                    return 1;
                }
                etd->de_turnaround[de].presid = idx;
                if (!pmd_idmap_lookup(r->eep_ids, eepid, &idx))
                {
                    klv_reader_error_at(r, "ETD %d refers to uknown EEP payload %d "
                                        "parsing DE turnaround\n",
                                        id, eepid);
                    return 1;
                }
                etd->de_turnaround[de].eepid = idx;
                ++de;
                presid = get_(rp, ETD2_DE_PRESID(bo,ed2,de));
                eepid = get_(rp, ETD2_DE_EEPID(bo,ed2,de));
            }
            etd->de_presentations = de;
        }
        payload_bits = ETD_PAYLOAD_BITS(ed2,de);
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


#endif /* KLV_ETD_H_ */
