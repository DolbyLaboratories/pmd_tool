/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Removed authorization validation from Query WebSocket API.
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

#ifndef NMOS_QUERY_WS_API_H
#define NMOS_QUERY_WS_API_H

#include "nmos/websockets.h"

namespace slog
{
    class base_gate;
}

// Query API websocket implementation
// See https://github.com/AMWA-TV/nmos-discovery-registration/blob/v1.2/docs/4.2.%20Behaviour%20-%20Querying.md
namespace nmos
{
    struct registry_model;

    web::websockets::experimental::listener::validate_handler make_query_ws_validate_handler(nmos::registry_model& model, slog::base_gate& gate);
    // note, model mutex is assumed to also protect websockets
    web::websockets::experimental::listener::open_handler make_query_ws_open_handler(const nmos::id& source_id, nmos::registry_model& model, nmos::websockets& websockets, slog::base_gate& gate);
    // note, model mutex is assumed to also protect websockets
    web::websockets::experimental::listener::close_handler make_query_ws_close_handler(nmos::registry_model& model, nmos::websockets& websockets, slog::base_gate& gate);

    // note, model mutex is assumed to also protect websockets
    inline web::websockets::experimental::listener::websocket_listener_handlers make_query_ws_api(const nmos::id& source_id, nmos::registry_model& model, nmos::websockets& websockets, slog::base_gate& gate)
    {
        return{
            nmos::make_query_ws_validate_handler(model, gate),
            nmos::make_query_ws_open_handler(source_id, model, websockets, gate),
            nmos::make_query_ws_close_handler(model, websockets, gate),
            {}
        };
    }

    // note, model mutex is assumed to also protect websockets
    void send_query_ws_events_thread(web::websockets::experimental::listener::websocket_listener& listener, nmos::registry_model& model, nmos::websockets& websockets, slog::base_gate& gate);
}

#endif
