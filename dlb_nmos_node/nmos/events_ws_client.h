/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Updated URL in comment.
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

#ifndef NMOS_EVENTS_WS_CLIENT_H
#define NMOS_EVENTS_WS_CLIENT_H

#include "cpprest/ws_client.h" // for web::websockets::client::websocket_close_status, etc.
#include "nmos/id.h" // forward declaration

namespace web
{
    namespace json
    {
        class value;
    }
}

namespace slog
{
    class base_gate;
}

// Events API websocket implementation
// See https://github.com/AMWA-TV/nmos-event-tally/blob/v1.0/docs/5.2.%20Transport%20-%20Websocket.md
namespace nmos
{
    namespace details
    {
        struct events_ws_client_impl;
    }

    // an events_ws_message_handler callback indicates the specified message has been received on the specified connection
    typedef std::function<void(const web::uri& connection_uri, const web::json::value& message)> events_ws_message_handler;

    // an events_ws_close_handler callback indicates the connection to the specified WebSocket URI has been closed with the specified status and reason
    typedef std::function<void(const web::uri& connection_uri, web::websockets::client::websocket_close_status close_status, const utility::string_t& close_reason)> events_ws_close_handler;

    class events_ws_client
    {
    public:
        events_ws_client(slog::base_gate& gate);
        events_ws_client(web::websockets::client::websocket_client_config config, slog::base_gate& gate);
        events_ws_client(web::websockets::client::websocket_client_config config, int events_heartbeat_interval, slog::base_gate& gate);
        ~events_ws_client();

        // update or create a subscription for the specified id, to the specified WebSocket URI and source id
        // by opening a new connection (and potentially closing an existing connection) and/or issuing subscription commands as required
        // heartbeat commands will also be issued at the appropriate intervals until the connection is closed
        pplx::task<void> subscribe(const nmos::id& id, const web::uri& connection_uri, const nmos::id& source_id);

        // remove the subscription for the specified id
        // by issuing a subscription command or closing the existing connection as required
        pplx::task<void> unsubscribe(const nmos::id& id);

        // close all connections normally
        pplx::task<void> close();

        // close all connections with the specified status and reason
        pplx::task<void> close(web::websockets::client::websocket_close_status close_status, const utility::string_t& close_reason = {});

        void set_close_handler(events_ws_close_handler close_handler);

        void set_message_handler(events_ws_message_handler message_handler);

        const web::websockets::client::websocket_client_config& configuration() const;

        events_ws_client(events_ws_client&& other);
        events_ws_client& operator=(events_ws_client&& other);

    private:
        events_ws_client(const events_ws_client& other);
        events_ws_client& operator=(const events_ws_client& other);

        // hmm, would shared implementation like e.g. web::websockets::client::websocket_client be more flexible?
        std::unique_ptr<details::events_ws_client_impl> impl;
    };
}

#endif
