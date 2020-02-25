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

#ifndef PMD_EDT_H
#define PMD_EDT_H

#include "pmd_abd_aod.h"

/**
 * @file pmd_edt.h
 * @brief implementation of DE/ED2 turnaround 
 */


/**
 * @def PMD_ETD_MAX_PRESENTATIONS
 * @brief maximum presentations per ED2 turnaround
 */
#define PMD_ETD_MAX_PRESENTATIONS (15)


/**
 * @def PMD_ETD_RESERVED_ID
 * @brief reserved value for ED2 turnaround id
 */
#define PMD_ETD_RESERVED_ID (0)


/**
 * @def PMD_ETD_MAX_ID
 * @brief maximum value for ED2 turnaround id
 */
#define PMD_ETD_MAX_ID (255)


/**
 * @brief helper struct associating an individual presentation to a particular
 * set of AC3 program metadata; 0 means 'default'
 */
typedef struct
{
    uint16_t presid; /**< presentation idx */
    uint8_t eepid;   /**< EAC3 encoder parameters idx */
} turnaround;


/**
 * @brief ED2 turnaround id type
 */
typedef uint8_t ed2_turnaround_id;


/**
 * @brief type of PMD ED2 Turnaround
 */
typedef struct
{
    ed2_turnaround_id          id;                /**< ED2 turnaround id */
    
    /* ------------- optional ED2 parameters ------------- */
    unsigned int               ed2_presentations; /**< number of ED2 presentations, or 0 */
    dlb_pmd_frame_rate         ed2_framerate;     /**< ED2 frame rate */
    turnaround                 ed2_turnaround[PMD_ETD_MAX_PRESENTATIONS];

    /* ------------- optional DE parameters ------------- */
    unsigned int               de_presentations;  /**< number of DE presentations, or 0 */
    dlb_pmd_frame_rate         de_framerate;      /**< DE frame rate */
    dlb_pmd_de_program_config  pgm_config;        /**< DE program config, 0-23 */
    turnaround                 de_turnaround[PMD_ETD_MAX_PRESENTATIONS];
} pmd_etd;


/**
 * @brief validate an ED2 turnaround id
 */
static inline
dlb_pmd_payload_status  /** @return validation status */
pmd_validate_ed2_turnaround_id
    (unsigned int id    /**< [in] ED2 turnaround id */
    )
{
    dlb_pmd_payload_status status = DLB_PMD_PAYLOAD_STATUS_OK;

    if (id > PMD_ETD_MAX_ID)
    {
        status = DLB_PMD_PAYLOAD_STATUS_VALUE_OUT_OF_RANGE;
    }
    else if (id == PMD_ETD_RESERVED_ID)
    {
        status = DLB_PMD_PAYLOAD_STATUS_VALUE_RESERVED;
    }

    return status;
}


#define PMD_FRAMERATE_RESERVED_0 (0)
#define PMD_FRAMERATE_RESERVED_0x06 (0x06)
#define PMD_FRAMERATE_RESERVED_0x0F (0x0f)


/**
 * @brief validate an encoded ED2 frame rate
 */
static inline
dlb_pmd_payload_status          /** @return validation status */
validate_pmd_encoded_ed2_frame_rate
    (uint8_t frame_rate         /**< [in] Encoded ED2 frame rate */
    )
{
    dlb_pmd_payload_status status = DLB_PMD_PAYLOAD_STATUS_OK;

    if (frame_rate > PMD_FRAMERATE_RESERVED_0x0F)
    {
        status = DLB_PMD_PAYLOAD_STATUS_VALUE_OUT_OF_RANGE;
    }
    else if ( (frame_rate == PMD_FRAMERATE_RESERVED_0) ||
             ((frame_rate >= PMD_FRAMERATE_RESERVED_0x06) && (frame_rate <= PMD_FRAMERATE_RESERVED_0x0F)))
    {
        status = DLB_PMD_PAYLOAD_STATUS_VALUE_RESERVED;
    }

    return status;
}


#define PMD_DE_PGMCFG_RESERVED_FIRST (0x18)
#define PMD_DE_PGMCFG_RESERVED_LAST (0x1f)


/**
 * @brief validate a Dolby E program config value
 */
static inline
dlb_pmd_payload_status      /** @return validation status */
validate_pmd_dolby_e_program_config
    (unsigned int config    /**< [in] Dolby E program config value */
    )
{
    dlb_pmd_payload_status status = DLB_PMD_PAYLOAD_STATUS_OK;

    if (config > PMD_DE_PGMCFG_RESERVED_LAST)
    {
        status = DLB_PMD_PAYLOAD_STATUS_VALUE_OUT_OF_RANGE;
    }
    else if ((config >= PMD_DE_PGMCFG_RESERVED_FIRST) && (config <= PMD_DE_PGMCFG_RESERVED_LAST))
    {
        status = DLB_PMD_PAYLOAD_STATUS_VALUE_RESERVED;
    }

    return status;
}


#endif /* PMD_ETD_H */


