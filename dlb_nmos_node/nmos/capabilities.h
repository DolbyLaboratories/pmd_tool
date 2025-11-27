/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Removed pattern parameter from string constraint creation;
 * removed generic constraint matching function.
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

#ifndef NMOS_CAPABILITIES_H
#define NMOS_CAPABILITIES_H

#include "cpprest/json_utils.h"
#include "nmos/rational.h"

namespace nmos
{
    // BCP-004-01 Receiver Capabilities
    // See https://github.com/AMWA-TV/nmos-receiver-capabilities/blob/v1.0.0/docs/1.0.%20Receiver%20Capabilities.md
    namespace fields
    {
        const web::json::field_as_value_or constraint_sets{ U("constraint_sets"), {} };
    }

    template <typename T> inline std::vector<T> no_enum() { return std::vector<T>(); }
    template <typename T> inline T no_minimum() { return (std::numeric_limits<T>::max)(); }
    template <typename T> inline T no_maximum() { return (std::numeric_limits<T>::min)(); }
    template <> nmos::rational inline no_minimum() { return (std::numeric_limits<int64_t>::max)(); }
    template <> nmos::rational inline no_maximum() { return 0; }

    // See https://github.com/AMWA-TV/nmos-receiver-capabilities/blob/v1.0.0/docs/1.0.%20Receiver%20Capabilities.md#string-constraint-keywords
    web::json::value make_caps_string_constraint(const std::vector<utility::string_t>& enum_values = {});

    // See https://github.com/AMWA-TV/nmos-receiver-capabilities/blob/v1.0.0/docs/1.0.%20Receiver%20Capabilities.md#integer-and-number-constraint-keywords
    web::json::value make_caps_integer_constraint(const std::vector<int64_t>& enum_values = {}, int64_t minimum = no_minimum<int64_t>(), int64_t maximum = no_maximum<int64_t>());

    // See https://github.com/AMWA-TV/nmos-receiver-capabilities/blob/v1.0.0/docs/1.0.%20Receiver%20Capabilities.md#integer-and-number-constraint-keywords
    web::json::value make_caps_number_constraint(const std::vector<double>& enum_values = {}, double minimum = no_minimum<double>(), double maximum = no_maximum<double>());

    // See https://github.com/AMWA-TV/nmos-receiver-capabilities/blob/v1.0.0/docs/1.0.%20Receiver%20Capabilities.md#boolean-constraint-keywords
    web::json::value make_caps_boolean_constraint(const std::vector<bool>& enum_values = {});

    // See https://github.com/AMWA-TV/nmos-receiver-capabilities/blob/v1.0.0/docs/1.0.%20Receiver%20Capabilities.md#rational-constraint-keywords
    web::json::value make_caps_rational_constraint(const std::vector<nmos::rational>& enum_values = {}, const nmos::rational& minimum = no_minimum<nmos::rational>(), const nmos::rational& maximum = no_maximum<nmos::rational>());

    bool match_string_constraint(const utility::string_t& value, const web::json::value& constraint);
    bool match_integer_constraint(int64_t value, const web::json::value& constraint);
    bool match_number_constraint(double value, const web::json::value& constraint);
    bool match_boolean_constraint(bool value, const web::json::value& constraint);
    bool match_rational_constraint(const nmos::rational& value, const web::json::value& constraint);

    // NMOS Parameter Registers - Capabilities register
    // See https://github.com/AMWA-TV/nmos-parameter-registers/blob/main/capabilities/README.md
    namespace caps
    {
        namespace meta
        {
            const web::json::field_as_string_or label{ U("urn:x-nmos:cap:meta:label"), U("") };
            const web::json::field_as_integer_or preference{ U("urn:x-nmos:cap:meta:preference"), 0 };
            const web::json::field_as_bool_or enabled{ U("urn:x-nmos:cap:meta:enabled"), true };
        }
        namespace format
        {
            const web::json::field_as_value_or media_type{ U("urn:x-nmos:cap:format:media_type"), {} }; // string
            const web::json::field_as_value_or grain_rate{ U("urn:x-nmos:cap:format:grain_rate"), {} }; // rational
            const web::json::field_as_value_or frame_width{ U("urn:x-nmos:cap:format:frame_width"), {} }; // integer
            const web::json::field_as_value_or frame_height{ U("urn:x-nmos:cap:format:frame_height"), {} }; // integer
            const web::json::field_as_value_or interlace_mode{ U("urn:x-nmos:cap:format:interlace_mode"), {} }; // string
            const web::json::field_as_value_or colorspace{ U("urn:x-nmos:cap:format:colorspace"), {} }; // string
            const web::json::field_as_value_or transfer_characteristic{ U("urn:x-nmos:cap:format:transfer_characteristic"), {} }; // string
            const web::json::field_as_value_or color_sampling{ U("urn:x-nmos:cap:format:color_sampling"), {} }; // string
            const web::json::field_as_value_or component_depth{ U("urn:x-nmos:cap:format:component_depth"), {} }; // integer
            const web::json::field_as_value_or channel_count{ U("urn:x-nmos:cap:format:channel_count"), {} }; // integer
            const web::json::field_as_value_or sample_rate{ U("urn:x-nmos:cap:format:sample_rate"), {} }; // rational
            const web::json::field_as_value_or sample_depth{ U("urn:x-nmos:cap:format:sample_depth"), {} }; // integer
            const web::json::field_as_value_or event_type{ U("urn:x-nmos:cap:format:event_type"), {} }; // string
        }
        namespace transport
        {
            const web::json::field_as_value_or packet_time{ U("urn:x-nmos:cap:transport:packet_time"), {} }; // number
            const web::json::field_as_value_or max_packet_time{ U("urn:x-nmos:cap:transport:max_packet_time"), {} }; // number
            const web::json::field_as_value_or st2110_21_sender_type{ U("urn:x-nmos:cap:transport:st2110_21_sender_type"), {} }; // string
        }
    }
}

#endif
