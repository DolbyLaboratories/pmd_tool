/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Removed deprecated component creation function.
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

#include "nmos/components.h"

#include "cpprest/json_ops.h"

namespace nmos
{
    web::json::value make_component(const nmos::component_name& name, unsigned int width, unsigned int height, unsigned int bit_depth)
    {
        return web::json::value_of({
            { U("name"), web::json::value::string(name.name) },
            { U("width"), width },
            { U("height"), height },
            { U("bit_depth"), bit_depth }
        });
    }

    web::json::value make_components(chroma_subsampling chroma_subsampling, unsigned int frame_width, unsigned int frame_height, unsigned int bit_depth)
    {
        using web::json::value;
        using web::json::value_of;

        switch (chroma_subsampling)
        {
        case RGB444:
            return value_of({
                make_component(component_names::R, frame_width, frame_height, bit_depth),
                make_component(component_names::G, frame_width, frame_height, bit_depth),
                make_component(component_names::B, frame_width, frame_height, bit_depth)
            });
        case YCbCr422:
            return  value_of({
                make_component(component_names::Y, frame_width, frame_height, bit_depth),
                make_component(component_names::Cb, frame_width / 2, frame_height, bit_depth),
                make_component(component_names::Cr, frame_width / 2, frame_height, bit_depth)
            });
        default:
            return web::json::value::null();
        }
    }
}
