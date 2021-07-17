/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2016-2021 by Dolby Laboratories,
 *                Copyright (C) 2016-2021 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

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
