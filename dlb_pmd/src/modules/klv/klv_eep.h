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
 * @file klv_eep.h
 * @brief defines reading and writing EAC3 Encoding Parameters (EEP) payloads
 */

#ifndef KLV_EAC3_ENCODING_PARAMETERS_INC_H
#define KLV_EAC3_ENCODING_PARAMETERS_INC_H


#include "pmd_eep.h"
#include "klv_writer.h"
#include "klv_reader.h"
#include "pmd_idmap.h"


/**
 * @def KLV_SURMIX_OFFSET
 * @brief maps valid surrounds to the bitstream values
 *
 * There are only 5 valid surround downmix values, -1.5, -3.0, -4.5, -6.0, -inf
 * but these correspond to bitstream values 3,4,5,6,7 (the others are RESERVED).
 * This constant converts between internal enum and bitstream values.
 */
#define KLV_SURMIX_OFFSET (3)


/* Define offsets within the structure for each field.  Note that
 * because this structure has 3 optional chunks (BSI, ExtBSI and AC4
 * metadata, we have to write in stages, adjusting offsets to take
 * into account variations.
 */

/* 1nd chunk, the header, bo is the bit-in-byte of 1st bit of this description */
#define EEP1_ID(bo)              (bo),        8
#define EEP1_B_ENCODER(bo)       ((bo)+ 8),   1

/* The following bitfields are used when the 1st optional payload (encoder params) is present */
#define EEP1_DYNRNG_PROF(bo)     ((bo)+ 9),   3
#define EEP1_COMPR_PROF(bo)      ((bo)+12),   3
#define EEP1_SURROUND_90(bo)     ((bo)+15),   1
#define EEP1_HMIXLEV(bo)         ((bo)+16),   5


/**
 * @def EEP_ENCODER_BITS
 * @brief size of optional encoder parameters payload chunk in bits
 */
#define EEP_ENCODER_BITS (12)


/**
 * @def OPT1(bo,e,off)
 * @brief calculate bit-offset for 3rd chunk
 *
 * The bit-offset for the bitstream parameters presence bit and payload (if
 * present) depends on whether or not the encoding parameters payload is
 * present.
 */
#define OPT1(bo,e,off) ((bo) + (off) + ((e)?0:(-EEP_ENCODER_BITS)))


/* 3rd chunk, optional bitstream parameter flag and data
 * These bitfields assume that the encoder parameters payload chunk
 * is present.  If not, then they must be offset according to presence
 * flag.
 *
 * entries into a nominal 128-bit int:  shift    len
 */

#define EEP2_B_BITSTREAM(bo,e)      OPT1(bo,e,21),    1

#define EEP2_BSMOD(bo,e)            OPT1(bo,e,22),    3
#define EEP2_SURMOD(bo,e)           OPT1(bo,e,25),    2
#define EEP2_DIALNORM(bo,e)         OPT1(bo,e,27),    5
#define EEP2_PREF_DMIXMOD(bo,e)     OPT1(bo,e,32),    2
#define EEP2_LTRT_CMIXLEV(bo,e)     OPT1(bo,e,34),    3
#define EEP2_LTRT_SURMIXLEV(bo,e)   OPT1(bo,e,37),    3
#define EEP2_LORO_CMIXLEV(bo,e)     OPT1(bo,e,40),    3
#define EEP2_LORO_SURMIXLEV(bo,e)   OPT1(bo,e,43),    3


/**
 * @def EEP_BITSTREAM_BITS
 * @brief size of optional Extended BSI payload chunk
 */
#define EEP_BITSTREAM_BITS (24)


/**
 * @def OPT2(bo,e,b,off)
 * @brief calcuate bit-offset within a nominal 128-bit word for 4th chunk
 *
 * The bit-offset for the DRC advanced codec settings depends on
 * whether or not the encoder and bistream chunks exist.
 */
#define OPT2(bo,e,b,off) (OPT1(bo,e,off)+((b)?0:-EEP_BITSTREAM_BITS))


#define EEP_DRC_BITS (15)

#define EEP3_B_DRC(bo,e,b)             OPT2(bo,e,b,46), 1
#define EEP3_DRC_PORT_SPKR(bo,e,b)     OPT2(bo,e,b,47), 3
#define EEP3_DRC_PORT_HPHN(bo,e,b)     OPT2(bo,e,b,50), 3
#define EEP3_DRC_PORT_FLAT(bo,e,b)     OPT2(bo,e,b,53), 3
#define EEP3_DRC_PORT_HTHR(bo,e,b)     OPT2(bo,e,b,56), 3
#define EEP3_DRC_PORT_DDPL(bo,e,b)     OPT2(bo,e,b,59), 3


/* laast chunk, the presentation list */
#define EEP4_PRESLIST(bo,e,b,d,n)       (OPT2(bo,e,b,62) + ((d)?0:-EEP_DRC_BITS) + (n)*9), 9


/**
 * @def EEP_PAYLOAD_BITS(n,e,b,drc)
 * @brief compute size of encoder config payload (in bits) according
 * to the presence or absence of encoding, bitstream, drc info
 * presentations
 */
#define EEP_PAYLOAD_BITS(n,e,b,d) \
    ( (11 + ((n)+1)*9) \
    + ((e)? EEP_ENCODER_BITS:0) \
    + ((b)? EEP_BITSTREAM_BITS:0) \
    + ((d)? EEP_DRC_BITS:0) \
    )

/**
 * @brief write as many EAC3 encoding parameter structs to the KLV output stream
 * as will fit
 */
static inline
int                                 /** @return 0 on success, 1 on error */
klv_eep_write
    (klv_writer *w                  /**< [in] KLV writer */
    ,dlb_pmd_model *model           /**< [in] model to write */
    )
{
    if (klv_write_local_key_opened(w))
    {
        pmd_apd *apd_list = w->model->apd_list;
        pmd_eep *eep;
        unsigned int i;
        unsigned int j;
        uint8_t *wp = w->wp;
        unsigned int bo = 0;  /* start bit offset */
        
        eep = &model->eep_list[model->eep_written];
        for (i = model->eep_written; i != model->num_eep; ++i)
        {
            unsigned int b = (eep->options & PMD_EEP_BITSTREAM_PRESENT) ? 1 : 0;
            unsigned int e = (eep->options & PMD_EEP_ENCODER_PRESENT) ? 1 : 0;
            unsigned int d = (eep->options & PMD_EEP_DRC_PRESENT) ? 1 : 0;
            unsigned int n = eep->num_presentations;
            size_t payload_bits = EEP_PAYLOAD_BITS(n,e,b,d);
            pmd_presentation_id *p = eep->presentations;
                        
            if (klv_writer_space(w) < (bo+payload_bits+7)/8)
            {
                break;
            }
            else
            {               
                TRACE(("        EAC3 Encoding Parameters %u\n", eep->id));
                if (!bo)
                {
                    *wp = 0;
                }
                memset(wp+1, '\0', (bo + payload_bits+7)/8-1);

                set_(wp, EEP1_ID(bo),              eep->id);
                set_(wp, EEP1_B_ENCODER(bo),       e);
                if (e)
                {
                    set_(wp, EEP1_DYNRNG_PROF(bo), eep->dynrng_prof);
                    set_(wp, EEP1_COMPR_PROF(bo),  eep->compr_prof);
                    set_(wp, EEP1_SURROUND_90(bo), eep->surround90);
                    set_(wp, EEP1_HMIXLEV(bo),     eep->hmixlev);
                }
                set_(wp, EEP2_B_BITSTREAM(bo,e),   b);
                if (b)
                {
                    set_(wp, EEP2_BSMOD(bo,e),          eep->bsmod);
                    set_(wp, EEP2_SURMOD(bo,e),         eep->dsurmod);
                    set_(wp, EEP2_DIALNORM(bo,e),       eep->dialnorm);
                    set_(wp, EEP2_PREF_DMIXMOD(bo,e),   eep->dmixmod);
                    set_(wp, EEP2_LTRT_CMIXLEV(bo,e),   eep->ltrtcmixlev);
                    set_(wp, EEP2_LTRT_SURMIXLEV(bo,e), eep->ltrtsurmixlev+KLV_SURMIX_OFFSET);
                    set_(wp, EEP2_LORO_CMIXLEV(bo,e),   eep->lorocmixlev);
                    set_(wp, EEP2_LORO_SURMIXLEV(bo,e), eep->lorosurmixlev+KLV_SURMIX_OFFSET);
                }
                set_(wp, EEP3_B_DRC(bo,e,b), d);
                if (d)
                {
                    set_(wp, EEP3_DRC_PORT_SPKR(bo,e,b), eep->drc_port_spkr);
                    set_(wp, EEP3_DRC_PORT_HPHN(bo,e,b), eep->drc_port_hphone);
                    set_(wp, EEP3_DRC_PORT_FLAT(bo,e,b), eep->drc_flat_panl);
                    set_(wp, EEP3_DRC_PORT_HTHR(bo,e,b), eep->drc_home_thtr);
                    set_(wp, EEP3_DRC_PORT_DDPL(bo,e,b), eep->drc_ddplus);
                }

                for (j = 0; j != n; ++j)
                {
                    unsigned int idx = p[j];
                    if (idx >= w->model->num_apd)
                    {
                        /* invalid presentation id */
                        return 1;
                    }
                    set_(wp, EEP4_PRESLIST(bo,e,b,d,j), apd_list[idx].id);
                }
                set_(wp, EEP4_PRESLIST(bo,e,b,d,n), 0);

                wp += (bo + payload_bits) / 8;
                bo = (bo + payload_bits) % 8;
            }
            ++eep;
            w->wp = wp;
        }
        if (bo)
        {
            w->wp += 1;
        }
        model->eep_written = i;
    }
    return 0;
}


/**
 * @brief extract eac3 encoding parameters blocks from serialized form
 */
static inline
int                                /** @return 0 on success, 1 on error */
klv_eep_read
    (klv_reader *r                 /**< [in] KLV buffer to read */
    ,int payload_length            /**< [in] bytes in presentation payload */
    )
{
    pmd_eep *eep;
    dlb_pmd_model *model = r->model;
    uint8_t *end = r->rp + payload_length;
    unsigned int presid;
    unsigned int bo = 0;
    unsigned int payload_bits;
    uint16_t id;
    uint8_t *rp = r->rp;
    uint16_t idx;

    while (r->rp < end-1)
    {
        unsigned int n = 0;
        unsigned int e = 0;
        unsigned int b = 0;
        unsigned int d = 0;

        id = (uint16_t)get_(rp, EEP1_ID(bo));
        if (!pmd_idmap_lookup(r->eep_ids, id, &idx))
        {
            if (model->num_eep >= MAX_EAC3_ENCODING_PARAMETERS)
            {
                klv_reader_error_at(r, "No space for EEP payload in model\n");
                return 1;
            }            

            idx = (uint16_t)model->num_eep;
            pmd_idmap_insert(r->eep_ids, id, idx);
            model->num_eep += 1;
        }
        TRACE(("        %u\n", id));
        eep = &model->eep_list[idx];
        eep->options  = 0;
        eep->id       = id;
        eep->num_presentations = 0;
    
        if (get_(rp, EEP1_B_ENCODER(bo)))
        {
            e = 1;
            eep->options |= PMD_EEP_ENCODER_PRESENT;
            eep->dynrng_prof = get_(rp, EEP1_DYNRNG_PROF(bo));
            eep->compr_prof  = get_(rp, EEP1_COMPR_PROF(bo));
            eep->surround90  = (pmd_bool)get_(rp, EEP1_SURROUND_90(bo));
            eep->hmixlev     = (unsigned char)get_(rp, EEP1_HMIXLEV(bo));
        }
        if (get_(rp, EEP2_B_BITSTREAM(bo,e)))
        {
            b = 1;
            eep->options |= PMD_EEP_BITSTREAM_PRESENT;
            eep->bsmod         = get_(rp, EEP2_BSMOD(bo,e));
            eep->dsurmod       = get_(rp, EEP2_SURMOD(bo,e));
            eep->dialnorm      = (pmd_dialogue_norm)get_(rp, EEP2_DIALNORM(bo,e));
            eep->dmixmod       = get_(rp, EEP2_PREF_DMIXMOD(bo,e));
            eep->ltrtcmixlev   = get_(rp, EEP2_LTRT_CMIXLEV(bo,e));
            eep->ltrtsurmixlev = get_(rp, EEP2_LTRT_SURMIXLEV(bo,e))-KLV_SURMIX_OFFSET;
            eep->lorocmixlev   = get_(rp, EEP2_LORO_CMIXLEV(bo,e));
            eep->lorosurmixlev = get_(rp, EEP2_LORO_SURMIXLEV(bo,e))-KLV_SURMIX_OFFSET;
        }
        if (get_(rp, EEP3_B_DRC(bo,e,b)))
        {
            d = 1;
            eep->options |= PMD_EEP_DRC_PRESENT;
            eep->drc_port_spkr   = get_(rp, EEP3_DRC_PORT_SPKR(bo,e,b));
            eep->drc_port_hphone = get_(rp, EEP3_DRC_PORT_HPHN(bo,e,b));
            eep->drc_flat_panl   = get_(rp, EEP3_DRC_PORT_FLAT(bo,e,b));
            eep->drc_home_thtr   = get_(rp, EEP3_DRC_PORT_HTHR(bo,e,b));
            eep->drc_ddplus      = get_(rp, EEP3_DRC_PORT_DDPL(bo,e,b));
        }

        presid = (unsigned int)get_(rp, EEP4_PRESLIST(bo,e,b,d,0));
        while (0 != presid)
        {
            if (!pmd_idmap_lookup(r->apd_ids, presid, &idx))
            {
                klv_reader_error_at(r, "EEP %d refers to uknown presentation id %d\n",
                                    id, presid);
                return 1;
            }

            eep->presentations[n] = idx;
            ++n;
            presid = get_(rp, EEP4_PRESLIST(bo,e,b,d,n));
        }

        eep->num_presentations = n;
        payload_bits = EEP_PAYLOAD_BITS(n,e,b,d);
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
#undef OPT2


#endif /* KLV_EAC3_ENCODING_PARAMETERS_INC_H */

