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

#ifndef S337M_SADM_BITSTREAM_ENCODER_H_
#define S337M_SADM_BITSTREAM_ENCODER_H_


/**
 * @file sadm_bitstream_encoder.h
 * @brief definitions for generating an sADM bitstream suitable for embedding
 * within SMPTE-337m encoded PCM.
 */


#include "pmd_smpte_337m.h"
#include "dlb_pmd_sadm.h"
#include "zlib.h"


/**
 * @brief abstract type of sADM bitstream encoder
 */
typedef struct
{
    char xmlbuf[MAX_DATA_BYTES*4]; /**< sADM precompression buffer */
    size_t size;
    dlb_pmd_sadm_writer *w;
} sadm_bitstream_encoder;


/**
 * @brief sadm write callback
 */
static
int
pcm_sadm_get_buffer
    (void *arg
    ,char *pos
    ,char **buf
    ,size_t *capacity
    )
{
    sadm_bitstream_encoder *sadm = (sadm_bitstream_encoder *)arg;

    if (NULL == buf)
    {
        /* end */
        sadm->size = pos - sadm->xmlbuf;
    }
    else if (pos)
    {
        /* pos will only be non-zero if we have already written something into
         * the buffer.  Since we only have one buffer, this means we've already
         * exhausted it, so we must return 0;
         */
        return 0;
    }
    else
    {
        *buf = sadm->xmlbuf;
        *capacity = sadm->size;
    }
    return 1;
}


/**
 * @brief determine memory requirements for the sADM bitstream encoder
 */
static inline
size_t                                    /** @return size of memory required */
sadm_bitstream_encoder_query_mem
    (dlb_pmd_model_constraints *limits    /**< [in] PMD model limits */
    )
{
    return sizeof(sadm_bitstream_encoder)
        + dlb_pmd_sadm_writer_query_mem(limits);
}


/**
 * @brief initialize the sADM bitstream encoder
 */
static inline
dlb_pmd_success
sadm_bitstream_encoder_init
    (dlb_pmd_model_constraints *limits
    ,void *mem
    ,sadm_bitstream_encoder **gen
    )
{
    sadm_bitstream_encoder *g;
    uintptr_t mc = (uintptr_t)mem;

    g = (sadm_bitstream_encoder*)mc;
    mc += sizeof(sadm_bitstream_encoder);
    
    if (dlb_pmd_sadm_writer_init(&g->w, limits, (void*)mc))
    {
        return PMD_FAIL;
    }
    *gen = g;
    return PMD_SUCCESS;
}


/**
 * @brief helper function to compress the encoder's XML buffer to the
 * given byte buffer
 */
static inline
int                                /** @return bytes used */
compress_sadm_xml
   (sadm_bitstream_encoder *enc    /**< [in] bitstream encoder */
   ,uint8_t *buf                   /**< [in] output compression buffer */
   )
{
    z_stream s;
    int bytecount;
    int res;

    s.zalloc = 0;
    s.zfree = 0;
    s.next_in = (uint8_t*)enc->xmlbuf;
    s.avail_in = (uInt)enc->size;
    s.next_out = buf;
    s.avail_out = MAX_DATA_BYTES;

    deflateInit(&s, Z_BEST_COMPRESSION);

    res = deflate(&s, Z_NO_FLUSH);
    assert(res == Z_OK);
    (void)res;

    if (s.avail_in || !s.avail_out)
    {
        /* not enough space in output buffer */
        return 0;
    }
    
    res = deflate(&s, Z_FINISH);
    assert(res == Z_STREAM_END);

    bytecount = MAX_DATA_BYTES - s.avail_out;
    deflateEnd(&s);
    return bytecount;
}


/**
 * @brief inline helper function to encapsulate the process of generating an sADM
 * bitstream
 */
static inline
int                                 /** @return bytes used, or 0 if none */
sadm_bitstream_encoder_encode
    (pmd_s337m *s337m               /**< [in] S337m abstraction */
    ,sadm_bitstream_encoder *enc    /**< [in] bitstream encoder */
    ,dlb_pmd_model *model           /**< [in] model to write */
    ,dlb_pmd_frame_rate rate        /**< [in] frame rate */
    ,uint8_t *outbuf                /**< [in] compression buffer */
    )
{
    unsigned int vfsize = pmd_s337m_min_frame_size(rate);
    unsigned int bitcount;
    int max_bytes = (vfsize - 2 * GUARDBAND - SADM_PREAMBLE) * (1 + s337m->pair);
    int byte_size;

    enc->size = sizeof(enc->xmlbuf);
    s337m->framelen = vfsize - 2 * GUARDBAND;
    s337m->sadm_ai = 1;
    s337m->sadm_ff = 1;    
    if (dlb_pmd_sadm_writer_write(enc->w, model, pcm_sadm_get_buffer, 0, enc))
    {
        return 0;
    }
    
    (void)max_bytes;
    byte_size = compress_sadm_xml(enc, outbuf);
    assert(byte_size <= max_bytes);

    bitcount = byte_size * 8;
    if (s337m->sadm_ai)
    {
        bitcount += 20;
    }
    if (s337m->sadm_ff)
    {
        bitcount += 20;
    }
    return (bitcount + 7)/8;
}


#endif /* S337M_SADM_BITSTREAM_ENCODER_H_ */
