/************************************************************************
 * dlb_buffer
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
#include "dlb_buffer/include/dlb_buffer.h"
#include "dlb_buffer/include/internal/dlb_buffer_convert.h"
#include "dlb_intrinsics.h"

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


static inline int
dlb_buffer_convert_short16_lfract
    (const dlb_buffer  *in
    ,const dlb_buffer  *out
    ,unsigned           ioffset
    ,unsigned           ooffset
    ,unsigned           count
    ,unsigned           ndims
    ,unsigned           hdrm
    )
{
    unsigned i, j, k;

    for (i = 0 ; i < in->nchannel; i++)
    {
        short      *src = (short *)in->ppdata[i];
        DLB_LFRACT *dst = (DLB_LFRACT *)out->ppdata[i];

        src += ioffset * ndims;
        dst += ooffset * ndims;
        for (j = 0; j < count; j++)
        {
            short      *src_dim = src;
            DLB_LFRACT *dst_dim = dst;

            for (k = 0; k < ndims; k++)
            {
                *dst_dim = DLB_L_16U((int16_t)*src_dim, hdrm);

                dst_dim++;
                src_dim++;
            }

            src += in->nstride;
            dst += out->nstride;
        }
    }

    return DBC_OK;
}

static inline int
dlb_buffer_convert_intleft_lfract
    (const dlb_buffer  *in
    ,const dlb_buffer  *out
    ,unsigned           ioffset
    ,unsigned           ooffset
    ,unsigned           count
    ,unsigned           ndims
    ,unsigned           hdrm
    )
{
    unsigned i, j, k;

#if INT_MIN > -0x40000000
     /* Use a left shift */
     #define DLB_BUFFER_INTQx_TO_Q31_LEFT_SHIFT ((DLB_BUFFER_INTERNAL_LOG2(-0x40000000 / INT_MIN)) + 1)
#else
     /* Use a right shift */
     #define DLB_BUFFER_INTQx_TO_Q31_RIGHT_SHIFT ((DLB_BUFFER_INTERNAL_LOG2(INT_MIN / -0x40000000)) - 1)
#endif
    
    for (i = 0 ; i < in->nchannel; i++)
    {
        int        *src = (int *)in->ppdata[i];
        DLB_LFRACT *dst = (DLB_LFRACT *)out->ppdata[i];

        src += ioffset * ndims;
        dst += ooffset * ndims;
        for (j = 0; j < count; j++)
        {
            int        *src_dim = src;
            DLB_LFRACT *dst_dim = dst;

            for (k = 0; k < ndims; k++)
            {
#ifdef DLB_BUFFER_INTQx_TO_Q31_LEFT_SHIFT
                *dst_dim = DLB_L_32U((int32_t)(*src_dim << DLB_BUFFER_INTQx_TO_Q31_LEFT_SHIFT), hdrm);
#else
                *dst_dim = DLB_L_32U((int32_t)(*src_dim >> DLB_BUFFER_INTQx_TO_Q31_RIGHT_SHIFT), hdrm);
#endif
                dst_dim++;
                src_dim++;
            }

            src += in->nstride;
            dst += out->nstride;
        }
    }

    return DBC_OK;
}

static inline int
dlb_buffer_convert_long32_lfract
    (const dlb_buffer  *in
    ,const dlb_buffer  *out
    ,unsigned           ioffset
    ,unsigned           ooffset
    ,unsigned           count
    ,unsigned           ndims
    ,unsigned           hdrm
    )
{
    unsigned i, j, k;

    for (i = 0 ; i < in->nchannel; i++)
    {
        long       *src = (long *)in->ppdata[i];
        DLB_LFRACT *dst = (DLB_LFRACT *)out->ppdata[i];

        src += ioffset * ndims;
        dst += ooffset * ndims;
        for (j = 0; j < count; j++)
        {
            long       *src_dim = src;
            DLB_LFRACT *dst_dim = dst;

            for (k = 0; k < ndims; k++)
            {
                *dst_dim = DLB_L_32U((int32_t)*src_dim, hdrm);

                dst_dim++;
                src_dim++;
            }

            src += in->nstride;
            dst += out->nstride;
        }
    }

    return DBC_OK;
}

#ifndef DLB_BUFFER_NOFLOAT
static inline int
dlb_buffer_convert_float_lfract
    (const dlb_buffer  *in
    ,const dlb_buffer  *out
    ,unsigned           ioffset
    ,unsigned           ooffset
    ,unsigned           count
    ,unsigned           ndims
    ,unsigned           hdrm
    )
{
    unsigned i, j, k;

    for (i = 0 ; i < in->nchannel; i++)
    {
        float      *src = (float *)in->ppdata[i];
        DLB_LFRACT *dst = (DLB_LFRACT *)out->ppdata[i];

        src += ioffset * ndims;
        dst += ooffset * ndims;
        for (j = 0; j < count; j++)
        {
            float      *src_dim = src;
            DLB_LFRACT *dst_dim = dst;

            for (k = 0; k < ndims; k++)
            {
                *dst_dim = DLB_L_FU(*src_dim, hdrm);

                dst_dim++;
                src_dim++;
            }

            src += in->nstride;
            dst += out->nstride;
        }
    }

    return DBC_OK;
}
#endif

static inline int
dlb_buffer_convert_lfract_short16
    (const dlb_buffer  *in
    ,const dlb_buffer  *out
    ,unsigned           ioffset
    ,unsigned           ooffset
    ,unsigned           count
    ,unsigned           ndims
    ,unsigned           hdrm
    )
{
    unsigned i, j, k;

    for (i = 0 ; i < in->nchannel; i++)
    {
        DLB_LFRACT *src = (DLB_LFRACT *)in->ppdata[i];
        short      *dst = (short *)out->ppdata[i];

        src += ioffset * ndims;
        dst += ooffset * ndims;
        for (j = 0; j < count; j++)
        {
            DLB_LFRACT  *src_dim = src;
            short       *dst_dim = dst;

            for (k = 0; k < ndims; k++)
            {
                *dst_dim = (short)DLB_16srndLU(*src_dim, hdrm);

                dst_dim++;
                src_dim++;
            }

            src += in->nstride;
            dst += out->nstride;
        }
    }

    return DBC_OK;
}

static inline int
dlb_buffer_convert_lfract_intleft
    (const dlb_buffer  *in
    ,const dlb_buffer  *out
    ,unsigned           ioffset
    ,unsigned           ooffset
    ,unsigned           count
    ,unsigned           ndims
    ,unsigned           hdrm
    )
{
    unsigned i, j, k;

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

        src += ioffset * ndims;
        dst += ooffset * ndims;
        for (j = 0; j < count; j++)
        {
            DLB_LFRACT  *src_dim = src;
            int         *dst_dim = dst;

            for (k = 0; k < ndims; k++)
            {
                int32_t q31 = DLB_32srndLU(*src_dim, hdrm);
#ifdef DLB_BUFFER_Q31_TO_INTQx_LEFT_SHIFT
                *dst = q31 << DLB_BUFFER_Q31_TO_INTQx_LEFT_SHIFT;
#else
                *dst = q31 >> DLB_BUFFER_Q31_TO_INTQx_RIGHT_SHIFT;
#endif
                dst_dim++;
                src_dim++;
            }

            src += in->nstride;
            dst += out->nstride;
        }
    }

    return DBC_OK;
}

static inline int
dlb_buffer_convert_lfract_long32
    (const dlb_buffer  *in
    ,const dlb_buffer  *out
    ,unsigned           ioffset
    ,unsigned           ooffset
    ,unsigned           count
    ,unsigned           ndims
    ,unsigned           hdrm
    )
{
    unsigned i, j, k;

    for (i = 0 ; i < in->nchannel; i++)
    {
        DLB_LFRACT *src = (DLB_LFRACT *)in->ppdata[i];
        long       *dst = (long *)out->ppdata[i];

        src += ioffset * ndims;
        dst += ooffset * ndims;
        for (j = 0; j < count; j++)
        {
            DLB_LFRACT  *src_dim = src;
            long        *dst_dim = dst;

            for (k = 0; k < ndims; k++)
            {
                *dst_dim = (long)DLB_32srndLU(*src_dim, hdrm);

                dst_dim++;
                src_dim++;
            }

            src += in->nstride;
            dst += out->nstride;
        }
    }

    return DBC_OK;
}

#ifndef DLB_BUFFER_NOFLOAT
static inline int
dlb_buffer_convert_lfract_float
    (const dlb_buffer  *in
    ,const dlb_buffer  *out
    ,unsigned           ioffset
    ,unsigned           ooffset
    ,unsigned           count
    ,unsigned           ndims
    ,unsigned           hdrm
    )
{
    unsigned i, j, k;

    for (i = 0 ; i < in->nchannel; i++)
    {
        DLB_LFRACT *src = (DLB_LFRACT *)in->ppdata[i];
        float      *dst = (float *)out->ppdata[i];

        src += ioffset * ndims;
        dst += ooffset * ndims;
        for (j = 0; j < count; j++)
        {
            DLB_LFRACT  *src_dim = src;
            float       *dst_dim = dst;

            for (k = 0; k < ndims; k++)
            {
                *dst_dim = (float)DLB_F_LU(*src_dim, hdrm);

                dst_dim++;
                src_dim++;
            }

            src += in->nstride;
            dst += out->nstride;
        }
    }

    return DBC_OK;
}
#endif

static inline int
dlb_buffer_convert_lfract_lfract
    (const dlb_buffer  *in
    ,const dlb_buffer  *out
    ,unsigned           ioffset
    ,unsigned           ooffset
    ,unsigned           count
    ,unsigned           ndims
    ,int                hdrm
    )
{
    unsigned i, j, k;
    unsigned headroom = (unsigned)DLB_IabsI(hdrm);

    if (hdrm > 0) /* From without headroom to with headroom. */
    {
        for (i = 0 ; i < in->nchannel; i++)
        {
            DLB_LFRACT *src = (DLB_LFRACT *)in->ppdata[i];
            DLB_LFRACT *dst = (DLB_LFRACT *)out->ppdata[i];

            src += ioffset * ndims;
            dst += ooffset * ndims;
            for (j = 0; j < count; j++)
            {
                DLB_LFRACT  *src_dim = src;
                DLB_LFRACT  *dst_dim = dst;

                for (k = 0; k < ndims; k++)
                {
                    *dst_dim = DLB_LheadLU(*src_dim, headroom);

                    dst_dim++;
                    src_dim++;
                }

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

            src += ioffset * ndims;
            dst += ooffset * ndims;
            for (j = 0; j < count; j++)
            {
                DLB_LFRACT  *src_dim = src;
                DLB_LFRACT  *dst_dim = dst;

                for (k = 0; k < ndims; k++)
                {
                    *dst_dim = DLB_LleftLU(*src_dim, headroom);

                    dst_dim++;
                    src_dim++;
                }

                src += in->nstride;
                dst += out->nstride;
            }
        }
    }

    return DBC_OK;
}

int
dlb_buffer_convert
    (const dlb_buffer  *in
    ,const dlb_buffer  *out
    ,unsigned           ioffset
    ,unsigned           ooffset
    ,unsigned           count
    ,unsigned           ndims
    ,int                hdrm
    )
{
    /* Function does not down or up mix channels */
    if (in->nchannel != out->nchannel)
    {
        return DBC_ERR_INVALID_ARGUMENT;
    }

    switch(in->data_type)
    {
    case DLB_BUFFER_LFRACT:
    {
        switch(out->data_type)
        {
        case DLB_BUFFER_SHORT_16:
        {
            return
                dlb_buffer_convert_lfract_short16
                ( in
                , out
                , ioffset
                , ooffset
                , count
                , ndims
                , (unsigned)hdrm
                );
        }
        case DLB_BUFFER_INT_LEFT:
        {
            return
                dlb_buffer_convert_lfract_intleft
                ( in
                , out
                , ioffset
                , ooffset
                , count
                , ndims
                , (unsigned)hdrm
                );
        }
        case DLB_BUFFER_LONG_32:
        {
            return
                dlb_buffer_convert_lfract_long32
                ( in
                , out
                , ioffset
                , ooffset
                , count
                , ndims
                , (unsigned)hdrm
                );
        }
#ifndef DLB_BUFFER_NOFLOAT
        case DLB_BUFFER_FLOAT:
        {
            return
                dlb_buffer_convert_lfract_float
                ( in
                , out
                , ioffset
                , ooffset
                , count
                , ndims
                , (unsigned)hdrm
                );
        }
#endif
        case DLB_BUFFER_LFRACT:
        {
            return
                dlb_buffer_convert_lfract_lfract
                ( in
                , out
                , ioffset
                , ooffset
                , count
                , ndims
                , hdrm
                );
        }
        default:
        {
            return DBC_ERR_UNSUPPORTED_CONVERSION;
        }
        }

        break;
    }

    case DLB_BUFFER_SHORT_16:
    {
        switch(out->data_type)
        {
        case DLB_BUFFER_LFRACT:
        {
            return
                dlb_buffer_convert_short16_lfract
                ( in
                , out
                , ioffset
                , ooffset
                , count
                , ndims
                , (unsigned)hdrm
                );
        }
        default:
            return DBC_ERR_UNSUPPORTED_CONVERSION;
        }

        break;
    }

    case DLB_BUFFER_INT_LEFT:
    {
        switch(out->data_type)
        {
        case DLB_BUFFER_LFRACT:
        {
            return
                dlb_buffer_convert_intleft_lfract
                ( in
                , out
                , ioffset
                , ooffset
                , count
                , ndims
                , (unsigned)hdrm
                );
        }
        default:
            return DBC_ERR_UNSUPPORTED_CONVERSION;
        }

        break;
    }

    case DLB_BUFFER_LONG_32:
    {
        switch(out->data_type)
        {
        case DLB_BUFFER_LFRACT:
        {
            return
                dlb_buffer_convert_long32_lfract
                ( in
                , out
                , ioffset
                , ooffset
                , count
                , ndims
                , (unsigned)hdrm
                );
        }
        default:
            return DBC_ERR_UNSUPPORTED_CONVERSION;
        }

        break;
    }
#ifndef DLB_BUFFER_NOFLOAT
    case DLB_BUFFER_FLOAT:
    {
        switch(out->data_type)
        {
        case DLB_BUFFER_LFRACT:
        {
            return
                dlb_buffer_convert_float_lfract
                ( in
                , out
                , ioffset
                , ooffset
                , count
                , ndims
                ,(unsigned) hdrm
                );
        }
        default:
            return DBC_ERR_UNSUPPORTED_CONVERSION;
        }

        break;
    }
#endif
    default:
    {
        return DBC_ERR_UNSUPPORTED_CONVERSION;
    }
    }

    return DBC_OK;
}
