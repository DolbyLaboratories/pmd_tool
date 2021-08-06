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

#include "sadm_bitstream_decoder.h"
#include "dlb_adm/include/dlb_adm_api.h"
#include "zlib.h"


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
   ,uint8_t                 *buf
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
    ,uint8_t                        *bitstream
    ,size_t                          datasize
    ,dlb_adm_core_model             *model
    ,sadm_bitstream_dec_callback     callback
    ,void                           *cbarg
    )
{
    dlb_adm_container_counts     counts;
    dlb_adm_xml_container       *container = NULL;
    dlb_pmd_success              result = PMD_SUCCESS;

    memset(&counts, 0, sizeof(counts));
    dec->model = model;
    dec->size = decompress_sadm_xml(dec, bitstream, datasize);
    if (!dec->size)
    {
        if (callback)
        {
            callback(cbarg, SADM_DECOMPRESS_ERR);
        }
        return PMD_FAIL;
    }

    if (dlb_adm_container_open(&container, &counts)                                         ||
        dlb_adm_container_read_xml_buffer(container, dec->xmlbuf, dec->size, DLB_ADM_FALSE) ||
        dlb_adm_core_model_ingest_xml_container(model, container)
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
    }

    return result;
}
