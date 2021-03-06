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

#ifndef S337M_SADM_BITSTREAM_ENCODER_H_
#define S337M_SADM_BITSTREAM_ENCODER_H_


/**
 * @file sadm_bitstream_encoder.h
 * @brief definitions for generating an sADM bitstream suitable for embedding
 * within SMPTE-337m encoded PCM.
 */


#include "pmd_smpte_337m.h"
#include "dlb_pmd_sadm.h"

#include "dlb_pmd/include/dlb_pmd_lib_dll.h"

#define TEST_DLL_ENTRY DLB_DLL_ENTRY


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief abstract type of sADM bitstream encoder
 */
typedef struct
{
    char xmlbuf[MAX_DATA_BYTES * DLB_PMD_SADM_XML_COMPRESSION]; /**< sADM precompression buffer */
    size_t size;
    dlb_pmd_sadm_writer *w;
} sadm_bitstream_encoder;


/**
 * @brief determine memory requirements for the sADM bitstream encoder
 */
TEST_DLL_ENTRY
size_t                                    /** @return size of memory required */
sadm_bitstream_encoder_query_mem
    (dlb_pmd_model_constraints *limits    /**< [in] PMD model limits */
    );


/**
 * @brief initialize the sADM bitstream encoder
 */
TEST_DLL_ENTRY
dlb_pmd_success
sadm_bitstream_encoder_init
    (dlb_pmd_model_constraints *limits
    ,void *mem
    ,sadm_bitstream_encoder **gen
    );


/**
 * @brief helper function to compress the encoder's XML buffer to the
 * given byte buffer
 */
TEST_DLL_ENTRY
int                                /** @return bytes used */
compress_sadm_xml
   (sadm_bitstream_encoder *enc    /**< [in] bitstream encoder */
   ,uint8_t *buf                   /**< [in] output compression buffer */
   );


/**
 * @brief function to encapsulate the process of generating an sADM bitstream
 */
int                                 /** @return bytes used, or 0 if none */
sadm_bitstream_encoder_encode
    (pmd_s337m *s337m               /**< [in] S337m abstraction */
    ,sadm_bitstream_encoder *enc    /**< [in] bitstream encoder */
    ,dlb_pmd_model *model           /**< [in] model to write */
    ,dlb_pmd_frame_rate rate        /**< [in] frame rate */
    ,uint8_t *outbuf                /**< [in] compression buffer */
);


#ifdef __cplusplus
}
#endif


#endif /* S337M_SADM_BITSTREAM_ENCODER_H_ */
