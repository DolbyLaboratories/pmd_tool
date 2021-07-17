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

#ifndef BUFTYPE_UTIL_H
#define BUFTYPE_UTIL_H

#include "buftype.h"

typedef struct {
    long     error;
    unsigned mant_bits;
    int      b_was_float;
} compare_result;

compare_result compare_sample(const buffer_type *bt1, const void *data1, const buffer_type *bt2, const void *data2);

compare_result compare_sample_limited(const buffer_type *bt1, const void *data1, const buffer_type *bt2, const void *data2, unsigned max_mant_bits);

void convert_sample(const buffer_type *dest_buftype, void *dest, const buffer_type *src_buftype, const void *src);

#endif /* BUFTYPE_UTIL_H */
