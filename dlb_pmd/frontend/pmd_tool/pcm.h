/************************************************************************
 * dlb_pmd
 * Copyright (c) 2021, Dolby Laboratories Inc.
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
 * @file pcm.h
 * @brief PCM Wave-file reader/writer functionality for pmd tool
 */

#include "dlb_pmd_pcm.h"


/**
 * @def MAX_FILENAME_LEN
 * @brief maximum size of filenames
 */
#define MAX_FILENAME_LEN (256)


/**
 * @brief extract SMPTE 337m-wrapped KLV from input PCM wave file
 */
int                                      /** @return 0 on success, 1 on failure */
pcm_read
    (const char             *infile     /**< [in]  name of PCM file to ingest */
    ,const char             *logfile    /**< [in]  name of log file -- may be NULL or empty */
    ,dlb_pmd_frame_rate      rate       /**< [in]  video frame rate */
    ,unsigned int            chan       /**< [in]  channel locating smpte 337m to decode */
    ,dlb_pmd_bool            is_pair    /**< [in]  1: decode pair, 0: decode single channel */
    ,size_t                  vsync      /**< [in]  number of samples until vsync */
    ,size_t                  skip       /**< [in]  number of samples to skip (to simulate random access) */
    ,dlb_pmd_model_combo    *model      /**< [out] destination struct for model */
    );


/**
 * @brief overwrite final two channels of PCM with SMPTE 337m-wrapped KLV
 */
int                                /** @return 0 on success, 1 on failure */
pcm_write
    (const char                 *infile             /**< [in] name of PCM file to read */
    ,const char                 *outfile            /**< [in] name of file to write */
    ,dlb_pmd_frame_rate          rate               /**< [in] video frame rate */
    ,unsigned int                chan               /**< [in] index of channel being overwritten */
    ,dlb_pmd_bool                is_pair            /**< [in] S337m encode a pair? or a single channel? */
    ,dlb_klvpmd_universal_label  ul                 /**< [in] SMPTE 2109 universal label */
    ,dlb_pmd_bool                mark_empty_blocks  /**< [in] mark empty PMD blocks with SMPTE 337m NULL data bursts
                                                      *       (default is to leave them silent) */
    ,dlb_pmd_bool                sadm               /**< [in] generate sADM instead of PMD? */
    ,dlb_pmd_model_combo        *model              /**< [in] PMD model to write */
    );
