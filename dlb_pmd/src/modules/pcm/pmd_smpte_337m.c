/************************************************************************
 * dlb_pmd
 * Copyright (c) 2016-2020, Dolby Laboratories Inc.
 * Copyright (c) 2016-2020, Dolby International AB.
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
 * @file pmd_smpte_337m.c
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
#include "dlb_pmd_pcm.h"


#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif


/**
 * @brief serial ADM stuff
 */
enum serial_adm_defs
{
    /* sADM SMPTE 337m Pc word --------------------------------------------- */
    SADM_PC_DT          = 0x1f, /**< sADM smpte 337m data type, 31 */
    SADM_PC_DM_16       = 0x0,  /**< sADM smpte 337m data mode, 0x0 -> 16-bit */
    SADM_PC_DM_20       = 0x1,  /**< sADM smpte 337m data mode, 0x1 -> 20-bit */
    SADM_PC_DM_24       = 0x2,  /**< sADM smpte 337m data mode, 0x2 -> 24-bit */
    SADM_PC_ERRF        = 0x0,  /**< sADM error flag */
    SADM_PC_CMF         = 0x1,  /**< sADM metadata-has-changed flag */
    SADM_PC_AI_FULL     = 0x0,  /**< sADM assemble_info present flag, full-frame mode */
    SADM_PC_AI_MULTI    = 0x1,  /**< sADM assemble_info present flag, multi-burst mode */
    SADM_PC_FF          = 0x1,  /**< sADM format_info present flag */
    SADM_PC_MCF         = 0x0,  /**< sADM multiple chunk flag, 0 = single chunk, 1 = multi chunk */
    SADM_PC_DSN         = 0x0,  /**< sADM data stream number, 0-6 */

    PC_SADM_16 = ((SADM_PC_DT       << 0)
               |  (SADM_PC_DM_16    << 5)
               |  (SADM_PC_ERRF     << 7)
               |  (SADM_PC_CMF      << 8)
               |  (SADM_PC_AI_FULL  << 9)
               |  (SADM_PC_FF       << 10)
               |  (SADM_PC_MCF      << 11)
               |  (SADM_PC_DSN      << 13)) << 16,

    PC_SADM_20 = ((SADM_PC_DT       << 4)
               |  (SADM_PC_DM_20    << 9)
               |  (SADM_PC_ERRF     << 11)
               |  (SADM_PC_CMF      << 12)
               |  (SADM_PC_AI_FULL  << 13)
               |  (SADM_PC_FF       << 14)
               |  (SADM_PC_MCF      << 15)
               |  (SADM_PC_DSN      << 17)) << 12,

    PC_SADM_24 = ((SADM_PC_DT       << 8)
               |  (SADM_PC_DM_24    << 13)
               |  (SADM_PC_ERRF     << 15)
               |  (SADM_PC_CMF      << 16)
               |  (SADM_PC_AI_FULL  << 17)
               |  (SADM_PC_FF       << 18)
               |  (SADM_PC_MCF      << 19)
               |  (SADM_PC_DSN      << 21)) << 8,

    PE_SADM_16 = 0x10000,
    PE_SADM_20 = 0x1000,
    PE_SADM_24 = 0x100,

    PF_SADM = 0,

    SADM_PC_AI_MASK = 0x1   << ( 9 + 16),   /**< sADM assemble_info present flag mask (bit-depth independent) */
    SADM_PC_FF_MASK = 0x1   << (10 + 16),   /**< sADM format_info   present flag mask (bit-depth independent) */

    /* sADM assemble_info --------------------------------------------- */
    SADM_IN_TIMELINE_FLAG = 0,
    SADM_TRACK_NUMBERS    = 0,
    SADM_TRACK_ID         = 0,

    SADM_ASSEMBLE_INFO_16 = (SADM_IN_TIMELINE_FLAG)
                          | (SADM_TRACK_NUMBERS << 2)
                          | (SADM_TRACK_ID << 8) << 16,

    SADM_ASSEMBLE_INFO_20 = (SADM_IN_TIMELINE_FLAG << 4)
                          | (SADM_TRACK_NUMBERS << 6)
                          | (SADM_TRACK_ID << 12) << 12,

    SADM_ASSEMBLE_INFO_24 = (SADM_IN_TIMELINE_FLAG << 8)
                          | (SADM_TRACK_NUMBERS << 10)
                          | (SADM_TRACK_ID << 16) << 8,

    /* sADM format_info ----------------------------------------------- */
    SADM_FORMAT_TYPE      = 1,  /* gzip, as specified in RFC-1952) */
    SADM_FORMAT_INFO_16   = (SADM_FORMAT_TYPE) << 16,
    SADM_FORMAT_INFO_20   = (SADM_FORMAT_TYPE << 4) << 12,
    SADM_FORMAT_INFO_24   = (SADM_FORMAT_TYPE << 8) << 8
};


/**
 * @brief SMPTE 337m sync words
 *
 * Our KLV uses 20- or 24-bit SMPTE 337m.  Theoretically, it could use 16-bit as well.
 */
enum s337m_values
{
    PA_16          = 0xf8720000,   /* IEC 958 preamble a (sync word 1) */
    PA_20          = 0x6f872000,   /* IEC 958 preamble a (sync word 1) */
    PA_24          = 0x96f87200,   /* IEC 958 preamble a (sync word 1) */

    PB_16          = 0x4e1f0000,   /* IEC 958 preamble b (sync word 2) */
    PB_20          = 0x54e1f000,   /* IEC 958 preamble b (sync word 2) */
    PB_24          = 0xa54e1f00,   /* IEC 958 preamble b (sync word 2) */

    PMD_PC_16      = 0x001b0000,   /* PMD Pc data mode & data type, bit depth 16 */
    PMD_PC_20      = 0x003b0000,   /* PMD Pc data mode & data type, bit depth 20 */
    PMD_PC_24      = 0x005b0000,   /* PMD Pc data mode & data type, bit depth 24 */

    PC_NULL        = 0x00000000,   /* preamble C (all bit depths, NULL) */

    PC_MASK        = 0x007f0000,   /* SMPTE preamble C data_mode & data_type mask */
    PC_KEY_FLAG    = 0x01000000,   /* KLV 'key_flag' to indicate presence of Universal Key */

    WORD_16_BITS = 16,
    WORD_20_BITS = 20,
    WORD_24_BITS = 24,
    SAMPLE_BITS  = 32,

    PAIR_16_BITS = 2 * WORD_16_BITS,
    PAIR_20_BITS = 2 * WORD_20_BITS,
    PAIR_24_BITS = 2 * WORD_24_BITS,

};


static inline
uint32_t                      /** @return correct value for bit depth */
select_s337m_value
    (pmd_s337m *s337m         /**< [in] PCM smpte state */
    ,int32_t v16              /**< [in] bit depth 16 value */
    ,int32_t v20              /**< [in] bit depth 20 value */
    ,int32_t v24              /**< [in] bit depth 24 value */
    )
{
    int32_t v = 0;

    switch (s337m->bit_depth)
    {
    case 16:
        v = v16;
        break;
    case 20:
        v = v20;
        break;
    case 24:
        v = v24;
        break;
    default:
        break;
    }

    return (uint32_t)v;
}


#define PA_VALUE(S) select_s337m_value(S, PA_16, PA_20, PA_24)
#define PB_VALUE(S) select_s337m_value(S, PB_16, PB_20, PB_24)
#define PC_PMD_VALUE(S) select_s337m_value(S, PMD_PC_16, PMD_PC_20, PMD_PC_24)
#define PC_SADM_VALUE(S) select_s337m_value(S, PC_SADM_16, PC_SADM_20, PC_SADM_24)
#define PE_SADM_VALUE(S) select_s337m_value(S, PE_SADM_16, PE_SADM_20, PE_SADM_24)
#define SADM_FORMAT_INFO_VALUE(S) select_s337m_value(S, SADM_FORMAT_INFO_16, SADM_FORMAT_INFO_20, SADM_FORMAT_INFO_24)
#define SADM_ASSEMBLE_INFO_VALUE(S) select_s337m_value(S, SADM_ASSEMBLE_INFO_16, SADM_ASSEMBLE_INFO_20, SADM_ASSEMBLE_INFO_24)

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif


#define ROUND_UP(size, rounding) ((((size) + (rounding) - 1)/(rounding))*(rounding))


/* -------------------- common utility functions ------------------------ */

/**
 * @brief advance to the next word in the PCM stream, maintaining correctly
 *        the state of the s337m instance
 */
static inline
uint32_t *                    /** @return next PCM word position */
next_word
    (pmd_s337m *s337m         /**< [in] PCM smpte state */
    ,uint32_t *pcm            /**< [in] PCM channel buffer */
    )
{
    if (s337m->pair)
    {
        if (s337m->pairity)
        {
            s337m->pairity = 0;
            pcm += (s337m->stride - 1);
        }
        else
        {
            s337m->pairity = 1;
            --s337m->vsync_offset;
            ++pcm;
        }
    }
    else
    {
        --s337m->vsync_offset;
        pcm += s337m->stride;
    }

    return pcm;
}


/* -------------------- write interface functions ------------------------ */

static inline
uint32_t *                    /** @return next PCM sample to write */
write_word
    (pmd_s337m *s337m         /**< [in] PCM smpte state */
    ,uint32_t *pcm            /**< [in/out] PCM channel buffer to modify */
    ,uint32_t *end            /**< [in] 1st sample after end of block */
    ,uint32_t word            /**< [in] value to write */
    ,dlb_pmd_bool is_data     /**< [in] true if this word is in the payload */
    )
{
    assert(pcm < end);
    (void)end;
    *pcm = word;
    if (is_data)
    {
        s337m->databits -= s337m->bit_depth;
    }
    return next_word(s337m, pcm);
}


static inline
uint32_t *                    /** @return next PCM sample to write */
write_repeated
    (pmd_s337m *s337m         /**< [in] PCM smpte state */
    ,uint32_t *pcm            /**< [in/out] PCM channel buffer to modify */
    ,uint32_t *end            /**< [in] 1st sample after end of block */
    ,size_t *counter          /**< [in/out] counter */
    ,uint32_t word            /**< [in] Word value to repeat */
    )
{
    assert(counter != &s337m->vsync_offset);    /* no help in release builds... */

    if (s337m->pair)    /* frame mode */
    {
        if (s337m->pairity)                 /* even out the second channel */
        {
            pcm = write_word(s337m, pcm, end, word, PMD_FALSE);
        }
        while (*counter && pcm < end) /* write the word for both channels */
        {
            pcm = write_word(s337m, pcm, end, word, PMD_FALSE);
            assert(pcm < end);
            pcm = write_word(s337m, pcm, end, word, PMD_FALSE);
            *counter -= 1;
        }
    }
    else                /* subframe mode */
    {
        while (*counter && pcm < end) /* write the word */
        {
            pcm = write_word(s337m, pcm, end, word, PMD_FALSE);
            *counter -= 1;
        }
    }

    return pcm;
}


static inline
uint32_t *                    /** @return next PCM sample to write */
phase_write_padding
    (pmd_s337m *s337m         /**< [in] PCM smpte state */
    ,uint32_t *pcm            /**< [in/out] PCM channel buffer to modify */
    ,uint32_t *end            /**< [in] 1st sample after end of block */
    )
{
    pcm = write_repeated(s337m, pcm, end, &s337m->padding, 0);
    if (!s337m->padding)
    {
        if (s337m->next(s337m))
        {
            s337m->phase = S337M_PHASE_PREAMBLEA;
        }
        else
        {
            s337m->phase = S337M_PHASE_VSYNC;
        }
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
    size_t vo = s337m->vsync_offset;    /* copy vsync_offset or we'll double-decrement it!  @see next_word() */

    pcm = write_repeated(s337m, pcm, end, &vo, 0);
    assert(vo == s337m->vsync_offset);
    if (!s337m->vsync_offset)
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
    pcm = write_repeated(s337m, pcm, end, &s337m->padding, 0);
    if (!s337m->padding)
    {
        s337m->phase = S337M_PHASE_PREAMBLEA;
    }

    return pcm;
}


static inline
uint32_t *
phase_write_sadm_ff
    (pmd_s337m *s337m       /**< [in] PCM smpte state */
    ,uint32_t *pcm          /**< [in/out] PCM channel buffer to modify */
    ,uint32_t *end          /**< [in] 1st sample after end of block */
    )
{
    assert(s337m->sadm);
    assert(s337m->sadm_ff);

    pcm = write_word(s337m, pcm, end, SADM_FORMAT_INFO_VALUE(s337m), PMD_TRUE);
    s337m->phase = S337M_PHASE_DATA;

    return pcm;
}


static inline
uint32_t *
phase_write_sadm_ai
    (pmd_s337m *s337m       /**< [in] PCM smpte state */
    ,uint32_t *pcm          /**< [in/out] PCM channel buffer to modify */
    ,uint32_t *end          /**< [in] 1st sample after end of block */
    )
{
    assert(s337m->sadm);
    assert(s337m->sadm_ai);

    pcm = write_word(s337m, pcm, end, SADM_ASSEMBLE_INFO_VALUE(s337m), PMD_TRUE);
    if (s337m->sadm_ff)
    {
        s337m->phase = S337M_PHASE_SADM_FF;
    }
    else
    {
        s337m->phase = S337M_PHASE_DATA;
    }

    return pcm;
}


static inline
uint32_t *
phase_write_preamble_pf
    (pmd_s337m *s337m       /**< [in] PCM smpte state */
    ,uint32_t *pcm          /**< [in/out] PCM channel buffer to modify */
    ,uint32_t *end          /**< [in] 1st sample after end of block */
    )
{
    assert(s337m->sadm);

    pcm = write_word(s337m, pcm, end, PF_SADM, PMD_TRUE);
    s337m->phase =
        (s337m->sadm_ai) ? S337M_PHASE_SADM_AI :
        (s337m->sadm_ff) ? S337M_PHASE_SADM_FF :
        S337M_PHASE_DATA;

    return pcm;
}


static inline
uint32_t*
phase_write_preamble_pe
    (pmd_s337m *s337m       /**< [in] PCM smpte state */
    ,uint32_t *pcm          /**< [in/out] PCM channel buffer to modify */
    ,uint32_t *end          /**< [in] 1st sample after end of block */
    )
{
    assert(s337m->sadm);

    pcm = write_word(s337m, pcm, end, PE_SADM_VALUE(s337m), PMD_TRUE);
    s337m->phase = S337M_PHASE_PREAMBLEF;

    return pcm;
}


static inline
uint32_t *                    /** @return next PCM sample to write */
phase_write_preamble_pd
    (pmd_s337m *s337m         /**< [in] PCM smpte state */
    ,uint32_t *pcm            /**< [in/out] PCM channel buffer to modify */
    ,uint32_t *end            /**< [in] 1st sample after end of block */
    )
{
    unsigned int bits_per_sample = s337m->bit_depth;
    unsigned int preamble_samples = PREAMBLE_WORDS;

    if (s337m->pair)
    {
        bits_per_sample *= 2;
        preamble_samples /= 2;
    }
    
    if (s337m->databits)
    {
        size_t databits;

        if (s337m->sadm)
        {
            s337m->phase = S337M_PHASE_PREAMBLEE;
            /* sADM preamble includes Pe and Pf words, which must be added
             * to the databit count; we must also add for the AI and/or FF
             * words, if present
             */
            if (s337m->sadm_ai && s337m->sadm_ff)       /* both AI and FF */
            {
                s337m->databits += 4 * s337m->bit_depth;
            }
            else if (s337m->sadm_ai || s337m->sadm_ff)  /* one of AI or FF*/
            {
                s337m->databits += 3 * s337m->bit_depth;
            }
            else                                        /* no AI nor FF */
            {
                s337m->databits += 2 * s337m->bit_depth;
            }
        }
        else
        {
            s337m->phase = S337M_PHASE_DATA;
        }

        pcm = write_word(s337m, pcm, end, (uint32_t)(s337m->databits << (SAMPLE_BITS - s337m->bit_depth)), PMD_FALSE);
        s337m->databits = ROUND_UP(s337m->databits, s337m->bit_depth);  /* bit length in even data words */
        databits = ROUND_UP(s337m->databits, bits_per_sample);          /* bit length in even samples */
        assert(s337m->framelen >= (preamble_samples + (s337m->databits / bits_per_sample)));
        s337m->padding = s337m->framelen - preamble_samples - (s337m->databits / bits_per_sample);
        /* TODO: possibly padding could be short by one sample -- is that a problem? */
    }
    else
    {
        pcm = write_word(s337m, pcm, end, 0, PMD_FALSE);
        s337m->phase = S337M_PHASE_PADDING;
        s337m->padding = s337m->framelen - preamble_samples;
    }

    return pcm;
}


static inline
uint32_t *                    /** @return next PCM sample to write */
phase_write_preamble_pc
    (pmd_s337m *s337m         /**< [in] PCM smpte state */
    ,uint32_t *pcm            /**< [in/out] PCM channel buffer to modify */
    ,uint32_t *end            /**< [in] 1st sample after end of block */
    )
{
    uint32_t word;

    if (s337m->databits)
    {
        word = ((s337m->sadm) ? PC_SADM_VALUE(s337m) : PC_PMD_VALUE(s337m)) | PC_KEY_FLAG;
    }
    else if (s337m->mark_empty)
    {
        word = PC_NULL; /* NULL databurst */
    }
    else
    {
        word = 0;       /* silence */
    }
    pcm = write_word(s337m, pcm, end, word, PMD_FALSE);
    s337m->phase = S337M_PHASE_PREAMBLED;

    return pcm;
}


static inline
uint32_t *
phase_write_preamble_pb
    (pmd_s337m *s337m       /**< [in] PCM smpte state */
    ,uint32_t *pcm          /**< [in/out] PCM channel buffer to modify */
    ,uint32_t *end          /**< [in] 1st sample after end of block */
    )
{
    uint32_t word;

    if (s337m->databits || s337m->mark_empty)
    {
        word = PB_VALUE(s337m);
    }
    else
    {
        word = 0;
    }
    pcm = write_word(s337m, pcm, end, word, PMD_FALSE);
    s337m->phase = S337M_PHASE_PREAMBLEC;

    return pcm;
}


static inline
uint32_t *
phase_write_preamble_pa
    (pmd_s337m *s337m       /**< [in] PCM smpte state */
    ,uint32_t *pcm          /**< [in/out] PCM channel buffer to modify */
    ,uint32_t *end          /**< [in] 1st sample after end of block */
    )
{
    uint32_t word;

    if (s337m->databits || s337m->mark_empty)
    {
        word = PA_VALUE(s337m);
    }
    else
    {
        word = 0;
    }
    pcm = write_word(s337m, pcm, end, word, PMD_FALSE);
    s337m->phase = S337M_PHASE_PREAMBLEB;

    return pcm;
}


static inline
uint32_t *
phase_write_data_burst_20
    (pmd_s337m *s337m       /**< [in] PCM smpte state */
    ,uint32_t *pcm          /**< [in/out] PCM channel buffer to modify */
    ,uint32_t *end          /**< [in] 1st sample after end of block */
    )
{
    size_t databits = s337m->databits;
    uint32_t tmp;
    uint8_t *p;
    dlb_pmd_bool isodd = s337m->isodd;
    
#ifndef NDEBUG
    assert(databits % WORD_20_BITS == 0);
    if (s337m->pair)
        assert(s337m->vsync_offset >= databits / PAIR_20_BITS + s337m->padding);
    else
        assert(s337m->vsync_offset >= databits / WORD_20_BITS + s337m->padding);
#endif

    p = s337m->data;
    while (databits >= WORD_20_BITS && pcm < end)
    {
        switch (isodd)
        {
        case 0:
            tmp = *p++;
            tmp = (tmp << 8) | *p++;
            tmp = (tmp << 8) | (*p & 0xf0);
            pcm = write_word(s337m, pcm, end, tmp << 8, PMD_TRUE);
            break;
        case 1:
            tmp = (*p++ & 0x0f);
            tmp = (tmp << 8) | *p++;
            tmp = (tmp << 8) | *p++;
            pcm = write_word(s337m, pcm, end, tmp << 12, PMD_TRUE);
            break;
        default:
            abort();
        }
        
        databits -= WORD_20_BITS;
        isodd = 1 - isodd;
    }

    assert(databits == s337m->databits);
    s337m->data = p;
    s337m->databits = databits;
    s337m->isodd = isodd;

#ifndef NDEBUG
    if (s337m->pair)
        assert(s337m->vsync_offset >= databits / PAIR_20_BITS + s337m->padding);
    else
        assert(s337m->vsync_offset >= databits / WORD_20_BITS + s337m->padding);
#endif

    if (!databits)
    {
        s337m->phase = S337M_PHASE_PADDING;
        s337m->isodd = 0;
    }

    return pcm;
}


static inline
uint32_t *
phase_write_data_burst_24
    (pmd_s337m *s337m       /**< [in] PCM smpte state */
    ,uint32_t *pcm          /**< [in/out] PCM channel buffer to modify */
    ,uint32_t *end          /**< [in] 1st sample after end of block */
    )
{
    size_t databits = s337m->databits;
    dlb_pmd_bool isodd = s337m->isodd;
    uint32_t sample;
    uint8_t *p;

#ifndef NDEBUG
    assert(databits % WORD_24_BITS == 0);
    if (s337m->pair)
        assert(s337m->vsync_offset >= databits / PAIR_24_BITS + s337m->padding);
    else
        assert(s337m->vsync_offset >= databits / WORD_24_BITS + s337m->padding);
#endif

    p = s337m->data;
    while (databits >= WORD_24_BITS && pcm != end)
    {
        sample = *p++;
        sample = (sample << 8) | *p++;
        sample = (sample << 8) | *p++;
        pcm = write_word(s337m, pcm, end, sample << 8, PMD_TRUE);
        databits -= WORD_24_BITS;
        isodd = 1 - isodd;
    }

    assert(databits == s337m->databits);
    s337m->data = p;
    s337m->databits = databits;
    s337m->isodd = isodd;

#ifndef NDEBUG
    if (s337m->pair)
        assert(s337m->vsync_offset >= databits / PAIR_24_BITS + s337m->padding);
    else
        assert(s337m->vsync_offset >= databits / WORD_24_BITS + s337m->padding);
#endif

    if (!databits)
    {
        s337m->phase = S337M_PHASE_PADDING;
        s337m->isodd = 0;
    }

    return pcm;
}


static inline
uint32_t *
phase_write_data_burst
    (pmd_s337m *s337m       /**< [in] PCM smpte state */
    ,uint32_t *pcm          /**< [in/out] PCM channel buffer to modify */
    ,uint32_t *end          /**< [in] 1st sample after end of block */
    )
{
    switch (s337m->bit_depth)
    {
    case 20:
        pcm = phase_write_data_burst_20(s337m, pcm, end);
        break;
    case 24:
        pcm = phase_write_data_burst_24(s337m, pcm, end);
        break;
    default:
        while (s337m->databits && pcm != end)   /* This will probably only be correct for bit depth 16 */
        {
            pcm = write_word(s337m, pcm, end, 0u, PMD_TRUE);
        }
        break;
    }

    return pcm;
}


/* -------------------- read interface functions ------------------------ */

static inline
uint32_t *                    /** @return next PCM sample to read */
read_word
    (pmd_s337m *s337m         /**< [in] PCM smpte */
    ,uint32_t *pcm            /**< [in/out] PCM channel buffer to read */
    ,uint32_t *end            /**< [in] 1st sample after end of block */
    ,uint32_t *word           /**< [out] word that was read */
    ,dlb_pmd_bool is_data     /**< [in] true if this word is in the payload */
    )
{
    assert(pcm < end);
    (void)end;
    *word = *pcm;
    if (is_data)
    {
        s337m->databits -= s337m->bit_depth;
    }
    return next_word(s337m, pcm);
}


static inline
uint32_t *                    /** @return next PCM sample to read */
read_repeated
    (pmd_s337m *s337m         /**< [in] PCM smpte */
    ,uint32_t *pcm            /**< [in/out] PCM channel buffer to read */
    ,uint32_t *end            /**< [in] 1st sample after end of block */
    ,size_t *counter          /**< [in/out] counter */
    )
{
    uint32_t word;

    assert(counter != &s337m->vsync_offset);    /* no help in release builds... */

    if (s337m->pair)    /* frame mode */
    {
        if (s337m->pairity)                 /* even up the second channel */
        {
            pcm = read_word(s337m, pcm, end, &word, PMD_FALSE);
        }
        while (*counter && pcm < end)       /* read the word for both channels */
        {
            pcm = read_word(s337m, pcm, end, &word, PMD_FALSE);
            assert(pcm < end);
            pcm = read_word(s337m, pcm, end, &word, PMD_FALSE);
            *counter -= 1;
        }
    }
    else                /* subframe mode */
    {
        while (*counter && pcm < end)       /* read the word */
        {
            pcm = read_word(s337m, pcm, end, &word, PMD_FALSE);
            *counter -= 1;
        }
    }

    return pcm;
}


static inline
uint32_t *                    /** @return next PCM sample to read */
phase_read_padding
    (pmd_s337m *s337m         /**< [in] PCM smpte */
    ,uint32_t *pcm            /**< [in/out] PCM channel buffer to read */
    ,uint32_t *end            /**< [in] 1st sample after end of block */
    )
{
    pcm = read_repeated(s337m, pcm, end, &s337m->padding);
    if (!s337m->padding)
    {
        s337m->phase = S337M_PHASE_PREAMBLEA;
    }

    return pcm;
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
    dlb_pmd_bool isodd = s337m->isodd;
    uint32_t sample;
    uint8_t *p;

    assert(databits % s337m->bit_depth == 0);

    p = s337m->data;
    switch (s337m->bit_depth)
    {
    case WORD_16_BITS:
        while (databits >= WORD_16_BITS && pcm < end)
        {
            pcm = read_word(s337m, pcm, end, &sample, PMD_TRUE);
            p[0] = ((sample >> 24) & 0xff);
            p[1] = ((sample >> 16) & 0xff);
            databits -= WORD_16_BITS;
            isodd = 1 - isodd;
            p += 2;
        }
        break;

    case WORD_20_BITS:
        while (databits >= WORD_20_BITS && pcm < end)
        {
            pcm = read_word(s337m, pcm, end, &sample, PMD_TRUE);
            switch (isodd)
            {
            case 0:
                p[0] = ((sample >> 24) & 0xff);
                p[1] = ((sample >> 16) & 0xff);
                p[2] = ((sample >> 8)  & 0xf0);
                p += 2;
                break;

            case 1:
                p[0] |= ((sample >> 28) & 0x0f);
                p[1]  = ((sample >> 20) & 0xff);
                p[2]  = ((sample >> 12) & 0xff);
                p += 3;
                break;

            default:
                abort();
            }
            databits -= WORD_20_BITS;
            isodd = 1 - isodd;
        }
        break;

    case WORD_24_BITS:
        while (databits >= WORD_24_BITS && pcm < end)
        {
            pcm = read_word(s337m, pcm, end, &sample, PMD_TRUE);
            p[0] = ((sample >> 24) & 0xff);
            p[1] = ((sample >> 16) & 0xff);
            p[2] = ((sample >> 8)  & 0xff);
            p += 3;
            databits -= WORD_24_BITS;
            isodd = 1 - isodd;
        }
        break;
    
    default:
        assert(  (s337m->bit_depth == WORD_16_BITS)
              || (s337m->bit_depth == WORD_20_BITS) 
              || (s337m->bit_depth == WORD_24_BITS));
        break;
    }

    assert(databits == s337m->databits);
    s337m->data = p;
    s337m->databits = databits;
    if (!databits)
    {
        s337m->phase = S337M_PHASE_PADDING;
        s337m->next(s337m);
        s337m->isodd = 0;
    }
    else
    {
        s337m->isodd = isodd;
    }

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
    size_t vo = s337m->vsync_offset;    /* copy vsync_offset or we'll double-decrement it!  @see next_word() */

    pcm = read_repeated(s337m, pcm, end, &vo);
    assert(vo == s337m->vsync_offset);
    if (!s337m->vsync_offset)
    {
        s337m->phase = S337M_PHASE_PREAMBLEA;
    }

    return pcm;
}


static inline
uint32_t *
phase_read_sadm_ff
    (pmd_s337m *s337m       /**< [in] PCM smpte state */
    ,uint32_t *pcm          /**< [in/out] PCM channel buffer to modify */
    ,uint32_t *end          /**< [in] 1st sample after end of block */
    )
{
    uint32_t got_ff;
    uint32_t expected_ff = SADM_FORMAT_INFO_VALUE(s337m);
    dlb_pmd_bool ff_is_gzip;

    pcm = read_word(s337m, pcm, end, &got_ff, PMD_TRUE);

    /* check for frame format == gzip */
    ff_is_gzip = (got_ff == expected_ff);

    if (ff_is_gzip)
    {
        s337m->phase = S337M_PHASE_DATA;
    }
    else
    {
        /* Reset s337m->databits value to buffer size. Dereferencing the extractor buffer will cause the next() call
        to only reset the s337m struct (and not try to decode the buffer contents) */
        s337m->data = 0;
        s337m->next(s337m);            

        s337m->phase = S337M_PHASE_PREAMBLEA;
    }

    return pcm;
}


static inline
uint32_t *
phase_read_sadm_ai
    (pmd_s337m *s337m       /**< [in] PCM smpte state */
    ,uint32_t *pcm          /**< [in/out] PCM channel buffer to modify */
    ,uint32_t *end          /**< [in] 1st sample after end of block */
    )
{
    uint32_t ai;

    pcm = read_word(s337m, pcm, end, &ai, PMD_TRUE);
    
    /* todo: decode sADM assemble info field */

    s337m->phase = (s337m->sadm_ff) ? S337M_PHASE_SADM_FF : S337M_PHASE_DATA;

    return pcm;
}


static inline
uint32_t *
phase_read_preamble_pf
    (pmd_s337m *s337m       /**< [in] PCM smpte state */
    ,uint32_t *pcm          /**< [in/out] PCM channel buffer to modify */
    ,uint32_t *end          /**< [in] 1st sample after end of block */
    )
{
    uint32_t pf;

    pcm = read_word(s337m, pcm, end, &pf, PMD_TRUE);
    if (pf == PF_SADM)
    {
        s337m->phase =
            (s337m->sadm_ai) ? S337M_PHASE_SADM_AI :
            (s337m->sadm_ff) ? S337M_PHASE_SADM_FF :
            S337M_PHASE_DATA;
    }
    else
    {
        s337m->phase = S337M_PHASE_PREAMBLEA;
    }

    return pcm;
}


static inline
uint32_t *
phase_read_preamble_pe
    (pmd_s337m *s337m       /**< [in] PCM smpte state */
    ,uint32_t *pcm          /**< [in/out] PCM channel buffer to modify */
    ,uint32_t *end          /**< [in] 1st sample after end of block */
    )
{
    uint32_t got_pe;
    uint32_t expected_pe = PE_SADM_VALUE(s337m);

    pcm = read_word(s337m, pcm, end, &got_pe, PMD_TRUE);
    if (got_pe == expected_pe)
    {
        s337m->phase = S337M_PHASE_PREAMBLEF;
    }
    else
    {
        s337m->phase = S337M_PHASE_PREAMBLEA;
    }

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
    uint32_t pd;
    unsigned int bits_per_sample = s337m->bit_depth;
    unsigned int preamble_samples = PREAMBLE_WORDS;
    unsigned int databits;
    size_t reqsamples;

    if (s337m->pair)
    {
        bits_per_sample *= 2;
        preamble_samples /= 2;
    }

    pcm = read_word(s337m, pcm, end, &pd, PMD_FALSE);
    databits = pd >> (SAMPLE_BITS - s337m->bit_depth);
    if (databits > s337m->databits)
    {
        /* datasize too large for input buffer, ignoring */
        databits = 0;

        /* Reset s337m->databits value to max buffer size. Dereferencing the extractor buffer will cause the next() call
        to only reset the s337m struct (and not try to decode the buffer contents) */
        s337m->data = 0;
        s337m->next(s337m);       

        s337m->phase = S337M_PHASE_PREAMBLEA;
        return(pcm);
    }
    
    if (s337m->sadm)
    {
        s337m->phase = S337M_PHASE_PREAMBLEE;
    }
    else
    {
        s337m->phase = S337M_PHASE_DATA;
        s337m->framelen = DLB_PCMPMD_BLOCK_SIZE;
    }

    s337m->databits = ROUND_UP(databits, s337m->bit_depth);         /* bit length in even data words */
    databits = ROUND_UP(databits, bits_per_sample);                 /* bit length in even samples */
    reqsamples = preamble_samples + (databits / bits_per_sample);
    if (s337m->framelen < reqsamples)
    {
        /* we have corrupt data. Ignore and wait for the next one */
        s337m->phase = S337M_PHASE_PREAMBLEA;
    }
    else
    {
        /* Due to the guardband, the number of samples between the first
         * and second blocks of a video frame, (MTx(0) and MTx(1)) will be
         * 128 samples rather than 160.  We may not accurately know where
         * we are in the frame, so the rule is: *if* the databits could
         * fit into 128 samples, then assume the frame length *is*
         * 128. This means that the state machine will not miss the start
         * of the next block.  In the worst case, where we are not at the
         * start of the frame and the actual Pa spacing between blocks is 160
         * samples, setting the frame length to 128 simply means we have to
         * wait in the PA preamble phase for an additional 32 samples.
         */
        if (s337m->framelen - GUARDBAND >= reqsamples)
        {
            s337m->framelen -= GUARDBAND;
        }
        
        s337m->padding = s337m->framelen - preamble_samples - (databits / bits_per_sample);
    }

    return pcm;
}


static inline
uint32_t *                    /** @return next PCM sample to read */
phase_read_preamble_pc
    (pmd_s337m *s337m         /**< [in] PCM smpte state */
    ,uint32_t *pcm            /**< [in/out] PCM channel buffer to modify */
    ,uint32_t *end            /**< [in] 1st sample after end of block */
    )
{
    uint32_t pc;
    uint32_t masked_pc;
    uint32_t expected_pmd_pc  = PC_PMD_VALUE(s337m)  & PC_MASK;
    uint32_t expected_sadm_pc = PC_SADM_VALUE(s337m) & PC_MASK;

    s337m->phase = S337M_PHASE_PREAMBLED;
    pcm = read_word(s337m, pcm, end, &pc, PMD_FALSE);
    masked_pc = pc & PC_MASK;
    if (masked_pc == expected_pmd_pc)
    {
        s337m->sadm = 0;
    }
    else if (masked_pc == expected_sadm_pc)
    {
        dlb_pmd_bool has_assemble_info = !!(pc & SADM_PC_AI_MASK);
        dlb_pmd_bool has_frame_format  = !!(pc & SADM_PC_FF_MASK);

        s337m->sadm = 1;
        if (has_assemble_info)
        {
            /* As of now, we allow sADM in full-frame mode only, which should not be
             * produced with an assemble info word */
             /* TODO: make available to the calling application a status/warning/error message */
            s337m->phase = S337M_PHASE_PREAMBLEA;
        } 
        else if (!has_frame_format)
        {
            /* As of now, we allow sADM in compressed (gzip) format only, which requires a
             * frame format (FF) word */
             /* TODO: make available to the calling application a status/warning/error message */
            s337m->phase = S337M_PHASE_PREAMBLEA;
        }
        else
        {
            s337m->sadm_ai = 0;
            s337m->sadm_ff = 1;
        }
    }
    else                /* Neither PMD nor sADM, punt! */
    {
        s337m->phase = S337M_PHASE_PREAMBLEA;
    }

    return pcm;
}


static inline
uint32_t *
phase_read_preamble_pb
    (pmd_s337m *s337m       /**< [in] PCM smpte state */
    ,uint32_t *pcm          /**< [in/out] PCM channel buffer to modify */
    ,uint32_t *end          /**< [in] 1st sample after end of block */
    )
{
    uint32_t got_pb;
    uint32_t expected_pb = PB_VALUE(s337m);

    pcm = read_word(s337m, pcm, end, &got_pb, PMD_FALSE);
    if (got_pb == expected_pb)
    {
        s337m->phase = S337M_PHASE_PREAMBLEC;
    }
    else
    {
        s337m->phase = S337M_PHASE_PREAMBLEA;
    }
    
    return pcm;
}


static inline
uint32_t *
phase_read_preamble_pa
    (pmd_s337m *s337m       /**< [in] PCM smpte state */
    ,uint32_t *pcm          /**< [in/out] PCM channel buffer to modify */
    ,uint32_t *end          /**< [in] 1st sample after end of block */
    )
{
    dlb_pmd_bool pa_present = PMD_FALSE;
    uint32_t pa;

    pcm = read_word(s337m, pcm, end, &pa, PMD_FALSE);
    switch (pa)
    {
    case PA_16:
        s337m->bit_depth = WORD_16_BITS;
        pa_present = PMD_TRUE;
        break;

    case PA_20:
        s337m->bit_depth = WORD_20_BITS;
        pa_present = PMD_TRUE;
        break;

    case PA_24:
        s337m->bit_depth = WORD_24_BITS;
        pa_present = PMD_TRUE;
        break;

    default:
        break;
    }
    
    if (pa_present)
    {
        s337m->phase = S337M_PHASE_PREAMBLEB;
    }

    return pcm;
}


/* ---------------------------- public api --------------------------- */

void
pmd_s337m_init
    (pmd_s337m *s337m
    ,unsigned int wrap_depth
    ,unsigned int stride
    ,next_block next
    ,void *nextarg
    ,dlb_pmd_bool pair
    ,unsigned int start
    ,dlb_pmd_bool mark_empty_block
    ,dlb_pmd_bool sadm
    )
{
    memset(s337m, '\0', sizeof(*s337m));

    s337m->wrap_depth = (wrap_depth == 24) ? 24 : 20;
    s337m->bit_depth  = (wrap_depth == 16) ? 16 : s337m->wrap_depth;
    s337m->pair = pair;
    s337m->pairity = 0;
    s337m->start = pair ? 2*start : start;
    s337m->pa_found = NO_PA_FOUND;
    s337m->phase = S337M_PHASE_VSYNC;
    s337m->stride = stride;
    s337m->next = next;
    s337m->nextarg = nextarg;
    s337m->padding = 0;
    s337m->vsync_offset = ~0u;
    s337m->mark_empty = mark_empty_block;
    s337m->sadm = sadm;
    if (sadm)
    {
        /* If we also allow UTF-8 mode later, these values will need to be calculated differently */
        s337m->sadm_ai = SADM_PC_AI_FULL;
        s337m->sadm_ff = SADM_PC_FF;
    }

    s337m->next(s337m);
    s337m->isodd = 0;
}


static
unsigned int
data_byte_count
    (pmd_s337m *s337m
    ,unsigned int sample_count
    ,unsigned int reserved_bits
    )
{
    unsigned int byte_count;
    unsigned int channel_count;
    unsigned int bit_count;

    if (s337m == NULL || sample_count == 0)
    {
        return 0;
    }
    channel_count = (s337m->pair ? 2 : 1);
    bit_count = s337m->bit_depth * sample_count * channel_count - reserved_bits;
    byte_count = bit_count / 8;

    return byte_count;
}


unsigned int
pmd_s337m_data_bytes
    (pmd_s337m *s337m
    ,unsigned int sample_count
    )
{
    return data_byte_count(s337m, sample_count, 0);
}


unsigned int
pmd_s337m_pmd_data_bytes
    (pmd_s337m *s337m
    ,unsigned int block_number
    ,unsigned int block_count
    )
{
    unsigned int preamble_samples = PREAMBLE_WORDS;
    unsigned int sample_count = DLB_PCMPMD_BLOCK_SIZE;

    if (s337m == NULL || block_number >= block_count)
    {
        return 0;
    }

    if (s337m->pair)
    {
        preamble_samples /= 2;
    }
    preamble_samples += 4;  /* KLV "long preamble" */

    if (block_number == 0 || block_number == block_count - 1)
    {
        sample_count -= GUARDBAND;
    }

    return data_byte_count(s337m, sample_count - preamble_samples, 0);
}


unsigned int
pmd_s337m_sadm_data_bytes
    (pmd_s337m *s337m
    ,dlb_pmd_frame_rate rate
    )
{
    unsigned int preamble_samples = PREAMBLE_WORDS;
    unsigned int sample_count = pmd_s337m_min_frame_size(rate) - 2 * GUARDBAND;
    unsigned int reserved_words = 2;    /* Pe and Pf */

    if (s337m == NULL)
    {
        return 0;
    }

    if (s337m->pair)
    {
        preamble_samples /= 2;
    }

    if (s337m->sadm_ai)
    {
        reserved_words++;
    }

    if (s337m->sadm_ff)
    {
        reserved_words++;
    }

    return data_byte_count(s337m, sample_count - preamble_samples, reserved_words * s337m->bit_depth);
}


uint32_t *
pmd_s337m_wrap
    (pmd_s337m *s337m
    ,uint32_t *pcm
    ,uint32_t *end
    )
{
    s337m->bit_depth = s337m->wrap_depth;

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
            
        case S337M_PHASE_PREAMBLEE:
            pcm = phase_write_preamble_pe(s337m, pcm, end);
            break;

        case S337M_PHASE_PREAMBLEF:
            pcm = phase_write_preamble_pf(s337m, pcm, end);
            break;

        case S337M_PHASE_SADM_AI:
            pcm = phase_write_sadm_ai(s337m, pcm, end);
            break;

        case S337M_PHASE_SADM_FF:
            pcm = phase_write_sadm_ff(s337m, pcm, end);
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
    uint32_t *pcm_begin = pcm;
    s337m->pa_found = NO_PA_FOUND;

    while (pcm < end)
    {
        switch (s337m->phase)
        {
        case S337M_PHASE_VSYNC:
            pcm = phase_read_wait_vsync(s337m, pcm, end);
            break;
                
        case S337M_PHASE_PADDING:
            pcm = phase_read_padding(s337m, pcm, end);
            break;
            
        case S337M_PHASE_PREAMBLEA:
        case S337M_PHASE_GUARDBAND:
            pcm = phase_read_preamble_pa(s337m, pcm, end);
            break;
            
        case S337M_PHASE_PREAMBLEB:
            pcm = phase_read_preamble_pb(s337m, pcm, end);
            break;
            
        case S337M_PHASE_PREAMBLEC:
            s337m->pa_found = (pcm - pcm_begin)/s337m->stride - 1 - !s337m->pair;
            if (s337m->pa_found_cb)
            {
                (*s337m->pa_found_cb)(s337m->pa_found_cb_arg, s337m->pa_found);
            }
            pcm = phase_read_preamble_pc(s337m, pcm, end);
            break;
            
        case S337M_PHASE_PREAMBLED:
            pcm = phase_read_preamble_pd(s337m, pcm, end);
            break;
            
        case S337M_PHASE_PREAMBLEE:
            pcm = phase_read_preamble_pe(s337m, pcm, end);
            break;

        case S337M_PHASE_PREAMBLEF:
            pcm = phase_read_preamble_pf(s337m, pcm, end);
            break;

        case S337M_PHASE_SADM_AI:
            pcm = phase_read_sadm_ai(s337m, pcm, end);
            break;

        case S337M_PHASE_SADM_FF:
            pcm = phase_read_sadm_ff(s337m, pcm, end);
            break;

        case S337M_PHASE_DATA:
            pcm = phase_read_data_burst(s337m, pcm, end);
            break;
        
        default:
            assert(  S337M_PHASE_VSYNC <= s337m->phase
                  || S337M_PHASE_DATA  >= s337m->phase);
            s337m->phase = S337M_PHASE_PREAMBLEA;
            break;
        }
    }
    return pcm;
}
