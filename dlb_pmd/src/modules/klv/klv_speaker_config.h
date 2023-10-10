/************************************************************************
 * dlb_pmd
 * Copyright (c) 2023, Dolby Laboratories Inc.
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
