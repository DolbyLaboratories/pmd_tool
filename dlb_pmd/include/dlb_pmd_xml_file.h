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
 * @file dlb_pmd_xml_file.h
 * @brief stdio, FILE-based xml reading and writing routines
 */

#ifndef DLB_PMD_XML_FILE_H
#define DLB_PMD_XML_FILE_H

#include "dlb_pmd_xml.h"


#ifdef __cplusplus
extern "C" {
#endif


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
dlb_xmlpmd_file_read
   (const char                *filename      /**< [in] file to read */
   ,dlb_pmd_model             *model         /**< [in] PMD model struct to populate */
   ,dlb_pmd_bool               strict        /**< [in] apply strict checking? */
   ,dlb_xmlpmd_error_callback  err           /**< [in] error callback */
   ,void                      *arg           /**< [in] user-parameter for err callback */
   );


/**
 * @brief helper function to determine whether a file contains PMD XML or not
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_bool                 /** @return 1 if it contains <ProfessionalMetadata tag, 0 otherwise */
dlb_xmlpmd_file_is_pmd
     (const char *filename   /**< [in] name of file to check */
     );


/**
 * @brief helper function to write PMD model to an XML file
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                  /** @return 0 if file written successfully, 1 otherwise */
dlb_xmlpmd_file_write
   (const char          *filename   /**< [in] file to write */
   ,const dlb_pmd_model *model      /**< [in] PMD model struct data to write to file */
   );



#ifdef __cplusplus
}
#endif

#endif /* DLB_PMD_XML_FILE_H */
