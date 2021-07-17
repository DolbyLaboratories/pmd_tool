/**
 * @brief  Extension to dlb_wave for reading/writing arrays of standard integer
 *         types.
 * @note   Confidential Information - Limited distribution to authorized persons
 *         only. This material is protected under international copyright laws
 *         as an unpublished work. Do not copy.
 *         Copyright (C) 2011 Dolby Laboratories Inc. All rights reserved.
 *         
 */
#ifndef dlb_wave_int_H
#define dlb_wave_int_H

#include "dlb_wave.h"
#include "dlb_buffer/include/dlb_buffer.h"

/******************************************************************************
functions for reading and writing short, int and long data.
This works for dlb_buffers of the following types:
- DLB_BUFFER_SHORT_16
- DLB_BUFFER_INT_LEFT
- DLB_BUFFER_LONG_32
For other dlb_buffer formats, DLB_WAVE_E_BUFFER is returned.

If you need support for DLB_BUFFER_FLOAT and DLB_BUFFER_DOUBLE, you need to
call the dlb_wave_float_* functions instead.

If you need support for DLB_BUFFER_LFRACT, you need to call the
dlb_wave_lfract_* functions from the dlb_wave_intrinsics component.
******************************************************************************/
int
dlb_wave_int_read
    (dlb_wave_file    *pwf        /**< wave file open for reading */
    ,const dlb_buffer *pbuffer    /**< pre-initialised buffer descriptor */
    ,size_t            ndata      /**< maximum samples per channel to read */
    ,size_t           *pnread     /**< (optional) number of samples actually read */
    );

int
dlb_wave_int_write
    (dlb_wave_file    *pwf        /**< wave file open for reading */
    ,const dlb_buffer *pbuffer    /**< pre-initialised buffer descriptor */
    ,size_t            ndata      /**< samples per channel to write */
    );

#endif
