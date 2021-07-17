/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2019-2021 by Dolby Laboratories,
 *                Copyright (C) 2019-2021 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#ifndef S337M_SADM_BITSTREAM_DECODER_H_
#define S337M_SADM_BITSTREAM_DECODER_H_


/**
 * @file sadm_bitstream_decoder.h
 * @brief definitions for extracting a PMD model from an S-ADM bitstream embedded within
 * within SMPTE-337m encoded PCM.
 */

#include "pmd_smpte_337m.h"
#include "pmd_error_helper.h"
#include "dlb_pmd_sadm.h"
#include "dlb_adm/include/dlb_adm_fwd_type.h"

#include "dlb_pmd/include/dlb_pmd_lib_dll.h"

#define TEST_DLL_ENTRY DLB_PMD_DLL_ENTRY


#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief status of S-ADM bitstream decoder
 */
typedef enum
{
    SADM_OK,
    SADM_PARSE_ERR,
    SADM_DECOMPRESS_ERR,
} sadm_dec_state;


/**
 * @brief callback function type for obtaining status of S-ADM bitstream decoder
 */
typedef
void
(*sadm_bitstream_dec_callback)
    (void           *arg
    ,sadm_dec_state  state);


/**
 * @brief S-ADM bitstream decoder struct
 */
typedef struct
{
    dlb_adm_core_model  *model;
    char                 xmlbuf[MAX_DATA_BYTES * DLB_PMD_SADM_XML_COMPRESSION]; /**< S-ADM decompression buffer */
    size_t               size;
} sadm_bitstream_decoder;


/**
 * @brief determine memory requirements for the S-ADM bitstream decoder
 */
TEST_DLL_ENTRY
size_t                                    /** @return size of memory required */
sadm_bitstream_decoder_query_mem
    (void
    );


/**
 * @brief initialize the S-ADM bitstream decoder
 */
TEST_DLL_ENTRY
dlb_pmd_success
sadm_bitstream_decoder_init
    (void                       *mem
    ,sadm_bitstream_decoder    **dec
    );


/**
 * @brief helper function to decompress the input buffer to the decoder's
 * XML buffer.
 */
TEST_DLL_ENTRY
int                                 /** @return bytes used */
decompress_sadm_xml
    (sadm_bitstream_decoder *dec        /**< [in] bitstream decoder */
    ,uint8_t                *buf        /**< [in] input compressed data */
    ,size_t                  datasize   /**< [in] size of compressed input in bytes */
    );


/**
 * @brief function to encapsulate the process of decoding an sADM bitstream
 */
dlb_pmd_success                     /** @return 0 on success, 1 on failure */
sadm_bitstream_decoder_decode
    (sadm_bitstream_decoder         *dec        /**< [in] bitstream decoder */
    ,uint8_t                        *bitstream  /**< [in] bits to decode */
    ,size_t                          datasize   /**< [in] number of bytes in bitstream */
    ,dlb_adm_core_model             *model      /**< [in] model to write */
    ,sadm_bitstream_dec_callback     callback   /**< [in] status callback function - may be NULL */
    ,void                           *cbarg      /**< [in] callback state argument */
    );


#ifdef __cplusplus
}
#endif


#endif /* S337M_SADM_BITSTREAM_DECODER_H_ */
