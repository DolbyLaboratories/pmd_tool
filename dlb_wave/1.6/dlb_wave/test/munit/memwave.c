/************************************************************************
 * dlb_wave
 * Copyright (c) 2015, Dolby Laboratories Inc.
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

#include "memwave.h"
#include <string.h>
#include <assert.h>

static unsigned le16_mem(const unsigned char *p_mem)
{
    return p_mem[0] + 256u * p_mem[1];
}

static unsigned long le24_mem(const unsigned char *p_mem)
{
    return p_mem[0] + 256ul * le16_mem(p_mem + 1);
}

static unsigned long le32_mem(const unsigned char *p_mem)
{
    return p_mem[0] + 256ul * le24_mem(p_mem + 1);
}

static
const char *
find_chunk
    (const unsigned char  *p_riff
    ,size_t                riff_size
    ,const char           *p_chunk_name
    ,const unsigned char **pp_chunk_data
    ,size_t               *p_chunk_size
    )
{
    size_t int_riff_size;

    if (riff_size < 12)
    {
        return "Not enough data supplied to be a wave file";
    }

    if  (   p_riff[0] != 'R'
        ||  p_riff[1] != 'I'
        ||  p_riff[2] != 'F'
        ||  p_riff[3] != 'F'
        )
    {
        return "Not RIFF data";
    }

    riff_size -= 8;
    int_riff_size = (size_t)le32_mem(p_riff + 4);
    if (int_riff_size < 4 || int_riff_size > riff_size)
    {
        return "Invalid RIFF size";
    }
    riff_size  = int_riff_size - 4;

    if  (   p_riff[8] != 'W'
        ||  p_riff[9] != 'A'
        ||  p_riff[10] != 'V'
        ||  p_riff[11] != 'E'
        )
    {
        return "Not WAVE data";
    }

    p_riff    += 12;

    while (riff_size > 8)
    {
        size_t chunk_size = (size_t)le32_mem(p_riff + 4);

        size_t whole_ck_size = 8 + chunk_size + (chunk_size & 1);

        if (whole_ck_size > riff_size)
        {
            return "Invalid RIFF subchunk size";
        }

        if  (   p_riff[0] == p_chunk_name[0]
            &&  p_riff[1] == p_chunk_name[1]
            &&  p_riff[2] == p_chunk_name[2]
            &&  p_riff[3] == p_chunk_name[3]
            )
        {
            *p_chunk_size = chunk_size;
            *pp_chunk_data = p_riff + 8;
            return NULL;
        }

        riff_size -= whole_ck_size;
        p_riff    += whole_ck_size;
    }

    return "Could not find chunk";
}

const char *
memwave_find_data
    (const unsigned char  *p_riff
    ,size_t                riff_size
    ,const unsigned char **pp_data
    ,size_t               *p_data_size
    )
{
    return find_chunk(p_riff, riff_size, "data", pp_data, p_data_size);
}

/* Returns an error description if the format fails to parse. */
static
const char *
parse_wave_format
    (const unsigned char *p_data
    ,size_t               size
    ,dlb_wave_format     *p_fmt
    ,int                 *b_is_float
    )
{
    static const unsigned char a_pcm_guid[]   = {0x01,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x80,0x00,0x00,0xaa,0x00,0x38,0x9b,0x71};
    static const unsigned char a_float_guid[] = {0x03,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x80,0x00,0x00,0xaa,0x00,0x38,0x9b,0x71};

    unsigned      cbsize = 0;
    int           format_float = 0;

    if (size < 16)
    {
        return "Not enough data in wave format";
    }

    p_fmt->format_type              = (unsigned short)le16_mem(p_data + 0);
    p_fmt->channel_count            = (unsigned short)le16_mem(p_data + 2);
    p_fmt->sample_rate              = le32_mem(p_data + 4);
    p_fmt->bytes_per_second         = le32_mem(p_data + 8);
    p_fmt->block_alignment          =(unsigned short) le16_mem(p_data + 12);
    p_fmt->bits_per_sample          = (unsigned short)le16_mem(p_data + 14);
    p_fmt->octets_per_sample        = (p_fmt->bits_per_sample + 7) / 8;
    p_fmt->valid_bits_per_sample    = p_fmt->bits_per_sample;
    p_fmt->channel_mask             = 0;

    size -= 16;
    p_data += 16;

    /* All formats other than PCM should contain cbSize member we permit them
     * not to have it because so many wave writers are broken. */
    if (p_fmt->format_type != 0x01 && size >= 2)
    {
        cbsize = le16_mem(p_data + 0);
        size -= 2;
        p_data += 2;
    }

    if (cbsize > size)
    {
        return "cbSize member of wave format was invaid";
    }

    if (p_fmt->format_type == 0xFFFEu)
    {
        if (cbsize < 22)
            return "Wave marked as extensible but format does not contain enough data";

        p_fmt->valid_bits_per_sample = (unsigned short)le16_mem(p_data + 0);
        p_fmt->channel_mask          = le32_mem(p_data + 2);
        format_float = (memcmp(p_data + 6, a_float_guid, 16) == 0);

        if (!format_float && memcmp(p_data + 6, a_pcm_guid, 16) != 0)
            return "Wave is extensible but is neither uncompressed PCM nor floating point";
    }
    else if (p_fmt->format_type == 0x0003u)
    {
        format_float = 1;
    }
    else if (p_fmt->format_type != 0x0001u)
    {
        return "Wave format is not supported by the implementation";
    }

    *b_is_float = format_float;
    return NULL;
}

const char *
memwave_load_format
    (const unsigned char  *p_riff
    ,size_t                riff_size
    ,dlb_wave_format      *p_fmt
    ,int                  *b_is_float
    )
{
    size_t fsz;
    const unsigned char *p_fmt_data;
    const char *p_err;

    p_err = find_chunk(p_riff, riff_size, "fmt ", &p_fmt_data, &fsz);
    if (p_err != NULL)
    {
        return p_err;
    }

    return parse_wave_format(p_fmt_data, fsz, p_fmt, b_is_float);
}
