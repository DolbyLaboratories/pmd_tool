/************************************************************************
 * dlb_pmd
 * Copyright (c) 2020 - 2022, Dolby Laboratories Inc.
 * Copyright (c) 2022, Dolby International AB.
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
 * @file klv_pld.h
 * @brief defines reading and writing the Presentation Loudness Description (PLD) payload
 */

#ifndef KLV_PLD_H_
#define KLV_PLD_H_

#include "pmd_idmap.h"
#include "klv_writer.h"
#include "klv_reader.h"

#include <ctype.h>


#define PLD_VERSION     (0)  /* hard code version to 0 */

#define PLD_PRESID      (9)
#define PLD_VERSION_LEN (2)
#define PLD_PRACTYPE    (4)
#define PLD_CORRTYPE    (1)
#define PLD_LOUDVAL    (11) /* loudness values are 11 bits long */
#define PLD_OFFSET     (11) /* program boundary offset is 11 bits long */
#define PLD_DIALGATE    (3) /* dialogue gating values are 3 bits long */
#define PLD_LRA        (10) /* loudness range value is 10 bits long */
#define PLD_LRA_PTYPE   (3) /* loudness range practice type is 3 bits long */

#define PLD_EXTENSION_BITS_SIZE (5)
#define PLD_EXTENSION_BITS_MAX ((1ul << PLD_EXTENSION_BITS_SIZE) - 1)
#define PLD_EXTENSION_VARICHUNK_SIZE (4)
#define PLD_EXTENSION_VARICHUNK_MASK ((1ul << PLD_EXTENSION_VARICHUNK_SIZE)-1)


/**
 * @def SET_2(wp,len,input)
 * @brief helper macro to abstract the process of writing a value
 * and increasing bit and byte offsets
 *
 * Note that this macro is only valid when bo (bit offset within byte)
 * is defined, and is intended only for use in
 * #klv_pld_write
 */
#define SET_2(wp,len,input) { set_(wp, bo, len, input); wp += (bo+len)/8; bo = (bo + len) % 8; }


/**
 * @def GET_2(rp,len)
 * @brief helper macro to abstract the process of reading a value
 * and increasing bit and byte offsets
 *
 * This requires local variables rp2, bo2, bo to be defined in the context, and
 * only intended to be used in #klv_pld_read
 */
#define GET_2(rp,len) (rp2 = rp, bo2 = bo, rp = rp + (bo+len)/8, bo = (bo+len)%8, get_(rp2, bo2, len))
    

/**
 * @def SET_IF(wp,option,len,input)
 * @brief helper macro to abstract the process of writing a conditional field
 *
 * Note that this macro is only valid when bo (bit offset within byte)
 * is defined, and is intended only for use in
 * #klv_pld_write
 */
#define SET_IF(wp,option,len,input)     \
    if (option)                         \
    {                                   \
         SET_2(wp,1,1);                 \
         SET_2(wp,len,input);           \
    }                                   \
    else                                \
    {                                   \
         SET_2(wp,1,0);                 \
    }                           


/**
 * @def GET_IF(rp,opt,len,var)
 * @brief helper macro to abstract the process of reading a conditional field
 *
 * This requires local variables rp2, bo2, bo to be defined in the context, and
 * only intended to be used in #klv_pld_read
 */
#define GET_IF(rp,opt,len,var)                 \
    if (GET_2(rp,1))                            \
    {                                           \
        pld->options |= opt;                    \
        var = GET_2(rp,len);                    \
    }
    

/**
 * @brief compute payload size in bits
 */
static inline
unsigned int
klv_pld_payload_bits
    (pmd_pld *pld
    )
{
    unsigned long o = pld->options;
    unsigned int bits = 0;

    bits += PLD_PRESID;       /* presentation id */
    bits += PLD_VERSION_LEN;  /* version */
    bits += PLD_PRACTYPE;     /* practice type */

    if (pld->lpt)
    {
        bits += 1; /* option bit */
        if (o & PMD_PLD_OPT_LOUDCORR_DIALGATE) bits += PLD_DIALGATE;
        bits += PLD_CORRTYPE;
    }
    
    /* 12 option bits */
    bits += 12;

    if (o & PMD_PLD_OPT_LOUDRELGAT)     bits += PLD_LOUDVAL;
    if (o & PMD_PLD_OPT_LOUDSPCHGAT)    bits += PLD_LOUDVAL + PLD_DIALGATE;
    if (o & PMD_PLD_OPT_LOUDSTRM3S)     bits += PLD_LOUDVAL;
    if (o & PMD_PLD_OPT_MAX_LOUDSTRM3S) bits += PLD_LOUDVAL;    
    if (o & PMD_PLD_OPT_TRUEPK)         bits += PLD_LOUDVAL;    
    if (o & PMD_PLD_OPT_MAX_TRUEPK)     bits += PLD_LOUDVAL;    
    if (o & PMD_PLD_OPT_LRA)            bits += PLD_LOUDVAL + PLD_LRA_PTYPE;    
    if (o & PMD_PLD_OPT_LOUDMNTRY)      bits += PLD_LOUDVAL;    
    if (o & PMD_PLD_OPT_MAX_LOUDMNTRY)  bits += PLD_LOUDVAL;    
    
    if (o & PMD_PLD_OPT_PRGMBNDY)
    {
        bits += labs(pld->prgmbndy);
        bits += 1;
        bits += 1; /* b_end_or_start */
        if (o & PMD_PLD_OPT_PRGMBNDY_OFFSET) bits += PLD_OFFSET;
    }
    
    if (o & PMD_PLD_OPT_EXTENSION)
    {
        bits += 5;
        if (pld->extension_bits >= 31)
        {
            unsigned int tmp = pld->extension_bits - 31;
            do
            {
                tmp = (tmp-1) >> PLD_EXTENSION_VARICHUNK_SIZE;
                bits += 1+PLD_EXTENSION_VARICHUNK_SIZE;
            }
            while (tmp);
        }
        bits += pld->extension_bits;
    }
    return bits;
}


/**
 * @brief write presentation loudness metadata to the KLV output stream
 */
static inline
int
klv_pld_write
    (klv_writer *w
    ,dlb_pmd_model *model
    )
{
    if (klv_write_local_key_opened(w))
    {
        pmd_pld *pld;
        uint8_t *wp = w->wp;
        unsigned int bo = 0;
        unsigned int i;
        unsigned int j;
        unsigned int tmp;
        pmd_apd *pres = w->model->apd_list;

        pld = &model->pld_list[model->write_state.pld_written];
        for (j = model->write_state.pld_written; j != model->num_pld; ++j, ++pld)
        {
            unsigned int payload_bits = klv_pld_payload_bits(pld);
            unsigned long o = pld->options;

            if (klv_writer_space(w) < (payload_bits + 7)/8)
            {
                break;
            }

            if (!bo)
            {
                *wp = 0;
            }
            memset(wp+1, '\0', (payload_bits + 7)/8);

            SET_2(wp, PLD_PRESID,      pres[pld->presid].id);
            SET_2(wp, PLD_VERSION_LEN, PLD_VERSION);
            SET_2(wp, PLD_PRACTYPE,    pld->lpt);
            if (pld->lpt != 0)
            {
                SET_IF(wp, (o & PMD_PLD_OPT_LOUDCORR_DIALGATE), PLD_DIALGATE, pld->dpt);
                SET_2(wp, PLD_CORRTYPE, pld->corrty);
            }
            SET_IF(wp, (o & PMD_PLD_OPT_LOUDRELGAT),    PLD_LOUDVAL,  pld->lrg);
            SET_IF(wp, (o & PMD_PLD_OPT_LOUDSPCHGAT),   PLD_LOUDVAL,  pld->lsg);
            if (o & PMD_PLD_OPT_LOUDSPCHGAT)
            {
                SET_2(wp,  PLD_DIALGATE, pld->sdpt);
            }
            SET_IF(wp, (o & PMD_PLD_OPT_LOUDSTRM3S),    PLD_LOUDVAL,  pld->l3g);
            SET_IF(wp, (o & PMD_PLD_OPT_MAX_LOUDSTRM3S),PLD_LOUDVAL,  pld->l3g_max);
            SET_IF(wp, (o & PMD_PLD_OPT_TRUEPK),        PLD_LOUDVAL,  pld->tpk);
            SET_IF(wp, (o & PMD_PLD_OPT_MAX_TRUEPK),    PLD_LOUDVAL,  pld->tpk_max);
            if (!(o & PMD_PLD_OPT_PRGMBNDY))
            {
                SET_2(wp, 1, 0);                  /* b_prgmbndy */
            }
            else
            {
                SET_2(wp, 1, 1);                  /* b_prgmbndy */
                tmp = labs(pld->prgmbndy);
                for (i = 1; i < tmp; ++i)
                {
                    SET_2(wp, 1, 0); 
                }
                SET_2(wp, 1, 1);
                SET_2(wp, 1, (pld->prgmbndy < 0) ? 0 : 1);  /* b_end_or_start */
                SET_IF(wp, (o & PMD_PLD_OPT_PRGMBNDY_OFFSET), PLD_OFFSET, pld->prgmbndy_offset);
            }
            
            SET_IF(wp, (o & PMD_PLD_OPT_LRA),           PLD_LRA,      pld->lra);
            if (o & PMD_PLD_OPT_LRA)
            {
                SET_2(wp, PLD_LRA_PTYPE, pld->lrap);
            }

            SET_IF(wp, (o & PMD_PLD_OPT_LOUDMNTRY),     PLD_LOUDVAL,  pld->ml);
            SET_IF(wp, (o & PMD_PLD_OPT_MAX_LOUDMNTRY), PLD_LOUDVAL,  pld->ml_max);
            if (!(o & PMD_PLD_OPT_EXTENSION))
            {
                SET_2(wp, 1, 0);
            }
            else
            {
                tmp = pld->extension_bits;
                SET_2(wp, 1, 1);
                if (tmp < PLD_EXTENSION_BITS_MAX)
                {
                    SET_2(wp, PLD_EXTENSION_BITS_SIZE, tmp);
                }
                else
                {
                    unsigned int varichunks = 0;
                    unsigned int tmp2 = 0;

                    SET_2(wp, PLD_EXTENSION_BITS_SIZE, PLD_EXTENSION_BITS_MAX);
                    tmp -= PLD_EXTENSION_BITS_MAX;

                    /* first, count number of chunks of variable bits needed */
                    tmp += 1;
                    do
                    {
                        tmp -= 1;
                        tmp2 <<= PLD_EXTENSION_VARICHUNK_SIZE;
                        tmp2 |= tmp & PLD_EXTENSION_VARICHUNK_MASK;
                        tmp = tmp >> PLD_EXTENSION_VARICHUNK_SIZE;
                        varichunks += 1;
                    } while (tmp);

                    while (varichunks)
                    {
                        SET_2(wp, PLD_EXTENSION_VARICHUNK_SIZE,
                              (tmp2 & PLD_EXTENSION_VARICHUNK_MASK));
                        tmp2 = tmp2 >> PLD_EXTENSION_VARICHUNK_SIZE;
                        --varichunks;
                        if (varichunks)
                        {
                            SET_2(wp, 1, 1);  /* b_read_more */
                        }
                    }
                    SET_2(wp, 1, 0);  /* b_read_more */
                }
                seta_(wp, bo, pld->extension_bits, pld->extension);
                wp += (bo + pld->extension_bits) / 8;
                bo = (bo + pld->extension_bits) % 8;
            }
            w->wp = wp;
            
            TRACE(("        PLD: %u\n", pld->presid));
        }
        model->write_state.pld_written = j;
        w->wp = wp;
        if (bo)
        {
            w->wp += 1;
        }
    }
    return 0;
}


/**
 * @brief extract presentation loudness from serialized form
 */
static inline
int                                             /** @return 0 on success, 1 on error */
klv_pld_read
    (klv_reader *r                              /**< [in] KLV buffer to read */
    ,int payload_length                         /**< [in] bytes in presentation payload */
    ,dlb_pmd_payload_status_record *read_status /**< [out] read status record, may be NULL */
    )
{
    dlb_pmd_model *model = r->model;
    pmd_presentation_id id;
    pmd_pld *pld;
    uint8_t *rp = r->rp;
    uint8_t *rp2 = r->rp;
    uint8_t *end = rp + payload_length;
    unsigned int bo = 0;
    unsigned int bo2 = 0;
    unsigned int tmp;
    uint16_t apd_idx;
    uint16_t pld_idx;
    
    while (rp < end-1)      /* TODO: is this correct for "bytes remain"? */
    {
        dlb_pmd_payload_status ps;

        /* Validate the ID, and check that it refers to a known presentation */
        id = (pmd_presentation_id)GET_2(rp, PLD_PRESID);
        ps = pmd_validate_presentation_id(id);
        if (ps != DLB_PMD_PAYLOAD_STATUS_OK)
        {
            klv_reader_error_at(r, ps, read_status, "PLD payload %u has invalid presentation id %u\n",
                                (unsigned int)(model->num_pld + 1), (unsigned int)id);
            return 1;
        }
        if (!pmd_idmap_lookup(r->apd_ids, id, &apd_idx))
        {
            /* presentation does not exist */
            klv_reader_error_at(r, DLB_PMD_PAYLOAD_STATUS_INCORRECT_STRUCTURE, read_status,
                                "PLD payload %u refers to unknown presentation %u\n",
                                (unsigned int)(model->num_pld + 1), (unsigned int)id);
            return 1;
        }

        /* Does the model have room? */
        if (model->num_pld >= model->profile.constraints.max.num_loudness)
        {
            klv_reader_error_at(r, DLB_PMD_PAYLOAD_STATUS_OUT_OF_MEMORY, read_status,
                                "No space for PLD payload %u in model\n",
                                (unsigned int)(model->num_pld + 1));
            return 1;
        }

        /* Clear the next PLD record in the model */
        pld_idx = model->num_pld;
        pld = &model->pld_list[model->num_pld];
        memset(pld, '\0', sizeof(*pld));
        model->num_pld += 1;
        pld->presid = apd_idx;      /* idx seems to be correct, although the name would imply id is correct... */
      
        /* Field-by-field validation */
        TRACE(("        Loudness (Presentation %u)\n", id));
        tmp = (unsigned int)GET_2(rp, PLD_VERSION_LEN);
        if (PLD_VERSION != tmp)
        {
            klv_reader_error_at(r, DLB_PMD_PAYLOAD_STATUS_VALUE_OUT_OF_RANGE, read_status,
                                "PLD payload %u has invalid bitstream version %u (%u expected)\n",
                                (unsigned int)model->num_pld, tmp, (unsigned int)PLD_VERSION);
            return 1;
        }

        pld->lpt = GET_2(rp, PLD_PRACTYPE);
        ps = validate_pmd_loudness_practice(pld->lpt);
        if (ps != DLB_PMD_PAYLOAD_STATUS_OK)
        {
            klv_reader_error_at(r, ps, read_status,
                                "PLD payload %u has invalid loudness practice type %u\n",
                                (unsigned int)model->num_pld, (unsigned int)pld->lpt);
            return 1;
        }
        if (PMD_LPT_NI != pld->lpt)
        {
            if (GET_2(rp, 1))
            {
                pld->options |= PMD_PLD_OPT_LOUDCORR_DIALGATE;
                pld->dpt = GET_2(rp, PLD_DIALGATE);                         /* No validation needed */
            }
            pld->options |= PMD_PLD_OPT_LOUDCORR_TYPE;
            pld->corrty = GET_2(rp, PLD_CORRTYPE);                          /* No validation needed */
        }

        GET_IF(rp, PMD_PLD_OPT_LOUDRELGAT,     PLD_LOUDVAL, pld->lrg);      /* No validation needed */
        GET_IF(rp, PMD_PLD_OPT_LOUDSPCHGAT,    PLD_LOUDVAL, pld->lsg);      /* No validation needed */
        if (pld->options & PMD_PLD_OPT_LOUDSPCHGAT)
        {
            pld->sdpt = GET_2(rp, PLD_DIALGATE);
        }
        GET_IF(rp, PMD_PLD_OPT_LOUDSTRM3S,     PLD_LOUDVAL, pld->l3g);      /* No validation needed */
        GET_IF(rp, PMD_PLD_OPT_MAX_LOUDSTRM3S, PLD_LOUDVAL, pld->l3g_max);  /* No validation needed */
        GET_IF(rp, PMD_PLD_OPT_TRUEPK,         PLD_LOUDVAL, pld->tpk);      /* No validation needed */
        GET_IF(rp, PMD_PLD_OPT_MAX_TRUEPK,     PLD_LOUDVAL, pld->tpk_max);  /* No validation needed */

        if (GET_2(rp, 1))
        {
            /* pgmbndy */
            pld->options |= PMD_PLD_OPT_PRGMBNDY;
            pld->prgmbndy = 1;          /* This must start at 1 to match spec */

            while (!GET_2(rp, 1))
            {
                pld->prgmbndy += 1;     /* NOTE: spec says pld->prgmbndy <<= 1, apparently we are saving the exponent, not the value */
            }
            if (!GET_2(rp, 1)) /* b_end_or_start */
            {
                pld->prgmbndy *= -1;
            }
            ps = validate_pmd_programme_boundary(pld->prgmbndy);
            if (ps != DLB_PMD_PAYLOAD_STATUS_OK)
            {
                klv_reader_error_at(r, ps, read_status,
                                    "PLD payload %u has invalid programme boundary %u\n",
                                    (unsigned int)model->num_pld, (unsigned int)pld->prgmbndy);
                return 1;
            }
            GET_IF(rp, PMD_PLD_OPT_PRGMBNDY_OFFSET, PLD_OFFSET, pld->prgmbndy_offset);  /* No validation needed */
        }
            
        GET_IF(rp, PMD_PLD_OPT_LRA,            PLD_LRA,      pld->lra);     /* No validation needed */
        if (pld->options & PMD_PLD_OPT_LRA)
        {
            pld->lrap = GET_2(rp, PLD_LRA_PTYPE);
            ps = validate_pmd_loudness_range_practice(pld->lrap);
            if (ps != DLB_PMD_PAYLOAD_STATUS_OK)
            {
                klv_reader_error_at(r, ps, read_status,
                                    "PLD payload %u has invalid loudness range practice %u\n",
                                    (unsigned int)model->num_pld, (unsigned int)pld->lrap);
                return 1;
            }
        }
        GET_IF(rp, PMD_PLD_OPT_LOUDMNTRY,      PLD_LOUDVAL,  pld->ml);      /* No validation needed */
        GET_IF(rp, PMD_PLD_OPT_MAX_LOUDMNTRY,  PLD_LOUDVAL,  pld->ml_max);  /* No validation needed */

        /* extension (no validation) */
        if (GET_2(rp, 1))
        {
            pld->options |= PMD_PLD_OPT_EXTENSION;
            tmp = (unsigned int)GET_2(rp, PLD_EXTENSION_BITS_SIZE);
            if (PLD_EXTENSION_BITS_MAX == tmp)
            {
                /* read additional variable bits */
                unsigned int tmp2 = ~0u;
                do
                {
                    tmp2 += 1;
                    tmp2 = tmp2 << PLD_EXTENSION_VARICHUNK_SIZE;
                    tmp2 |= GET_2(rp, PLD_EXTENSION_VARICHUNK_SIZE);
                } while (GET_2(rp, 1));
                tmp = tmp2 + PLD_EXTENSION_BITS_MAX;
            }
            pld->extension_bits = tmp;
            geta_(rp, bo, pld->extension_bits, pld->extension);
            rp += (bo + pld->extension_bits) / 8;
            bo = (bo + pld->extension_bits) % 8;
        }
        r->rp = rp;
    }
    if (bo)
    {
        r->rp += 1;
    }
    return 0;
}


#endif /* KLV_PLD_H_ */
