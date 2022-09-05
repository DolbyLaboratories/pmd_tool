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
 * @file sadm_bitstream_encoder.c
 * @brief definitions for generating a S-ADM bitstream suitable for embedding
 * within SMPTE-337m encoded PCM.
 */

#include "sadm_bitstream_encoder.h"
#include "dlb_adm/include/dlb_adm_api.h"
#include "zlib.h"

#include <string.h>
#include <assert.h>
#include <string.h>

/**
 * @brief sadm write callback
 */
static
int
pcm_sadm_get_buffer
    (void       *arg
    ,char       *pos
    ,char      **buf
    ,size_t     *capacity
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
 * @brief determine memory requirements for the S-ADM bitstream encoder
 */
size_t                                    /** @return size of memory required */
sadm_bitstream_encoder_query_mem
    (void
    )
{
    return sizeof(sadm_bitstream_encoder);
}


dlb_pmd_success
sadm_bitstream_encoder_init
    (void                       *mem
    ,sadm_bitstream_encoder    **enc
    )
{
    sadm_bitstream_encoder *e = (sadm_bitstream_encoder*)mem;

    if ((mem == NULL) || (enc == NULL))
    {
        return PMD_FAIL;
    }

    memset(e, 0, sizeof(*e));
    *enc = e;

    return PMD_SUCCESS;
}


int 
compress_sadm_xml
   (sadm_bitstream_encoder  *enc
   ,uint8_t                 *buf
   )
{
    z_stream s;
    int bytecount;
    int res;

    memset(&s, 0, sizeof(s));
    s.zalloc = 0;
    s.zfree = 0;
    s.next_in = (uint8_t*)enc->xmlbuf;
    s.avail_in = (uInt)enc->size;
    s.next_out = buf;
    s.avail_out = MAX_DATA_BYTES;

    res = deflateInit2
    (
        &s,
        Z_BEST_COMPRESSION,
        Z_DEFLATED,
        MAX_WBITS + 16,     /* + 16 is "write a simple gzip header" */
        8,                  /* memory level (default) */
        Z_DEFAULT_STRATEGY
    );
    if (res != Z_OK)
    {
        return MAX_DATA_BYTES + 1;
    }

    res = deflate(&s, Z_NO_FLUSH);
    if (res != Z_OK)
    {
        return MAX_DATA_BYTES + 1;
    }

    if (s.avail_in || !s.avail_out)
    {
        /* not enough space in output buffer */
        return 0;
    }
    
    res = deflate(&s, Z_FINISH);
    if (res != Z_STREAM_END)
    {
        return MAX_DATA_BYTES + 1;
    }

    bytecount = MAX_DATA_BYTES - s.avail_out;
    deflateEnd(&s);
    return bytecount;
}


int
sadm_bitstream_encoder_payload
    (sadm_bitstream_encoder     *enc
    ,const dlb_adm_core_model   *model
    ,uint8_t                    *outbuf
    )
{
    dlb_adm_xml_container   *container = NULL;
    int                      byte_size = 0;
    int                      status;

    enc->size = sizeof(enc->xmlbuf);
    status = dlb_adm_container_open_from_core_model(&container, model);
    if (status != DLB_ADM_STATUS_OK) goto finish;
    status = dlb_adm_container_write_xml_buffer(container, pcm_sadm_get_buffer, enc);
    if (status != DLB_ADM_STATUS_OK) goto finish;
    byte_size = compress_sadm_xml(enc, outbuf);

finish:
    if (container != NULL)
    {
        (void)dlb_adm_container_close(&container);
    }

    return byte_size;
}


int
sadm_bitstream_encoder_encode
    (pmd_s337m                  *s337m
    ,sadm_bitstream_encoder     *enc
    ,const dlb_adm_core_model   *model
    ,dlb_pmd_frame_rate          rate
    ,uint8_t                    *outbuf
    )
{
    dlb_adm_xml_container   *container = NULL;
    size_t                   min_frame_size = pmd_s337m_min_frame_size(rate);
    int                      frame_byte_count = (int)pmd_s337m_sadm_data_bytes(s337m, rate);
    int                      byte_size = 0;
    int                      status;

    enc->size = sizeof(enc->xmlbuf);
    s337m->framelen = min_frame_size - 2 * GUARDBAND;   /* Note: this could be short by a sample - TODO: can that be a problem? */

    status = dlb_adm_container_open_from_core_model(&container, model);
    if (status != DLB_ADM_STATUS_OK) goto finish;
    status = dlb_adm_container_write_xml_buffer(container, pcm_sadm_get_buffer, enc);
    if (status != DLB_ADM_STATUS_OK) goto finish;

    byte_size = compress_sadm_xml(enc, outbuf);
    if (byte_size > frame_byte_count)
    {
        byte_size = 0;
    }

finish:
    if (container != NULL)
    {
        (void)dlb_adm_container_close(&container);
    }

    return byte_size;
}
