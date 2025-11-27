/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Removed authorization support from node behavior - simplified
 * function signatures.
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

#ifndef NMOS_NODE_BEHAVIOUR_H
#define NMOS_NODE_BEHAVIOUR_H

#include <functional>
#include "nmos/certificate_handlers.h"

namespace web
{
    class uri;
}

namespace slog
{
    class base_gate;
}

namespace mdns
{
    class service_advertiser;
    class service_discovery;
}

// Node behaviour including both registered operation and peer to peer operation
// See https://github.com/AMWA-TV/nmos-discovery-registration/blob/v1.2/docs/4.1.%20Behaviour%20-%20Registration.md
// and https://github.com/AMWA-TV/nmos-discovery-registration/blob/v1.2/docs/3.1.%20Discovery%20-%20Registered%20Operation.md
namespace nmos
{
    struct model;

    // a registration_handler is a notification that the current Registration API has changed
    // registration uri should be like http://api.example.com/x-nmos/registration/{version}
    // or empty if errors have been encountered when interacting with all discoverable Registration APIs
    // this callback should not throw exceptions
    typedef std::function<void(const web::uri& registration_uri)> registration_handler;

    // uses the default DNS-SD implementation
    // callbacks from this function are called with the model locked, and may read or write directly to the model
    void node_behaviour_thread(nmos::model& model, load_ca_certificates_handler load_ca_certificates, registration_handler registration_changed, slog::base_gate& gate);

    // uses the specified DNS-SD implementation
    // callbacks from this function are called with the model locked, and may read or write directly to the model
    void node_behaviour_thread(nmos::model& model, registration_handler registration_changed, mdns::service_advertiser& advertiser, mdns::service_discovery& discovery, slog::base_gate& gate);

    // uses the default DNS-SD implementation
    void node_behaviour_thread(nmos::model& model, load_ca_certificates_handler load_ca_certificates, slog::base_gate& gate);

    // uses the specified DNS-SD implementation
    void node_behaviour_thread(nmos::model& model, load_ca_certificates_handler load_ca_certificates, mdns::service_advertiser& advertiser, mdns::service_discovery& discovery, slog::base_gate& gate);
}

#endif
