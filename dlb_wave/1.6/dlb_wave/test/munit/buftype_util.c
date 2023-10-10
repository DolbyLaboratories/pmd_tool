/************************************************************************
 * dlb_wave
 * Copyright (c) 2023, Dolby Laboratories Inc.
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

#include "buftype_util.h"
#include <limits.h>
#include <assert.h>

static long scale_down_long_round(long value, unsigned scale)
{
    long ov = value;

    if (scale)
    {
        /* Has a single bit set for where the new sign bit is. */
        const unsigned long uval            = (unsigned long)ov;
        const unsigned long uval_sign_bit   = (ULONG_MAX >> 1) + 1ul;
        const unsigned long valint_sign_bit = uval_sign_bit >> scale;
        const unsigned long valint_max      = valint_sign_bit - 1ul;
        const unsigned long valfrac_half    = 1ul << (scale - 1);
        const unsigned long frac_mask       = (valfrac_half << 1) - 1ul;
        const unsigned long valfrac         = uval & frac_mask;
        unsigned long valint                = uval >> scale;

        /* Round to nearest even. */
        if ((valint != valint_max) && ((valfrac > valfrac_half) || ((valfrac == valfrac_half) && (valint & 1u))))
        {
            valint++;
        }

        if (valint & valint_sign_bit)
        {
            ov = -(long)(~valint & valint_max) - 1l;
        }
        else
        {
            /* We need to perform the mask as the round operation may have set
             * a bit beyond the sign bit (rounding from a -1 to 0). */
            ov = valint & valint_max;
        }

        assert(ov >= 0 || value < 0);
        assert(ov <= 0 || value > 0);
    }

    return ov;
}

static long saturate_sub(long a, long b, long vmin, long vmax)
{
    assert(vmin < 0 && vmax > 0);

    if ((a > 0 && b < a - vmax) || (b < 0 && a > vmax + b))
    {
        return vmax;
    }

    if ((a < 0 && b > a - vmin) || (b > 0 && a < vmin + b))
    {
        return vmin;
    }

    return a - b;
}

compare_result compare_sample_limited(const buffer_type *bt1, const void *data1, const buffer_type *bt2, const void *data2, unsigned max_mant_bits)
{
    const unsigned least_in_mant_bits = (bt1->nb_mant_bits < bt2->nb_mant_bits) ? bt1->nb_mant_bits : bt2->nb_mant_bits;
    const unsigned least_mant_bits = (max_mant_bits < least_in_mant_bits) ? max_mant_bits : least_in_mant_bits;
    const long max_val = (long)((1ul << least_mant_bits) - 1u);
    const long min_val = -max_val - 1;
    compare_result result;

    result.mant_bits = least_mant_bits;
    result.b_was_float = bt1->b_native_float || bt2->b_native_float;

    if (result.b_was_float)
    {
        /* Compare as floats. */
        double err = -(bt1->native_to_double(bt1, data1) - bt2->native_to_double(bt2, data2)) * min_val;

        if (err >= max_val)
        {
            result.error = max_val;
        }
        else if (err <= min_val)
        {
            result.error = min_val;
        }
        else
        {
            result.error = (long)err;
        }
    }
    else
    {
        /* Compare as longs. */
        long v1 = scale_down_long_round(bt1->native_to_long(bt1, data1), bt1->nb_mant_bits - least_mant_bits);
        long v2 = scale_down_long_round(bt2->native_to_long(bt2, data2), bt2->nb_mant_bits - least_mant_bits);

        v1 = (v1 > max_val) ? max_val : v1;
        v2 = (v2 > max_val) ? max_val : v2;
        v1 = (v1 < min_val) ? min_val : v1;
        v2 = (v2 < min_val) ? min_val : v2;

        result.error = saturate_sub(v1, v2, min_val, max_val);
    }

    return result;
}

compare_result compare_sample(const buffer_type *bt1, const void *data1, const buffer_type *bt2, const void *data2)
{
    return compare_sample_limited(bt1, data1, bt2, data2, UINT_MAX);
}

void convert_sample(const buffer_type *dest_buftype, void *dest, const buffer_type *src_buftype, const void *src)
{
    if (src_buftype->b_native_float || dest_buftype->b_native_float)
    {
        dest_buftype->double_to_native(dest_buftype, dest, src_buftype->native_to_double(src_buftype, src));
    }
    else
    {
        long val = src_buftype->native_to_long(src_buftype, src);
        int scale = dest_buftype->nb_mant_bits - (int)src_buftype->nb_mant_bits;
        long max_val = (long)((1ul << dest_buftype->nb_mant_bits) - 1l);
        long min_val = -max_val - 1l;

        assert(scale < 31);

        if (scale <= 0)
        {
            val = scale_down_long_round(val, (unsigned)-scale);
        }
        else
        {
            val = val * (1l << scale);
        }

        val = (val > max_val) ? max_val : val;
        val = (val < min_val) ? min_val : val;

        dest_buftype->long_to_native(dest_buftype, dest, val);
    }
}
