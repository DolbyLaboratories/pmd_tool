/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2019-2021 by Dolby Laboratories,
 *                Copyright (C) 2019-2021 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

/**
 * @file dlb_pmd_sadm_file.h
 * @brief definitions for converting to and from Dolby-constrained S-ADM files
 */

#ifndef DLB_PMD_SADM_FILE_H
#define DLB_PMD_SADM_FILE_H

#include "dlb_pmd_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief type of error callback
 */
typedef
void
(*dlb_pmd_sadm_error_callback)
    (const char *msg
    ,void *arg
    );

/**
 * @brief decide whether the given XML file contains S-ADM or not
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_bool                 /** @return 1 if the file very likely contains S-ADM XML, 0 otherwise */
dlb_xmlpmd_file_is_sadm
     (const char *filename   /**< [in] name of file to check */
     );


/**
 * @brief helper routine to actually read and parse PMD from file
 *
 * Upon parse error, this function will return 1, and the error message
 * can be retrieved via dlb_pmd_error().
 *
 * see the note on #dlb_xmlpmd_parse for an explanation of the #strict
 * field.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success          /** @return 0 if file read and parsed successfully, 1 otherwise */
dlb_pmd_sadm_file_read
   (const char                  *filename           /**< [in] file to read */
   ,dlb_pmd_model_combo         *model              /**< [in] model to populate */
   ,dlb_pmd_bool                 use_common_defs    /**< [in] Use S-ADM common definitions? */
   ,dlb_pmd_sadm_error_callback  err                /**< [in] error callback */
   ,void                        *arg                /**< [in] user-parameter for error callback */
   );


/**
 * @brief write a PMD model to a file in S-ADM XML format
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                     /** @return 0 on success, 1 on failure */
dlb_pmd_sadm_file_write
   (const char              *filename       /**< [in] file to write */
   ,dlb_pmd_model_combo     *model          /**< [in] model to convert to sADM and write */
   );


#ifdef __cplusplus
}
#endif

#endif /* DLB_PMD_SADM_FILE_H */
