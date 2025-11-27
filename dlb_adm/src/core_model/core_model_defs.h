/************************************************************************
 * dlb_adm
 * Copyright (c) 2020-2025, Dolby Laboratories Inc.
 * Copyright (c) 2020-2025, Dolby International AB.
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

#ifndef DLB_ADM_CORE_MODEL_DEFS_H
#define DLB_ADM_CORE_MODEL_DEFS_H

#include "dlb_adm/include/dlb_adm_api_types.h"

namespace DlbAdm
{

    typedef dlb_adm_sample_rate SampleRate;
    typedef dlb_adm_bit_depth BitDepth;

    static const SampleRate UNKNOWN_SAMPLE_RATE = 0u;
    static const SampleRate DEFAULT_SAMPLE_RATE = 48000u;
    static const BitDepth UNKNOWN_BIT_DEPTH = 0u;
    static const BitDepth DEFAULT_BIT_DEPTH = 24u;

    typedef dlb_adm_source_group_id SourceGroupID;
    typedef dlb_adm_channel_number ChannelNumber;

    static const SourceGroupID UNKNOWN_SOURCE_GROUP_ID = 0u;
    static const SourceGroupID DEFAULT_SOURCE_GROUP_ID = 1u;
    static const ChannelNumber UNKNOWN_CHANNEL_NUMBER = 0u;

    static const size_t DEFAULT_NAME_LIMIT = 32u;

    typedef struct
    {
        const std::string     name;
        const std::string     version;
        const std::string     level;
        const std::string     value;
        const DLB_ADM_PROFILE type;
    } SupportedProfileDescriptor;

    const std::vector<SupportedProfileDescriptor> SUPPORTED_PROFILES = 
    {
        /* latest version of particular profile should be the first*/
        SupportedProfileDescriptor{"Advanced sound system: ADM and S-ADM profile for emission", "1", "1", "ITU-R BS.2168", DLB_ADM_PROFILE_SADM_EMISSION_PROFILE},        
        SupportedProfileDescriptor{"AdvSS Emission S-ADM Profile", "1.0.0", "1", "ITU-R BS.[ADM-NGA-EMISSION]-X", DLB_ADM_PROFILE_SADM_EMISSION_PROFILE},
        SupportedProfileDescriptor{"Dolby E ADM and S-ADM Profile for emission", "1", "1", "Dolby E ADM and S-ADM Profile for emission", DLB_ADM_PROFILE_SADM_DOLBY_E},
        SupportedProfileDescriptor{"Dolby E ADM and S-ADM Profile for emission", "1", "1", "Dolby E ADM and S-ADM Profile for emission v1.0", DLB_ADM_PROFILE_SADM_DOLBY_E}
    };

}

#endif  // DLB_ADM_CORE_MODEL_DEFS_H
