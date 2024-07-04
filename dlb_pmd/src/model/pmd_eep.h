/************************************************************************
 * dlb_pmd
 * Copyright (c) 2017-2020, Dolby Laboratories Inc.
 * Copyright (c) 2017-2020, Dolby International AB.
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

#ifndef PMD_EEP_H
#define PMD_EEP_H

#include "pmd_abd_aod.h"

/**
 * @file pmd_eep.h
 * @brief model of EAC3 Encoding Parameters (EEP) payload
 *
 * This file defines the data structure used to represent EAC3 encoding
 * parameter payloads inside the PMD model.
 */

/**
 * @def PMD_EEP_MAX_PRESENTATIONS (15)
 * @brief maximum number of supported AC4 presentations per AC3 program metadata
 */
#define PMD_EEP_MAX_PRESENTATIONS (15)


/**
 * @brief we use a bitmap to enumerate presence of optional components
 *
 * AC3 program metadata may optionally include BSI, ExtBSI and DRC
 * data.  We use a bitmap to indicate the presence or absence of each.
 */
typedef unsigned char pmd_apm_options;

#define PMD_EEP_BITSTREAM_PRESENT    (0x01)
#define PMD_EEP_ENCODER_PRESENT      (0x02)
#define PMD_EEP_DRC_PRESENT          (0x04)


/* backwards compatibility, retrofitting API */
typedef dlb_pmd_bsmod     pmd_bitstream_mode;
typedef dlb_pmd_cmixlev   pmd_cmix_level;
typedef dlb_pmd_surmixlev pmd_surmix_level;
typedef dlb_pmd_surmod    pmd_surround_mode;
typedef dlb_pmd_dialnorm  pmd_dialogue_norm;
typedef dlb_pmd_compr     pmd_compression_mode;
typedef dlb_pmd_prefdmix  pmd_preferred_downmix;


/**
 * @brief type of PMD EAC3 encoder parameters
 */
typedef struct
{
    pmd_apm_options options;                  /**< BSI/ExtBSI/DRC present? */
    uint16_t                   id;            /**< AC3 program metadata id */

    /* ---------- optional encoder parameters------------------------ */
    pmd_compression_mode       dynrng_prof;   /**< compression profile required for
                                                * dynrng DRC gain words for output DD(+)
                                                * bitstream */
    pmd_compression_mode       compr_prof;    /**< RF mode (heavy) compression profile */

    pmd_bool                   surround90;    /**< 90-degree phase shift in surrounds? */
    unsigned char              hmixlev;       /**< Heights downmix level */
    
    /* ---------- optional bitstream parameters---------------------- */
    pmd_bitstream_mode         bsmod;         /**< bistream mode */
    pmd_surround_mode          dsurmod;       /**< Dolby surround mode status */
    pmd_dialogue_norm          dialnorm;      /**< dialogue normalization */
    pmd_preferred_downmix      dmixmod;       /**< preferred downmix mode */
    pmd_cmix_level             ltrtcmixlev;   /**< Center downmix for LtRt */
    pmd_surmix_level           ltrtsurmixlev; /**< Surround downmix level for LtRt */
    pmd_cmix_level             lorocmixlev;   /**< Center downmix for LoRo */
    pmd_surmix_level           lorosurmixlev; /**< Surround downmix level for LoRo */

    /* ---------- optional extended DRC data -------------- */
    pmd_compression_mode       drc_port_spkr; /**< compression for portable speakers */
    pmd_compression_mode       drc_port_hphone;/**< compression for portable headphones */
    pmd_compression_mode       drc_flat_panl; /**< compression for flat-screen TV */
    pmd_compression_mode       drc_home_thtr; /**< compression mode for Home Theatre */
    pmd_compression_mode       drc_ddplus;    /**< compression mode for DD+ encode */
    /* ---------- list of affected presentations ---------- */
    unsigned int num_presentations;
    pmd_presentation_id presentations[PMD_EEP_MAX_PRESENTATIONS];
} pmd_eep;


/**
 * @brief validate an EEP id
 */
static inline
dlb_pmd_payload_status      /** @return validation status */
validate_pmd_eep_id
    (unsigned int id        /**< [in] EEP id */
    )
{
    dlb_pmd_payload_status status = DLB_PMD_PAYLOAD_STATUS_OK;

    if (id > PMD_EAC3_PARAMS_ID_MAX)
    {
        status = DLB_PMD_PAYLOAD_STATUS_VALUE_OUT_OF_RANGE;
    }
    else if (id == PMD_EAC3_PARAMS_ID_RESERVED)
    {
        status = DLB_PMD_PAYLOAD_STATUS_VALUE_RESERVED;
    }

    return status;
}


/**
 * @brief validate a DRC compression mode value
 */
static inline
dlb_pmd_payload_status          /** @return validation status */
validate_pmd_compression_mode
    (pmd_compression_mode mode  /**< [in] PMD compression mode value */
    )
{
    dlb_pmd_payload_status status = DLB_PMD_PAYLOAD_STATUS_OK;

    if (mode > PMD_COMPR_RESERVED_LAST)
    {
        status = DLB_PMD_PAYLOAD_STATUS_VALUE_OUT_OF_RANGE;
    }
    else if (mode >= PMD_COMPR_RESERVED_FIRST && mode <= PMD_COMPR_RESERVED_LAST)
    {
        status = DLB_PMD_PAYLOAD_STATUS_VALUE_RESERVED;
    }

    return status;
}


/**
 * @brief validate a DRC surround mode value
 */
static inline
dlb_pmd_payload_status          /** @return validation status */
validate_pmd_surround_mode
    (pmd_surround_mode mode     /**< [in] PMD surround mode value */
    )
{
    dlb_pmd_payload_status status = DLB_PMD_PAYLOAD_STATUS_OK;

    if (mode > PMD_DSURMOD_RESERVED)
    {
        status = DLB_PMD_PAYLOAD_STATUS_VALUE_OUT_OF_RANGE;
    }
    else if (mode == PMD_DSURMOD_RESERVED)
    {
        status = DLB_PMD_PAYLOAD_STATUS_VALUE_RESERVED;
    }

    return status;
}


/**
 * @brief validate a DRC dialnorm value
 */
static inline
dlb_pmd_payload_status          /** @return validation status */
validate_pmd_dialogue_norm
    (pmd_dialogue_norm dialnorm /**< [in] PMD dialnorm value */
    )
{
    dlb_pmd_payload_status status = DLB_PMD_PAYLOAD_STATUS_OK;

    if (dialnorm > DLB_PMD_MAX_DIALNORM)
    {
        status = DLB_PMD_PAYLOAD_STATUS_VALUE_OUT_OF_RANGE;
    }
    else if (dialnorm == DLB_PMD_RESERVED_DIALNORM)
    {
        status = DLB_PMD_PAYLOAD_STATUS_VALUE_RESERVED;
    }

    return status;
}


#endif /* PMD_EEP_H */
