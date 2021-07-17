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
    char                         xmlbuf[MAX_DATA_BYTES * DLB_PMD_SADM_XML_COMPRESSION]; /**< S-ADM precompression buffer */
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
    (void                       *mem
    ,sadm_bitstream_encoder    **enc
    );


/**
 * @brief helper function to compress the encoder's XML buffer to the
 * given byte buffer
 */
TEST_DLL_ENTRY
int                                 /** @return bytes used, or zero if error */
compress_sadm_xml
    (sadm_bitstream_encoder *enc    /**< [in] bitstream encoder */
    ,uint8_t                *buf    /**< [in] output compression buffer */
    );


/**
 * @brief function to encapsulate the process of generating a compressed sADM payload
 */
TEST_DLL_ENTRY
int                                 /** @return bytes used, or 0 if none */
sadm_bitstream_encoder_payload
    (sadm_bitstream_encoder     *enc    /**< [in]  bitstream encoder */
    ,const dlb_adm_core_model   *model  /**< [in]  model to write */
    ,uint8_t                    *outbuf /**< [out] compression buffer - must be large enough! (suggested: >= 12012) */
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
