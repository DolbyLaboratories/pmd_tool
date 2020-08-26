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

#ifndef S337M_SADM_BITSTREAM_DECODER_H_
#define S337M_SADM_BITSTREAM_DECODER_H_


/**
 * @file sadm_bitstream_decoder.h
 * @brief definitions for extracting a PMD model from an sADM bitstream embedded within
 * within SMPTE-337m encoded PCM.
 */

#include "pmd_smpte_337m.h"
#include "pmd_error_helper.h"
#include "dlb_pmd_sadm_string.h"

#include "dlb_pmd/include/dlb_pmd_lib_dll.h"

#define TEST_DLL_ENTRY DLB_DLL_ENTRY


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief status of sADM bitstream decoder
 */
typedef enum
{
    SADM_OK,
    SADM_PARSE_ERR,
    SADM_DECOMPRESS_ERR,
} sadm_dec_state;


/**
 * @brief callback function type for obtaining status of sADM bitstream decoder
 */
typedef void (*sadm_bitstream_dec_callback) (void *arg, sadm_dec_state state);


/**
 * @brief abstract type of sADM bitstream decoder
 */
typedef struct
{
    unsigned int error_line;
    dlb_pmd_model *model;

    char xmlbuf[MAX_DATA_BYTES * DLB_PMD_SADM_XML_COMPRESSION]; /**< sADM decompression buffer */
    size_t size;
    dlb_pmd_sadm_reader *r;
} sadm_bitstream_decoder;


/**
 * @brief determine memory requirements for the sADM bitstream decoder
 */
TEST_DLL_ENTRY
size_t                                    /** @return size of memory required */
sadm_bitstream_decoder_query_mem
    (dlb_pmd_model_constraints *limits    /**< [in] PMD model limits */
    );


/**
 * @brief initialize the sADM bitstream decoder
 */
TEST_DLL_ENTRY
dlb_pmd_success
sadm_bitstream_decoder_init
    (dlb_pmd_model_constraints *limits
    ,void *mem
    ,sadm_bitstream_decoder **dec
    );


/**
 * @brief helper function to compress the decoder's XML buffer to the
 * given byte buffer
 */
TEST_DLL_ENTRY
int                                /** @return bytes used */
decompress_sadm_xml
   (sadm_bitstream_decoder *dec    /**< [in] bitstream decoder */
   ,uint8_t *buf                   /**< [in] input compressed data */
   ,size_t datasize                /**< [in] size of compressed input in bytes */
   );


/**
 * @brief function to encapsulate the process of generating an sADM bitstream
 */
dlb_pmd_success                     /** @return 0 on success, 1 on failure */
sadm_bitstream_decoder_decode
    (sadm_bitstream_decoder *dec            /**< [in] bitstream decoder */
    ,uint8_t *bitstream                     /**< [in] bits to decode */
    ,size_t datasize                        /**< [in] number of bytes in bitstream */
    ,dlb_pmd_model *model                   /**< [in] model to write */
    ,sadm_bitstream_dec_callback callback   /**< [in] status callback function - may be NULL */
    ,void *cbarg                            /**< [in] callback state argument */
    );


#ifdef __cplusplus
}
#endif


#endif /* S337M_SADM_BITSTREAM_DECODER_H_ */
