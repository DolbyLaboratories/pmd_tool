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
#ifndef dlb_wave_float_H
#define dlb_wave_float_H

#include "dlb_wave.h"
#include "dlb_buffer/include/dlb_buffer.h"

/******************************************************************************
functions for reading and writing short, int and long data.
This works for dlb_buffers of the following types:
- DLB_BUFFER_SHORT_16
- DLB_BUFFER_INT_LEFT
- DLB_BUFFER_LONG_32
- DLB_BUFFER_FLOAT
- DLB_BUFFER_DOUBLE
For other dlb_buffer formats, DLB_WAVE_E_BUFFER is returned.

If you need support for DLB_BUFFER_LFRACT, you need to call the
dlb_wave_lfract_* functions from the dlb_wave_intrinsics component.
******************************************************************************/
int
dlb_wave_float_read
    (dlb_wave_file    *pwf        /**< wave file open for reading */
    ,const dlb_buffer *pbuffer    /**< pre-initialised buffer descriptor */
    ,size_t            ndata      /**< maximum samples per channel to read */
    ,size_t           *pnread     /**< (optional) number of samples actually read */
    );

int
dlb_wave_float_write
    (dlb_wave_file    *pwf        /**< wave file open for reading */
    ,const dlb_buffer *pbuffer    /**< pre-initialised buffer descriptor */
    ,size_t            ndata      /**< samples per channel to write */
    );

#endif
