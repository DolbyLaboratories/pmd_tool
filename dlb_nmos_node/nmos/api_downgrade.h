/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Updated documentation URL from specs.amwa.tv to GitHub repository.
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

#ifndef NMOS_API_DOWNGRADE_H
#define NMOS_API_DOWNGRADE_H

#include "cpprest/json.h"

// "Downgrade queries permit old-versioned responses to be provided to clients which are confident
// that they can handle any missing attributes between the specified API versions."
// See https://github.com/AMWA-TV/nmos-discovery-registration/blob/v1.2/docs/2.5.%20APIs%20-%20Query%20Parameters.md#downgrade-queries
namespace nmos
{
    struct api_version;
    struct resource;
    struct type;

    bool is_permitted_downgrade(const nmos::resource& resource, const nmos::api_version& version);
    bool is_permitted_downgrade(const nmos::resource& resource, const nmos::api_version& version, const nmos::api_version& downgrade_version);
    bool is_permitted_downgrade(const nmos::api_version& resource_version, const nmos::api_version& resource_downgrade_version, const nmos::type& resource_type, const nmos::api_version& version, const nmos::api_version& downgrade_version);

    namespace details
    {
        // make user error information (to be used with status_codes::NotFound)
        utility::string_t make_permitted_downgrade_error(const nmos::resource& resource, const nmos::api_version& version);
        utility::string_t make_permitted_downgrade_error(const nmos::resource& resource, const nmos::api_version& version, const nmos::api_version& downgrade_version);
        utility::string_t make_permitted_downgrade_error(const nmos::api_version& resource_version, const nmos::type& resource_type, const nmos::api_version& version, const nmos::api_version& downgrade_version);
    }

    web::json::value downgrade(const nmos::resource& resource, const nmos::api_version& version);
    web::json::value downgrade(const nmos::resource& resource, const nmos::api_version& version, const nmos::api_version& downgrade_version);
    web::json::value downgrade(const nmos::api_version& resource_version, const nmos::api_version& resource_downgrade_version, const nmos::type& resource_type, const web::json::value& resource_data, const nmos::api_version& version, const nmos::api_version& downgrade_version);
}

#endif
