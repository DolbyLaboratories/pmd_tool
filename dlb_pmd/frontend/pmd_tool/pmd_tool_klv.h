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
 * @file pmd_tool_klv.h
 * @brief KLV reader/writer functionality for pmd tool
 */

#include "dlb_pmd_klv.h"

/**
 * @brief parse a KLV file to ingest a PMD model
 */
int                                 /** @return 0 on success, 1 on failure */
klv_read
    (const char                 *filename   /**< [in]  name of file to ingest */
    ,dlb_pmd_model_combo        *model      /**< [out] destination struct for model */
    );


/**
 * @brief write an KLV file to given filename
 *
 * @todo - complete: only dumps audio objects for now
 */
int                                 /** @return 0 on success, 1 on failure */
klv_write
    (const char                 *filename   /**< [in] name of file to write */
    ,dlb_pmd_model_combo        *model      /**< [in] PMD model to write */
    ,dlb_klvpmd_universal_label  ul         /**< [in] SMPTE2109 Universal Label */
    );
