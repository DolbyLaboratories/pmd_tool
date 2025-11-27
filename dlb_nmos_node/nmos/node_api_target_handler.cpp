/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Removed authorization token support from target handle.
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

#include "nmos/node_api_target_handler.h"

#include <boost/algorithm/string/trim.hpp>
#include "cpprest/http_client.h"
#include "nmos/activation_mode.h"
#include "nmos/client_utils.h"
#include "nmos/is05_versions.h"
#include "nmos/json_fields.h"
#include "nmos/model.h"
#include "nmos/slog.h"

namespace nmos
{
    // implement the Node API /receivers/{receiverId}/target endpoint using the Connection API implementation with the specified transport file parser and the specified validator
    // (the /target endpoint is only required to support RTP transport, other transport types use the Connection API)
    node_api_target_handler make_node_api_target_handler(nmos::node_model& model, load_ca_certificates_handler load_ca_certificates, transport_file_parser parse_transport_file, details::connection_resource_patch_validator validate_merged)
    {
        return [&model, load_ca_certificates, parse_transport_file, validate_merged](const nmos::id& receiver_id, const web::json::value& sender_data, slog::base_gate& gate)
        {
            using web::json::value;
            using web::json::value_of;

            if (sender_data.size() != 0)
            {
                // get sdp from sender, and then use this to connect

                const auto sender_id = nmos::fields::id(sender_data);
                // if manifest_href is null, this will throw json_exception which will be reported appropriately as 400 Bad Request
                const auto manifest_href = nmos::fields::manifest_href(sender_data).as_string();

                web::http::client::http_client client(manifest_href, nmos::with_read_lock(model.mutex, [&, load_ca_certificates] { return nmos::make_http_client_config(model.settings, load_ca_certificates, gate); }));
                return api_request(client, web::http::methods::GET, gate).then([manifest_href, &gate](web::http::http_response res)
                {
                    if (res.status_code() != web::http::status_codes::OK)
                    {
                        throw web::http::http_exception(U("Request for manifest: ") + manifest_href
                            + U(" failed, with response: ") + utility::ostringstreamed(res.status_code()) + U(" ") + res.reason_phrase());
                    }

                    // extract_string doesn't know about "application/sdp"
                    auto content_type = res.headers().content_type();
                    auto semicolon = content_type.find(U(';'));
                    if (utility::string_t::npos != semicolon) content_type.erase(semicolon);
                    boost::algorithm::trim(content_type);

                    if (content_type.empty())
                    {
                        slog::log<slog::severities::warning>(gate, SLOG_FLF) << "Missing Content-Type: should be application/sdp";
                    }
                    else if (U("application/sdp") != content_type)
                    {
                        throw web::http::http_exception(U("Incorrect Content-Type: ") + content_type + U(", should be application/sdp"));
                    }

                    return res.extract_string(true);
                }).then([&model, receiver_id, sender_id, parse_transport_file, validate_merged, &gate](const utility::string_t& sdp)
                {
                    // "The Connection Management API supersedes the now deprecated method of updating the 'target' resource on Node API Receivers in order to establish connections."
                    // See https://github.com/AMWA-TV/nmos-device-connection-management/blob/v1.0/docs/3.1.%20Interoperability%20-%20NMOS%20IS-04.md#support-for-legacy-is-04-connection-management

                    const auto patch = value_of({
                        { nmos::fields::sender_id, sender_id },
                        { nmos::fields::master_enable, true },
                        { nmos::fields::activation, value_of({
                            { nmos::fields::mode, nmos::activation_modes::activate_immediate.name }
                        })},
                        { nmos::fields::transport_file, value_of({
                            { nmos::fields::data, sdp },
                            { nmos::fields::type, U("application/sdp") }
                        })}
                    });

                    web::http::http_response res; // ignore the Connection API response
                    const auto version = nmos::is05_versions::v1_0; // hmm, could be based on supported API versions from settings?
                    nmos::details::handle_connection_resource_patch(res, model, version, { receiver_id, nmos::types::receiver }, patch, parse_transport_file, validate_merged, gate);
                });
            }
            else
            {
                // no sender data means disconnect

                return pplx::create_task([&model, receiver_id, validate_merged, &gate]
                {
                    const auto patch = value_of({
                        { nmos::fields::sender_id, value::null() },
                        { nmos::fields::master_enable, false },
                        { nmos::fields::activation, value_of({
                            { nmos::fields::mode, nmos::activation_modes::activate_immediate.name }
                        }) }
                    });

                    web::http::http_response res; // ignore the Connection API response
                    const auto version = nmos::is05_versions::v1_0; // hmm, could be based on supported API versions from settings?
                    nmos::details::handle_connection_resource_patch(res, model, version, { receiver_id, nmos::types::receiver }, patch, {}, validate_merged, gate);
                });
            }
        };
    }

    node_api_target_handler make_node_api_target_handler(nmos::node_model& model)
    {
        return make_node_api_target_handler(model, &nmos::parse_rtp_transport_file, {});
    }
}
