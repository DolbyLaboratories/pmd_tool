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
#include "zlib.h"


/**
 * @brief callback function to record and print error status for the sADM bitstream decoder
 */
static
void
sadm_bitstream_decoder_error_callback
    (const char *msg
    ,void       *arg
    )
{
    sadm_bitstream_decoder *dec = (sadm_bitstream_decoder *)arg;

    error(dec->model, "Could not read SADM: %s at line %u", msg, dec->error_line);
    /* TODO: printf might be too verbose for some situations... */
    printf("Could not read SADM: %s at line %u\n", msg, dec->error_line);
}


size_t
sadm_bitstream_decoder_query_mem
    (dlb_pmd_model_constraints  *limits
    )
{
    return sizeof(sadm_bitstream_decoder)
        + dlb_pmd_sadm_reader_query_mem(limits);
}


dlb_pmd_success
sadm_bitstream_decoder_init
    (dlb_pmd_model_constraints  *limits
    ,void                       *mem
    ,sadm_bitstream_decoder    **dec
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
    ,dlb_pmd_model                  *model
    ,sadm_bitstream_dec_callback     callback
    ,void                           *cbarg
    )
{
    dlb_pmd_success result;

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

    result = dlb_pmd_sadm_string_read(dec->r, "decompressed", dec->xmlbuf, dec->size,
                                      model, sadm_bitstream_decoder_error_callback, dec,
                                      &dec->error_line);

    if (callback)
    {
        callback(cbarg, (result == PMD_SUCCESS) ? SADM_OK : SADM_PARSE_ERR);
    }

    return result;
}

