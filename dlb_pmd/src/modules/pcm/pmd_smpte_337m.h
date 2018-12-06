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
 * @flle pmd_smpte_337m.h
 * @brief KLV SMPTE 337m wrapper/unwrapper code
 */


#include "dlb_pmd_types.h"


/**
 * @def MAX_DATA_BYTES_PAIR
 * @brief maximum number of bytes available to PMD in a single PMD block when
 * transmitted over a pair of channels
 *
 * PMD-over-PCM is transmitted in blocks of 160 samples.
 *
 * KLV uses 20 bits per 32-bit sample, like DE. So, 20 bits per
 * channel, 2 channels = 40 bits or 5 bytes, reserve 2 samples for
 * Smpte Preamble, and 4 more for zeros before next ('long
 * preamble')
 */
#define MAX_DATA_BYTES_PAIR ((DLB_PCMPMD_BLOCK_SIZE - 6) * 5)


/**
 * @def MAX_DATA_BYTES_CHAN
 * @brief maximum number of bytes available to PMD in a single PMD block when
 * transmitted over a single channel.
 * 
 * For a single-channel encode, we need 8 samples for the preamble, but
 * reserving only 2.5 bytes of data (20 bits).
 */ 
#define MAX_DATA_BYTES_CHAN (((DLB_PCMPMD_BLOCK_SIZE - 8) * 5)/2)
    

/**
 * @def GUARDBAND
 * @brief number of samples after video sync before PMD metadata starts
 *
 * The guardband is a measure of safety when splicing audio streams.
 */
#define GUARDBAND (32)


/**
 * @brief abstract type of PMD SMPTE 337m wrapper/unwrapper
 */
typedef struct pmd_s337m pmd_s337m;


/**
 * @brief augmentation phase
 */
typedef enum
{
    S337M_PHASE_VSYNC,          /**< write silence until vsync */
    S337M_PHASE_GUARDBAND,      /**< wait guardband samples after vsync */
    S337M_PHASE_PADDING,        /**< writing silence */
    S337M_PHASE_PREAMBLEA,      /**< writing SMPTE 337m preamble, Pa */
    S337M_PHASE_PREAMBLEB,      /**< writing SMPTE 337m preamble, Pb */
    S337M_PHASE_PREAMBLEC,      /**< writing SMPTE 337m preamble, Pc */
    S337M_PHASE_PREAMBLED,      /**< writing SMPTE 337m preamble, Pd */
    S337M_PHASE_DATA            /**< writing data burst */
} s337m_phase;


/**
 * @brief callback to retrieve next block to write
 */
typedef
int                         /** @return 1 if block available, 0 if waiting for vsync */
(*next_block)
    (pmd_s337m *wrapper     /**< [in] wrapper abstraction */
    );


/**
 * @brief state of SMPTE 337m wrapping/unwrapping
 *
 * Don't update these fields directly - use the various methods to
 * initialize and maintain state.
 */
struct pmd_s337m
{
    s337m_phase phase;         /**< current SMPTE 337m phase */
    dlb_pmd_bool pair;         /**< true for SMPTE 337m pair, false for single channel */
    dlb_pmd_bool isodd;        /**< current word is at odd index of pair */
    dlb_pmd_bool mark_empty;   /**< mark empty PMD blocks with SMPTE 337m NULL burst? */
    unsigned int start;        /**< start channel index */

    uint8_t *data;             /**< data to write/buffer to write to */
    size_t databits;           /**< size of data to write/capacity of buffer to write */
    size_t vsync_offset;       /**< next vsync offset in samples, or -1 if not known */
    size_t padding;            /**< padding samplesremaining */
    size_t stride;             /**< PCM stride */
    size_t framelen;           /**< SMPTE 337m frame length in samples */

    next_block next;           /**< callback to retrieve next block */
    void *nextarg;             /**< client argument to callback */
};


/**
 * @brief set up an S337m PMD object
 */
void
pmd_s337m_init
    (pmd_s337m *s337m              /**< structure to initialize */
    ,unsigned int stride           /**< PCM channel stride */
    ,next_block next               /**< callback to retrieve next PMD block */
    ,void *nextarg                 /**< user-supplied argument to callback */
    ,dlb_pmd_bool pair             /**< is this a SMPTE 337m pair or a channel? */
    ,unsigned int start            /**< index of channel, or 1st channel of pair */
    ,dlb_pmd_bool mark_empty_block /**< highlight empty PMD blocks with NULL data burst? */
    );


/**
 * @brief encode a block of KLV into SMPTE 337m
 *
 * Note that the input PCM is assumed to be the entire channel set.
 * Typically for legacy workflows, the first sample will correspond to
 * data for the Left channel.
 *
 * PCM is grouped into sequences of "sample sets", sometimes called lines,
 * or just 'sample' depending on context.  Each sample set corresponds to
 * one time period, and contains one audio sample for each target speaker,
 * Left, Right, Center, etc.
 *
 * The 'stride' of a sample set is the number of samples that you have
 * to skip before you get the next sample destined for the same
 * speaker.  If pcm[0] is the first sample for Left, then the second
 * sample will occur at pcm[stride], and the nth at pcm[n*stride].
 * Often, the stride will equal the number of samples, but it can be
 * larger.
 *
 * The pair_index indicates which of the samples in the sample set is
 * the sample pair which must be overwritten with SMPTE-337m wrapped
 * KLV metadata.  We specify that the KLV is on the last pair, so if
 * we give 8 channels of PCM, then the last two channels will be
 * overwritten with SMPTE 337m-wrapped KLV.  It is up to the application
 * to ensure that this is safe.
 *
 * We assume that the samples are "interleaved": i.e., the first
 * sample will typically be left, the next the sample for right at the
 * same time period, and so on.  The next left sample will occur
 * 'stride' samples after the first.
 */
uint32_t *                        /** @return pcm position after end */
pmd_s337m_wrap
    (pmd_s337m *s337m             /**< [in] SMPTE 337m state */
    ,uint32_t *pcm                /**< [in] PCM samples to overwrite */
    ,uint32_t *end                /**< [in] 1st sample after PCM block to write */
    );


/**
 * @brief extract KLV block from SMPTE 337m if possible
 *
 * If no data is available, then #datasize will be set to 0 
 */
uint32_t *                        /** @return pcm position after end */
pmd_s337m_unwrap
    (pmd_s337m *s337m             /**< [in] SMPTE 337m state */
    ,uint32_t *pcm                /**< [in] input PCM to read */
    ,uint32_t *end                /**< [in] 1st sample after PCM block to read */
    );
   

/**
 * @brief return the minimum frame size in samples for a given frame rate
 *
 * Some frame rates (29.97, 59.94, 119.88) have varying frame sizes.
 */
static inline
unsigned int                      /** @return min frame size in samples */
pmd_s337m_min_frame_size
   (dlb_pmd_frame_rate rate      /**< [in] frame rate */
   )
{
    switch (rate)
    {
        case DLB_PMD_FRAMERATE_2398:  return 2002; break;
        case DLB_PMD_FRAMERATE_2400:  return 2000; break;
        case DLB_PMD_FRAMERATE_2500:  return 1920; break;
        case DLB_PMD_FRAMERATE_2997:  return 1601; break;
        case DLB_PMD_FRAMERATE_3000:  return 1600; break;
        case DLB_PMD_FRAMERATE_5000:  return  960; break;
        case DLB_PMD_FRAMERATE_5994:  return  800; break;
        case DLB_PMD_FRAMERATE_6000:  return  800; break;
        case DLB_PMD_FRAMERATE_10000: return  480; break;
        case DLB_PMD_FRAMERATE_11988: return  400; break;
        case DLB_PMD_FRAMERATE_12000: return  400; break;
        default:                      return 1920; break;
    }
}
