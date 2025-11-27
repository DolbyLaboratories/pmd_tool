/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Removed authorization handlers include and simplified
 * function signatures by removing authorization parameters.
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

#ifndef NMOS_CONNECTION_EVENTS_ACTIVATION_H
#define NMOS_CONNECTION_EVENTS_ACTIVATION_H

#include "nmos/certificate_handlers.h"
#include "nmos/connection_activation.h"
#include "nmos/events_ws_client.h" // for nmos::events_ws_message_handler, etc.
#include "nmos/settings.h" // just a forward declaration of nmos::settings

namespace nmos
{
    struct node_model;

    // this handler can be used to (un)subscribe IS-07 Events WebSocket receivers with the specified handlers, when they are activated
    nmos::connection_activation_handler make_connection_events_websocket_activation_handler(nmos::load_ca_certificates_handler load_ca_certificates, nmos::events_ws_message_handler message_handler, nmos::events_ws_close_handler close_handler, const nmos::settings& settings, slog::base_gate& gate);

    inline nmos::connection_activation_handler make_connection_events_websocket_activation_handler(nmos::events_ws_message_handler message_handler, nmos::events_ws_close_handler close_handler, const nmos::settings& settings, slog::base_gate& gate)
    {
        return make_connection_events_websocket_activation_handler({}, std::move(message_handler), std::move(close_handler), settings, gate);
    }

    inline nmos::connection_activation_handler make_connection_events_websocket_activation_handler(nmos::events_ws_message_handler message_handler, const nmos::settings& settings, slog::base_gate& gate)
    {
        return make_connection_events_websocket_activation_handler(std::move(message_handler), {}, settings, gate);
    }

    namespace experimental
    {
        // an events_ws_receiver_event_handler callback indicates a "state" message (event) has been received for the specified (IS-04/IS-05) receiver/connection_receiver
        typedef std::function<void(const nmos::resource& receiver, const nmos::resource& connection_receiver, const web::json::value& message)> events_ws_receiver_event_handler;

        // since each Events WebSocket connection may potentially have subscriptions to a number of sources, for multiple receivers
        // this handler adaptor enables simple processing of "state" messages (events) per receiver
        nmos::events_ws_message_handler make_events_ws_message_handler(const nmos::node_model& model, events_ws_receiver_event_handler event_handler, slog::base_gate& gate);

        // this handler reflects Events WebSocket connection errors into the /active endpoint of all associated receivers by setting master_enable to false
        nmos::events_ws_close_handler make_events_ws_close_handler(nmos::node_model& model, slog::base_gate& gate);
    }
}

#endif
