/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Removed additional media types - removed SDP, HTML, and JSON schema media types.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright (c) 2019-2025, Dolby Laboratories Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef NMOS_MEDIA_TYPE_H
#define NMOS_MEDIA_TYPE_H

#include "cpprest/basic_utils.h"
#include "nmos/string_enum.h"

namespace nmos
{
    // Media types (used in flows and receivers)
    // See https://github.com/AMWA-TV/nmos-discovery-registration/blob/v1.2/APIs/schemas/flow_video.json
    // and https://github.com/AMWA-TV/nmos-discovery-registration/blob/v1.2/APIs/schemas/flow_audio_raw.json
    // and https://github.com/AMWA-TV/nmos-discovery-registration/blob/v1.2/APIs/schemas/receiver_video.json
    // etc.
    DEFINE_STRING_ENUM(media_type)
    namespace media_types
    {
        // Video media types

        // Uncompressed Video
        // See https://tools.ietf.org/html/rfc4175#section-6
        const media_type video_raw{ U("video/raw") };

        // JPEG XS
        // See https://tools.ietf.org/html/draft-ietf-payload-rtp-jpegxs-09#section-6
        const media_type video_jxsv{ U("video/jxsv") };

        // Audio media types

        inline media_type audio_L(unsigned int bit_depth)
        {
            return media_type{ U("audio/L") + utility::ostringstreamed(bit_depth) };
        }

        const media_type audio_L24 = audio_L(24);
        const media_type audio_L16 = audio_L(16);
        const media_type audio_AM824{ U("audio/AM824") };

        // Data media types

        const media_type video_smpte291{ U("video/smpte291") };

        const media_type application_json{ U("application/json") };

        const media_type application_st2110_41{ U("application/ST2110-41")};

        // Mux media types

        // See SMPTE ST 2022-8:2019
        const media_type video_SMPTE2022_6{ U("video/SMPTE2022-6") };
    }
}

#endif
