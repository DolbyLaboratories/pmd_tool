/************************************************************************
 * dlb_buffer
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
