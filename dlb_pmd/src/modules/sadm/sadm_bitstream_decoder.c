/************************************************************************
 * dlb_pmd
 * Copyright (c) 2020-2021, Dolby Laboratories Inc.
 * Copyright (c) 2020-2021, Dolby International AB.
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

#include "sadm_bitstream_decoder.h"
#include "dlb_adm/include/dlb_adm_api.h"
#include "zlib.h"

/**
 * @brief 
 * 

 * @param datasize 
 * 
 */
static
dlb_pmd_bool                            /** @return 1 if buffer is compressed, 0 otherwise */
is_buffer_compressed
    (const uint8_t          *buf        /**< [in] */
    ,size_t                  datasize   /**< [in] */
    )
{
    const uint8_t ID1 = 0x1f;
    const uint8_t ID2 = 0x8b;
    dlb_pmd_bool status = PMD_FALSE;

    if (buf == NULL || datasize == 0)
    {
        return status;
    }

    /* GZIP file format specification version 4.3 */
    /* A gzip file consists of a series of "members" (compressed data sets).
     * Each member has the following structure:

         +---+---+---+---+---+---+---+---+---+---+
         |ID1|ID2|CM |FLG|     MTIME     |XFL|OS | (more-->)
         +---+---+---+---+---+---+---+---+---+---+
     * Check if buffer contains ID1 and ID2
     */
    if (buf[0] == ID1 && buf[1] == ID2)
    {
        status = PMD_TRUE;
    }

    return status;
}


/**
 * @brief determine memory requirements for the sADM bitstream decoder
 */
size_t                                    /** @return size of memory required */
sadm_bitstream_decoder_query_mem
    (void
    )
{
    return sizeof(sadm_bitstream_decoder);
}


dlb_pmd_success
sadm_bitstream_decoder_init
    (void                       *mem
    ,sadm_bitstream_decoder    **dec
    )
{
    sadm_bitstream_decoder *d = (sadm_bitstream_decoder*)mem;

    memset(d, 0, sizeof(*d));
    *dec = d;

    return PMD_SUCCESS;
}


int
decompress_sadm_xml
   (sadm_bitstream_decoder  *dec
   ,const uint8_t           *buf
   ,size_t                   datasize
   )
{
    z_stream s;
    int res;
    unsigned int done = 0;

    memset(&s, 0, sizeof(s));
    s.zalloc = 0;
    s.zfree = 0;
    s.next_in = buf;
    s.total_out = 0;
    s.avail_in = (uInt)datasize;
    s.next_out = (uint8_t*)dec->xmlbuf;
    s.avail_out = sizeof(dec->xmlbuf);

    if (inflateInit2(&s, MAX_WBITS + 32)) // 32 - autodetect header type
    {
        return 0;
    }

    while(!done)
    {
        s.next_out = (uint8_t*) (dec->xmlbuf + s.total_out);
        s.avail_out -= s.total_out;

        res = inflate(&s, Z_NO_FLUSH);
        if (res == Z_STREAM_END)
        {
            done = 1;
            break;
        }
        else if (Z_OK != res)
        {
#ifndef NDEBUG
            printf("zlib error: %s\n", s.msg);
#endif
            s.total_out = 0;
            break;
        }
    }

    inflateEnd(&s);
    return s.total_out;
}


dlb_pmd_success
sadm_bitstream_decoder_decode
    (sadm_bitstream_decoder         *dec
    ,const uint8_t                  *bitstream
    ,size_t                          datasize
    ,dlb_adm_core_model             *model
    ,dlb_adm_bool                    use_common_defs
    ,sadm_bitstream_dec_callback     callback
    ,void                           *cbarg
    )
{
    dlb_adm_container_counts     counts;
    dlb_adm_container_counts     counts_flat;
    dlb_adm_xml_container       *container = NULL;
    dlb_adm_xml_container       *container_flat = NULL;
    dlb_pmd_success              result = PMD_SUCCESS;

    memset(&counts, 0, sizeof(counts));
    dec->model = model;
    if (is_buffer_compressed(bitstream, datasize))
    {
        dec->size = decompress_sadm_xml(dec, bitstream, datasize);
        if (!dec->size)
        {
            if (callback)
            {
                callback(cbarg, SADM_DECOMPRESS_ERR);
            }
            return PMD_FAIL;
        }
    }
    else
    {
        if (datasize > DLB_PMD_SADM_MAX_XML_SIZE)
        {
            if (callback)
            {
                callback(cbarg, SADM_DECOMPRESS_ERR);
            }
            return PMD_FAIL;
        }
        dec->size = datasize;
        memcpy(dec->xmlbuf, bitstream, dec->size);
    }

    if (dlb_adm_container_open(&container, &counts)                                           ||
        dlb_adm_container_open(&container_flat, &counts_flat)                                 ||
        dlb_adm_container_read_xml_buffer(container, dec->xmlbuf, dec->size, use_common_defs) ||
        dlb_adm_container_flatten(container, container_flat)                                  ||
        dlb_adm_core_model_clear(model)                                                       ||
        dlb_adm_core_model_ingest_xml_container(model, container_flat)
       )
    {
        result = PMD_FAIL;
    }

    if (callback)
    {
        callback(cbarg, (result == PMD_SUCCESS) ? SADM_OK : SADM_PARSE_ERR);
    }

    if (container != NULL)
    {
        (void)dlb_adm_container_close(&container);
        (void)dlb_adm_container_close(&container_flat);
    }

    return result;
}
