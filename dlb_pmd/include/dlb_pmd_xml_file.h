/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2016-2020 by Dolby Laboratories,
 *                Copyright (C) 2016-2020 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

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
