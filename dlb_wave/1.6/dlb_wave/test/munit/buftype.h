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
