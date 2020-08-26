/************************************************************************
 * dlb_pmd
 * Copyright (c) 2020, Dolby Laboratories Inc.
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
 * @brief definitions for converting to and from Dolby-constrained sADM files
 */

#ifndef DLB_SADM_FILE_H
#define DLB_SADM_FILE_H

#include "dlb_sadm_model.h"
#include "dlb_sadm_reader.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief check whether a file contains serial ADM
 */
DLB_DLL_ENTRY
dlb_pmd_bool                  /** @return 1 if the file is plausibly a serial ADM file */
dlb_sadm_file_is_sadm
     (const char *filename    /**< [in] file to check */
     );


/**
 * @brief helper routine to actually read and parse serial ADM from file
 *
 * Upon parse error, this function will return 1, and the error message
 * can be retrieved via dlb_pmd_error().
 */
DLB_DLL_ENTRY
dlb_pmd_success          /** @return 0 if file read and parsed successfully, 1 otherwise */
dlb_sadm_file_read
   (const char                *filename      /**< [in] file to read */
   ,dlb_sadm_model            *model         /**< [in] sADM model struct to populate */
   ,dlb_xmlpmd_error_callback  err           /**< [in] error callback */
   ,void                      *arg           /**< [in] user-parameter for err callback */
   );


/**
 * @brief write a model in sADM-XML format to a sequence of buffers
 *
 * The sADM writer repeatedly asks for sequences of output buffers
 * to fill.  When the callback is invoked, it is safe to write the
 * existing data to file.
 */
DLB_DLL_ENTRY
dlb_pmd_success                      /** @return 0 on success, 1 on failure */
dlb_sadm_file_write
      (const char     *filename      /**< [in] file to write */
      ,dlb_sadm_model *model         /**< [in] sADM model struct data to write to file */
      );


#ifdef __cplusplus
}
#endif

#endif /* DLB_SADM_FILE_H */
