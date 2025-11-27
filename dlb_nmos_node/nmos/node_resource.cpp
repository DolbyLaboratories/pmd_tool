/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Removed authorization fields from node API endpoints.
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

#include "nmos/node_resource.h"

#include "cpprest/uri_builder.h"
#include "nmos/api_utils.h" // for nmos::http_scheme
#include "nmos/clock_name.h"
#include "nmos/clock_ref_type.h"
#include "nmos/is04_versions.h"
#include "nmos/resource.h"

namespace nmos
{
    // See  https://github.com/AMWA-TV/nmos-discovery-registration/blob/v1.2/schemas/node.json
    nmos::resource make_node(const nmos::id& id, const web::json::value& clocks, const web::json::value& interfaces, const nmos::settings& settings)
    {
        using web::json::value;
        using web::json::value_from_elements;
        using web::json::value_of;

        auto data = details::make_resource_core(id, settings);

        auto uri = web::uri_builder()
            .set_scheme(nmos::http_scheme(settings))
            .set_host(nmos::get_host(settings))
            .set_port(nmos::fields::node_port(settings))
            .to_uri();

        data[U("href")] = value::string(uri.to_string());
        data[U("hostname")] = value::string(nmos::get_host_name(settings));
        data[U("api")][U("versions")] = value_from_elements(nmos::is04_versions::from_settings(settings) | boost::adaptors::transformed(make_api_version));

        const auto hosts = nmos::get_hosts(settings);

        for (const auto& host : hosts)
        {
            web::json::push_back(data[U("api")][U("endpoints")], value_of({
                { U("host"), host },
                { U("port"), uri.port() },
                { U("protocol"), uri.scheme() }
            }));
        }

        data[U("caps")] = value::object();

        data[U("services")] = value::array();

        data[U("clocks")] = !web::json::empty(clocks) ? clocks : value::array();

        data[U("interfaces")] = !web::json::empty(interfaces) ? interfaces : value::array();

        return{ is04_versions::v1_3, types::node, std::move(data), false };
    }

    nmos::resource make_node(const nmos::id& id, const nmos::settings& settings)
    {
        // for now, default clocks and interfaces are empty...
        return make_node(id, {}, {}, settings);
    }

    // See https://github.com/AMWA-TV/nmos-discovery-registration/blob/v1.2/APIs/schemas/clock_internal.json
    web::json::value make_internal_clock(const nmos::clock_name& clk)
    {
        using web::json::value_of;

        return value_of({
            { nmos::fields::name, clk.name },
            { nmos::fields::ref_type, nmos::clock_ref_types::internal.name }
        });
    }

    // See https://github.com/AMWA-TV/nmos-discovery-registration/blob/v1.2/APIs/schemas/clock_ptp.json
    web::json::value make_ptp_clock(const nmos::clock_name& clk, bool traceable, const utility::string_t& gmid, bool locked)
    {
        using web::json::value_of;

        return value_of({
            { nmos::fields::name, clk.name },
            { nmos::fields::ref_type, nmos::clock_ref_types::ptp.name },
            { nmos::fields::ptp_version, U("IEEE1588-2008") }, // cf. sdp::ptp_versions::IEEE1588_2008
            { nmos::fields::traceable, traceable },
            { nmos::fields::gmid, gmid },
            { nmos::fields::locked, locked }
        });
    }
}
