/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                  Copyright (C) 2011 by Dolby Laboratories.
 *                            All rights reserved.
 ******************************************************************************/
#ifndef dlb_buffer_convert_H
#define dlb_buffer_convert_H

#include "dlb_buffer/include/dlb_buffer.h"

/** No errors when converting between dlb_buffers. */
#define DBC_OK                         0
/** An invalid argument was passed into dlb_buffer_convert(). */
#define DBC_ERR_INVALID_ARGUMENT       1
/** The conversion requested is not implemeneted yet. */
#define DBC_ERR_UNSUPPORTED_CONVERSION 2

/**
 * Takes an input dlb_buffer and converts to another dlb_buffer format.
 * All memory referenced by the output dlb_buffer should be allocated by
 * the caller.
 */
int /** @returns DBC_OK if conversion was ok, otherwise one of the DBC_ERR_* is returned. */
dlb_buffer_convert
    (const dlb_buffer  *in      /**< Input dlb_buffer to convert from. */
    ,const dlb_buffer  *out     /**< Output dlb_buffer to convert to. */
    ,unsigned           ioffset /**< Number of input buffer elements to offset. */
    ,unsigned           ooffset /**< Number of output buffer elements to offset. */
    ,unsigned           count   /**< Number of elements to convert. */
    ,unsigned           ndims   /**< Number of dimensions for each element. 
                                  *  e.g. audio ndims = 1, For complex data ndims = 2 (Real & Imaginary. 
                                  */
    ,int                hdrm    /**< The number of headroom bits to add or remove when converting.
                                  *  This parameter is only valid when either the input or output
                                  *  buffers is an DLB_BUFFER_LFRACT type. 
                                  *
                                  *  When converting DLB_BUFFER_LFRACT to DLB_BUFFER_LFRACT:
                                  *  hdrm > 0  : Takes DLB_BUFFER_LFRACT with no headroom as input and adds hdrm bits.
                                  *  hdrm < 0  : Takes DLB_BUFFER_LFRACT with headroom as input and removes the headroom.
                                  *  hdrm == 0 : Simple copy from input to output buffer with no headroom change.
                                  *
                                  *  When converting from DLB_BUFFER_LFRACT to other buffer types:
                                  *  hdrm > 0  : lfract has headroom and will be removed
                                  *  when converting to the other buffer types. 
                                  *  hdrm == 0 : Converting from lfract with no headroom.
                                  *  hdrm < 0  : Invalid.
                                  *
                                  *  When converting from other buffer types to DLB_BUFFER_LFRACT:
                                  *  hdrm > 0  : Headroom will be added when converting from another buffer type.
                                  *  hdrm == 0 : Converting to lfract with no headroom.
                                  *  hdrm < 0  : Invalid.
                                  */
    );

#endif
