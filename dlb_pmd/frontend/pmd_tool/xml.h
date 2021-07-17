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
 * @file xml.h
 * @brief XML reader/writer functionality for pmd tool
 */

#include "dlb_pmd_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief parse an XML file to ingest a PMD model
 */
int                                 /** @return 0 on success, 1 on failure */
xml_read
    (const char             *filename           /**< [in] name of file to ingest */
    ,dlb_pmd_model_combo    *model              /**< [out] destination struct for model */
    ,dlb_pmd_bool            strict             /**< [in] strict XML checking? */
    ,dlb_pmd_bool            use_common_defs    /**< [in] use ADM common definitions? */
    );


/**
 * @brief write an XML file to given filename
 */
int                                 /** @return 0 on success, 1 on failure */
xml_write
    (const char             *filename  /**< [in] name of file to write */
    ,dlb_pmd_model_combo    *model     /**< [in] PMD model to write */
    ,dlb_pmd_bool            sadm_out  /**< [in] generate serial ADM instead */
    );

#ifdef __cplusplus
}
#endif
