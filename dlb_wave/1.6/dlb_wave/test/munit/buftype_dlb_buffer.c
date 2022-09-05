/************************************************************************
 * dlb_wave
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

#include "buftype_dlb_buffer.h"
#include <stdlib.h>
#include <limits.h>
#include <assert.h>

static long short16_as_long(const buffer_type *bt, const void *buf)
{
    (void)bt;
    return *((short *)buf);
}

static void short16_from_long(const buffer_type *bt, void *buf, long v)
{
    (void)bt;
    assert(v >= -32768l && v <= 32767l);
    *((short *)buf) = (short)v;
}

static long intleft_as_long(const buffer_type *bt, const void *buf)
{
    (void)bt;
    return *((int *)buf);
}

static void intleft_from_long(const buffer_type *bt, void *buf, long v)
{
    (void)bt;
    assert(v >= INT_MIN && v <= INT_MAX);
    *((int *)buf) = (int)v;
}

static long long32_as_long(const buffer_type *bt, const void *buf)
{
    (void)bt;
    return *((long *)buf);
}

static void long32_from_long(const buffer_type *bt, void *buf, long v)
{
    (void)bt;
    assert(v >= (-2147483647l - 1l) && v <= 2147483647l);
    *((long *)buf) = v;
}

static double float_as_float(const buffer_type *bt, const void *buf)
{
    (void)bt;
    return *((float *)buf);
}

static void float_from_float(const buffer_type *bt, void *buf, double f)
{
    (void)bt;
    *((float *)buf) = (float)f;
}

static double double_as_float(const buffer_type *bt, const void *buf)
{
    (void)bt;
    return *((double *)buf);
}

static void double_from_float(const buffer_type *bt, void *buf, double f)
{
    (void)bt;
    *((double *)buf) = f;
}

static long roundclip_double(double f, unsigned mantbits)
{
    f = f * (1l << mantbits) + ((f >= 0.0) ? 0.5 : -0.5);
    if (f >= (double)(1l << mantbits))
    {
        return (1l << mantbits);
    }
    else if (f <= -(double)(1l << mantbits))
    {
        return -(1l << mantbits);
    }
    else
    {
        return (long)f;
    }
}

static long floattype_as_long(const buffer_type *bt, const void *buf)
{
    return roundclip_double(bt->native_to_double(bt, buf), bt->nb_mant_bits);
}

static void inttype_from_float(const buffer_type *bt, void *buf, double f)
{
    long max = (long)((1ul << bt->nb_mant_bits) - 1ul);
    long min = -max - 1l;
    long l;

    f = -f * min;
    f = (f < 0.0) ? (f - 0.5) : (f + 0.5);

    if (f <= (double)min)
    {
        l = min;
    }
    else if (f >= (double)max)
    {
        l = max;
    }
    else
    {
        l = (long)f;
        assert(l >= min && l <= max);
    }

    bt->long_to_native(bt, buf, l);
}

static double inttype_as_float(const buffer_type *bt, const void *buf)
{
    return bt->native_to_long(bt, buf) * (1.0 / (1ul << bt->nb_mant_bits));
}

static unsigned nb_bits(unsigned value)
{
    unsigned bits;
    for (bits = 0; (value); bits++, value >>= 1);
    return bits;
}

void get_dlb_buffer_type(buffer_type *bt, int dlb_buffer_type)
{
    switch (dlb_buffer_type)
    {
    case DLB_BUFFER_SHORT_16:
        bt->nb_mant_bits = 15;
        bt->nb_octets = sizeof(short);
        bt->b_native_float = 0;
        bt->double_to_native = inttype_from_float;
        bt->long_to_native = short16_from_long;
        bt->native_to_double = inttype_as_float;
        bt->native_to_long = short16_as_long;
        break;
    case DLB_BUFFER_INT_LEFT:
        bt->nb_mant_bits = nb_bits((unsigned)INT_MAX);
        bt->nb_octets = sizeof(int);
        bt->b_native_float = 0;
        bt->double_to_native = inttype_from_float;
        bt->long_to_native = intleft_from_long;
        bt->native_to_double = inttype_as_float;
        bt->native_to_long = intleft_as_long;
        break;
    case DLB_BUFFER_LONG_32:
        bt->nb_mant_bits = 31;
        bt->nb_octets = sizeof(long);
        bt->b_native_float = 0;
        bt->double_to_native = inttype_from_float;
        bt->long_to_native = long32_from_long;
        bt->native_to_double = inttype_as_float;
        bt->native_to_long = long32_as_long;
        break;
    case DLB_BUFFER_FLOAT:
        bt->nb_mant_bits = 24;
        bt->nb_octets = sizeof(float);
        bt->b_native_float = 1;
        bt->double_to_native = float_from_float;
        bt->long_to_native = NULL;
        bt->native_to_double = float_as_float;
        bt->native_to_long = floattype_as_long;
        break;
    case DLB_BUFFER_DOUBLE:
        bt->nb_mant_bits = 31; /* Really it's 53 - but we won't assume long can handle that. */
        bt->nb_octets = sizeof(double);
        bt->b_native_float = 1;
        bt->double_to_native = double_from_float;
        bt->long_to_native = NULL;
        bt->native_to_double = double_as_float;
        bt->native_to_long = floattype_as_long;
        break;
    default:
        abort();
    }
}
