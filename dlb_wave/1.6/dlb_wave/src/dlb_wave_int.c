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

#include "dlb_wave/include/dlb_wave_int.h"
#include <assert.h>

/**************************************************************************//**
Macros for calculating log2(num/den) at compile time. This is needed so we
can determine how many bits we need to shift the data down to fit in in a
certain number of octets, given that we don't know how many bits there are
in the short, int and long types a priori.
******************************************************************************/
#define DLB_LOGRATIO2_1(num, den) (((num) > (den)) ? 1:0)

#define DLB_LOGRATIO2_2(num, den)\
    ((((num)>>1) > (den))\
    ?   (1 + DLB_LOGRATIO2_1(((num)>>1), (den)))\
        :   DLB_LOGRATIO2_1((num), (den)))

#define DLB_LOGRATIO2_4(num, den)\
    ((((num)>>2) > (den))\
    ?   (2 + DLB_LOGRATIO2_2(((num)>>2), (den)))\
        :   DLB_LOGRATIO2_2((num), (den)))

#define DLB_LOGRATIO2_8(num, den)\
    ((((num)>>4) > (den))\
    ?   (4 + DLB_LOGRATIO2_4(((num)>>4), (den)))\
        :   DLB_LOGRATIO2_4((num), (den)))

#define DLB_LOGRATIO2_16(num, den)\
    ((((num)>>8) > (den))\
        ?   (8 + DLB_LOGRATIO2_8(((num)>>8), (den)))\
        :   DLB_LOGRATIO2_8((num), (den)))

#define DLB_LOGRATIO2_32(num, den)\
    ((((num)>>16) > (den))\
        ?   (16 + DLB_LOGRATIO2_16(((num)>>16), (den)))\
        :   DLB_LOGRATIO2_16((num), (den)))

#define DLB_LOGRATIO2_48(num, den)\
    ((((num)>>16) > (den))\
        ?   (16 + DLB_LOGRATIO2_32(((num)>>16), (den)))\
        :   DLB_LOGRATIO2_16((num), (den)))

#define DLB_LOGRATIO2_64(num, den)\
    ((((num)>>16) > (den))\
        ?   (16 + DLB_LOGRATIO2_48(((num)>>16), (den)))\
        :   DLB_LOGRATIO2_16((num), (den)))

#define DLB_LOGRATIO2_80(num, den)\
    ((((num)>>16) > (den))\
        ?   (16 + DLB_LOGRATIO2_64(((num)>>16), (den)))\
        :   DLB_LOGRATIO2_16((num), (den)))

#define DLB_LOGRATIO2_96(num, den)\
    ((((num)>>16) > (den))\
        ?   (16 + DLB_LOGRATIO2_80(((num)>>16), (den)))\
        :   DLB_LOGRATIO2_16((num), (den)))

#define DLB_LOGRATIO2_112(num, den)\
    ((((num)>>16) > (den))\
        ?   (16 + DLB_LOGRATIO2_96(((num)>>16), (den)))\
        :   DLB_LOGRATIO2_16((num), (den)))

#define DLB_LOGRATIO2_128(num, den)\
    ((((num)>>16) > (den))\
        ?   (16 + DLB_LOGRATIO2_112(((num)>>16), (den)))\
        :   DLB_LOGRATIO2_16((num), (den)))

#define DLB_LOGRATIO2(num, den)\
    (((num) >= (den))\
        ?   (+DLB_LOGRATIO2_128((num), (den)))\
        :   (-DLB_LOGRATIO2_128((den), (num))))\

static long twosu32(unsigned long in)
{
    if (in & 0x80000000ul)
    {
        return -((long)(~in & 0x7FFFFFFFul)) - 1l;
    }
    assert(in <= 0x7FFFFFFFul);
    return (long)in;
}

static int twosuintmax(unsigned in)
{
    if (in & ((unsigned)INT_MAX + 1u))
    {
        return -((int)(~in & (unsigned)INT_MAX)) - 1;
    }
    assert(in <= INT_MAX);
    return (int)in;
}

static int twos_to_intmax(unsigned long in, unsigned long in_max)
{
    if ((unsigned long)INT_MAX < in_max)
    {
        unsigned long dval    = (in_max + 1ul) / ((unsigned)INT_MAX + 1u);
        unsigned long hdval   = dval >> 1;
        unsigned valint       = (unsigned)(in / dval);
        unsigned long valfrac = (unsigned)(in & (dval - 1u));
        in = valint + ((valint != (unsigned)INT_MAX) && (valfrac > hdval || (valfrac == hdval && (valint & 1))));
    }
    else if ((unsigned long)INT_MAX > in_max)
    {
        in *= ((unsigned)INT_MAX + 1u) / (unsigned)(in_max + 1ul);
    }
    return twosuintmax(in);
}

static unsigned long intmax_to_twos(int in, unsigned long twos_max)
{
    unsigned long x = (unsigned long)in;
    if ((unsigned long)INT_MAX < twos_max)
    {
        x *= (twos_max + 1ul) / ((unsigned)INT_MAX + 1u);
    }
    else if ((unsigned long)INT_MAX > twos_max)
    {
        unsigned long dval    = ((unsigned)INT_MAX + 1u) / (twos_max + 1ul);
        unsigned long hdval   = dval >> 1;
        unsigned valint       = (unsigned)(x / dval);
        unsigned long valfrac = (unsigned)(x & (dval - 1u));
        x = valint + ((valint != twos_max) && (valfrac > hdval || (valfrac == hdval && (valint & 1))));
    }
    return x & ((twos_max << 1) + 1u);
}

static int twosu16(unsigned in)
{
    if (in & 0x8000u)
    {
        return -((int)(~in & 0x7FFFu)) - 1;
    }
    assert(in <= 0x7FFFul);
    return (int)in;
}

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
    data[0] = (unsigned char)(le24 & 0xFFu);
    le16mem(le24 >> 8, data + 1);
}

static void le32mem(unsigned long le32, unsigned char *data)
{
    data[0] = (unsigned char)(le32 & 0xFFu);
    le24mem(le32 >> 8, data + 1);
}

static long unpack_float_long(const unsigned char *blob, unsigned nb_mantbits)
{
    const long max_ret = (long)((1ul << nb_mantbits) - 1ul);
    const long min_ret = -max_ret - 1l;
    int exponent;
    int sign;
    unsigned long mantissa;

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
            return (sign) ? min_ret : max_ret;
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
        return (sign) ? min_ret : max_ret;
    }

    /* Denormalize */
    exponent += (int)nb_mantbits - 23;
    if (exponent > 0)
    {
        assert(exponent <= 8);
        mantissa <<= exponent;
    }
    else if (exponent < 0)
    {
        if (exponent <= -25)
        {
            mantissa = 0;
        }
        else
        {
            unsigned long mfrachalf = 1ul << (-exponent - 1);
            unsigned long mint      = mantissa >> -exponent;
            unsigned long mfracmax  = (mfrachalf << 1) - 1ul;
            unsigned long mfrac     = mantissa & mfracmax;
            mantissa = mint + (unsigned long)(mint != (unsigned long)max_ret && (mfrac > mfrachalf || (mfrac == mfrachalf && (mint & 1u))));
        }
    }

    if (sign)
    {
        assert(-(long)mantissa >= min_ret);
        return -(long)mantissa;
    }

    assert((long)mantissa <= max_ret);
    return (long)mantissa;
}

static void pack_long_float(unsigned char *blob, long val, unsigned nb_mantbits)
{
    unsigned long mantissa;
    unsigned exponent;
    unsigned long mask;
    unsigned long sign;
    unsigned step;
    unsigned msbp = 0;

    if (val == 0)
    {
        /* special case for 0 */
        blob[0] = 0;
        blob[1] = 0;
        blob[2] = 0;
        blob[3] = 0;
        return;
    }

    mantissa = (unsigned long)val;
    sign     = mantissa & 0x80000000ul;
    
    if (sign)
    {
        mantissa = (~mantissa & 0x7FFFFFFFul) + 1ul;
    }

    assert(mantissa != 0);

    /* Find the position of the most significant bit. */
    for (step = 16, mask = mantissa; step; step >>= 1)
    {
        if (mask > (1ul << step) - 1)
        {
            mask >>= step;
            msbp += step;
        }
    }

    /* Normalize the mantissa into the range (0x800000ul <= x <= 0xFFFFFFu). */
    if (msbp > 23)
    {
        unsigned sft            = (unsigned)(msbp - 23);
        unsigned long mfrachalf = 1ul << (sft - 1);
        unsigned long mint      = mantissa >> sft;
        unsigned long mfracmax  = (mfrachalf << 1) - 1ul;
        unsigned long mfrac     = mantissa & mfracmax;

        if (mfrac < mfrachalf || (mfrac == mfrachalf && !(mint & 1u)))
        {
            mantissa = mint;
        }
        else if (mint != 0xFFFFFFu)
        {
            mantissa = mint + 1;
        }
        else
        {
            mantissa = 0x800000ul;
            msbp++;
        }
    }
    else if (msbp < 23)
    {
        mantissa <<= (23 - msbp);
    }

    exponent = 127 - nb_mantbits + msbp;

    assert(exponent > 0 && exponent < 0xFFu);
    assert(mantissa >= 0x800000ul && mantissa <= 0xFFFFFFu);

    le32mem(sign | (exponent << 23) | (mantissa & 0x7FFFFFu), blob);
}

static int
dlb_wave_short_16_read
    (dlb_wave_file *pwf         /**< wave file open for reading */
    ,void * const   pvdata[]    /**< one pointer per channel */
    ,size_t         ndata       /**< maximum samples per channel to read */
    ,ptrdiff_t      nstride     /**< distance (chars) between samples of one channel */
    ,size_t        *pnread      /**< number of samples actually read */
    )
{
    unsigned channel_count = dlb_wave_get_channel_count(pwf);
    size_t i = 0;
    short * const * const pdata = (short * const *) pvdata;
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
                pdata[c][i*nstride] = (short)unpack_float_long(blob, 15);
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
                    unsigned valint;
                    unsigned valfrac;

                    status = dlb_wave_read_data(pwf, blob, 4, 0);
                    if (status)
                    {
                        if (pnread)
                            *pnread = i;
                        return status;
                    }

                    valint  = memle16(blob + 2);
                    valfrac = memle16(blob);
                    valint += (valint != 0x7FFFu && (valfrac > 0x8000u || (valfrac == 0x8000u && valint & 0x1u)));

                    pdata[c][i*nstride] = (short)twosu16(valint & 0xFFFFu);
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
                    unsigned valint;
                    unsigned valfrac;

                    status = dlb_wave_read_data(pwf, blob, 3, 0);
                    if (status)
                    {
                        if (pnread)
                            *pnread = i;
                        return status;
                    }

                    valint  = memle16(blob + 1);
                    valfrac = ((unsigned)blob[0] & 0xFFu);
                    valint += (valint != 0x7FFFu && (valfrac > 0x80u || (valfrac == 0x80u && valint & 0x1u)));

                    pdata[c][i*nstride] = (short)twosu16(valint & 0xFFFFu);
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

                    pdata[c][i*nstride] = (short)twosu16(memle16(blob));
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

                    pdata[c][i*nstride] = (short)twosu16(((blob[0] & 0xFFu) ^ 0x80u) << 8);
                }
            }
            break;
        }
    if (pnread)
        *pnread = i;
    return DLB_RIFF_OK;
}

static int
dlb_wave_short_16_write
    (dlb_wave_file      *pwf        /**< wave file open for writing */
    ,const void * const  pvdata[]   /**< one pointer per channel */
    ,size_t              ndata      /**< samples per channel to write */
    ,ptrdiff_t           nstride    /**< distance (chars) between samples of one channel */
    )
{
    int status;
    size_t i;
    unsigned channel_count = dlb_wave_get_channel_count(pwf);
    const short * const * const pdata = (const short * const *) pvdata;
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

                pack_long_float(blob, pdata[c][i*nstride], 15);

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
                    unsigned x = (unsigned)pdata[c][i*nstride];
                    unsigned char blob[4];

                    blob[0] = 0;
                    blob[1] = 0;
                    le16mem(x & 0xFFFFu, blob + 2);

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
                    unsigned x = (unsigned)pdata[c][i*nstride];
                    unsigned char blob[3];

                    blob[0] = 0;
                    le16mem(x & 0xFFFFu, blob + 1);

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
                    unsigned x = (unsigned)pdata[c][i*nstride];
                    unsigned char blob[2];

                    le16mem(x & 0xFFFFu, blob);

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
                    unsigned x = (unsigned)pdata[c][i*nstride];
                    unsigned char blob[1];
                    unsigned valint;
                    unsigned valfrac;

                    valint  = x >> 8;
                    valfrac = x & 0xFFu;
                    valint += (valint != 0x7Fu && (valfrac > 0x80u || (valfrac == 0x80u && valint & 0x1u)));

                    blob[0] = (unsigned char)((valint & 0xFFu) ^ 0x80u);

                    status = dlb_wave_write_data(pwf, blob, 1);
                    if (status)
                        return status;
                }
            }
            break;
        }
    return DLB_RIFF_OK;
}

/** Read some audio data from the file. */
static int
dlb_wave_int_left_read
    (dlb_wave_file *pwf         /**< wave file open for reading */
    ,void * const   pvdata[]    /**< one pointer per channel */
    ,size_t         ndata       /**< maximum samples per channel to read */
    ,ptrdiff_t      nstride     /**< distance (chars) between samples of one channel */
    ,size_t        *pnread      /**< number of samples actually read */
    )
{
    unsigned channel_count = dlb_wave_get_channel_count(pwf);
    size_t i = 0;
    int * const * const pdata = (int * const *) pvdata;
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
                pdata[c][i*nstride] = (int)unpack_float_long(blob, DLB_LOGRATIO2(INT_MAX, 1) + 1);
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

                    pdata[c][i*nstride] = twos_to_intmax(memle32(blob), 0x7FFFFFFFul);
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

                    pdata[c][i*nstride] = twos_to_intmax(memle24(blob), 0x7FFFFFul);
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

                    pdata[c][i*nstride] = twos_to_intmax(memle16(blob), 0x7FFFu);
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

                    pdata[c][i*nstride] = twos_to_intmax(blob[0] ^ 0x80u, 0x7Fu);
                }
            }
            break;
        }
    if (pnread)
        *pnread = i;
    return DLB_RIFF_OK;
}

/** Write some audio data to the file. */
static
int
dlb_wave_int_left_write
    (dlb_wave_file     *pwf        /**< wave file open for writing */
    ,const void * const pvdata[]   /**< one pointer per channel */
    ,size_t             ndata      /**< samples per channel to write */
    ,ptrdiff_t          nstride    /**< distance (chars) between samples of one channel */
    )
{
    int status;
    size_t i;
    unsigned channel_count = dlb_wave_get_channel_count(pwf);
    const int * const * const pdata = (const int * const *) pvdata;
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

                pack_long_float(blob, pdata[c][i*nstride], DLB_LOGRATIO2(INT_MAX, 1) + 1);

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

                    le32mem(intmax_to_twos(pdata[c][i*nstride], 0x7FFFFFFFul), blob);

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

                    le24mem(intmax_to_twos(pdata[c][i*nstride], 0x7FFFFFul), blob);

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

                    le16mem((unsigned)intmax_to_twos(pdata[c][i*nstride], 0x7FFFu), blob);

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

                    blob[0] = (unsigned char)intmax_to_twos(pdata[c][i*nstride], 0x7Fu) ^ 0x80;

                    status = dlb_wave_write_data(pwf, blob, 1);
                    if (status)
                        return status;
                }
            }
            break;
        }
    return DLB_RIFF_OK;
}

/** Read some audio data from the file. */
static
int
dlb_wave_long_32_read
    (dlb_wave_file *pwf         /**< wave file open for reading */
    ,void * const   pvdata[]    /**< one pointer per channel */
    ,size_t         ndata       /**< maximum samples per channel to read */
    ,ptrdiff_t      nstride     /**< distance (longs) between samples of one channel */
    ,size_t        *pnread      /**< number of samples actually read */
    )
{
    unsigned channel_count = dlb_wave_get_channel_count(pwf);
    size_t i = 0;
    long * const * const pdata = (long * const *) pvdata;
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
                pdata[c][i*nstride] = unpack_float_long(blob, 31);
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

                    pdata[c][i*nstride] = twosu32(memle32(blob));
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

                    pdata[c][i*nstride] = twosu32(memle24(blob) << 8);
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

                    pdata[c][i*nstride] = twosu32((unsigned long)memle16(blob) << 16);
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

                    pdata[c][i*nstride] = twosu32((unsigned long)((blob[0] & 0xFFu) ^ 0x80u) << 24);
                }
            }
            break;
        }
    if (pnread)
        *pnread = i;
    return DLB_RIFF_OK;
}

/** Write some audio data to the file. */
static
int
dlb_wave_long_32_write
    (dlb_wave_file     *pwf        /**< wave file open for writing */
    ,const void * const pvdata[]   /**< one pointer per channel */
    ,size_t             ndata      /**< samples per channel to write */
    ,ptrdiff_t          nstride    /**< distance (longs) between samples of one channel */
    )
{
    int status;
    size_t i;
    unsigned channel_count = dlb_wave_get_channel_count(pwf);
    const long * const * const pdata = (const long * const *) pvdata;
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

                pack_long_float(blob, pdata[c][i*nstride], 31);

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
                    unsigned long x = (unsigned long)pdata[c][i*nstride];
                    unsigned char blob[4];

                    le32mem(x & 0xFFFFFFFFu, blob);

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
                    unsigned long x = (unsigned long)pdata[c][i*nstride];
                    unsigned char blob[3];
                    unsigned long valint;
                    unsigned valfrac;

                    valint  = (x >> 8);
                    valfrac = (unsigned)(x & 0xFFu);
                    valint += (valint != 0x7FFFFFu && (valfrac > 0x80u || (valfrac == 0x80u && valint & 0x1u)));

                    le24mem(valint & 0xFFFFFFu, blob);

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
                    unsigned long x = (unsigned long)pdata[c][i*nstride];
                    unsigned char blob[2];
                    unsigned valint;
                    unsigned valfrac;

                    valint  = (unsigned)(x >> 16);
                    valfrac = (unsigned)(x & 0xFFFFu);
                    valint += (valint != 0x7FFFu && (valfrac > 0x8000u || (valfrac == 0x8000u && valint & 0x1u)));

                    le16mem(valint & 0xFFFFu, blob);

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
                    unsigned long x = (unsigned long)pdata[c][i*nstride];
                    unsigned char blob[1];
                    unsigned valint;
                    unsigned long valfrac;

                    valint  = (unsigned)(x >> 24);
                    valfrac = (x & 0xFFFFFFu);
                    valint += (valint != 0x7Fu && (valfrac > 0x800000u || (valfrac == 0x800000u && valint & 0x1u)));

                    blob[0] = (valint & 0xFFu) ^ 0x80u;

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
dlb_wave_int_read
    (dlb_wave_file    *pwf        /**< wave file open for reading */
    ,const dlb_buffer *pbuffer    /**< pre-initialised buffer descriptor */
    ,size_t            ndata      /**< maximum samples per channel to read */
    ,size_t           *pnread     /**< (optional) number of samples actually read */
    )
{
    if (pbuffer->nchannel < dlb_wave_get_channel_count(pwf))
        return DLB_WAVE_E_NCHANNEL;

    switch (pbuffer->data_type)
    {
    case DLB_BUFFER_SHORT_16:
        return dlb_wave_short_16_read(pwf, pbuffer->ppdata, ndata, pbuffer->nstride, pnread);
    case DLB_BUFFER_INT_LEFT:
        return dlb_wave_int_left_read(pwf, pbuffer->ppdata, ndata, pbuffer->nstride, pnread);
    case DLB_BUFFER_LONG_32:
        return dlb_wave_long_32_read(pwf, pbuffer->ppdata, ndata, pbuffer->nstride, pnread);
    default:
        return DLB_WAVE_E_BUFFER;
    }
}

int
dlb_wave_int_write
    (dlb_wave_file    *pwf        /**< wave file open for reading */
    ,const dlb_buffer *pbuffer    /**< pre-initialised buffer descriptor */
    ,size_t            ndata      /**< samples per channel to write */
    )
{
    if (pbuffer->nchannel < dlb_wave_get_channel_count(pwf))
        return DLB_WAVE_E_NCHANNEL;

    switch (pbuffer->data_type)
    {
    case DLB_BUFFER_SHORT_16:
        return dlb_wave_short_16_write(pwf, (const void* const*) pbuffer->ppdata, ndata, pbuffer->nstride);
    case DLB_BUFFER_INT_LEFT:
        return dlb_wave_int_left_write(pwf, (const void* const*) pbuffer->ppdata, ndata, pbuffer->nstride);
    case DLB_BUFFER_LONG_32:
        return dlb_wave_long_32_write(pwf, (const void* const*) pbuffer->ppdata, ndata, pbuffer->nstride);
    default:
        return DLB_WAVE_E_BUFFER;
    }
}
