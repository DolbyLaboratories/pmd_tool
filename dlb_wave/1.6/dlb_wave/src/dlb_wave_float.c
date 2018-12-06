/************************************************************************
 * dlb_wave
 * Copyright (c) 2018, Dolby Laboratories Inc.
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

#include <float.h>
#include <assert.h>
#include "dlb_wave/include/dlb_wave_float.h"
#include "dlb_wave/include/dlb_wave_int.h"

static unsigned memle16(const unsigned char *data)
{
    return data[0] | ((unsigned)data[1] << 8);
}

static unsigned long memle24(const unsigned char *data)
{
    return data[0] | ((unsigned long)memle16(data + 1) << 8);
}

static unsigned long memle32(const unsigned char *data)
{
    return data[0] | (memle24(data + 1) << 8);
}

static void le16mem(unsigned le16, unsigned char *data)
{
    assert(le16 <= 0xFFFFu);
    data[0] = (unsigned char)(le16 & 0xFFu);
    data[1] = (unsigned char)(le16 >> 8);
}

static void le24mem(unsigned long le24, unsigned char *data)
{
    assert(le24 <= 0xFFFFFFu);
    data[0] = (unsigned char)(le24 & 0xFFu);
    le16mem((unsigned)(le24 >> 8), data + 1);
}

static void le32mem(unsigned long le32, unsigned char *data)
{
    data[0] = (unsigned char)(le32 & 0xFFu);
    le24mem(le32 >> 8, data + 1);
}

static long twosu32(unsigned long in)
{
    if (in & 0x80000000ul)
    {
        return -((long)(~in & 0x7FFFFFFFu)) - 1l;
    }
    assert(in <= 0x7FFFFFFFu);
    return (long)in;
}

static int twosu16(unsigned in)
{
    if (in & 0x8000ul)
    {
        return -((int)(~in & 0x7FFFu)) - 1l;
    }
    assert(in <= 0x7FFFu);
    return (long)in;
}

static float unpack_float(const unsigned char *blob)
{
    int exponent;
    int sign;
    unsigned long mantissa;
    float val;

    mantissa = memle32(blob);
    exponent = (int)((mantissa >> 23) & 0xFFu);
    sign     = (int)((mantissa >> 31) & 0x1u);
    mantissa = 0x800000u | (mantissa & 0x7FFFFFu);

    if (exponent == 255)
    {
        if (mantissa == 0x800000u)
        {
            /* +infinity produces largest positive number
             * -infinity produces largest negative number */
            return (sign) ? -1.0f : 1.0f;
        }

        /* NaNs produce 0 */
        return 0;
    }

    if (exponent == 0)
    {
        /* +0, -0 or denormal produces 0 */
        return 0;
    }

    /* Test for numbers with magnitude >= 1.0 and saturate. */
    exponent -= 127;
    if (exponent >= 0)
    {
        return (sign) ? -1.0f : 1.0f;
    }

    /* Denormalize */
    exponent -= 23;
    
    for (val = (float)mantissa; exponent; val *= 0.5f, exponent++);

    assert(exponent == 0);

    return (sign) ? -val : val;
}

static void pack_float(unsigned char *blob, float f)
{
    unsigned long mantissa;
    unsigned long sign;
    int exponent = 127 - 1;

    /* trap on nan. */
    assert(f == f);

    /* clip infinities */
    if (f > FLT_MAX)
    {
        f = FLT_MAX;
    }
    else if (f < -FLT_MAX)
    {
        f = -FLT_MAX;
    }

    /* special handling for 0 and denorms. */
    if (f > -FLT_MIN && f < FLT_MIN)
    {
        blob[0] = 0;
        blob[1] = 0;
        blob[2] = 0;
        blob[3] = 0;
        return;
    }

    /* need the absolute value for IEEE 754 */
    if (f < 0)
    {
        f = -f;
        sign = 0x80000000ul;
    }
    else
    {
        sign = 0;
    }

    /* normalise to 24 bit mantissa precision */
    while (f >= 1.0f)
    {
        f *= 0.5f;
        ++exponent;
    }
    while (f < 0.5f)
    {
        f *= 2.0f;
        --exponent;
    }

    mantissa = (unsigned long)(f * 0x1000000 + 0.5f);

    assert(mantissa >= 0x800000u);
    if (mantissa > 0xFFFFFFu)
    {
        mantissa >>= 1;
        ++exponent;
    }
    assert(mantissa >= 0x800000u);
    assert(mantissa <= 0xFFFFFFu);
    assert(exponent > 0 && exponent <= 255);

    le32mem(sign | (mantissa & 0x7FFFFFu) | ((unsigned long)exponent << 23), blob);
}

static
long
round_float(float f, unsigned mant_bits)
{
    long max = (long)((1ul << mant_bits) - 1ul);
    long min = -max - 1l;
    long l;

    f = (float)min * -f;
    f = (f < 0.0f) ? (f - 0.5f) : (f + 0.5f);

    if (f <= (float)min)
    {
        return min;
    }
    else if (f >= (float)max)
    {
        return max;
    }

    l = (long)f;
    assert(l >= min && l <= max);

    return l;
}

static
long
round_double(double f, unsigned mant_bits)
{
    long max = (long)((1ul << mant_bits) - 1ul);
    long min = -max - 1l;
    long l;

    f = (double)min * -f;
    f = (f < 0.0) ? (f - 0.5) : (f + 0.5);

    if (f <= (double)min)
    {
        return min;
    }
    else if (f >= (double)max)
    {
        return max;
    }

    l = (long)f;
    assert(l >= min && l <= max);

    return l;
}

static
int
dlb_wave_core_float_read
    (dlb_wave_file *pwf
    ,void * const   pvdata[]
    ,size_t         ndata
    ,ptrdiff_t      nstride
    ,size_t        *pnread
    )
{
    unsigned channel_count = dlb_wave_get_channel_count(pwf);
    size_t i = 0;
    float * const * const pdata = (float * const *) pvdata;
    if (dlb_wave_get_format_flags(pwf) & DLB_WAVE_FLOAT)
    {
        /* Special case for IEEE_FLOAT files.
         * We can't just directly read floats from disk because the float implementation
         * used by the C compiler might not be IEEE 754. Since this is supposed to
         * be an absolutely vanilla standard C library we therefore have to manually
         * read IEEE 754 bits.
         */
        for (i = 0; i < ndata; i++)
        {
            unsigned c;
            for (c = 0; c < channel_count; c++)
            {
                unsigned char blob[4];
                int status;

                status = dlb_wave_read_data(pwf, blob, 4, 0);
                if (status)
                {
                    if (pnread)
                        *pnread = i;
                    return status;
                }

                /* Write the resulting data to the array */
                pdata[c][i*nstride] = unpack_float(blob);
            }
        }
    }
    else
        switch (dlb_wave_get_format(pwf)->octets_per_sample)
        {
        case 4:
            for (i = 0; i < ndata; i++)
            {
                unsigned c;
                for (c = 0; c < channel_count; c++)
                {
                    int status;
                    unsigned char blob[4];
                    status = dlb_wave_read_data(pwf, blob, 4, 0);
                    if (status)
                    {
                        if (pnread)
                            *pnread = i;
                        return status;
                    }

                    pdata[c][i*nstride] = twosu32(memle32(blob)) * (1.0f / (1ul << 31));
                }
            }
            break;
        case 3:
            for (i = 0; i < ndata; i++)
            {
                unsigned c;
                for (c = 0; c < channel_count; c++)
                {
                    int status;
                    unsigned char blob[3];

                    status = dlb_wave_read_data(pwf, blob, 3, 0);
                    if (status)
                    {
                        if (pnread)
                            *pnread = i;
                        return status;
                    }

                    pdata[c][i*nstride] = twosu32(memle24(blob) << 8) * (1.0f / (1ul << 31));
                }
            }
            break;
        case 2:
            for (i = 0; i < ndata; i++)
            {
                unsigned c;
                for (c = 0; c < channel_count; c++)
                {
                    int status;
                    unsigned char blob[2];

                    status = dlb_wave_read_data(pwf, blob, 2, 0);
                    if (status)
                    {
                        if (pnread)
                            *pnread = i;
                        return status;
                    }

                    pdata[c][i*nstride] = twosu16(memle16(blob)) * (1.0f / (1ul << 15));
                }
            }
            break;
        case 1:
            for (i = 0; i < ndata; i++)
            {
                unsigned c;
                for (c = 0; c < channel_count; c++)
                {
                    int status;
                    unsigned char blob[1];

                    status = dlb_wave_read_data(pwf, blob, 1, 0);
                    if (status)
                    {
                        if (pnread)
                            *pnread = i;
                        return status;
                    }

                    pdata[c][i*nstride] = twosu16((unsigned)(blob[0] ^ 0x80u) << 8) * (1.0f / (1ul << 15));
                }
            }
            break;
        }
    if (pnread)
        *pnread = i;
    return DLB_RIFF_OK;
}

static
int
dlb_wave_core_float_write
    (dlb_wave_file     *pwf
    ,const void * const pvdata[]
    ,size_t             ndata
    ,ptrdiff_t          nstride
    )
{
    int status;
    size_t i;
    unsigned channel_count = dlb_wave_get_channel_count(pwf);
    const float * const * const pdata = (const float * const *) pvdata;
    if (dlb_wave_get_format_flags(pwf) & DLB_WAVE_FLOAT)
    {
        /* Special case for IEEE_FLOAT files.
         * We can't just directly write floats to disk because the float implementation
         * used by the C compiler might not be IEEE 754. Since this is supposed to
         * be an absolutely vanilla standard C library we therefore have to manually
         * produce IEEE 754 bits.
         */
        for (i = 0; i < ndata; i++)
        {
            unsigned c;
            for (c = 0; c < channel_count; c++)
            {
                unsigned char blob[4];

                pack_float(blob, pdata[c][i*nstride]);

                /* write it to disk */
                status = dlb_wave_write_data(pwf, blob, 4);
                if (status)
                    return status;
            }
        }
    }
    else
        switch (dlb_wave_get_format(pwf)->octets_per_sample)
        {
        case 4:
            for (i = 0; i < ndata; i++)
            {
                unsigned c;

                for (c = 0; c < channel_count; c++)
                {
                    unsigned char blob[4];

                    le32mem((unsigned long)round_float(pdata[c][i*nstride], 31) & 0xFFFFFFFFu, blob);

                    status = dlb_wave_write_data(pwf, blob, 4);
                    if (status)
                        return status;
                }
            }
            break;
        case 3:
            for (i = 0; i < ndata; i++)
            {
                unsigned c;

                for (c = 0; c < channel_count; c++)
                {
                    unsigned char blob[3];

                    le24mem((unsigned long)round_float(pdata[c][i*nstride], 23) & 0xFFFFFFu, blob);

                    status = dlb_wave_write_data(pwf, blob, 3);
                    if (status)
                        return status;
                }
            }
            break;
        case 2:
            for (i = 0; i < ndata; i++)
            {
                unsigned c;

                for (c = 0; c < channel_count; c++)
                {
                    unsigned char blob[2];

                    le16mem((unsigned)round_float(pdata[c][i*nstride], 15) & 0xFFFFu, blob);

                    status = dlb_wave_write_data(pwf, blob, 2);
                    if (status)
                        return status;
                }
            }
            break;
        case 1:
            for (i = 0; i < ndata; i++)
            {
                unsigned c;

                for (c = 0; c < channel_count; c++)
                {
                    unsigned char blob[1];

                    blob[0] = ((unsigned)round_float(pdata[c][i*nstride], 7) & 0xFFu) ^ 0x80u;

                    status = dlb_wave_write_data(pwf, blob, 1);
                    if (status)
                        return status;
                }
            }
            break;
        }
    return DLB_RIFF_OK;
}

static
int
dlb_wave_core_double_read
    (dlb_wave_file *pwf
    ,void * const   pvdata[]
    ,size_t         ndata
    ,ptrdiff_t      nstride
    ,size_t        *pnread
    )
{
    unsigned channel_count = dlb_wave_get_channel_count(pwf);
    size_t i = 0;
    double * const * const pdata = (double * const *) pvdata;
    if (dlb_wave_get_format_flags(pwf) & DLB_WAVE_FLOAT)
    {
        /* Special case for IEEE_FLOAT files.
         * We can't just directly read floats from disk because the float implementation
         * used by the C compiler might not be IEEE 754. Since this is supposed to
         * be an absolutely vanilla standard C library we therefore have to manually
         * read IEEE 754 bits.
         */
        for (i = 0; i < ndata; i++)
        {
            unsigned c;
            for (c = 0; c < channel_count; c++)
            {
                unsigned char blob[4];
                int status;

                status = dlb_wave_read_data(pwf, blob, 4, 0);
                if (status)
                {
                    if (pnread)
                        *pnread = i;
                    return status;
                }

                /* Write the resulting data to the array */
                pdata[c][i*nstride] = unpack_float(blob);
            }
        }
    }
    else
        switch (dlb_wave_get_format(pwf)->octets_per_sample)
        {
        case 4:
            for (i = 0; i < ndata; i++)
            {
                unsigned c;
                for (c = 0; c < channel_count; c++)
                {
                    int status;
                    unsigned char blob[4];

                    status = dlb_wave_read_data(pwf, blob, 4, 0);
                    if (status)
                    {
                        if (pnread)
                            *pnread = i;
                        return status;
                    }

                    pdata[c][i*nstride] = twosu32(memle32(blob)) * (1.0 / (1ul << 31));
                }
            }
            break;
        case 3:
            for (i = 0; i < ndata; i++)
            {
                unsigned c;
                for (c = 0; c < channel_count; c++)
                {
                    int status;
                    unsigned char blob[3];

                    status = dlb_wave_read_data(pwf, blob, 3, 0);
                    if (status)
                    {
                        if (pnread)
                            *pnread = i;
                        return status;
                    }

                    pdata[c][i*nstride] = twosu32(memle24(blob) << 8) * (1.0 / (1ul << 31));
                }
            }
            break;
        case 2:
            for (i = 0; i < ndata; i++)
            {
                unsigned c;
                for (c = 0; c < channel_count; c++)
                {
                    int status;
                    unsigned char blob[2];

                    status = dlb_wave_read_data(pwf, blob, 2, 0);
                    if (status)
                    {
                        if (pnread)
                            *pnread = i;
                        return status;
                    }

                    pdata[c][i*nstride] = twosu16(memle16(blob)) * (1.0 / (1ul << 15));
                }
            }
            break;
        case 1:
            for (i = 0; i < ndata; i++)
            {
                unsigned c;
                for (c = 0; c < channel_count; c++)
                {
                    int status;
                    unsigned char blob[1];

                    status = dlb_wave_read_data(pwf, blob, 1, 0);
                    if (status)
                    {
                        if (pnread)
                            *pnread = i;
                        return status;
                    }

                    pdata[c][i*nstride] = twosu16((unsigned)(blob[0] ^ 0x80u) << 8) * (1.0 / (1ul << 15));
                }
            }
            break;
        }
    if (pnread)
        *pnread = i;
    return DLB_RIFF_OK;
}

static
int
dlb_wave_core_double_write
    (dlb_wave_file     *pwf
    ,const void * const pvdata[]
    ,size_t             ndata
    ,ptrdiff_t          nstride
    )
{
    int status;
    size_t i;
    unsigned channel_count = dlb_wave_get_channel_count(pwf);
    const double * const * const pdata = (const double * const *) pvdata;
    if (dlb_wave_get_format_flags(pwf) & DLB_WAVE_FLOAT)
    {
        /* Special case for IEEE_FLOAT files.
         * We can't just directly write floats to disk because the float implementation
         * used by the C compiler might not be IEEE 754. Since this is supposed to
         * be an absolutely vanilla standard C library we therefore have to manually
         * produce IEEE 754 bits.
         */
        for (i = 0; i < ndata; i++)
        {
            unsigned c;
            for (c = 0; c < channel_count; c++)
            {
                unsigned char blob[4];

                pack_float(blob, (float)pdata[c][i*nstride]);

                /* write it to disk */
                status = dlb_wave_write_data(pwf, blob, 4);
                if (status)
                    return status;
            }
        }
    }
    else
        switch (dlb_wave_get_format(pwf)->octets_per_sample)
        {
        case 4:
            for (i = 0; i < ndata; i++)
            {
                unsigned c;

                for (c = 0; c < channel_count; c++)
                {
                    unsigned char blob[4];

                    le32mem((unsigned long)round_double(pdata[c][i*nstride], 31) & 0xFFFFFFFFu, blob);

                    status = dlb_wave_write_data(pwf, blob, 4);
                    if (status)
                        return status;
                }
            }
            break;
        case 3:
            for (i = 0; i < ndata; i++)
            {
                unsigned c;

                for (c = 0; c < channel_count; c++)
                {
                    unsigned char blob[3];

                    le24mem((unsigned long)round_double(pdata[c][i*nstride], 23) & 0xFFFFFFu, blob);

                    status = dlb_wave_write_data(pwf, blob, 3);
                    if (status)
                        return status;
                }
            }
            break;
        case 2:
            for (i = 0; i < ndata; i++)
            {
                unsigned c;

                for (c = 0; c < channel_count; c++)
                {
                    unsigned char blob[2];

                    le16mem((unsigned)round_double(pdata[c][i*nstride], 15) & 0xFFFFu, blob);

                    status = dlb_wave_write_data(pwf, blob, 2);
                    if (status)
                        return status;
                }
            }
            break;
        case 1:
            for (i = 0; i < ndata; i++)
            {
                unsigned c;

                for (c = 0; c < channel_count; c++)
                {
                    unsigned char blob[1];

                    blob[0] = ((unsigned)round_double(pdata[c][i*nstride], 7) & 0xFFu) ^ 0x80u;

                    status = dlb_wave_write_data(pwf, blob, 1);
                    if (status)
                        return status;
                }
            }

            break;
        }
    return DLB_RIFF_OK;
}

int
dlb_wave_float_read
    (dlb_wave_file    *pwf
    ,const dlb_buffer *pbuffer
    ,size_t            ndata
    ,size_t           *pnread
    )
{
    if (pbuffer->nchannel < dlb_wave_get_channel_count(pwf))
        return DLB_WAVE_E_NCHANNEL;

    switch (pbuffer->data_type)
    {
    case DLB_BUFFER_FLOAT:
        return dlb_wave_core_float_read(pwf, pbuffer->ppdata, ndata, pbuffer->nstride, pnread);
    case DLB_BUFFER_DOUBLE:
        return dlb_wave_core_double_read(pwf, pbuffer->ppdata, ndata, pbuffer->nstride, pnread);
    default:
        return dlb_wave_int_read(pwf, pbuffer, ndata, pnread);
    }
}

int
dlb_wave_float_write
    (dlb_wave_file    *pwf
    ,const dlb_buffer *pbuffer
    ,size_t            ndata
    )
{
    if (pbuffer->nchannel < dlb_wave_get_channel_count(pwf))
        return DLB_WAVE_E_NCHANNEL;

    switch (pbuffer->data_type)
    {
    case DLB_BUFFER_FLOAT:
        return dlb_wave_core_float_write(pwf, (const void* const*) pbuffer->ppdata, ndata, pbuffer->nstride);
    case DLB_BUFFER_DOUBLE:
        return dlb_wave_core_double_write(pwf, (const void* const*) pbuffer->ppdata, ndata, pbuffer->nstride);
    default:
        return dlb_wave_int_write(pwf, pbuffer, ndata);
    }
}
