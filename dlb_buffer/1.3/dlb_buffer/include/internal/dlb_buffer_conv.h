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
 
#ifndef DLB_BUFFER_CONV_H
#define DLB_BUFFER_CONV_H

/* This header file provides the implementation for the dlb_buffer_conv() 
 * function and the dlb_buffer_unit_size() function. All other symbols defined
 * by this file should be considered internal and subject to change.
 * Before including this file, you may choose to define:
 *  DLB_BUFFER_NOFLOAT and/or
 *  DLB_BUFFER_NODOUBLE
 * If these are defined, then this file will not contain any 'float' and/or 
 * 'double' data types. 
 * 
 * dlb_buffer_conv() looks like this:
 * 
 * void
 * dlb_buffer_conv
 *  (const dlb_buffer  *in
 *  ,const dlb_buffer  *out
 *  ,unsigned           count
 *  ,int                hdrm    * The number of headroom bits to add or remove 
 *                              * when converting. This parameter is only valid
 *                              * when either the input or output buffers is an
 *                              * DLB_BUFFER_LFRACT type.
 *                              *
 *                              * When converting DLB_BUFFER_LFRACT to 
 *                              * DLB_BUFFER_LFRACT:
 *                              *   hdrm > 0: Takes DLB_BUFFER_LFRACT with no
 *                              *             headroom as input and adds hdrm
 *                              *             bits.
 *                              *   hdrm < 0: Takes DLB_BUFFER_LFRACT with
 *                              *             headroom as input and removes the
 *                              *             headroom.
 *                              *   hdrm == 0: Simple copy from input to output
 *                              *             buffer with no headroom change.
 *                              *
 *                              *  When converting from DLB_BUFFER_LFRACT to
 *                              *  other buffer types:
 *                              *  hdrm > 0:  lfract has headroom and will be
 *                              *             removed when converting to the
 *                              *             other buffer types.
 *                              *  hdrm == 0: Converting from lfract with no 
 *                              *             headroom.
 *                              *  hdrm < 0:  Invalid.
 *                              *
 *                              *  When converting from other buffer types to 
 *                              *  DLB_BUFFER_LFRACT:
 *                              *    hdrm > 0 : Headroom will be added when 
 *                              *               converting from another buffer
 *                              *               type.
 *                              *    hdrm == 0: Converting to lfract with no 
 *                              *               headroom.
 *                              *    hdrm < 0 : Invalid.
 *  );
 *
 * If you pass invalid parameters, then behaviour is undefined.
 * Not all conversions are supported:
 *          +-------------------------------------------------------------------
 *          | INPUT TYPE
 *          | LFRACT | SHORT_16 | INT_LEFT | LONG_32 | FLOAT | DOUBLE | OCTET(1)
 *          +--------+----------+----------+---------+-------+--------+---------
 * LFRACT   | ok     | ok       | ok       | ok      | ok(2) | ok(3)  | -
 * SHORT_16 | ok     | -        | -        | -       | -     | -      | -
 * INT_LEFT | ok     | -        | -        | -       | -     | -      | -
 * LONG_32  | ok     | -        | -        | -       | -     | -      | -
 * FLOAT    | ok(2)  | -        | -        | -       | -     | -      | -
 * DOUBLE   | ok(3)  | -        | -        | -       | -     | -      | -
 * OCTET(1) | -      | -        | -        | -       | -     | -      | -
 *
 * (1) Both packed and unpacked are unsupported
 * (2) Only if DLB_BUFFER_NOFLOAT was not defined
 * (3) Only if DLB_BUFFER_NODOUBLE was not defined
 *
 * Any attempt to do an invalid conversion will result in undefined behaviour.
 *
 * All memory must be allocated and owned by the caller.
 *
 * The number of channels in the input and output buffers must match exactly,
 * or else we have undefined behaviour.
 *
 *
 * dlb_buffer_unit_size() looks like this:
 *
 *   size_t dlb_buffer_unit_size(int data_type);
 *
 * It takes a DLB_BUFFER_* value, and returns the number of bytes in a single
 * value of that type. Note that the definition of 'byte' is the C definition,
 * which is just the size of a 'char'. This means that DLB_BUFFER_OCTETUNPACKED
 * always has a dlb_buffer_unit_size() of 1 on every platform.
 * The behaviour is undefined if data_type is DLB_BUFFER_PACKED, or if it is
 * an a value which doesn't correspond to a DLB_BUFFER_* constant.
 */

#include <assert.h>
#include "dlb_buffer/include/dlb_buffer.h"
#include "dlb_intrinsics.h"
#include <limits.h>

/* This is a fairly stupid implementation of a compile time log2 function. It
 * only works correctly for unsigned 32-bit numbers. Inputs that are not powers
 * of two will be rounded down. */
#define DLB_BUFFER_INTERNAL_LOG2(x) \
    (  ((x) / 0x2u        == 0) ? 0 \
     : ((x) / 0x4u        == 0) ? 1 \
     : ((x) / 0x8u        == 0) ? 2 \
     : ((x) / 0x10u       == 0) ? 3 \
     : ((x) / 0x20u       == 0) ? 4 \
     : ((x) / 0x40u       == 0) ? 5 \
     : ((x) / 0x80u       == 0) ? 6 \
     : ((x) / 0x100u      == 0) ? 7 \
     : ((x) / 0x200u      == 0) ? 8 \
     : ((x) / 0x400u      == 0) ? 9 \
     : ((x) / 0x800u      == 0) ? 10 \
     : ((x) / 0x1000u     == 0) ? 11 \
     : ((x) / 0x2000u     == 0) ? 12 \
     : ((x) / 0x4000u     == 0) ? 13 \
     : ((x) / 0x8000u     == 0) ? 14 \
     : ((x) / 0x10000u    == 0) ? 15 \
     : ((x) / 0x20000u    == 0) ? 16 \
     : ((x) / 0x40000u    == 0) ? 17 \
     : ((x) / 0x80000u    == 0) ? 18 \
     : ((x) / 0x100000u   == 0) ? 19 \
     : ((x) / 0x200000u   == 0) ? 20 \
     : ((x) / 0x400000u   == 0) ? 21 \
     : ((x) / 0x800000u   == 0) ? 22 \
     : ((x) / 0x1000000u  == 0) ? 23 \
     : ((x) / 0x2000000u  == 0) ? 24 \
     : ((x) / 0x4000000u  == 0) ? 25 \
     : ((x) / 0x8000000u  == 0) ? 26 \
     : ((x) / 0x10000000u == 0) ? 27 \
     : ((x) / 0x20000000u == 0) ? 28 \
     : ((x) / 0x40000000u == 0) ? 29 \
     : ((x) / 0x80000000u == 0) ? 30 \
     :                            31 \
    )

static inline size_t dlb_buffer_unit_size(int data_type)
{
    switch (data_type)
    {
    case DLB_BUFFER_OCTET_UNPACKED:
        return sizeof(char); /* always 1 */
    case DLB_BUFFER_LFRACT:
        return sizeof(DLB_LFRACT);
    case DLB_BUFFER_SHORT_16:
        return sizeof(short);
    case DLB_BUFFER_INT_LEFT:
        return sizeof(int);
    case DLB_BUFFER_LONG_32:
        return sizeof(long);
    case DLB_BUFFER_FLOAT:
        return sizeof(float);
    case DLB_BUFFER_DOUBLE:
         return sizeof(double);
    case DLB_BUFFER_OCTET_PACKED:
    default:
        assert(0);
        return 0;
    }
}

static inline void
dlb_buffer_convert_short16_lfract
    (const dlb_buffer  *in
    ,const dlb_buffer  *out
    ,unsigned           count
    ,unsigned           hdrm
    )
{
    unsigned i, j;
    assert(in->data_type == DLB_BUFFER_SHORT_16);
    assert(out->data_type == DLB_BUFFER_LFRACT);
    
    for (i = 0 ; i < in->nchannel; i++)
    {
        short      *src = (short *)in->ppdata[i];
        DLB_LFRACT *dst = (DLB_LFRACT *)out->ppdata[i];

        for (j = 0; j < count; j++)
        {
            *dst = DLB_L_16U((int16_t)*src, hdrm);
            
            src += in->nstride;
            dst += out->nstride;
        }
    }
}

static inline void
dlb_buffer_convert_intleft_lfract
    (const dlb_buffer  *in
    ,const dlb_buffer  *out
    ,unsigned           count
    ,unsigned           hdrm
    )
{
    unsigned i, j;
    
    assert(in->data_type == DLB_BUFFER_INT_LEFT);
    assert(out->data_type == DLB_BUFFER_LFRACT);
    
    /* dlb_intrinsics doesn't have any support for left justified int values
     * so we need to use its Q.31 int32_t support, and shift the input
     * We should be able to determine the shift amount at compile time based
     * on the difference between an 'int' and an 'int32_t'. */

    /* right shift amount = log2(INT_MIN / -0x40000000) - 1 */
    /*  left shift amount = log2(-0x40000000 / INT_MIN) + 1 */

#if INT_MIN > -0x40000000
     /* Use a left shift */
     #define DLB_BUFFER_INTQx_TO_Q31_LEFT_SHIFT ((DLB_BUFFER_INTERNAL_LOG2(-0x40000000 / INT_MIN)) + 1)
#else
     /* Use a right shift */
     #define DLB_BUFFER_INTQx_TO_Q31_RIGHT_SHIFT ((DLB_BUFFER_INTERNAL_LOG2(INT_MIN / -0x40000000)) - 1)
#endif
    
    for (i = 0; i < in->nchannel; i++)
    {
        int        *src = (int *)in->ppdata[i];
        DLB_LFRACT *dst = (DLB_LFRACT *)out->ppdata[i];

        for (j = 0; j < count; j++)
        {
#ifdef DLB_BUFFER_INTQx_TO_Q31_LEFT_SHIFT
            *dst = DLB_L_32U((int32_t)(*src << DLB_BUFFER_INTQx_TO_Q31_LEFT_SHIFT), hdrm);
#else
            *dst = DLB_L_32U((int32_t)(*src >> DLB_BUFFER_INTQx_TO_Q31_RIGHT_SHIFT), hdrm);
#endif
            
            src += in->nstride;
            dst += out->nstride;
        }
    }
}

static inline void
dlb_buffer_convert_long32_lfract
    (const dlb_buffer  *in
    ,const dlb_buffer  *out
    ,unsigned           count
    ,unsigned           hdrm
    )
{
    unsigned i, j;
    
    assert(in->data_type == DLB_BUFFER_LONG_32);
    assert(out->data_type == DLB_BUFFER_LFRACT);
    
    for (i = 0; i < in->nchannel; i++)
    {
        long       *src = (long *)in->ppdata[i];
        DLB_LFRACT *dst = (DLB_LFRACT *)out->ppdata[i];

        for (j = 0; j < count; j++)
        {
            *dst = DLB_L_32U((int32_t)*src, hdrm);
            
            src += in->nstride;
            dst += out->nstride;
        }
    }
}

#ifndef DLB_BUFFER_NOFLOAT
static inline void
dlb_buffer_convert_float_lfract
    (const dlb_buffer  *in
    ,const dlb_buffer  *out
    ,unsigned           count
    ,unsigned           hdrm
    )
{
    unsigned i, j;
    assert(in->data_type == DLB_BUFFER_FLOAT);
    assert(out->data_type == DLB_BUFFER_LFRACT);
    
    for (i = 0; i < in->nchannel; i++)
    {
        float      *src = (float *)in->ppdata[i];
        DLB_LFRACT *dst = (DLB_LFRACT *)out->ppdata[i];

        for (j = 0; j < count; j++)
        {
            *dst = DLB_L_FU(*src, hdrm);
            
            src += in->nstride;
            dst += out->nstride;
        }
    }
}
#endif
#ifndef DLB_BUFFER_NODOUBLE
static inline void
dlb_buffer_convert_double_lfract
    (const dlb_buffer  *in
    ,const dlb_buffer  *out
    ,unsigned           count
    ,unsigned           hdrm
    )
{
    unsigned i, j;
    assert(in->data_type == DLB_BUFFER_DOUBLE);
    assert(out->data_type == DLB_BUFFER_LFRACT);
    
    for (i = 0; i < in->nchannel; i++)
    {
        double     *src = (double *)in->ppdata[i];
        DLB_LFRACT *dst = (DLB_LFRACT *)out->ppdata[i];

        for (j = 0; j < count; j++)
        {
            *dst = DLB_L_FU(*src, hdrm);
            
            src += in->nstride;
            dst += out->nstride;
        }
    }
}
#endif

static inline void
dlb_buffer_convert_lfract_short16
    (const dlb_buffer  *in
    ,const dlb_buffer  *out
    ,unsigned           count
    ,unsigned           hdrm
    )
{
    unsigned i, j;
    
    assert(in->data_type == DLB_BUFFER_LFRACT);
    assert(out->data_type == DLB_BUFFER_SHORT_16);
    
    for (i = 0 ; i < in->nchannel; i++)
    {
        DLB_LFRACT *src = (DLB_LFRACT *)in->ppdata[i];
        short      *dst = (short *)out->ppdata[i];

        for (j = 0; j < count; j++)
        {
            *dst = (short)DLB_16srndLU(*src, hdrm);
            
            src += in->nstride;
            dst += out->nstride;
        }
    }
}

static inline void
dlb_buffer_convert_lfract_intleft
    (const dlb_buffer  *in
    ,const dlb_buffer  *out
    ,unsigned           count
    ,unsigned           hdrm
    )
{
    unsigned i, j;
    
    assert(in->data_type == DLB_BUFFER_LFRACT);
    assert(out->data_type == DLB_BUFFER_INT_LEFT);
    
    /* dlb_intrinsics doesn't have any support for left justified int values
     * so we need to use its Q.31 int32_t support, and shift the result.
     * We should be able to determine the shift amount at compile time based
     * on the difference between an 'int' and an 'int32_t'. */

    /*  left shift amount = log2(INT_MIN / -0x40000000) - 1 */
    /* right shift amount = log2(-0x40000000 / INT_MIN) + 1 */

#if INT_MIN > -0x40000000
     /* Use a right shift */
     #define DLB_BUFFER_Q31_TO_INTQx_RIGHT_SHIFT ((DLB_BUFFER_INTERNAL_LOG2(-0x40000000 / INT_MIN)) + 1)
#else
     /* Use a left shift */
     #define DLB_BUFFER_Q31_TO_INTQx_LEFT_SHIFT ((DLB_BUFFER_INTERNAL_LOG2(INT_MIN / -0x40000000)) - 1)
#endif
    
    for (i = 0 ; i < in->nchannel; i++)
    {
        DLB_LFRACT *src = (DLB_LFRACT *)in->ppdata[i];
        int        *dst = (int *)out->ppdata[i];

        for (j = 0; j < count; j++)
        {
            int32_t q31 = DLB_32srndLU(*src, hdrm);
#ifdef DLB_BUFFER_Q31_TO_INTQx_LEFT_SHIFT
            *dst = q31 << DLB_BUFFER_Q31_TO_INTQx_LEFT_SHIFT;
#else
            *dst = q31 >> DLB_BUFFER_Q31_TO_INTQx_RIGHT_SHIFT;
#endif
            src += in->nstride;
            dst += out->nstride;
        }
    }
}

static inline void
dlb_buffer_convert_lfract_long32
    (const dlb_buffer  *in
    ,const dlb_buffer  *out
    ,unsigned           count
    ,unsigned           hdrm
    )
{
    unsigned i, j;
    
    assert(in->data_type == DLB_BUFFER_LFRACT);
    assert(out->data_type == DLB_BUFFER_LONG_32);
    
    for (i = 0; i < in->nchannel; i++)
    {
        DLB_LFRACT *src = (DLB_LFRACT *)in->ppdata[i];
        long       *dst = (long *)out->ppdata[i];

        for (j = 0; j < count; j++)
        {
            *dst = (long)DLB_32srndLU(*src, hdrm);
            
            src += in->nstride;
            dst += out->nstride;
        }
    }
}

#ifndef DLB_BUFFER_NOFLOAT
static inline void
dlb_buffer_convert_lfract_float
    (const dlb_buffer  *in
    ,const dlb_buffer  *out
    ,unsigned           count
    ,unsigned           hdrm
    )
{
    unsigned i, j;
    
    assert(in->data_type == DLB_BUFFER_LFRACT);
    assert(out->data_type == DLB_BUFFER_FLOAT);
    
    for (i = 0; i < in->nchannel; i++)
    {
        DLB_LFRACT *src = (DLB_LFRACT *)in->ppdata[i];
        float      *dst = (float *)out->ppdata[i];

        for (j = 0; j < count; j++)
        {
            *dst = (float)DLB_F_LU(*src, hdrm);
            
            src += in->nstride;
            dst += out->nstride;
        }
    }
}
#endif
#ifndef DLB_BUFFER_NODOUBLE
static inline void
dlb_buffer_convert_lfract_double
    (const dlb_buffer  *in
    ,const dlb_buffer  *out
    ,unsigned           count
    ,unsigned           hdrm
    )
{
    unsigned i, j;
    
    assert(in->data_type == DLB_BUFFER_LFRACT);
    assert(out->data_type == DLB_BUFFER_DOUBLE);
    
    for (i = 0; i < in->nchannel; i++)
    {
        DLB_LFRACT *src = (DLB_LFRACT *)in->ppdata[i];
        double     *dst = (double *)out->ppdata[i];

        for (j = 0; j < count; j++)
        {
            *dst = DLB_F_LU(*src, hdrm);
            
            src += in->nstride;
            dst += out->nstride;
        }
    }
}
#endif

static inline void
dlb_buffer_convert_lfract_lfract
    (const dlb_buffer  *in
    ,const dlb_buffer  *out
    ,unsigned           count
    ,int                hdrm
    )
{
    unsigned i, j;
    unsigned headroom = (unsigned)DLB_IabsI(hdrm);
    
    assert(in->data_type == DLB_BUFFER_LFRACT);
    assert(out->data_type == DLB_BUFFER_LFRACT);
    
    if (hdrm > 0) /* From without headroom to with headroom. */
    {
        for (i = 0 ; i < in->nchannel; i++)
        {
            DLB_LFRACT *src = (DLB_LFRACT *)in->ppdata[i];
            DLB_LFRACT *dst = (DLB_LFRACT *)out->ppdata[i];

            for (j = 0; j < count; j++)
            {
                *dst = DLB_LheadLU(*src, headroom);
                
                src += in->nstride;
                dst += out->nstride;
            }
        }
    }
    else /* From with headroom to no headroom. */
    {
        for (i = 0 ; i < in->nchannel; i++)
        {
            DLB_LFRACT *src = (DLB_LFRACT *)in->ppdata[i];
            DLB_LFRACT *dst = (DLB_LFRACT *)out->ppdata[i];

            for (j = 0; j < count; j++)
            {
                *dst = DLB_LleftLU(*src, headroom);
                
                src += in->nstride;
                dst += out->nstride;
            }
        }
    }
}

static inline
void
dlb_buffer_conv
    (const dlb_buffer  *in
    ,const dlb_buffer  *out
    ,unsigned           count
    ,int                hdrm
    )
{
    /* Function does not down or up mix channels */
    assert(in->nchannel == out->nchannel);
    
    switch(in->data_type)
    {
    case DLB_BUFFER_LFRACT:
        {
            switch(out->data_type)
            {
            case DLB_BUFFER_SHORT_16:
                assert(hdrm >= 0);
                dlb_buffer_convert_lfract_short16
                    (in
                    ,out
                    ,count
                    ,(unsigned)hdrm
                    );
                return;
            case DLB_BUFFER_INT_LEFT:
                assert(hdrm >= 0);
                dlb_buffer_convert_lfract_intleft
                    (in
                    ,out
                    ,count
                    ,(unsigned)hdrm
                    );
                return;
            case DLB_BUFFER_LONG_32:
                assert(hdrm >= 0);
                dlb_buffer_convert_lfract_long32
                    (in
                    ,out
                    ,count
                    ,(unsigned)hdrm
                    );
                return;
#ifndef DLB_BUFFER_NOFLOAT
            case DLB_BUFFER_FLOAT:
                assert(hdrm >= 0);
                dlb_buffer_convert_lfract_float
                    (in
                    ,out
                    ,count
                    ,(unsigned)hdrm
                    );
                return;
#endif
#ifndef DLB_BUFFER_NODOUBLE
            case DLB_BUFFER_DOUBLE:
                assert(hdrm >= 0);
                dlb_buffer_convert_lfract_double
                    (in
                    ,out
                    ,count
                    ,(unsigned)hdrm
                    );
                return;
#endif
            case DLB_BUFFER_LFRACT:
                dlb_buffer_convert_lfract_lfract
                    (in
                    ,out
                    ,count
                    ,hdrm
                    );
                return;
            }
            assert(0 && "Conversion not supported");
            return;
        }
        
        case DLB_BUFFER_SHORT_16:
        {
            switch(out->data_type)
            {
            case DLB_BUFFER_LFRACT:
                assert(hdrm >= 0);
                dlb_buffer_convert_short16_lfract
                    (in
                    ,out
                    ,count
                    ,(unsigned)hdrm
                    );
                return;
            }
            assert(0 && "Conversion not supported");
            return;
        }
        case DLB_BUFFER_INT_LEFT:
        {
            switch(out->data_type)
            {
            case DLB_BUFFER_LFRACT:
                assert(hdrm >= 0);
                dlb_buffer_convert_intleft_lfract
                    (in
                    ,out
                    ,count
                    ,(unsigned)hdrm
                    );
                return;
            }
            assert(0 && "Conversion not supported");
            return;
        }
        case DLB_BUFFER_LONG_32:
        {
            switch(out->data_type)
            {
            case DLB_BUFFER_LFRACT:
                assert(hdrm >= 0);
                dlb_buffer_convert_long32_lfract
                    (in
                    ,out
                    ,count
                    ,(unsigned)hdrm
                    );
                return;
            }
            assert(0 && "Conversion not supported");
            return;
        }
#ifndef DLB_BUFFER_NOFLOAT
        case DLB_BUFFER_FLOAT:
        {
            switch(out->data_type)
            {
            case DLB_BUFFER_LFRACT:
                assert(hdrm >= 0);
                dlb_buffer_convert_float_lfract
                    (in
                    ,out
                    ,count
                    ,(unsigned)hdrm
                    );
                return;
            }
            assert(0 && "Conversion not supported");
            return;
        }
#endif
#ifndef DLB_BUFFER_NODOUBLE
        case DLB_BUFFER_DOUBLE:
        {
            switch(out->data_type)
            {
            case DLB_BUFFER_LFRACT:
                assert(hdrm >= 0);
                dlb_buffer_convert_double_lfract
                    (in
                    ,out
                    ,count
                    ,(unsigned)hdrm
                    );
                return;
            }
            assert(0 && "Conversion not supported");
            return;
        }
#endif
    }
    assert(0 && "Conversion not supported");
}

#endif

