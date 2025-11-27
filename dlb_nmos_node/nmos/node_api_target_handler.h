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

#ifndef NMOS_NODE_API_TARGET_HANDLER_H
#define NMOS_NODE_API_TARGET_HANDLER_H

#include "nmos/certificate_handlers.h"
#include "nmos/connection_api.h"

namespace web
{
    namespace json
    {
        class value;
    }
}

namespace nmos
{
    struct node_model;

    // handler for the Node API /receivers/{receiverId}/target endpoint
    typedef std::function<pplx::task<void>(const nmos::id& receiver_id, const web::json::value& sender_data, slog::base_gate& gate)> node_api_target_handler;

    // implement the Node API /receivers/{receiverId}/target endpoint using the Connection API implementation with the specified handlers
    // (the /target endpoint is only required to support RTP transport, other transport types use the Connection API)
    node_api_target_handler make_node_api_target_handler(nmos::node_model& model, load_ca_certificates_handler load_ca_certificates, transport_file_parser parse_transport_file, details::connection_resource_patch_validator validate_merged);

    inline node_api_target_handler make_node_api_target_handler(nmos::node_model& model, transport_file_parser parse_transport_file, details::connection_resource_patch_validator validate_merged)
    {
        return make_node_api_target_handler(model, {}, std::move(parse_transport_file), std::move(validate_merged));
    }

    inline node_api_target_handler make_node_api_target_handler(nmos::node_model& model, transport_file_parser parse_transport_file)
    {
        return make_node_api_target_handler(model, std::move(parse_transport_file), {});
    }

    node_api_target_handler make_node_api_target_handler(nmos::node_model& model);
}

#endif
