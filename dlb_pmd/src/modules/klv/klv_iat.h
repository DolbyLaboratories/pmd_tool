/************************************************************************
 * dlb_pmd
 * Copyright (c) 2023, Dolby Laboratories Inc.
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
 * @file klv_iat.h
 * @brief defines reading and writing the Identity and Timing (IAT) payload
 */

#ifndef KLV_IAT_H_
#define KLV_IAT_H_

#include "pmd_iat.h"
#include "pmd_idmap.h"
#include "klv_writer.h"
#include "klv_reader.h"

/* Define offsets within the structure for each field.  Note that
 * because this structure has 3 optional chunks (BSI, ExtBSI and AC4
 * metadata, we have to write in stages, where the beginning of each
 * chunk is written as if *it* were the start of a nominal 64-bit
 * int.
 */

/* 1st chunk, the header, and the 1st optional payload (Content ID) */
/* entries into bitarray :            bitpos    len */
#define IAT1_VERSION                      0,      2
#define IAT1_EXTENDED_VERSION             2,      4

/**
 * @def OPT1(v,off)
 * @brief bit-offset of 2nd chunk given version
 */
#define OPT1(v,off)  ((v==3)?4+(off):(off))

#define IAT1_B_CONTENT_ID(v)       OPT1(v, 2),      1
#define IAT1_CONTENT_ID_TYPE(v)    OPT1(v, 3),      5
#define IAT1_CONTENT_ID_SIZE_M1(v) OPT1(v, 8),      5
#define IAT1_CONTENT_ID(v)         OPT1(v,13)


/**
 * @def OPT2(v,cc,off)
 * @brief calculate bit-offset of 2nd chunk, when given size of
 * content id (c)
 */
#define OPT2(v,c,x) (OPT1(v,x) + ((c)?(c*8)+10:0))
#define IAT2_B_DISTRIBUTION_ID(v,c)         OPT2(v,c, 3),   1
#define IAT2_DISTRIBUTION_ID_TYPE(v,c)      OPT2(v,c, 4),   3
#define IAT2_DISTRIBUTION_ID_SIZE_M1(v,c)   OPT2(v,c, 7),   4
#define IAT2_DISTRIBUTION_ID(v,c)           OPT2(v,c,11)


#define OPT3(v,c,d,x) (OPT2(v,c,x) + ((d)?(d*8)+7:0))
#define IAT3_TIMESTAMP(v,c,d)               OPT3(v,c,d, 4),  35
#define IAT3_B_OFFSET(v,c,d)                OPT3(v,c,d,39),   1
#define IAT3_OFFSET(v,c,d)                  OPT3(v,c,d,40),  11


/**
 * @def OPT4(v,c,d,o,x)
 * @brief compute offset x given version #v, Content ID size #c, distribution id
 * size #d and B_OFFSET flag
 */
#define OPT4(v,c,d,o,x) (OPT3(v,c,d,x) + ((o)?11:0))
#define IAT4_B_VALIDITY_DURATION(v,c,d,o)  OPT4(v,c,d,o,40),  1
#define IAT4_VALIDITY_DURATION(v,c,d,o)    OPT4(v,c,d,o,41), 11


/**
 * @def OPT5(v,c,d,o,w,x)
 * @brief compute offset x given version #v, Content ID size #c B_OFFSET flag and
 * B_VALIDITY flag and destination id size #d and validity duration flag #w
 */
#define OPT5(v,c,d,o,w,x) (OPT4(v,c,d,o,x) + ((w)?11:0))
#define IAT5_B_USER_DATA(v,c,d,o,w)           OPT5(v,c,d,o,w,41),    1
#define IAT5_USER_DATA_SIZE_M1(v,c,d,o,w)     OPT5(v,c,d,o,w,42),    8
#define IAT5_USER_DATA(v,c,d,o,w)             OPT5(v,c,d,o,w,50)

/**
 * @def OPT6(v,c,d,o,w,u,x)
 * @brief compute offset x given version #v, Content ID size #c, distribution id size #d,
 * B_OFFSET, B_VALIDITY_DURATION flags and user data size #u
 */
#define OPT6(v,c,d,o,w,u,x) (OPT5(v,c,d,o,w,x)+((u)?(u*8+8):0))

#define IAT6_B_EXTENSION(v,c,d,o,w,u)         OPT6(v,c,d,o,w,u,42),  1
#define IAT6_EXTENSION_SIZE_M1(v,c,d,o,w,u)   OPT6(v,c,d,o,w,u,43),  8
#define IAT6_EXTENSION_DATA(v,c,d,o,w,u)      OPT6(v,c,d,o,w,u,51)


/**
 * @def IAT_PAYLOAD_BITS(v,c,d,o,w,u,e)
 * @brief compute size of encoder config payload (in bytes) according
 * to the version, Content ID size, the B_OFFSET and B_VALIDITY flags, the user data size
 * and the extension size
 */
#define IAT_PAYLOAD_BITS(v,c,d,o,w,u,e)               \
    ((46 + ((v==3)?4:0) + ((c)?(c*8)+10:0) + ((d)?(d*8)+7:0) + ((o)?11:0) + ((w)?11:0) + ((u)?(u*8)+8:0) + ((e)?(e*8)+8:0)))


/**
 * @brief write an IAT payload to KLV output stream
 */
static inline
int
klv_iat_write
    (klv_writer *w            /**< [in] writer state */
    ,dlb_pmd_model *model     /**< [in] model to write */
    ,pmd_bool *written        /**< [out] was IAT written (enough space)? */
    )
{
    unsigned int v = 0;  /* version */
    unsigned int c = 0;  /* content id size */
    unsigned int d = 0;  /* distribution id size */
    unsigned int o = 0;  /* offset presence */
    unsigned int p = 0;  /* validity duration presence */
    unsigned int u = 0;  /* user data size */
    unsigned int e = 0;  /* extension data size */
    size_t payload_size;
    uint8_t *wp = w->wp;
    pmd_iat *iat = model->iat;

    *written = PMD_FALSE;
    if (iat && (iat->options & PMD_IAT_PRESENT))
    {
        c = iat->content_id_size;
        d = iat->distribution_id_size;
        o = (iat->options & PMD_IAT_OFFSET_PRESENT) != 0;
	p = (iat->options & PMD_IAT_VALIDITY_DUR_PRESENT) != 0;
        u = iat->user_data_size;
        e = iat->extension_size;
        
        payload_size = (IAT_PAYLOAD_BITS(v,c,d,o,p,u,e) + 7)/8;
        
        if (klv_writer_space(w) >= payload_size)
        {
            TRACE(("       IAT\n"));
            *written = PMD_TRUE;
            memset(wp, '\0', payload_size);
	    set_(wp, IAT1_VERSION, v);
	    if (v == 3)
	    {
	        /* placeholder in case code is ever changed to allow version to be 3.
                 * coverity will complain because this is dead code, but it is here
                 * for future-proofing
                 */
                set_(wp, IAT1_EXTENDED_VERSION, 0);
	    }
            set_(wp, IAT1_B_CONTENT_ID(v),   c != 0);
            if (c)
            {
                set_ (wp, IAT1_CONTENT_ID_TYPE(v),    iat->content_id_type);
                set_ (wp, IAT1_CONTENT_ID_SIZE_M1(v), c-1);
                seta_(wp, IAT1_CONTENT_ID(v), c*8, iat->content_id);
            }
            set_(wp, IAT2_B_DISTRIBUTION_ID(v,c), d != 0);
            if (d)
            {
                set_ (wp, IAT2_DISTRIBUTION_ID_TYPE(v,c), iat->distribution_id_type);
                set_ (wp, IAT2_DISTRIBUTION_ID_SIZE_M1(v,c), iat->distribution_id_size-1);
                seta_(wp, IAT2_DISTRIBUTION_ID(v,c), d*8, iat->distribution_id);
            }
            set_(wp, IAT3_TIMESTAMP(v,c,d), iat->timestamp);
            set_(wp, IAT3_B_OFFSET(v,c,d),  o);
            if (o)
            {
                set_(wp, IAT3_OFFSET(v,c,d),  iat->offset);
            }
            set_(wp, IAT4_B_VALIDITY_DURATION(v,c,d,o),  p);
            if (p)
            {
                set_(wp, IAT4_VALIDITY_DURATION(v,c,d,o), iat->validity_duration);
            }
            set_(wp, IAT5_B_USER_DATA(v,c,d,o,p), (u != 0));
            if (u)
            {
                set_ (wp, IAT5_USER_DATA_SIZE_M1(v,c,d,o,p), u-1);
                seta_(wp, IAT5_USER_DATA(v,c,d,o,p), u*8, iat->user_data);
            }
            set_(wp, IAT6_B_EXTENSION(v,c,d,o,p,u), (e != 0));
            if (e)
            {
                set_ (wp, IAT6_EXTENSION_SIZE_M1(v,c,d,o,p,u), e-1);
                seta_(wp, IAT6_EXTENSION_DATA(v,c,d,o,p,u), e*8, iat->extension_data);
            }
            w->wp += payload_size;
            return 0;
        }
        /* It is not an error to have no space left to write, because this
         * payload does not have to be in the 1st payload of a video frame,
         * and so there might be too much stuff already.  However, if there is
         * *nothing* in the buffer, it must be an error, because the buffer is
         * too small to fit one IAT payload. */
        return w->wp == w->buffer;
    }
    return 0;
}


/**
 * @brief extract an IAT from serialized form
 */
static inline
int                                             /** @return 0 on success, 1 on error */
klv_iat_read
    (klv_reader *r                              /**< [in] KLV buffer to read */
    ,int payload_length                         /**< [in] bytes in IAT payload */
    ,pmd_iat *iat                               /**< [in] IAT to populate */
    ,dlb_pmd_payload_status_record *read_status /**< [out] read status record, may be NULL */
    )
{
    dlb_pmd_payload_status ps;
    unsigned int v = 0;  /* version */
    unsigned int c = 0;  /* content id size */
    unsigned int d = 0;  /* distribution id size */
    unsigned int o = 0;  /* offset presence */
    unsigned int p = 0;  /* validity duration presence */
    unsigned int u = 0;  /* user data size */
    unsigned int e = 0;  /* extension data size */
    int payload_size;
    uint8_t *rp = r->rp;

    iat->options = 0;
    iat->content_id_size = 0;
    iat->distribution_id_size = 0;
    iat->user_data_size = 0;
    iat->extension_size = 0;

    v = get_(rp, IAT1_VERSION);
    if (v != 0)
    {
        klv_reader_error_at(r, DLB_PMD_PAYLOAD_STATUS_VALUE_OUT_OF_RANGE, read_status, "Incorrect IAT version %u\n", v);
        return 1;
    }

    if (get_(rp, IAT1_B_CONTENT_ID(v)))
    {
        iat->content_id_type = get_(rp, IAT1_CONTENT_ID_TYPE(v));
        ps = pmd_validate_encoded_iat_content_id_type(iat->content_id_type);
        if ((ps != DLB_PMD_PAYLOAD_STATUS_OK) && (ps != DLB_PMD_PAYLOAD_STATUS_VALUE_RESERVED)) /* Allow reserved */
        {
            klv_reader_error_at(r, ps, read_status, "Invalid IAT content id type %u\n", (unsigned int)iat->content_id_type);
            return 1;
        }

        iat->content_id_size = c = get_(rp, IAT1_CONTENT_ID_SIZE_M1(v))+1;      /* No validation necessary */
        geta_(rp, IAT1_CONTENT_ID(v), c*8, iat->content_id);                    /* No validation necessary */
    }

    if (get_(rp, IAT2_B_DISTRIBUTION_ID(v,c)))
    {
        iat->distribution_id_type = get_(rp, IAT2_DISTRIBUTION_ID_TYPE(v, c));
        ps = pmd_validate_encoded_iat_distribution_id_type(iat->distribution_id_type);
        if ((ps != DLB_PMD_PAYLOAD_STATUS_OK) && (ps != DLB_PMD_PAYLOAD_STATUS_VALUE_RESERVED)) /* Allow reserved */
        {
            klv_reader_error_at(r, ps, read_status, "Invalid IAT distribution id type %u\n", (unsigned int)iat->distribution_id_type);
            return 1;
        }

        iat->distribution_id_size = d = get_(rp, IAT2_DISTRIBUTION_ID_SIZE_M1(v,c))+1;  /* No validation necessary */
        geta_(rp, IAT2_DISTRIBUTION_ID(v,c), d*8, iat->distribution_id);                /* No validation necessary */
    }

    iat->timestamp = get_(rp, IAT3_TIMESTAMP(v,c,d));                           /* No validation necessary */

    if (get_(rp, IAT3_B_OFFSET(v,c,d)))
    {
        o = 1;
        iat->options |= PMD_IAT_OFFSET_PRESENT;
        iat->offset = get_(rp, IAT3_OFFSET(v,c,d));                             /* No validation necessary */
    }

    if (get_(rp, IAT4_B_VALIDITY_DURATION(v,c,d,o)))
    {
        p = 1;
        iat->options |= PMD_IAT_VALIDITY_DUR_PRESENT;
        iat->validity_duration = get_(rp, IAT4_VALIDITY_DURATION(v,c,d,o));     /* No validation necessary */
    }

    if (get_(rp, IAT5_B_USER_DATA(v,c,d,o,p)))
    {
        iat->user_data_size = u = get_(rp, IAT5_USER_DATA_SIZE_M1(v,c,d,o,p))+1;    /* No validation necessary */
        geta_(rp, IAT5_USER_DATA(v,c,d,o,p), u*8, iat->user_data);                  /* No validation necessary */
    }

    if (get_(rp, IAT6_B_EXTENSION(v,c,d,o,p,u)))
    {
        iat->extension_size = e = get_(rp, IAT6_EXTENSION_SIZE_M1(v,c,d,o,p,u))+1;  /* No validation necessary */
        geta_(rp, IAT6_EXTENSION_DATA(v,c,d,o,p,u), e*8, iat->extension_data);      /* No validation necessary */
    }

    payload_size = (IAT_PAYLOAD_BITS(v,c,d,o,p,u,e)+7)/8;
    if (payload_size != payload_length)
    {
        klv_reader_error_at(r, DLB_PMD_PAYLOAD_STATUS_VALUE_OUT_OF_RANGE, read_status,
                            "IAT payload size (%d) != payload length (%d)\n",
                            payload_size, payload_length);
        return 1;
    }
    r->rp += payload_length;
    return 0;
}


#undef OPT1
#undef OPT2
#undef OPT3
#undef OPT4
#undef OPT5

#endif /* KLV_IAT_H_ */
