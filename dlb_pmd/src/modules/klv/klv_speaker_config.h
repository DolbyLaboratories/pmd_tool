/******************************************************************************
 * This program is protected under international and U.S. copyright laws as
 * an unpublished work. This program is confidential and proprietary to the
 * copyright owners. Reproduction or disclosure, in whole or in part, or the
 * production of derivative works therefrom without the express permission of
 * the copyright owners is prohibited.
 *
 *                Copyright (C) 2018-2021 by Dolby Laboratories,
 *                Copyright (C) 2018-2021 by Dolby International AB.
 *                            All rights reserved.
 ******************************************************************************/

#ifndef KLV_SPEAKER_CONFIG_H_
#define KLV_SPEAKER_CONFIG_H_

#include "dlb_pmd_types.h"

#define KLV_SPEAKER_CONFIG_RESERVED_FIRST (0x07)
#define KLV_SPEAKER_CONFIG_RESERVED_LAST (0x1c)
#define KLV_SPEAKER_CONFIG_NOT_INDICATED (0x1f)
#define KLV_SPEAKER_CONFIG_LAST KLV_SPEAKER_CONFIG_NOT_INDICATED
#define KLV_SPEAKER_CONFIG_DIFF (KLV_SPEAKER_CONFIG_RESERVED_LAST - KLV_SPEAKER_CONFIG_RESERVED_FIRST + 1)

/**
 * @brief encode a speaker config to its bitstream
 */
static inline
unsigned int                         /** @return PMD speaker config bitstream value */
klv_encode_speaker_config
    (dlb_pmd_speaker_config cfg      /**< [in] PMD speaker config */
    )
{
    unsigned int encoded_cfg = cfg;

    if (cfg > DLB_PMD_SPEAKER_CONFIG_9_1_6)
    {
        encoded_cfg += KLV_SPEAKER_CONFIG_DIFF;
    }

    return encoded_cfg;
}


/**
 * @brief decode a speaker config encoded bitstream value
 */
static inline
pmd_bool                         /** @return PMD_TRUE on success, PMD_FALSE otherwise */
klv_decode_speaker_config
    (unsigned int bscfg          /**< [in]  bitstream value */
    ,dlb_pmd_speaker_config *cfg /**< [out] decoded bitstream */
    )
{
    pmd_bool ok = PMD_FALSE;

    if (cfg != NULL)
    {
        if (bscfg < KLV_SPEAKER_CONFIG_RESERVED_FIRST)
        {
            *cfg = (dlb_pmd_speaker_config)bscfg;
            ok = PMD_TRUE;
        }
        else if (bscfg > KLV_SPEAKER_CONFIG_RESERVED_LAST && bscfg < KLV_SPEAKER_CONFIG_NOT_INDICATED)
        {
            *cfg = (dlb_pmd_speaker_config)(bscfg - KLV_SPEAKER_CONFIG_DIFF);
            ok = PMD_TRUE;
        }
    }

    return ok;    
}

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-constant-out-of-range-compare"
#endif

/**
 * @brief validate a speaker config value
 */
static inline
dlb_pmd_payload_status              /** @return validation status */
klv_speaker_config_validate
    (dlb_pmd_speaker_config config  /**< [in] PMD speaker config */
    )
{
    dlb_pmd_payload_status status = DLB_PMD_PAYLOAD_STATUS_OK;

    if (config > KLV_SPEAKER_CONFIG_LAST)
    {
        status = DLB_PMD_PAYLOAD_STATUS_VALUE_OUT_OF_RANGE;
    }
    else if (config >= KLV_SPEAKER_CONFIG_RESERVED_FIRST && config <= KLV_SPEAKER_CONFIG_RESERVED_LAST)
    {
        status = DLB_PMD_PAYLOAD_STATUS_VALUE_RESERVED;
    }

    return status;
}

#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#endif /* KLV_SPEAKER_CONFIG_H_ */
