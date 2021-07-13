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
 * @file dlb_pmd_sadm.h
 * @brief definitions for converting to and from Dolby-constrained sADM
 */

#ifndef DLB_PMD_SADM_H
#define DLB_PMD_SADM_H

#include "dlb_pmd_api.h"
#include "dlb_pmd_xml.h"
#include "sadm/dlb_sadm_model_type.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @def DLB_PMD_SADM_XML_COMPRESSION (10)
 * @brief approximate compression factor of XML by zlib - used to size buffers
 */
#define DLB_PMD_SADM_XML_COMPRESSION (10)


/**
 * @brief decide whether the given XML buffer contains serial ADM or not
 */
DLB_DLL_ENTRY
dlb_pmd_bool              /** @return 1 if buffer has coreMetadata tag, 0 otherwise */
dlb_xmlpmd_is_sadm
    (const char *buffer   /**< [in] XML buffer */
    ,size_t length        /**< [in] length of data in XML buffer */
    );


/**
 * @brief abstract type of structure used to read sADM and convert to PMD
 */
typedef struct dlb_pmd_sadm_reader dlb_pmd_sadm_reader;


/**
 * @brief determine memory requirements for #dlb_pmd_sadm_reader
 *
 * This depends on the capacity of the model we wish to store 
 */
DLB_DLL_ENTRY
size_t                                   /** @return required memory size in bytes */
dlb_pmd_sadm_reader_query_mem
    (dlb_pmd_model_constraints *limits   /**< [in] target model constraints */
    );
    

/**
 * @brief create an sADM reader
 */
DLB_DLL_ENTRY
dlb_pmd_success
dlb_pmd_sadm_reader_init
    (dlb_pmd_sadm_reader **rdr           /**< [out] the new sADM reader */
    ,dlb_pmd_model_constraints *limits   /**< [in] target model constraints */
    ,void *mem                           /**< [in] memory for the reader */
    );


/**
 * @brief finish an sADM reader
 */
DLB_DLL_ENTRY
void
dlb_pmd_sadm_reader_finish
    (dlb_pmd_sadm_reader *rdr     /**< [in] sADM reader to finish */
    );


/**
 * @brief parse incoming sADM (which is another XML format)
 *
 * Note that since sADM is an XML format, we re-use definitions from
 * the PMD XML format.
 */
DLB_DLL_ENTRY
dlb_pmd_success                     /** @return PMD_SUCCESS if ok, PMD_FAIL otherwise */
dlb_pmd_sadm_reader_read
    (dlb_pmd_sadm_reader *rdr       /**< [in] sADM reader to finish */
    ,dlb_pmd_model *model           /**< [in] PMD model to populate */
    ,const char *title              /**< [in] required title for generated PMD model */
    ,dlb_xmlpmd_line_callback lcb   /**< [in] get new buffer callback */
    ,dlb_xmlpmd_error_callback ecb  /**< [in] report error callback */
    ,void *cbarg                    /**< [in] client argument for callbacks */
    );


/**
 * @brief extract the sADM model from a sADM reader object
 */
DLB_DLL_ENTRY
const dlb_sadm_model *              /** @return sADM model or NULL if error */
dlb_pmd_sadm_reader_get_sadm_model
    (dlb_pmd_sadm_reader *rdr       /**< [in] sADM reader from which to get the sADM model */
    );


/**
 * @brief abstract type of PMD to sADM writer
 */
typedef struct dlb_pmd_sadm_writer dlb_pmd_sadm_writer;


/**
 * @brief determine memory requirements for #dlb_pmd_sadm_writer
 *
 * This depends on the capacity of the model we wish to store 
 */
DLB_DLL_ENTRY
size_t                                   /** @return required memory size in bytes */
dlb_pmd_sadm_writer_query_mem
    (dlb_pmd_model_constraints *limits   /**< [in] target model constraints */
    );
    

/**
 * @brief create an sADM writer
 */
DLB_DLL_ENTRY
dlb_pmd_success
dlb_pmd_sadm_writer_init
    (dlb_pmd_sadm_writer **w             /**< [out] the new sADM writer */
    ,dlb_pmd_model_constraints *limits   /**< [in] target model constraints */
    ,void *mem                           /**< [in] memory for the writer */
    );


/**
 * @brief finish an sADM writer
 */
DLB_DLL_ENTRY
void
dlb_pmd_sadm_writer_finish
    (dlb_pmd_sadm_writer *w       /**< [in] sADM writer to finish */
    );


/**
 * @brief write a model in sADM-XML format to a sequence of buffers
 *
 * The sADM writer repeatedly asks for sequences of output buffers
 * to fill.  When the callback is invoked, it is safe to write the
 * existing data to file.
 */
DLB_DLL_ENTRY
dlb_pmd_success                /** @return 0 on success, 1 on failure */
dlb_pmd_sadm_writer_write
   (dlb_pmd_sadm_writer *w     /**< [in] PMD to sADM writer */
   ,const dlb_pmd_model *model /**< [in] PMD model to write */
   ,dlb_xmlpmd_get_buffer gb   /**< [in] callback when writer needs more output space */
   ,unsigned int indent        /**< [in] initial indentation level */
   ,void *cbarg                /**< [in] client-supplied callback argument */
   );


/**
 * @brief extract the sADM model from a sADM writer object
 */
DLB_DLL_ENTRY
const dlb_sadm_model *          /** @return sADM model or NULL if error */
dlb_pmd_sadm_writer_get_sadm_model
   (dlb_pmd_sadm_writer *w     /**< [in] sADM writer from which to extract the sADM model */
   );


#ifdef __cplusplus
}
#endif

#endif /* DLB_PMD_SADM_H */
