/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Removed client ID tracking from resources - simplified resource constructor.
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

#ifndef NMOS_RESOURCE_H
#define NMOS_RESOURCE_H

#include <set>
#include "nmos/api_version.h"
#include "nmos/copyable_atomic.h"
#include "nmos/json_fields.h"
#include "nmos/health.h"
#include "nmos/id.h"
#include "nmos/settings.h"
#include "nmos/tai.h"
#include "nmos/type.h"

namespace nmos
{
    // Resources have an API version, resource type and representation as json data
    // Everything else is (internal) registry information: their id, references to their sub-resources, creation and update timestamps,
    // and health which is usually propagated from a node, because only nodes get heartbeats and keep all their sub-resources alive
    struct resource
    {
        resource() {}

        // the API version, type, id and creation timestamp are logically const after construction*, other data may be modified
        // when any data is modified, the update timestamp must be set, and resource events should be generated
        // *or more accurately, after insertion into the registry

        resource(api_version version, type type, web::json::value&& data, nmos::id id, bool never_expire)
            : version(version)
            , downgrade_version()
            , type(type)
            , data(std::move(data))
            , id(id)
            , created(tai_now())
            , updated(created)
            , health(never_expire ? health_forever : created.seconds)
        {}

        resource(api_version version, type type, web::json::value data, bool never_expire)
            : resource(version, type, std::move(data), fields::id(data), never_expire)
        {}

        // the API version of the Node API, Registration API or Query API exposing this resource
        api_version version;

        // the minimum API version to which this resource should be permitted to be downgraded
        nmos::api_version downgrade_version;

        // the type of the resource, e.g. node, device, source, flow, sender, receiver
        // see nmos/type.h
        nmos::type type;

        // resource data is stored directly as json rather than e.g. being deserialized to a class hierarchy to allow quick
        // prototyping; json validation at the API boundary ensures the data met the schema for the specified version
        web::json::value data;
        // when the resource data is null, the resource has been deleted or expired
        bool has_data() const { return !data.is_null(); }

        // universally unique identifier for the resource
        // typically corresponds to the "id" property of the resource data
        // see nmos/id.h
        nmos::id id;

        // sub-resources are tracked in order to optimise resource expiry and deletion
        std::set<nmos::id> sub_resources;

        // see https://github.com/AMWA-TV/nmos-discovery-registration/blob/v1.2/docs/2.5.%20APIs%20-%20Query%20Parameters.md#pagination
        tai created;
        tai updated;

        // see https://github.com/AMWA-TV/nmos-discovery-registration/blob/v1.2/docs/4.1.%20Behaviour%20-%20Registration.md#heartbeating
        mutable details::copyable_atomic<nmos::health> health;
    };

    namespace details
    {
        // See https://github.com/AMWA-TV/nmos-discovery-registration/blob/v1.2/APIs/schemas/resource_core.json
        web::json::value make_resource_core(const nmos::id& id, const utility::string_t& label, const utility::string_t& description, const web::json::value& tags = web::json::value::object());

        web::json::value make_resource_core(const nmos::id& id, const nmos::settings& settings);
    }
}

#endif
