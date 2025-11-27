/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Removed extended component names (Yc, Cbc, Crc, X, Z, Key) and deprecated function marker.
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

#ifndef NMOS_COMPONENTS_H
#define NMOS_COMPONENTS_H

#include "cpprest/json.h"
#include "nmos/string_enum.h"

namespace nmos
{
    // Components (used in raw video flows)
    // See https://github.com/AMWA-TV/nmos-discovery-registration/blob/v1.2/APIs/schemas/flow_video_raw.json
    DEFINE_STRING_ENUM(component_name)
    namespace component_names
    {
        const component_name Y{ U("Y") };
        const component_name Cb{ U("Cb") };
        const component_name Cr{ U("Cr") };
        const component_name I{ U("I") };
        const component_name Ct{ U("Ct") };
        const component_name Cp{ U("Cp") };
        const component_name A{ U("A") };
        const component_name R{ U("R") };
        const component_name G{ U("G") };
        const component_name B{ U("B") };
        const component_name DepthMap{ U("DepthMap") };
    }

    web::json::value make_component(const nmos::component_name& name, unsigned int width, unsigned int height, unsigned int bit_depth);

    enum chroma_subsampling : int { YCbCr422, RGB444 };
    web::json::value make_components(chroma_subsampling chroma_subsampling = YCbCr422, unsigned int frame_width = 1920, unsigned int frame_height = 1080, unsigned int bit_depth = 10);
}

#endif
