/************************************************************************
 * dlb_pmd
 * Copyright (c) 2018, Dolby Laboratories Inc.
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

/**
 * @brief encode a speaker config to its bitstream
 */
static inline
unsigned int                         /** @return PMD speaker config bitstream value */
klv_encode_speaker_config
    (dlb_pmd_speaker_config cfg      /**< [in] PMD speaker config */
    )
{
    if (cfg < DLB_PMD_SPEAKER_CONFIG_PORTABLE)
    {
        return (unsigned int)cfg;
    }
    return (unsigned int)cfg + 22;
}


/**
 * @brief decode a speaker config encoded bitstream value
 */
static inline
pmd_bool                         /** @return 1 on success, 0 otherwise */
klv_decode_speaker_config
    (unsigned int bscfg          /**< [in] bitstream value */
    ,dlb_pmd_speaker_config *cfg /**< [out] decoded bitstream */
    )
{
    if (bscfg < DLB_PMD_SPEAKER_CONFIG_PORTABLE)
    {
        *cfg = (dlb_pmd_speaker_config)bscfg;
        return 1;
    }
    if (bscfg > 28)
    {
        *cfg = (dlb_pmd_speaker_config)(bscfg - 22);
        return 1;
    }
    return 0;    
}


#endif /* KLV_SPEAKER_CONFIG_H_ */
