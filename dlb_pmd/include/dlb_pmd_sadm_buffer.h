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
 * @file dlb_pmd_sadm_buffer.h
 * @brief definitions for converting to and from Dolby-constrained S-ADM buffer
 */

#ifndef DLB_PMD_SADM_BUFFER_H
#define DLB_PMD_SADM_BUFFER_H

#include "dlb_pmd_api.h"
#include "dlb_pmd_sadm.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct dlb_pmd_sadm_buffer_reader dlb_pmd_sadm_buffer_reader;
typedef struct dlb_pmd_sadm_buffer_writer dlb_pmd_sadm_buffer_writer;

/**
 * @brief Calculate memory size needed to perform read and parse operation
 */
DLB_PMD_DLL_ENTRY
size_t            /** @return size of memory to allocate in bytes, or 0 if there was an error */
dlb_pmd_sadm_buffer_reader_query_mem(void);

/**
 * @brief Initialize reader structure
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success         /** @return 0 if reader was successfuly initialized, 1 otherwise */
dlb_pmd_sadm_buffer_reader_init
    (dlb_pmd_sadm_buffer_reader **reader  /**< [in] reader structure to initialize */
    ,void                        *mem     /**< [in] memory to use */
    );

/**
 * @brief helper routine to actually read and parse S-ADM from buffer
 *
 * Upon parse error, this function will return 1, and the error message
 * can be retrieved via dlb_pmd_error().
 *
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success          /** @return 0 if buffer read and parsed successfully, 1 otherwise */
dlb_pmd_sadm_buffer_read
   (dlb_pmd_sadm_buffer_reader  *reader             /**< [in] reader structure to use */
   ,const uint8_t               *input_buffer       /**< [in] buffer to read */
   ,size_t                       buffer_size        /**< [in] size of the buffer*/
   ,dlb_pmd_model_combo         *model              /**< [in][out] model to populate */
   ,dlb_pmd_bool                 use_common_defs    /**< [in] Use S-ADM common definitions? */
   ,dlb_pmd_sadm_error_callback  err                /**< [in] error callback */
   ,void                        *arg                /**< [in] user-parameter for error callback */
   );

/**
 * @brief Calculate memory size needed to perform write and compress operation
 */
DLB_PMD_DLL_ENTRY
size_t            /** @return size of memory to allocate in bytes, or 0 if there was an error */
dlb_pmd_sadm_buffer_writer_query_mem(void);

/**
 * @brief Initialize writer structure
 */
dlb_pmd_success         /** @return 0 if writer was successfuly initialized, 1 otherwise */
dlb_pmd_sadm_buffer_writer_init
    (dlb_pmd_sadm_buffer_writer **writer  /**< [in] reader structure to initialize */
    ,void                        *mem     /**< [in] memory to use */
    );

/**
 * @brief helper routine to write S-ADM to the buffer
 *
 * Upon error, this function will return 1
 *
 */
DLB_PMD_DLL_ENTRY
dlb_pmd_success          /** @return 0 if buffer written and compressed successfully, 1 otherwise */
dlb_pmd_sadm_buffer_write
   (dlb_pmd_sadm_buffer_writer  *writer             /**< [in] writer structure to use */
   ,uint8_t                     *input_buffer       /**< [in] buffer to populate */
   ,size_t                       buffer_size        /**< [in] size of the buffer*/
   ,dlb_pmd_model_combo         *model              /**< [in] model to serialize */
   ,dlb_pmd_bool                 compression        /**< [in] 1 for compression, 0 for plain text */
   );


#ifdef __cplusplus
}
#endif

#endif /* DLB_PMD_SADM_BUFFER_H */

