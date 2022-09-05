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

#ifndef PMD_IAT_H
#define PMD_IAT_H

/**
 * @file pmd_iat.h
 * @brief implementation of ID and Audio Type (IAT) data structure
 *
 * This file defines the data structure used to represent IAT
 * objects inside the PMD model.
 */

#include <string.h>


#define PMD_IAT_PRESENT                 (1ul<<0)
#define PMD_IAT_OFFSET_PRESENT          (1ul<<1)
#define PMD_IAT_VALIDITY_DUR_PRESENT    (1ul<<2)


/**
 * @def PMD_IAT_CONTENT_ID_SPACE
 * @brief one more than max allowed content id length
 */
#define PMD_IAT_CONTENT_ID_SPACE (32)


/**
 * @def PMD_IAT_DISTRIBUTION_ID_SPACE
 * @brief one more than max allowed distribution id length
 */
#define PMD_IAT_DISTRIBUTION_ID_SPACE (16)


/**
 * @def PMD_IAT_USER_DATA_SPACE
 * @brief one more than max allowed user data
 */
#define PMD_IAT_USER_DATA_SPACE (256)

/**
 * @def PMD_IAT_EXTENSION_SPACE
 * @brief one more than max allowed extension data
 */
#define PMD_IAT_EXTENSION_SPACE (256)


/* retrofitting new public API to implementation API */
typedef dlb_pmd_content_id_type pmd_iat_content_id_type;
typedef dlb_pmd_distribution_id_type pmd_iat_distribution_id_type;


#define PMD_IAT_CONTENT_ID_RESERVED_FIRST (0x03)
#define PMD_IAT_CONTENT_ID_RESERVED_LAST (0x1e)
#define PMD_IAT_CONTENT_ID_UNSPECIFIED (0x1f)


#define PMD_IAT_DISTRIBUTION_ID_RESERVED_FIRST (0x01)
#define PMD_IAT_DISTRIBUTION_ID_RESERVED_LAST (0x06)
#define PMD_IAT_DISTRIBUTION_ID_UNSPECIFIED (0x07)


/**
 * @brief type of PMD IAT
 */
typedef struct
{
    uint8_t  options;                  /**< field presence flags */

    /**
     * type of content ID (if present)
     */
    pmd_iat_content_id_type content_id_type; 
    uint8_t  content_id_size;          /**< size of content ID (if present) */

    /**
     * type of distribution ID (if present)
     */
    pmd_iat_distribution_id_type distribution_id_type;
    uint8_t  distribution_id_size;     /**< size of distribution ID (if present) */

    uint8_t  user_data_size;           /**< size of user data (if present) */
    uint16_t offset;                   /**< offset (if present) */
    uint64_t timestamp;                /**< timestamp, int ticks fo 1/240,000 seconds */
    uint16_t validity_duration;        /**< validity duration (if present) */
    uint8_t  extension_size;           /**< size of extension data (if present) */
    uint8_t  content_id[PMD_IAT_CONTENT_ID_SPACE]; /**< space for content ID */
    uint8_t  distribution_id[PMD_IAT_DISTRIBUTION_ID_SPACE];  /**< space for distribution ID */
    uint8_t  user_data[PMD_IAT_USER_DATA_SPACE]; /**< space for user data */
    uint8_t  extension_data[PMD_IAT_EXTENSION_SPACE]; /**< space for extension data */
} pmd_iat;
    

/**
 * @brief initialize IAT data structure to indicate 'not present'
 *
 * We use the various 'size' fields to indicate presence or absence of
 * the field.
 */
static inline
void
pmd_iat_init
    (pmd_iat *iat
    )
{
    if (iat)
    {
        memset(iat, '\0', sizeof(*iat));
    }
}
    

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-constant-out-of-range-compare"
#endif


/**
 * @brief Validate the encoded value of an IAT content id type
 */
static inline
dlb_pmd_payload_status
pmd_validate_encoded_iat_content_id_type
    (pmd_iat_content_id_type id
    )
{
    dlb_pmd_payload_status status = DLB_PMD_PAYLOAD_STATUS_OK;

    if (id >= PMD_IAT_CONTENT_ID_RESERVED_FIRST && id <= PMD_IAT_CONTENT_ID_RESERVED_LAST)
    {
        status = DLB_PMD_PAYLOAD_STATUS_VALUE_RESERVED;
    }
    else if (id > PMD_IAT_CONTENT_ID_UNSPECIFIED)
    {
        status = DLB_PMD_PAYLOAD_STATUS_VALUE_OUT_OF_RANGE;
    }

    return status;
}
    

/**
 * @brief Validate the encoded value of an IAT distribution id type
 */
static inline
dlb_pmd_payload_status
pmd_validate_encoded_iat_distribution_id_type
    (pmd_iat_distribution_id_type id
    )
{
    dlb_pmd_payload_status status = DLB_PMD_PAYLOAD_STATUS_OK;

    if (id >= PMD_IAT_DISTRIBUTION_ID_RESERVED_FIRST && id <= PMD_IAT_DISTRIBUTION_ID_RESERVED_LAST)
    {
        status = DLB_PMD_PAYLOAD_STATUS_VALUE_RESERVED;
    }
    else if (id > PMD_IAT_DISTRIBUTION_ID_UNSPECIFIED)
    {
        status = DLB_PMD_PAYLOAD_STATUS_VALUE_OUT_OF_RANGE;
    }

    return status;
}

#if defined(__clang__)
#pragma clang diagnostic pop
#endif



#endif /* PMD_IAT_H */
