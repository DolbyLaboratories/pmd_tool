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
 * @file dlb_pmd_xml.h
 * @brief definitions for reading and writing XML
 */

#ifndef DLB_PMD_XML_H
#define DLB_PMD_XML_H

#include "dlb_pmd_api.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @def DLB_PMD_XML_STRICT
 * @brief symbol constant indicating strict XML checking
 */
#define DLB_PMD_XML_STRICT (1)


/**
 * @brief type of error callback
 */
typedef
void
(*dlb_xmlpmd_error_callback)
    (const char *msg
    ,void *arg
    );


/**
 * @brief callback that is invoked to read next line of data
 */
typedef
char *
(*dlb_xmlpmd_line_callback)
    (void *arg
    );


/**
 * @brief callback that is intended to retrieve more write buffer
 *
 * This will be invoked by the xml writing routine whenever it needs
 * more buffer, and also to deliver the final buffer for writing.  In
 * the End of File case, buf should be NULL indicating that no new
 * buffers are required.
 */
typedef
int  /** @return 1 on success, 0 on failure */
(*dlb_xmlpmd_get_buffer)
    (void    *arg           /**< [in] client-supplied parmeter */
    ,char    *pos           /**< [in] current write position of previous buffer */
    ,char   **buf           /**< [out] start of next buffer position, NULL for final write */
    ,size_t  *capacity      /**< [out] capacity of next buffer */
    );


/**
 * @brief decide whether the given XML buffer contains PMD or not
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_bool              /** @return 1 if buffer has ProfessionalMetadata tag, 0 otherwise */
dlb_xmlpmd_is_pmd
    (const char *buffer   /**< [in] XML buffer */
    ,size_t length        /**< [in] length of data in XML buffer */
    );


/**
 * @brief parse incoming XML
 *
 * The XML parser expects uses a callback to ask for more input data.
 * When it comes to writing errors, it will invoke the error callback
 * multiple times to give a 'stack trace' of XML tags.
 *
 * Upon parse error, this function will return 1, and the error message
 * can be retrieved via dlb_pmd_error().
 *
 * The parser attempts to verify validity of all fields, including
 * the presentation config, which is a string that needs to match the
 * format
 *
 *   <speaker config> [ME|CM] (+<n>(D|VDS|VO|SS|O|EI|EA))*
 *
 * which is more precisely defined in the schema regular expression.
 * However, an XML schema cannot express semantic constraints, such
 * that the meaning of the presentation config field exactly
 * represents the shape of the presentation.
 *
 * If #strict checking is set to 1, then the parser will require that
 * presentation config strings describe their presentation exactly;
 * otherwise it will allow partial descriptions, e.g., just the
 * speaker config.  In either case, incorrect information will be
 * rejected (e.g., if we choose to specify the objects, but do so
 * incorrectly, that will be regarded as an error).
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                    /** @return 0 on success, 1 on failure */
dlb_xmlpmd_parse
    (dlb_xmlpmd_line_callback lcb  /**< [in] get-more-input callback */
    ,dlb_xmlpmd_error_callback ecb /**< [in] callback to write error messages */
    ,void *cbarg                   /**< [in] client-supplied argument to the callbacks */
    ,dlb_pmd_model *model          /**< [in] model to populate */
    ,dlb_pmd_bool strict           /**< [in] apply strict checking? */
    );


/**
 * @brief write a model in XML format to a sequence of buffers
 *
 * The XML writer repeatedly asks for sequences of output buffers
 * to fill.  When the callback is invoked, it is safe to write the
 * existing data to file.
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success                     /** @return 0 on success, 1 on failure */
dlb_xmlpmd_write
   (      dlb_xmlpmd_get_buffer gb  /**< [in] callback when writer needs more output space */
   ,      unsigned int indent       /**< [in] initial indentation level */
   ,      void *cbarg               /**< [in] client-supplied callback argument */
   ,const dlb_pmd_model *model      /**< [in] model to write */
   );


#ifdef __cplusplus
}
#endif

#endif /* DLB_PMD_XML_H */
