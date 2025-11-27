/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Removed OCSP response handler parameter from listener
 * configuration functions.
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

#ifndef NMOS_SERVER_UTILS_H
#define NMOS_SERVER_UTILS_H

#include "cpprest/http_listener.h" // forward declaration of web::http::experimental::listener::http_listener_config
#include "cpprest/ws_listener.h" // forward declaration of web::websockets::experimental::listener::websocket_listener_config
#include "nmos/certificate_handlers.h"
#include "nmos/settings.h"

namespace slog { class base_gate; }

// Utility types, constants and functions for implementing NMOS REST API servers
namespace nmos
{
    // construct listener config based on settings
    web::http::experimental::listener::http_listener_config make_http_listener_config(const nmos::settings& settings, load_server_certificates_handler load_server_certificates, load_dh_param_handler load_dh_param, slog::base_gate& gate);

    // construct listener config based on settings
    web::websockets::experimental::listener::websocket_listener_config make_websocket_listener_config(const nmos::settings& settings, load_server_certificates_handler load_server_certificates, load_dh_param_handler load_dh_param, slog::base_gate& gate);

    namespace experimental
    {
        // map the configured client port to the server port on which to listen
        int server_port(int client_port, const nmos::settings& settings);
    }
}

#endif
