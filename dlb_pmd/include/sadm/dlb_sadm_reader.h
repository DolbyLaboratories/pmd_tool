/************************************************************************
 * dlb_pmd
 * Copyright (c) 2021, Dolby Laboratories Inc.
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
 * @file dlb_sadm_reader.h
 * @brief definitions for reading serial ADM and populating a model
 */

#ifndef DLB_SADM_READER_H
#define DLB_SADM_READER_H

#include "dlb_sadm_model.h"
#include "dlb_pmd_xml.h"

#ifdef __cplusplus
extern "C" 
{
#endif    

/**
 * @brief type of sadm parser
 */
typedef struct dlb_sadm_reader dlb_sadm_reader;
    

/**
 * @brief query memory requirements
 */
DLB_DLL_ENTRY
size_t                               /** @return size of memory required in bytes */
dlb_sadm_reader_query_memory
    (dlb_sadm_counts *limits         /**< [in] model limits */
    );
    

/**
 * @brief initialize an sADM reader
 */
DLB_DLL_ENTRY
dlb_pmd_success                        /** @return success or failure */
dlb_sadm_reader_init
    (dlb_sadm_counts *limits           /**< [in] model limits */
    ,void *mem                         /**< [in] memory to use to init reader */
    ,dlb_sadm_reader **reader          /**< [out] reader */
    );
    

/**
 * @brief finalize an sADM reader
 */
DLB_DLL_ENTRY
void
dlb_sadm_reader_finish
    (dlb_sadm_reader *reader          /**< [in] reader to finish */
    );
    

/**
 * @brief decide whether the given XML buffer contains serial ADM or not
 */
DLB_DLL_ENTRY
dlb_pmd_bool              /** @return 1 if buffer has coreMetadata tag, 0 otherwise */
dlb_sadm_reader_check_xml
    (const char *buffer   /**< [in] XML buffer */
    ,size_t length        /**< [in] length of data in XML buffer */
    );


/**
 * @brief parse incoming sADM (which is another XML format)
 *
 * Note that since sADM is an XML format, we re-use definitions from
 * the PMD XML format.
 */
DLB_DLL_ENTRY
dlb_pmd_success                        
dlb_sadm_reader_read
   (dlb_sadm_reader *reader
   ,dlb_xmlpmd_line_callback lcb
   ,dlb_xmlpmd_error_callback ecb
   ,void *cbarg
   ,dlb_sadm_model *model
   );


#ifdef __cplusplus
}
#endif


#endif /* DLB_SADM_READER_H */

