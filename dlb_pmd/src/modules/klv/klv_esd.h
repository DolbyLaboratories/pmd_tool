/************************************************************************
 * dlb_pmd
 * Copyright (c) 2021, Dolby Laboratories Inc.
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
 * @file klv_esd.h
 * @brief KLV payload structure for the ED2 stream description payload
 * 
 * ED2 is built on top of Dolby E which can carry a maximum of 8 PCM
 * channels.  Modern object-audio streams are larger, often 16
 * channels, but potentially unlimited.  This means we must deliver
 * audio in multiple parallel streams which have to be synchronized
 * together.
 *
 * The ED2 stream description information outlines the topology of the ED2
 * streams used to deliver one content experience.
 */

#ifndef KLV_ESD_H_
#define KLV_ESD_H_

#include "pmd_esd.h"
  

/* define offsets within the structure for each field in the ESD payload
 */
/*      field name        bitpos   bitlength */
#define ESD_STREAM_COUNT       0,  4
#define ESD_STREAM_INDEX       4,  4
#define ESD_STREAM_RATE        8,  4
#define ESD_STREAM_CONFIG     12,  5
#define ESD_STREAM_COMP       17,  3

#define ESD_PAYLOAD_BITS      (20)
#define ESD_PAYLOAD_BYTES     ((ESD_PAYLOAD_BITS + 7)/8)


/**
 * @brief write an audio presentation to the KLV output stream
 */
static inline
int                                /** @return 0 on success, 1 on error */
klv_esd_write
    (klv_writer *w                 /**< [in] writer state */
    ,pmd_esd *esys          /**< [in] ED2 system stream to write */
    ,pmd_bool *written             /**< [out] 1 if space to write */
    )
{
    if (klv_write_local_key_opened(w))
    {
        pmd_ed2_stream_description *pesd = &esys->streams[w->sindex];
        uint8_t *wp = w->wp;
        
        *written = PMD_FALSE;
        if (klv_writer_space(w) >= ESD_PAYLOAD_BYTES)
        {
            /* convert dlb_pmd_frame_rate enum to the KLV enum:
             * 1:23.98, 2:24, 3:25, 4:29.97, 5:30 */
            uint8_t rate = esys->rate+1;
            
            TRACE(("        ESD %u/%u\n", w->sindex, esys->count));

            memset(wp, '\0', ESD_PAYLOAD_BYTES);
            set_(wp, ESD_STREAM_COUNT,  esys->count-1);
            set_(wp, ESD_STREAM_INDEX,  w->sindex);
            set_(wp, ESD_STREAM_RATE,   rate);
            set_(wp, ESD_STREAM_CONFIG, pesd->config);
            set_(wp, ESD_STREAM_COMP,   pesd->compression);
            w->wp += ESD_PAYLOAD_BYTES;
            *written = PMD_TRUE;
            return 0;
        }
        /* it isn't an error to have no room for the payload, unless there
         * was nothing in the buffer already, in which case the buffer is
         * too small! */
        return w->wp == w->buffer;
    }
    return 0;
}


/**
 * @brief extract an ED2 stream description from serialized form
 */
static inline
int                                             /** @return 0 on success, 1 on error */
klv_esd_read
    (klv_reader *r                              /**< [in] KLV buffer to read */
    ,int payload_length                         /**< [in] bytes in presentation payload */
    ,pmd_esd *esys                              /**< [out] ESD record to populate */
    ,dlb_pmd_payload_status_record *read_status /**< [out] read status record, may be NULL */
    )
{
    dlb_pmd_frame_rate frame_rate;
    pmd_ed2_stream_description *pesd;
    uint8_t *rp = r->rp;
    uint8_t index;

    if (esys == NULL)
    {
        klv_reader_error_at(r, DLB_PMD_PAYLOAD_STATUS_OUT_OF_MEMORY, read_status,
                            "Processing ESD payload, but no ESD record in model\n");
        return 1;
    }

    if (payload_length != ESD_PAYLOAD_BYTES)
    {
        klv_reader_error_at(r, DLB_PMD_PAYLOAD_STATUS_VALUE_OUT_OF_RANGE, read_status,
                            "ESD payload length %d is incorrect, correct value is %d\n",
                            payload_length, (int)ESD_PAYLOAD_BYTES);
        return 1;
    }

    esys->count = get_(rp, ESD_STREAM_COUNT) + 1;
    /* No need to validate count, all bitstream values are valid */

    frame_rate = get_(rp, ESD_STREAM_RATE);
    if (frame_rate == 0)
    {
        klv_reader_error_at(r, DLB_PMD_PAYLOAD_STATUS_VALUE_RESERVED, read_status,
                            "ESD payload frame rate 0 is reserved\n");
        return 1;
    }
    esys->rate = frame_rate - 1;
    if (esys->rate > DLB_PMD_FRAMERATE_LAST_ED2)
    {
        klv_reader_error_at(r, DLB_PMD_PAYLOAD_STATUS_VALUE_OUT_OF_RANGE, read_status,
                            "ESD payload frame rate %d is out of range\n",
                            (int)esys->rate);
        return 1;
    }

    index = get_(rp, ESD_STREAM_INDEX);
    /* No need to validate count, all bitstream values are valid */
    pesd = &esys->streams[index];

    pesd->config = get_(rp, ESD_STREAM_CONFIG);
    if (pesd->config > PMD_DE_PGMCFG_LAST)
    {
        klv_reader_error_at(r, DLB_PMD_PAYLOAD_STATUS_VALUE_OUT_OF_RANGE, read_status,
                            "ESD payload Dolby E stream config %d is out of range\n",
                            (int)pesd->config);
        return 1;
    }

    pesd->compression = get_(rp, ESD_STREAM_COMP);
    /* Don't validate, value is mysterious... */

    esys->streams_read |= (1u << index);

    r->stream_index = index;
    r->rp += ESD_PAYLOAD_BYTES;
    return 0;
}


#endif /* KLV_ESD_H_ */
