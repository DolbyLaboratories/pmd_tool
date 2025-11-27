/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Removed control_protocol_connection_activation_handler typedef
 * and related parameter from connection_activation_thread function signature.
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

#ifndef NMOS_CONNECTION_ACTIVATION_H
#define NMOS_CONNECTION_ACTIVATION_H

#include <functional>

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

namespace nmos
{
    struct node_model;
    struct resource;

    // a connection_resource_auto_resolver overwrites every instance of "auto" in the specified transport_params array for the specified (IS-04/IS-05) sender/connection_sender or receiver/connection_receiver
    // it may throw e.g. web::json::json_exception or std::runtime_error, which will prevent activation and for immediate activations cause a 500 Internal Error status code to be returned
    typedef std::function<void(const nmos::resource& resource, const nmos::resource& connection_resource, web::json::value& transport_params)> connection_resource_auto_resolver;

    // a connection_sender_transportfile_setter updates the specified /transportfile endpoint for the specified (IS-04/IS-05) sender/connection_sender
    // this callback should not throw exceptions, as the active transport parameters will already have been changed and those changes will not be rolled back
    typedef std::function<void(const nmos::resource& sender, const nmos::resource& connection_sender, web::json::value& endpoint_transportfile)> connection_sender_transportfile_setter;

    // a connection_activation_handler is a notification that the active parameters for the specified (IS-04/IS-05) sender/connection_sender or receiver/connection_receiver have changed
    // this callback should not throw exceptions, as the active transport parameters will already have been changed and those changes will not be rolled back
    typedef std::function<void(const nmos::resource& resource, const nmos::resource& connection_resource)> connection_activation_handler;

    // callbacks from this function are called with the model locked, and may read but should not write directly to the model
    void connection_activation_thread(nmos::node_model& model, connection_resource_auto_resolver resolve_auto, connection_sender_transportfile_setter set_transportfile, connection_activation_handler connection_activated, slog::base_gate& gate);
}

#endif
