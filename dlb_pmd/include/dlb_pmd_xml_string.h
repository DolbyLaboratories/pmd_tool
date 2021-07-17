/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2017-2020 by Dolby Laboratories,
 *                Copyright (C) 2017-2020 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

/**
 * @file dlb_pmd_xml_string.h
 * @brief process XML embedded within a string
 */

#ifndef DLB_PMD_XML_STRING_H
#define DLB_PMD_XML_STRING_H

#include "dlb_pmd_xml.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief helper routine to write PMD XML to a string
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                /** @return 0 if string written successfully, 1 otherwise */
dlb_xmlpmd_string_write
   (const dlb_pmd_model  *pmd_model     /**< [in] PMD model struct to write */
   ,char                 *data          /**< [in/out] data buffer to hold written XML */
   ,size_t               *size          /**< [in/out] in: capacity of buffer, out: size of XML */
   );



/**
 * @brief helper routine to read and parse PMD XML from a string
 *
 * Upon parse error, this function will return 1, and the error message
 * can be retrieved via dlb_pmd_error().
 *
 * see the note on #dlb_xmlpmd_parse for an explanation of the #strict
 * field.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                /** @return 0 if string read and parsed successfully, 1 otherwise */
dlb_xmlpmd_string_read
   (const char                *data          /**< [in] data to read */
   ,size_t                     size          /**< [in] length of data */
   ,dlb_pmd_model             *pmd_model     /**< [in] PMD model struct to populate */
   ,dlb_pmd_bool               strict        /**< [in] apply strict checking? */
   ,dlb_xmlpmd_error_callback  err           /**< [in] error callback */
   ,void                      *arg           /**< [in] user-parameter for err callback */
   ,unsigned int              *error_line    /**< [in] error line */
   );


#ifdef __cplusplus
}
#endif

#endif /* DLB_PMD_XML_STRING_H */
