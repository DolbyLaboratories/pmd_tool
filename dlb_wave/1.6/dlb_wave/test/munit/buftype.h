/************************************************************************
 * dlb_wave
 * Copyright (c) 2020, Dolby Laboratories Inc.
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

#ifndef BUFTYPE_H
#define BUFTYPE_H

typedef struct buffer_type_s buffer_type;

struct buffer_type_s {
    /* Number of bits used to represent the fractional component of the
     * output. */
    unsigned   nb_mant_bits;

    /* Number of chars which are used to store the value. After using one of
     * the conversion functions, the buffer position can be incremented by
     * this many characters to move to the next element. */
    unsigned   nb_octets;

    /* Non-zero if the most-native format of this buffer is floating point. */
    int        b_native_float;

    /* Conversion functions to the underlying type. Floating point values
     * should be in the range -1.0 to 1.0. Long values should be in the range
     *   -(2^nb_mant_bits) <= l < 2^nb_mant_bits.
     * Values outside of this range produce undefined behavior. */
    void     (*double_to_native)(const buffer_type *bt, void *buf, double f);
    void     (*long_to_native)(const buffer_type *bt, void *buf, long l);

    /* Conversion functions from the underlying type. */
    double   (*native_to_double)(const buffer_type *bt, const void *buf);
    long     (*native_to_long)(const buffer_type *bt, const void *buf);
};

#endif /* BUFTYPE_H */
