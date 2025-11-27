/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Updated documentation URLs from specs.amwa.tv to GitHub repository links.
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

#include "nmos/did_sdid.h"

#include "cpprest/json_ops.h"
#include "nmos/json_fields.h"
#include "sdp/json.h"
#include "slog/all_in_one.h" // for slog::manip_function, etc.

namespace nmos
{
    namespace details
    {
        slog::manip_function<utility::istream_t> const_char_t(utility::char_t c)
        {
            return slog::manip_function<utility::istream_t>([c](utility::istream_t& is)
            {
                if (c != is.get()) is.setstate(std::ios_base::failbit);
            });
        }
    }

    // Data identification and Secondary data identification words
    // See https://github.com/AMWA-TV/nmos-discovery-registration/blob/v1.2/APIs/schemas/flow_sdianc_data.json
    utility::string_t make_did_or_sdid(const uint8_t& did_or_sdid)
    {
        utility::ostringstream_t os;
        os << std::uppercase << std::hex << std::setfill(U('0'))
            << U('0') << U('x') << std::setw(2) << (uint32_t)did_or_sdid;
        return os.str();
    }

    uint8_t parse_did_or_sdid(const utility::string_t& did_or_sdid)
    {
        utility::istringstream_t is(did_or_sdid);
        uint32_t id = 0;
        is >> std::hex
            >> details::const_char_t(U('0')) >> details::const_char_t(U('x')) >> id;
        return (uint8_t)id;
    }

    // Data identification and Secondary data identification words
    // See https://github.com/AMWA-TV/nmos-discovery-registration/blob/v1.2/APIs/schemas/flow_sdianc_data.json
    web::json::value make_did_sdid(const nmos::did_sdid& did_sdid)
    {
        return web::json::value_of({
            { nmos::fields::DID, make_did_or_sdid(did_sdid.did) },
            { nmos::fields::SDID, make_did_or_sdid(did_sdid.sdid) }
        });
    }

    nmos::did_sdid parse_did_sdid(const web::json::value& did_sdid)
    {
        return{
            parse_did_or_sdid(nmos::fields::DID(did_sdid)),
            parse_did_or_sdid(nmos::fields::SDID(did_sdid))
        };
    }

    // DID_SDID (Data Identification and Secondary Data Identification)
    // Grammar defined by ABNF as follows:
    // TwoHex = "0x" 1*2(HEXDIG)
    // DidSdid = "DID_SDID={" TwoHex "," TwoHex "}"
    // See https://tools.ietf.org/html/rfc8331
    utility::string_t make_fmtp_did_sdid(const nmos::did_sdid& did_sdid)
    {
        utility::ostringstream_t os;
        os << std::uppercase << std::hex << std::setfill(U('0'))
            << U('{')
            << U('0') << U('x') << std::setw(2) << (uint32_t)did_sdid.did
            << U(',')
            << U('0') << U('x') << std::setw(2) << (uint32_t)did_sdid.sdid
            << U('}');
        return os.str();
    }

    nmos::did_sdid parse_fmtp_did_sdid(const utility::string_t& did_or_sdid)
    {
        utility::istringstream_t is(did_or_sdid);
        uint32_t did = 0, sdid = 0;
        is >> std::hex
            >> details::const_char_t(U('{'))
            >> details::const_char_t(U('0')) >> details::const_char_t(U('x')) >> did
            >> details::const_char_t(U(','))
            >> details::const_char_t(U('0')) >> details::const_char_t(U('x')) >> sdid
            >> details::const_char_t(U('}'));
        return{ (uint8_t)did, (uint8_t)sdid };
    }
}
