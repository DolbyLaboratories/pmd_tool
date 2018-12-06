/************************************************************************
 * dlb_pmd
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

/**
 * @flle pmd_smpte_337m.c
 * @brief KLV SMPTE 337m wrapper/unwrapper code
 */

#ifndef _MSC_VER
#  include <stdint.h>
#else
#  define inline __inline
__pragma(warning(disable:4204))
#endif
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include "pmd_smpte_337m.h"


/**
 * @brief SMPTE 337m sync words
 *
 * KLV uses 20-bit SMPTE 337m
 */
enum preamble_words
{
    PA          = 0x6f872000,   /* IEC 958 preamble a (sync word 1) */
    PB          = 0x54e1f000,   /* IEC 958 preamble b (sync word 2) */
    PC_MASK     = 0x007f0000,   /* SMPTE preamble C data_mode & data_type mask */
    PC          = 0x003b0000,   /* preamble C (stream 0, 20-bit, KLV), bit 24 'key_flag' set */
    PC_KEY_FLAG = 0x01000000,   /* KLV 'key_flag' to indicate presence of Universal Key */
    PC_NULL     = 0x00000000,   /* preamble C (20-bit, NULL) */

    PREAMBLE_SAMPLE_PAIRS = 2,
    PREAMBLE_SAMPLES = 4,
    BYTES_PER_PAIR = 5,

    WORD_BITS = 20,
    PAIR_BITS = 2 * WORD_BITS,
};


#define ROUND_UP(size, rounding) ((((size) + (rounding) - 1)/(rounding))*(rounding))


/* -------------------- write interface functions ------------------------ */

static inline
uint32_t *                    /** @return next PCM sample to write */
phase_write_padding
    (pmd_s337m *s337m         /**< [in] PCM smpte state */
    ,uint32_t *pcm            /**< [in/out] PCM channel buffer to modify */
    ,uint32_t *end            /**< [in] 1st sample after end of block */
    )
{
    size_t padding = s337m->padding;
    size_t stride = s337m->stride;
    size_t vo = s337m->vsync_offset;
    
    while (pcm < end && padding)
    {
        pcm[0] = 0;
        if (s337m->pair)
        {
            pcm[1] = 0;
        }
        pcm += stride;
        --padding;
        --vo;
    }
    s337m->padding = padding;
    s337m->vsync_offset = vo;

    if (!padding)
    {
        if (s337m->next(s337m))
        {
            s337m->phase = S337M_PHASE_PREAMBLEA;
        }
        else
        {
            s337m->phase = S337M_PHASE_VSYNC;
        }
        s337m->isodd = 0;
    }
    return pcm;
}


static inline
uint32_t *                    /** @return next PCM sample to write */
phase_write_wait_vsync
    (pmd_s337m *s337m         /**< [in] PCM smpte state */
    ,uint32_t *pcm            /**< [in/out] PCM channel buffer to modify */
    ,uint32_t *end            /**< [in] 1st sample after end of block */
    )
{
    size_t stride = s337m->stride;
    size_t vo = s337m->vsync_offset;

    while (pcm < end && vo)
    {
        pcm[0] = 0;
        if (s337m->pair)
        {
            pcm[1] = 0;
        }
        pcm += stride;
        --vo;
    }
    s337m->vsync_offset = vo;

    if (!vo)
    {
        s337m->padding = GUARDBAND;
        s337m->phase = S337M_PHASE_GUARDBAND;
    }

    return pcm;
}


static inline
uint32_t *                    /** @return next PCM sample to write */
phase_write_guardband
    (pmd_s337m *s337m         /**< [in] PCM smpte state */
    ,uint32_t *pcm            /**< [in/out] PCM channel buffer to modify */
    ,uint32_t *end            /**< [in] 1st sample after end of block */
    )
{
    size_t stride = s337m->stride;
    size_t gband = s337m->padding;

    while (pcm < end && gband)
    {
        pcm[0] = 0;
        if (s337m->pair)
        {
            pcm[1] = 0;
        }
        pcm += stride;
        --gband;
    }

    s337m->padding = gband;
    if (!gband)
    {
        s337m->phase = S337M_PHASE_PREAMBLEA;
    }

    return pcm;
}


static inline
uint32_t *                    /** @return next PCM sample to write */
phase_write_preamble_pd
    (pmd_s337m *s337m         /**< [in] PCM augmmentor */
    ,uint32_t *pcm            /**< [in/out] PCM channel buffer to modify */
    ,uint32_t *end            /**< [in] 1st sample after end of block */
    )
{
    unsigned int bits_per_sample = s337m->pair ? PAIR_BITS : WORD_BITS;
    unsigned int preamble_samples = s337m->pair ? PREAMBLE_SAMPLE_PAIRS : PREAMBLE_SAMPLES;

    assert(pcm < end);
    (void)end;
    
    if (s337m->databits)
    {
        pcm[0] = (uint32_t)(s337m->databits << 12);
        s337m->phase = S337M_PHASE_DATA;
        s337m->databits = ROUND_UP(s337m->databits, bits_per_sample);
        s337m->padding = s337m->framelen - preamble_samples - (s337m->databits / bits_per_sample);
    }
    else
    {
        pcm[0] = 0;
        s337m->phase = S337M_PHASE_PADDING;
        s337m->padding = s337m->framelen - preamble_samples;
    }

    s337m->vsync_offset -= 1;
    return pcm + s337m->stride;
}


static inline
uint32_t *                    /** @return next PCM sample to write */
phase_write_preamble_pc
    (pmd_s337m *s337m         /**< [in] PCM augmmentor */
    ,uint32_t *pcm            /**< [in/out] PCM channel buffer to modify */
    ,uint32_t *end            /**< [in] 1st sample after end of block */
    )
{
    assert(pcm < end);

    (void)end;
    
    s337m->phase = S337M_PHASE_PREAMBLED;
    if (s337m->databits)
    {
        pcm[0] = PC | PC_KEY_FLAG;  
    }
    else if (s337m->mark_empty)
    {
        pcm[0] = PC_NULL; /* NULL databurst */
    }
    else
    {
        pcm[0] = 0;       /* silence */
    }
    if (s337m->pair)
    {
        return phase_write_preamble_pd(s337m, pcm+1, end) - 1;
    }
    s337m->vsync_offset -= 1;
    return pcm + s337m->stride;
}


static inline
uint32_t*
phase_write_preamble_pb
    (pmd_s337m *s337m       /**< [in] PCM smpte state */
    ,uint32_t *pcm          /**< [in/out] PCM channel buffer to modify */
    ,uint32_t *end          /**< [in] 1st sample after end of block */
    )
{
    assert(pcm < end);

    (void)end;

    if (s337m->databits || s337m->mark_empty)
    {
        pcm[0] = PB;
    }
    else
    {
        pcm[0] = 0;
    }
    s337m->phase = S337M_PHASE_PREAMBLEC;
    s337m->vsync_offset -= 1;
    return pcm + s337m->stride;
}


static inline
uint32_t*
phase_write_preamble_pa
    (pmd_s337m *s337m       /**< [in] PCM smpte state */
    ,uint32_t *pcm          /**< [in/out] PCM channel buffer to modify */
    ,uint32_t *end          /**< [in] 1st sample after end of block */
    )
{
    assert(pcm < end);

    (void)end;

    if (s337m->databits || s337m->mark_empty)
    {
        pcm[0] = PA;
    }
    else
    {
        pcm[0] = 0;
    }
    
    s337m->phase = S337M_PHASE_PREAMBLEB;
    if (s337m->pair)
    {
        return phase_write_preamble_pb(s337m, pcm+1, end)-1;
    }
    s337m->vsync_offset -= 1;
    return pcm + s337m->stride;
}


static inline
uint32_t *
phase_write_data_burst
    (pmd_s337m *s337m       /**< [in] PCM smpte state */
    ,uint32_t *pcm          /**< [in/out] PCM channel buffer to modify */
    ,uint32_t *end          /**< [in] 1st sample after end of block */
    )
{
    size_t databits = s337m->databits;
    size_t stride = s337m->stride;
    size_t vo = s337m->vsync_offset;
    uint32_t tmp;
    uint8_t *p;
    size_t pcmstride[2] = { 1, stride - 1 };
    size_t vo_diff = 0;
    dlb_pmd_bool isodd = s337m->isodd;

    if (!s337m->pair)
    {
        pcmstride[0] = stride;
        pcmstride[1] = stride;
        vo_diff     = 1;
    }
    
    assert(pcm < end);
    assert(databits % WORD_BITS == 0);

    if (s337m->pair)
        assert(s337m->vsync_offset >= databits / PAIR_BITS + s337m->padding);
    else
        assert(s337m->vsync_offset >= databits / WORD_BITS + s337m->padding);

    p = s337m->data;

    while (databits >= WORD_BITS && pcm < end)
    {
        switch (isodd)
        {
        case 0:
            tmp = *p++;
            tmp = (tmp << 8) | *p++;
            tmp = (tmp << 8) | (*p & 0xf0);
            *pcm = tmp << 8;
            pcm += pcmstride[0];
            vo -= vo_diff;
            break;
        case 1:
            tmp = (*p++ & 0x0f);
            tmp = (tmp << 8) | *p++;
            tmp = (tmp << 8) | *p++;
            *pcm = tmp << 12;
            pcm += pcmstride[1];
            vo -= 1;
            break;
        default: abort();
        }
        
        databits -= WORD_BITS;
        isodd = 1 - isodd;
    }

    s337m->data = p;
    s337m->databits = databits;
    s337m->vsync_offset = vo;
    s337m->isodd = isodd;

    if (s337m->pair)
        assert(s337m->vsync_offset >= databits / PAIR_BITS + s337m->padding);
    else
        assert(s337m->vsync_offset >= databits / WORD_BITS + s337m->padding);

    if (!databits)
    {
        s337m->phase = S337M_PHASE_PADDING;
    }
    return pcm;
}


/* -------------------- read interface functions ------------------------ */


static inline
uint32_t *                    /** @return next PCM sample to read */
phase_read_padding
    (pmd_s337m *s337m         /**< [in] PCM smpte */
    ,uint32_t *pcm            /**< [in/out] PCM channel buffer to read */
    ,uint32_t *end            /**< [in] 1st sample after end of block */
    )
{
    size_t padding = s337m->padding;
    size_t stride = s337m->stride;
    size_t vo = s337m->vsync_offset;

    assert(s337m->vsync_offset >= s337m->padding);

    pcm += stride * padding;
    vo -= padding;
    padding = 0;
    if (pcm > end)
    {
        padding = (pcm - end)/stride;
        vo += padding;
        pcm = end;
    }

    if (!padding)
    {
        s337m->phase = S337M_PHASE_PREAMBLEA;
    }
    s337m->padding = padding;
    s337m->vsync_offset = vo;
    
    assert(s337m->vsync_offset >= s337m->padding);

    return pcm;
}


static inline
uint32_t *                    /** @return next PCM sample to read */
phase_read_wait_vsync
    (pmd_s337m *s337m         /**< [in] PCM smpte extractor */
    ,uint32_t *pcm            /**< [in/out] PCM channel buffer to read */
    ,uint32_t *end            /**< [in] 1st sample after end of block */
    )
{
    size_t stride = s337m->stride;
    size_t vo = s337m->vsync_offset;

    if (pcm + vo * stride <= end)
    {
        pcm += vo * stride;
        vo = 0;
        s337m->padding = GUARDBAND;
        s337m->phase = S337M_PHASE_GUARDBAND;
    }
    else
    {
        vo -= (end - pcm) / stride;
        pcm = end;
    }
    
    s337m->vsync_offset = vo;    
    return pcm;
}


static inline
uint32_t *                    /** @return next PCM sample to read */
phase_read_guardband
    (pmd_s337m *s337m         /**< [in] PCM smpte extractor */
    ,uint32_t *pcm            /**< [in/out] PCM channel buffer to read */
    ,uint32_t *end            /**< [in] 1st sample after end of block */
    )
{
    size_t stride = s337m->stride;
    size_t gband = s337m->padding;

    if (pcm + gband * stride <= end)
    {
        pcm += gband * stride;
        gband = 0;
        s337m->phase = S337M_PHASE_PREAMBLEA;
    }
    else
    {
        gband -= (end - pcm) / stride;
        pcm = end;
    }
    
    s337m->padding = gband;
    return pcm;
}


static inline
uint32_t *                    /** @return next PCM sample to read */
phase_read_preamble_pd
    (pmd_s337m *s337m         /**< [in] PCM smpte state */
    ,uint32_t *pcm            /**< [in/out] PCM channel buffer to modify */
    ,uint32_t *end            /**< [in] 1st sample after end of block */
    )
{
    unsigned int bits_per_sample = s337m->pair ? PAIR_BITS : WORD_BITS;
    unsigned int preamble_samples = s337m->pair ? PREAMBLE_SAMPLE_PAIRS : PREAMBLE_SAMPLES;
    unsigned int databits;

    (void)end;
    assert(pcm < end);

    databits = *pcm >> 12;
    if (databits > s337m->databits)
    {
        printf("datasize too large for input buffer, ignoring\n");
        databits = 0;
    }
    
    s337m->phase = S337M_PHASE_DATA;
    s337m->databits = ROUND_UP(databits, bits_per_sample);
    s337m->padding = s337m->framelen - preamble_samples - (s337m->databits / bits_per_sample);
    s337m->vsync_offset -= 1;
    return pcm + s337m->stride;
}


static inline
uint32_t *                    /** @return next PCM sample to read */
phase_read_preamble_pc
    (pmd_s337m *s337m         /**< [in] PCM smpte state */
    ,uint32_t *pcm            /**< [in/out] PCM channel buffer to modify */
    ,uint32_t *end            /**< [in] 1st sample after end of block */
    )
{
    assert(pcm < end);

    if ((*pcm & PC_MASK) != PC)
    {
        s337m->phase = S337M_PHASE_PREAMBLEA;
        return pcm;
    }

    if (s337m->pair)
    {
        return phase_read_preamble_pd(s337m, pcm+1, end)-1;
    }
    s337m->vsync_offset -= 1;
    s337m->phase = S337M_PHASE_PREAMBLED;
    return pcm + s337m->stride;
}


static inline
uint32_t*
phase_read_preamble_pb
    (pmd_s337m *s337m       /**< [in] PCM smpte state */
    ,uint32_t *pcm          /**< [in/out] PCM channel buffer to modify */
    ,uint32_t *end          /**< [in] 1st sample after end of block */
    )
{
    assert(pcm < end);

    (void)end;
    
    if (pcm[0] == PB)
    {
        s337m->phase = S337M_PHASE_PREAMBLEC;
        s337m->vsync_offset -= 1;
        return pcm + s337m->stride;
    }
    s337m->phase = S337M_PHASE_PREAMBLEA;
    return pcm + s337m->stride;
}


static inline
uint32_t*
phase_read_preamble_pa
    (pmd_s337m *s337m       /**< [in] PCM smpte state */
    ,uint32_t *pcm          /**< [in/out] PCM channel buffer to modify */
    ,uint32_t *end          /**< [in] 1st sample after end of block */
    )
{
    assert(pcm < end);

    (void)end;
    
    if (*pcm == PA)
    {
        s337m->phase = S337M_PHASE_PREAMBLEB;
        if (s337m->pair)
        {
            return phase_read_preamble_pb(s337m, pcm+1, end)-1;
        }
    }
    s337m->vsync_offset -= 1;
    return pcm + s337m->stride;
}


static inline
uint32_t *
phase_read_data_burst
    (pmd_s337m *s337m       /**< [in] PCM smpte state */
    ,uint32_t *pcm          /**< [in] PCM channel buffer to read */
    ,uint32_t *end          /**< [in] 1st sample after end of block */
    )
{
    size_t databits = s337m->databits;
    size_t stride = s337m->stride;
    size_t vo = s337m->vsync_offset;    
    dlb_pmd_bool isodd = s337m->isodd;
    uint32_t sample;
    uint8_t *p;
    size_t pcmstride[2] = { 1, stride - 1 };
    size_t vo_diff      = 0;

    if (!s337m->pair)
    {
        pcmstride[0] = stride;
        pcmstride[1] = stride;
        vo_diff = 1;
    }
    
    assert(pcm < end);
    assert(databits % WORD_BITS == 0);

    p = s337m->data;
    while (databits >= WORD_BITS && pcm < end)
    {
        sample = *pcm;

        switch (isodd)
        {
        case 0:
            sample = *pcm;
            p[0] = (sample >> 24) & 0xff;
            p[1] = (sample >> 16) & 0xff;
            p[2] = ((sample >> 8) & 0xf0);
            p += 2;
            vo -= 1;
            break;

        case 1:
            p[0] |= ((sample >> 28) & 0x0f);
            p[1]  = (sample >> 20) & 0xff;
            p[2]  = (sample >> 12) & 0xff;
            p += 3;
            vo -= vo_diff;
            break;

         default:
             abort();
        }
        
        pcm += pcmstride[isodd];
        databits -= WORD_BITS;
        isodd = 1 - isodd;
    }

    s337m->data = p;
    s337m->databits = databits;
    s337m->vsync_offset = vo;
    s337m->isodd = isodd;

    if (!databits)
    {
        s337m->phase = S337M_PHASE_PADDING;
        s337m->next(s337m);
        s337m->isodd = 0;
    }
    return pcm;
}


/* ---------------------------- public api --------------------------- */


void
pmd_s337m_init
    (pmd_s337m *s337m
    ,unsigned int stride
    ,next_block next
    ,void *nextarg
    ,dlb_pmd_bool pair
    ,unsigned int start
    ,dlb_pmd_bool mark_empty_block
    )
{
    memset(s337m, '\0', sizeof(*s337m));

    s337m->pair = pair;
    s337m->start = pair ? 2*start : start;
    s337m->phase = S337M_PHASE_VSYNC;
    s337m->stride = stride;
    s337m->next = next;
    s337m->nextarg = nextarg;
    s337m->padding = 0;
    s337m->vsync_offset = ~0u;
    s337m->mark_empty = mark_empty_block;

    s337m->next(s337m);
    s337m->isodd = 0;
}


uint32_t *
pmd_s337m_wrap
    (pmd_s337m *s337m
    ,uint32_t *pcm
    ,uint32_t *end
    )
{
    while (pcm < end)
    {
        switch (s337m->phase)
        {
        case S337M_PHASE_VSYNC:
            pcm = phase_write_wait_vsync(s337m, pcm, end);
            break;
                
        case S337M_PHASE_GUARDBAND:
            pcm = phase_write_guardband(s337m, pcm, end);
            break;

        case S337M_PHASE_PADDING:
            pcm = phase_write_padding(s337m, pcm, end);
            break;
            
        case S337M_PHASE_PREAMBLEA:
            pcm = phase_write_preamble_pa(s337m, pcm, end);
            break;

        case S337M_PHASE_PREAMBLEB:
            pcm = phase_write_preamble_pb(s337m, pcm, end);
            break;
            
        case S337M_PHASE_PREAMBLEC:
            pcm = phase_write_preamble_pc(s337m, pcm, end);
            break;

        case S337M_PHASE_PREAMBLED:
            pcm = phase_write_preamble_pd(s337m, pcm, end);
            break;
            
        case S337M_PHASE_DATA:
            pcm = phase_write_data_burst(s337m, pcm, end);
            break;
        }
    }
    return pcm;
}


uint32_t *
pmd_s337m_unwrap
    (pmd_s337m *s337m
    ,uint32_t *pcm
    ,uint32_t *end
    )
{
    while (pcm < end)
    {
        switch (s337m->phase)
        {
        case S337M_PHASE_VSYNC:
            pcm = phase_read_wait_vsync(s337m, pcm, end);
            break;
                
        case S337M_PHASE_GUARDBAND:
            pcm = phase_read_guardband(s337m, pcm, end);
            break;

        case S337M_PHASE_PADDING:
            pcm = phase_read_padding(s337m, pcm, end);
            break;
            
        case S337M_PHASE_PREAMBLEA:
            pcm = phase_read_preamble_pa(s337m, pcm, end);
            break;
            
        case S337M_PHASE_PREAMBLEB:
            pcm = phase_read_preamble_pb(s337m, pcm, end);
            break;
            
        case S337M_PHASE_PREAMBLEC:
            pcm = phase_read_preamble_pc(s337m, pcm, end);
            break;
            
        case S337M_PHASE_PREAMBLED:
            pcm = phase_read_preamble_pd(s337m, pcm, end);
            break;
            
        case S337M_PHASE_DATA:
            pcm = phase_read_data_burst(s337m, pcm, end);
            break;
        }
    }
    return pcm;
}
