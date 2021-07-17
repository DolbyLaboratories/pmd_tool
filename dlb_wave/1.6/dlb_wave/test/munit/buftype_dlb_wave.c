/******************************************************************************  
 * This program is protected under international and U.S. copyright laws as  
 * an unpublished work. This program is confidential and proprietary to the  
 * copyright owners. Reproduction or disclosure, in whole or in part, or the  
 * production of derivative works therefrom without the express permission of  
 * the copyright owners is prohibited.  
 *  
 *                  Copyright (C) 2015 by Dolby Laboratories.  
 *                            All rights reserved.  
 ******************************************************************************/

#include "buftype_dlb_wave.h"
#include <assert.h>
#include <stdlib.h>

static long fixed_wave_buffer_read_long(const buffer_type *bt, const void *buf)
{
    unsigned long val_max;
    unsigned long val;
    unsigned scale_down;

    assert(bt->nb_octets <= 4u && bt->nb_octets >= 1u);
    assert(bt->nb_mant_bits <= 31u && bt->nb_mant_bits >= 1u);

    val_max    = (1ul << bt->nb_mant_bits) - 1ul;
    scale_down = 31 - bt->nb_mant_bits;

    if (bt->nb_octets == 1)
    {
        val = ((unsigned long)(((unsigned char *)buf)[0] ^ 0x80u)) << 24;
    }
    else
    {
        unsigned i;
        for (i = 0, val = 0; i < bt->nb_octets; i++)
        {
            val = (val >> 8) | (((unsigned long)(((unsigned char *)buf)[i])) << 24);
        }
    }

    val >>= scale_down;

    return (val >> bt->nb_mant_bits) ? (-(long)((~val) & val_max) - 1l) : (long)val;
}

static double fixed_wave_buffer_read_float(const buffer_type *bt, const void *buf)
{
    return (fixed_wave_buffer_read_long(bt, buf) / (double)(1ul << bt->nb_mant_bits));
}

static double float_wave_buffer_read_float(const buffer_type *bt, const void *buf)
{
    const unsigned char *blob = (const unsigned char *)buf;

    unsigned long val32 =
        ((unsigned long)(blob[0] & 0xFFu)) |
        (((unsigned long)(blob[1] & 0xFFu)) << 8) |
        (((unsigned long)(blob[2] & 0xFFu)) << 16) |
        (((unsigned long)(blob[3] & 0xFFu)) << 24);

    int exponent = (int)((val32 >> 23) & 0xFFu);
    unsigned long mantissa = 0x800000u | (val32 & 0x7FFFFFu);
    int sign = val32 & 0x80000000u;
    double val;

    (void)bt;

    if (!exponent)
    {
        /* +0, -0 or denormal produces 0 */
        return (sign) ? -0.0 : 0.0;
    }

    if (exponent == 0xFFu)
    {
        if (mantissa == 0x800000u)
        {
            /* +infinity produces largest positive number, -infinity produces smallest negative number */
            return (sign) ? -1.0 : 1.0;
        }

        /* NaNs produce 0 */
        return 0;
    }

    val = (double)mantissa;

    exponent = exponent - 127 - 23;

    while (exponent > 0)
    {
        val *= 2.0;
        exponent--;
    }
    while (exponent < 0)
    {
        val *= 0.5;
        exponent++;
    }

    val = (val > 1.0) ? 1.0 : val;
    val = (sign) ? -val : val;

    return val;
}

static long float_wave_buffer_read_long(const buffer_type *bt, const void *buf)
{
    double v = float_wave_buffer_read_float(bt, buf) * (double)0x1000000u;
    v = (v < 0.0) ? (long)(v - 0.5) : (long)(v + 0.5);
    return (long)((v < -(double)0x1000000l) ? -0x1000000l : ((v > 0xFFFFFFu) ? 0xFFFFFFl : (long)v));
}

const char *get_dlb_wave_buffer_type(buffer_type *bt, const dlb_wave_format *p_fmt, unsigned format_flags)
{
    bt->long_to_native = NULL;
    bt->double_to_native = NULL;

    if (format_flags & DLB_WAVE_FLOAT)
    {
        bt->b_native_float = 1;

        if (p_fmt->bits_per_sample == 32 && p_fmt->octets_per_sample == 4)
        {
            bt->nb_octets        = 4;
            bt->nb_mant_bits     = 24;
            bt->native_to_long   = float_wave_buffer_read_long;
            bt->native_to_double = float_wave_buffer_read_float;
        }
        else
        {
            return "Cannot understand wave format.";
        }
    }
    else
    {
        if (p_fmt->valid_bits_per_sample > p_fmt->bits_per_sample)
        {
            return "Format has more valid bits per sample than bits available.";
        }

        if (p_fmt->octets_per_sample < 1 || p_fmt->octets_per_sample > 4 || (p_fmt->bits_per_sample != p_fmt->octets_per_sample * 8))
        {
            return "Format has invalid bits per sample member.";
        }

        bt->b_native_float   = 0;
        bt->nb_mant_bits     = p_fmt->valid_bits_per_sample - 1;
        bt->nb_octets        = p_fmt->octets_per_sample;
        bt->native_to_long   = fixed_wave_buffer_read_long;
        bt->native_to_double = fixed_wave_buffer_read_float;
    }

    return NULL;
}
