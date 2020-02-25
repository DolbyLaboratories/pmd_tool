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

#ifndef S337M_SADM_BITSTREAM_DECODER_H_
#define S337M_SADM_BITSTREAM_DECODER_H_


/**
 * @file sadm_bitstream_decoder.h
 * @brief definitions for extracting a PMD model from an sADM bitstream embedded within
 * within SMPTE-337m encoded PCM.
 */

#include "pmd_smpte_337m.h"
#include "pmd_error_helper.h"
#include "dlb_pmd_sadm.h"
#include "dlb_pmd_sadm_string.h"
#include "zlib.h"


/**
 * @brief abstract type of sADM bitstream decoder
 */
typedef struct
{
    unsigned int error_line;
    dlb_pmd_model *model;

    char xmlbuf[MAX_DATA_BYTES*4]; /**< sADM precompression buffer */
    size_t size;
    dlb_pmd_sadm_reader *r;
} sadm_bitstream_decoder;


static
void
sadm_bitstream_decoder_error_callback
    (const char *msg
    ,void *arg
    )
{
    sadm_bitstream_decoder *dec = (sadm_bitstream_decoder *)arg;

    error(dec->model, "Could not read SADM: %s at line %u", msg, dec->error_line);
}


/**
 * @brief determine memory requirements for the sADM bitstream decoder
 */
static inline
size_t                                    /** @return size of memory required */
sadm_bitstream_decoder_query_mem
    (dlb_pmd_model_constraints *limits    /**< [in] PMD model limits */
    )
{
    return sizeof(sadm_bitstream_decoder)
        + dlb_pmd_sadm_reader_query_mem(limits);
}


/**
 * @brief initialize the sADM bitstream decoder
 */
static inline
dlb_pmd_success
sadm_bitstream_decoder_init
    (dlb_pmd_model_constraints *limits
    ,void *mem
    ,sadm_bitstream_decoder **dec
    )
{
    sadm_bitstream_decoder *d;
    uintptr_t mc = (uintptr_t)mem;

    d = (sadm_bitstream_decoder*)mc;
    mc += sizeof(sadm_bitstream_decoder);
    
    if (dlb_pmd_sadm_reader_init(&d->r, limits, (void*)mc))
    {
        return PMD_FAIL;
    }
    *dec = d;
    return PMD_SUCCESS;
}


/**
 * @brief helper function to compress the decoder's XML buffer to the
 * given byte buffer
 */
static inline
int                                /** @return bytes used */
decompress_sadm_xml
   (sadm_bitstream_decoder *dec    /**< [in] bitstream decoder */
   ,uint8_t *buf                   /**< [in] input compressed data */
   ,size_t datasize                /**< [in] size of compressed input in bytes */
   )
{
    z_stream s;
    int res;

    s.zalloc = 0;
    s.zfree = 0;
    s.next_in = buf;
    s.avail_in = (uInt)datasize;
    s.next_out = (uint8_t*)dec->xmlbuf;
    s.avail_out = sizeof(dec->xmlbuf);

    if (inflateInit2(&s, 15+32))
    {
        return 0;
    }

    res = inflate(&s, Z_NO_FLUSH);
    if (Z_OK != res && Z_STREAM_END != res)
    {
#ifndef NEBUG
        printf("zlib error: %s\n", s.msg);
#endif
        s.total_out = 0;
    }

    inflateEnd(&s);
    return s.total_out;
}


/**
 * @brief inline helper function to encapsulate the process of generating an sADM
 * bitstream
 */
static inline
dlb_pmd_success                     /** @return 0 on success, 1 on failure */
sadm_bitstream_decoder_decode
    (sadm_bitstream_decoder *dec    /**< [in] bitstream decoder */
    ,uint8_t *bitstream             /**< [in] bits to decode */
    ,size_t datasize                /**< [in] number of bytes in bitstream */
    ,dlb_pmd_model *model           /**< [in] model to write */
    )
{
    int size;

    dec->model = model;
    size = decompress_sadm_xml(dec, bitstream, datasize);
    if (!size)
    {
        return PMD_FAIL;
    }
    return dlb_pmd_sadm_string_read(dec->r, "decompressed", dec->xmlbuf, size,
                                    model, sadm_bitstream_decoder_error_callback, dec,
                                    &dec->error_line);
}


#endif /* S337M_SADM_BITSTREAM_DECODER_H_ */
