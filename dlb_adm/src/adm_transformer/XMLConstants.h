/************************************************************************
 * dlb_adm
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

#ifndef DLB_ADM_XML_CONSTANTS_H
#define DLB_ADM_XML_CONSTANTS_H

#include <string>
#include <vector>
#include <dlb_adm_api_types.h>

static const std::string XML_VERSION = "1.0";
static const std::string XML_ENCODING = "UTF-8";

static const std::string AUDIO_FORMAT_EXT_VERSION = "ITU-R_BS.2076-2";

static const std::string AUDIO_TRACK_FORMAT_LABEL = "0001";
static const std::string AUDIO_TRACK_FORMAT_DEFINITION = "PCM";

static const std::string AUDIO_FORMAT_CUSTOM_SET_ID = "AFC_1001";
static const std::string AUDIO_FORMAT_CUSTOM_SET_TYPE = "CUSTOM_SET_TYPE_NGA_EMISSION";
static const std::string AUDIO_FORMAT_CUSTOM_SET_VERSION = "1.0.0";

static const dlb_adm_bool OBJECT_INTERACT = DLB_ADM_FALSE;

typedef struct
{
    const std::string   name;
    const std::string   version;
    const dlb_adm_uint  level;
    const std::string   value;
    const DLB_ADM_PROFILE type;
} ProfileDescriptor;

const std::vector<ProfileDescriptor> SUPPORTED_PROFILES = 
    {
        // TODO: ensure those values are correct
        ProfileDescriptor{"AdvSS Emission S-ADM Profile", "1.0.0", 1, "ITU-R BS.[ADM-NGA-EMISSION]", DLB_ADM_PROFILE_SADM_EMISSION_PROFILE}
    };

#endif  // DLB_ADM_XML_CONSTANTS_H
