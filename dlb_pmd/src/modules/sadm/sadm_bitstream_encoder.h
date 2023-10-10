/************************************************************************
 * dlb_pmd
 * Copyright (c) 2023, Dolby Laboratories Inc.
 * Copyright (c) 2023, Dolby International AB.
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
 * @brief definitions for generating an S-ADM bitstream suitable for embedding
 * within SMPTE-337m encoded PCM.
 */


#include "pmd_smpte_337m.h"
#include "dlb_pmd_sadm.h"

#include "dlb_pmd/include/dlb_pmd_lib_dll.h"

#define TEST_DLL_ENTRY DLB_PMD_DLL_ENTRY


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief S-ADM bitstream encoder type
 */
typedef struct
{
    char                         xmlbuf[DLB_PMD_SADM_MAX_XML_SIZE]; /**< S-ADM precompression buffer */
    size_t                       size;
} sadm_bitstream_encoder;


/**
 * @brief determine memory requirements for the S-ADM bitstream encoder
 */
TEST_DLL_ENTRY
size_t                              /** @return size of memory required */
sadm_bitstream_encoder_query_mem
    (void
    );


/**
 * @brief initialize the S-ADM bitstream encoder
 */
TEST_DLL_ENTRY
dlb_pmd_success
sadm_bitstream_encoder_init
    (void                           *mem
    ,sadm_bitstream_encoder        **enc
    );


/**
 * @brief helper function to compress the encoder's XML buffer to the
 * given byte buffer
 */
TEST_DLL_ENTRY
int                                     /** @return bytes used, or zero if error */
compress_sadm_xml
    (sadm_bitstream_encoder *enc        /**< [in] bitstream encoder */
    ,uint8_t                *buf        /**< [in] output compression buffer */
    ,size_t                  buf_size   /**< [in] size of the output buffer */
    );


/**
 * @brief function to encapsulate the process of generating a compressed sADM payload
 */
TEST_DLL_ENTRY
int                                 /** @return bytes used, or 0 if none */
sadm_bitstream_encoder_payload
    (sadm_bitstream_encoder     *enc            /**< [in]  bitstream encoder */
    ,const dlb_adm_core_model   *model          /**< [in]  model to write */
    ,uint8_t                    *outbuf         /**< [out] compression buffer - must be large enough! (suggested: >= 12012) */
    );

/**
 * @brief function to encapsulate the process of generating a compressed sADM payload
 */
TEST_DLL_ENTRY
int                                 /** @return bytes used, or 0 if none */
sadm_bitstream_encoder_payload_ext
    (sadm_bitstream_encoder     *enc            /**< [in]  bitstream encoder */
    ,const dlb_adm_core_model   *model          /**< [in]  model to write */
    ,dlb_pmd_bool                compress       /**< [in]  1 for compression, 0 for plain text */
    ,uint8_t                    *outbuf         /**< [out] compression buffer - must be large enough! (suggested: >= 12012) */
    ,size_t                      outbuf_size    /**< [in] size of the output buffer */
    );


/**
 * @brief function to encapsulate the process of generating an sADM bitstream
 */
int                                 /** @return bytes used, or 0 if none */
sadm_bitstream_encoder_encode
    (pmd_s337m                  *s337m  /**< [in] S337m abstraction */
    ,sadm_bitstream_encoder     *enc    /**< [in] bitstream encoder */
    ,const dlb_adm_core_model   *model  /**< [in] model to write */
    ,dlb_pmd_frame_rate          rate   /**< [in] frame rate */
    ,uint8_t                    *outbuf /**< [in] compression buffer */
);


#ifdef __cplusplus
}
#endif


#endif /* S337M_SADM_BITSTREAM_ENCODER_H_ */
