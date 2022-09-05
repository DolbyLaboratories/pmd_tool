/************************************************************************
 * dlb_pmd
 * Copyright (c) 2020 - 2022, Dolby Laboratories Inc.
 * Copyright (c) 2022, Dolby International AB.
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
