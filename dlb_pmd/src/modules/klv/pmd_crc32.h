/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2017-2019 by Dolby Laboratories,
 *                Copyright (C) 2017-2019 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#ifndef PMD_CRC32_H_
#define PMD_CRC32_H_

#include <stdlib.h>

/**
 * @file pmd_crc32.h
 * @brief compute 32-bit Cyclic Redundancy Check
 *
 * The Cyclic Redundancy Check is an error-detection code used to verify
 * that data has been received without any errors.
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief compute 32-bit Cyclic Redundancy Check
 *
 * The Cyclic Redundancy Check is an error-detection code used to verify
 * that data has been received without any errors.
 */
uint32_t                   /** @return the CRC32 value */
pmd_compute_crc32
    (unsigned char *data   /**< [in] data stream to verify */
    ,size_t length         /**< [in] length of data stream to verify */
    );



#ifdef __cplusplus
} /* extern "C" */
#endif
#endif /* PMD_CRC32_H_ */

