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
 * @flle dlb_pmd_pcm.h
 * @brief PCM reader/writer
 *
 * This file contains the interface to routines to either:
 *
 * - adorn pure PCM with SMPTE 337m-wrapped KLV metadata (overwriting
 *    the final two channels)
 *
 * - read PCM with SMPTE 337m-wrapped KLV metadata and generate a model
 */

#ifndef DLB_PMD_PCM_H
#define DLB_PMD_PCM_H

#ifdef _MSC_VER
#  define __uint32_t uint32_t
#endif

#include <stdint.h>
#include "dlb_pmd_klv.h"
#include "dlb_pmd_api.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @def DLB_PCMPMD_BLOCK_SIZE (160)
 * @brief number of PCM sample sets in one block
 */
#define DLB_PCMPMD_BLOCK_SIZE (160)

/**
 * @def DLB_PMD_VSYNC_NONE
 * @brief symbolic constant indicating 'no sync point in next block'
 * 
 * both #dlb_pcmpmd_extract and #dlb_pcmpmd_augment need to know
 * whether there is a video sync point within the block they are
 * operating upon, and if so, where in the block it occurs. If no
 * such point occurs, use this value
 */
#define DLB_PMD_VSYNC_NONE (~0u)


/**
 * @brief abstract type of structure used to augment PCM with
 * KLV metadata in last two channels.
 */
typedef struct dlb_pcmpmd_augmentor dlb_pcmpmd_augmentor;


/**
 * @brief determine how much memory to provide to #dlb_pcmpmd_augmentor_init
 */
size_t                               /**< [out] size of memory in bytes */
dlb_pcmpmd_augmentor_query_mem
    (void
    );


/**
 * @brief initialize PCM augmentor
 *
 * Note that we assume:
 * 1. Audio sample rate is always 48 kHz.
 *
 * The augmentor can generate either 1- or 2- channel SMPTE 337m
 * wrapped PMD (SMPTE 2109, similar to KLV format). These follow
 * the subframe and frame-based encoding of SMPTE 337m.
 *
 * The augmentor will copy the source PCM to output PCM, overwriting the
 * selected channel or pair of channels with PMD.
 */
void
dlb_pcmpmd_augmentor_init
    (dlb_pcmpmd_augmentor **aug      /**< [in] PCM augmentor to initialize */
    ,dlb_pmd_model *model            /**< [in] PMD model */
    ,void *mem                       /**< [in] memory for PCM augmentor */
    ,dlb_pmd_frame_rate rate         /**< [in] video frame rate */
    ,dlb_klvpmd_universal_label ul   /**< [in] universal label */
    ,dlb_pmd_bool mark_pcm_blocks    /**< [in] mark empty PMD blocks with SMPTE 337m NULL data bursts */
    ,unsigned int numchannels        /**< [in] number of channels of PCM */
    ,unsigned int stride             /**< [in] channel stride */
    ,dlb_pmd_bool pmd_pair           /**< [in] 1 = pair of channels, 0 = single channel */
    ,unsigned int start              /**< [in] 1st pair or channel to write, (see #pmd_pair) */
    );

    
/**
 * @brief finalize PCM augmentor
 */
void
dlb_pcmpmd_augmentor_finish
    (dlb_pcmpmd_augmentor *aug       /**< [in] PCM augmentor to clean up */
    );


/**
 * @brief take a block of PCM and augment
 *
 * Note that PCM must be at least 20 bits, and have exactly
 * #DLB_PCMPMD_BLOCK_SIZE samples of audio for each channel.
 *
 * If this is not true, then behaviour is undefined. 
 *
 * Note that no blocks will be written until the first video
 * sync is detected, but the target pair will be zeroed out.
 *
 * Note that the PCM is assumed to be 24-bit samples, carried in a
 * 32-bit integer (because that is a handy machine datatype), such
 * that the 24 bits are shifted to the most significant bits.
 */
void
dlb_pcmpmd_augment
    (dlb_pcmpmd_augmentor *aug       /**< [in] PCM augmentor */
    ,uint32_t *pcm                   /**< [in/out] PCM channel buffer to modify */
    ,size_t num_samples              /**< [in] number of samples in current block */
    ,size_t video_sync               /**< [in] video frame sync occurs at given line,
                                       *       or #DLB_PMD_VSYNC_NONE if no frame sync. */
    );


/**
 * @brief abstract type of structure used to remove KLV metadata
 * from final two channels in PCM.
 */
typedef struct dlb_pcmpmd_extractor dlb_pcmpmd_extractor;


/**
 * @brief determine number of bytes required to intialize an extractor
 */
size_t                              /** @return number of bytes */
dlb_pcmpmd_extractor_query_mem
    (void
    );


/**
 * @brief initialize PCM extractor
 */
void
dlb_pcmpmd_extractor_init
    (dlb_pcmpmd_extractor **extptr  /**< [out] PCM extractor to initialize */
    ,void                 *mem      /**< [in] memory to use to initialize extractor */
    ,dlb_pmd_frame_rate    rate     /**< [in] video frame rate */
    ,unsigned int          chan     /**< [in] channel (or 1st channel of pair) to decode */
    ,unsigned int          nstride  /**< [in] PCM stride */
    ,dlb_pmd_bool          ispair   /**< [in] 1: extract a pair, 0: extract single channel */
    ,dlb_pmd_model        *model    /**< [in] model to populate */
    );


/**
 * @brief clean up resources
 */
void
dlb_pcmpmd_extractor_finish
    (dlb_pcmpmd_extractor *ext      /**< [in] PCM extractor to tidy up */
    );


/**
 * @brief unwrap one block of PCM of size #DLB_PCMPMD_BLOCK_SIZE
 *
 * This function assumes that the final two pairs of the incoming PCM
 * are SMPTE-337m wrapped KLV, and that there are exactly
 * #DLB_PCMPMD_BLOCK_SIZE samples per channels.
 *
 * Note that the PCM is expected to be 24-bit samples carried in
 * 32-bit machine words.
 *
 * Note that an non-zero return code indicates that the encoded
 * metadata failed to decode; the PCM can continue to be extracted.
 */
dlb_pmd_success                        /** @return 0 on success, non-zero otherwise */
dlb_pcmpmd_extract
    (dlb_pcmpmd_extractor *ext         /**< [in] PCM extractor struct */
    ,uint32_t             *pcm         /**< [in] PCM to unwrap */
    ,size_t                num_samples /**< [in] number of PCM sample sets */
    ,size_t                video_sync  /**< [in] video frame sync occurs at given line,
                                         *       or #DLB_PMD_VSYNC_NONE if no frame sync. */
    );


#ifdef __cplusplus
}
#endif

#endif /* DLB_PMD_PCM_H */
