/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Removed IS-10, IS-12, IS-14 schema loading and validation support.
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

#ifndef NMOS_JSON_SCHEMA_H
#define NMOS_JSON_SCHEMA_H

#include "cpprest/base_uri.h"
#include "cpprest/json.h"

namespace nmos
{
    struct api_version;
    struct type;

    namespace details
    {
        std::map<web::uri, web::json::value> make_schemas();
    }

    namespace experimental
    {
        web::uri make_systemapi_global_schema_uri(const nmos::api_version& version);

        web::uri make_registrationapi_resource_post_request_schema_uri(const nmos::api_version& version);
        web::uri make_queryapi_subscriptions_post_request_schema_uri(const nmos::api_version& version);

        web::uri make_nodeapi_receiver_target_put_request_schema_uri(const nmos::api_version& version);

        web::uri make_connectionapi_staged_patch_request_schema_uri(const nmos::api_version& version, const nmos::type& type);
        web::uri make_connectionapi_sender_staged_patch_request_schema_uri(const nmos::api_version& version);
        web::uri make_connectionapi_receiver_staged_patch_request_schema_uri(const nmos::api_version& version);

        web::uri make_channelmappingapi_map_activations_post_request_schema_uri(const nmos::api_version& version);

        // load the json schema for the specified base URI
        web::json::value load_json_schema(const web::uri& id);
    }
}

#endif
