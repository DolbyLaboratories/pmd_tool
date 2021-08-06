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
 * @file dlb_pmd_capture.h
 * @brief Capture metadata from a frame of audio input
 */

#include "dlb_pmd_types.h"
#include "dlb_pmd_lib_dll.h"

#ifndef DLB_PMD_CAPTURE_H
#define DLB_PMD_CAPTURE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief API function return status codes
 */
typedef enum
{
    DLB_PMD_FRAME_CAPTOR_STATUS_OK,                 /**< Olle Korrect! */
    DLB_PMD_FRAME_CAPTOR_STATUS_ERROR,              /**< An error occurred. */
    DLB_PMD_FRAME_CAPTOR_STATUS_NULL_POINTER,       /**< A required pointer argument was NULL. */
    DLB_PMD_FRAME_CAPTOR_STATUS_OUT_OF_MEMORY,      /**< malloc() failed to return memory. */
    DLB_PMD_FRAME_CAPTOR_STATUS_NOT_FOUND,          /**< Something was not found. */
} DLB_PMD_FRAME_CAPTOR_STATUS;


/**
 * @brief Frame captor instance handle
 */
typedef struct dlb_pmd_frame_captor dlb_pmd_frame_captor;


/**
 * @brief Data blob descriptor -- "what's in the blob?"
 */
typedef struct 
{
    uint16_t             number_of_samples;         /**< Number of samples in the data blob -- must be enough
                                                         to contain at least one complete video frame. */
    uint16_t             number_of_channels;        /**< Number of channels. */
    uint8_t              bit_depth;                 /**< Bit depth -- 24 or 32. */
    dlb_pmd_bool         big_endian;                /**< Non-zero if data is in big-endian form. */
} dlb_pmd_blob_descriptor;


/**
 * @brief Query how much memory is needed for a frame captor instance, in bytes.
 */
DLB_PMD_DLL_ENTRY
int                                                 /** @return Status code. */
dlb_pmd_frame_captor_query_memory_size
    (size_t *sz                                     /**< [out] Memory required. */
    );


/**
 * @brief Initialize a frame captor instance.  Memory may be passed in from an external
 * source, and must be of sufficient size; if not given, uses malloc().
 */
DLB_PMD_DLL_ENTRY
int                                                 /** @return Status code. */
dlb_pmd_frame_captor_open
    (dlb_pmd_frame_captor   **captor                /**< [out] Frame captor instance pointer. */
    ,void                    *memory                /**< [in]  Memory for frame captor instance, or NULL. */
    );

/**
 * @brief Using the descriptor to interpret the data blob, decode the first full frame
 * of metadata into a PMD metadata set.  The metadata set will remain valid until the
 * next call to capture, or until the frame captor is closed.
 */
DLB_PMD_DLL_ENTRY
int                                                 /** @return Status code. */
dlb_pmd_frame_captor_capture
    (dlb_pmd_metadata_set           **metadata_set  /**< [out] Metadata set pointer for result. */
    ,dlb_pmd_frame_captor            *captor        /**< [in]  Frame captor instance. */
    ,const dlb_pmd_blob_descriptor   *descriptor    /**< [in]  Data blob descriptor. */
    ,const void                      *data_blob     /**< [in]  Data blob. */
    );


/**
 * @brief Close a frame captor instance, freeing any internally-allocated resources.  Sets the
 * instance pointer to NULL.
 */
DLB_PMD_DLL_ENTRY
int                                                 /** @return Status code. */
dlb_pmd_frame_captor_close
    (dlb_pmd_frame_captor   **captor                /**< [in/out] Frame captor instance pointer. */
    );

#ifdef __cplusplus
}
#endif

#endif
