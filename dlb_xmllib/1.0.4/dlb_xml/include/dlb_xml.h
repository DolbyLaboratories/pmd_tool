/************************************************************************
 * dlb_xmllib
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

/** @file dlb_xml.h */

#ifndef DLB_XML_H
#define DLB_XML_H

#define DLB_XML_VERSION_MAJOR   1
#define DLB_XML_VERSION_MINOR   0
#define DLB_XML_VERSION_UPDATE  4

/* Error codes */
#define DLB_XML_SUCCESS         0 /**< No Error */
#define DLB_XML_ERROR           1 /**< Error */
#define DLB_XML_INVALID_POINTER 2 /**< Invalid pointer input parameter */

#if defined(_MSC_VER) && !defined(inline)
#  define inline __inline
#endif

/** @brief DLB_XML Version Parameters structure is the container of version information on the
 *  components of the dlb_xml library. */
typedef struct dlb_xml_version_s
{
    int version_major;      /**< major version number                   */
    int version_minor;      /**< minor version number                   */
    int version_update;     /**< update (build) version number          */
} dlb_xml_version;

/**
 *  @brief Returns the version on the dlb_xml library
 *  
 *  USAGE:
 *      Pass a pointer to a strcture 'dlb_xml_version' into this function, 
 *      and it will be filled with the version information of this library.
 * 
 *  RETURN:
 *      DLB_XML_SUCCESS - on success
 *      DLB_XML_INVALID_POINTER - when p_version is NULL
 */
int 
dlb_xml_query_version
    ( dlb_xml_version *p_version   /**< [out] version information of this library */
    );

/**
 * @brief Simple XML parser
 * 
 * USAGE:
 *     Provide a callback function to read the XML line by line, and 
 *     two callback functions to handle parsed XML elements and attributes.
 *     A caller-side context can be provided for the callback functions.
 * 
 * LIMITATIONS:
 *     Non-validating parser, ignoring external entities
 *     Limited error and syntax checking
 *     Entities are not supported
 *     Maximum significant characters in element tags: 34
 *     Maximum text length per element: 2048
 * 
 *  RETURN:
 *      DLB_XML_SUCCESS - on success
 *      DLB_XML_ERROR - on parsing error
 *      DLB_XML_INVALID_POINTER - when line_callback is NULL
 * 
 * @param p_context: Context, passed back when callbacks are called
 *   Can be NULL if not needed by the callback functions.
 * @param line_callback: Callback to load the next line. 
 *   Has to return pointer to first char of next line, 
 *   or NULL when no more lines are available.
 * @param element_callback: Called when element is opened (text is NULL,
 *   before any attribute callbacks) or closed (with text enclosed by element).
 *   Can be NULL.
 * @param attribute_callback: Called for each attribute inside an element's
 *   open tag or declaration. Can be NULL.
 * @param error_callback: Called to report parsing errors.  Can be NULL
 */
int dlb_xml_parse2
    ( void *p_context
    , char *(*line_callback)      (void *p_context)
    , int   (*element_callback)   (void *p_context, char *tag, char *text)
    , int   (*attribute_callback) (void *p_context, char *tag, char *attribute, char *value)
    , void  (*error_callback)     (void *p_context, char *msg)
    );


/**
 * @brief backwards-compatible interface to the parser
 */
static inline
int dlb_xml_parse
    ( void *p_context
    , char *(*line_callback)      (void *p_context)
    , int   (*element_callback)   (void *p_context, char *tag, char *text)
    , int   (*attribute_callback) (void *p_context, char *tag, char *attribute, char *value)
    )
{
    return dlb_xml_parse2(p_context, line_callback, element_callback, attribute_callback, NULL);
}



#endif /* DLB_XML_H */

