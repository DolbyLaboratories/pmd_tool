/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Changed documentation URL references from specs.amwa.tv to
 * GitHub repository links.
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

#ifndef NMOS_TRANSPORT_H
#define NMOS_TRANSPORT_H

#include "nmos/string_enum.h"

namespace nmos
{
    // Transports (used in senders and receivers)
    // See https://github.com/AMWA-TV/nmos-discovery-registration/blob/v1.2/docs/2.1.%20APIs%20-%20Common%20Keys.md#transport
    // and https://github.com/AMWA-TV/nmos-discovery-registration/blob/v1.2/APIs/schemas/sender.json
    // and https://github.com/AMWA-TV/nmos-discovery-registration/blob/v1.2/APIs/schemas/receiver_core.json
    // and experimentally, for IS-04 v1.3, IS-05 v1.1, IS-07 v1.0
    // also https://github.com/AMWA-TV/nmos-parameter-registers/pull/6
    DEFINE_STRING_ENUM(transport)
    namespace transports
    {
        const transport rtp{ U("urn:x-nmos:transport:rtp") };
        const transport rtp_ucast{ U("urn:x-nmos:transport:rtp.ucast") };
        const transport rtp_mcast{ U("urn:x-nmos:transport:rtp.mcast") };
        const transport dash{ U("urn:x-nmos:transport:dash") };

        const transport mqtt{ U("urn:x-nmos:transport:mqtt") };
        const transport websocket{ U("urn:x-nmos:transport:websocket") };
    }

    // "Subclassifications are defined as the portion of the URN which follows the first occurrence of a '.', but prior to any '/' character."
    // "Versions are defined as the portion of the URN which follows the first occurrence of a '/'."
    // See https://github.com/AMWA-TV/nmos-discovery-registration/blob/v1.2.2/docs/2.1.%20APIs%20-%20Common%20Keys.md#use-of-urns
    inline nmos::transport transport_base(const nmos::transport& transport)
    {
        return nmos::transport{ transport.name.substr(0, transport.name.find_first_of(U("./"))) };
    }
}

#endif
