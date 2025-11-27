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

#ifndef NMOS_DID_SDID_H
#define NMOS_DID_SDID_H

#include <tuple>
#include "cpprest/details/basic_types.h"

namespace web
{
    namespace json
    {
        class value;
    }
}

namespace nmos
{
    // SMPTE ST 291 Data Identification Words
    struct did_sdid
    {
        // Data Identifier
        uint8_t did;
        // Secondary Data Identifier
        uint8_t sdid;

        did_sdid(uint8_t did = 0, uint8_t sdid = 0) : did(did), sdid(sdid) {}

        auto tied() const -> decltype(std::tie(did, sdid)) { return std::tie(did, sdid); }
        friend bool operator==(const did_sdid& lhs, const did_sdid& rhs) { return lhs.tied() == rhs.tied(); }
        friend bool operator< (const did_sdid& lhs, const did_sdid& rhs) { return lhs.tied() <  rhs.tied(); }
        friend bool operator> (const did_sdid& lhs, const did_sdid& rhs) { return rhs < lhs; }
        friend bool operator!=(const did_sdid& lhs, const did_sdid& rhs) { return !(lhs == rhs); }
        friend bool operator<=(const did_sdid& lhs, const did_sdid& rhs) { return !(rhs < lhs); }
        friend bool operator>=(const did_sdid& lhs, const did_sdid& rhs) { return !(lhs < rhs); }
    };

    // SMPTE ST 291 Data Identification Word Assignments for Registered DIDs
    // See https://smpte-ra.org/smpte-ancillary-data-smpte-st-291
    namespace did_sdids
    {
        // extensible enum
    }

    // Data identification and Secondary data identification words
    // See https://github.com/AMWA-TV/nmos-discovery-registration/blob/v1.2/APIs/schemas/flow_sdianc_data.json
    utility::string_t make_did_or_sdid(const uint8_t& did_or_sdid);
    uint8_t parse_did_or_sdid(const utility::string_t& did_or_sdid);

    // Data identification and Secondary data identification words
    // See https://github.com/AMWA-TV/nmos-discovery-registration/blob/v1.2/APIs/schemas/flow_sdianc_data.json
    web::json::value make_did_sdid(const nmos::did_sdid& did_sdid);
    nmos::did_sdid parse_did_sdid(const web::json::value& did_sdid);

    // DID_SDID (Data Identification and Secondary Data Identification)
    // Grammar defined by ABNF as follows:
    // TwoHex = "0x" 1*2(HEXDIG)
    // DidSdid = "DID_SDID={" TwoHex "," TwoHex "}"
    // See https://tools.ietf.org/html/rfc8331
    utility::string_t make_fmtp_did_sdid(const nmos::did_sdid& did_sdid);
    nmos::did_sdid parse_fmtp_did_sdid(const utility::string_t& did_sdid);
}

#endif
