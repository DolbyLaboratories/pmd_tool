/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Added conditional compilation - LLDP support now requires HAVE_LLDP define.
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

#ifdef HAVE_LLDP
#include "nmos/lldp_handler.h"

#include "cpprest/json.h"
#include "nmos/json_fields.h"
#include "nmos/model.h"
#include "nmos/node_interfaces.h"
#include "nmos/slog.h"

namespace nmos
{
    namespace experimental
    {
        // make an LLDP handler to update the node interfaces JSON data with attached_network_device details
        lldp::lldp_handler make_lldp_handler(nmos::model& model, const std::map<utility::string_t, node_interface>& interfaces, slog::base_gate& gate)
        {
            return [&model, interfaces, &gate](const std::string& interface_id, const lldp::lldp_data_unit& lldpdu)
            {
                auto interface = interfaces.find(utility::s2us(interface_id));
                if (interfaces.end() == interface)
                {
                    slog::log<slog::severities::error>(gate, SLOG_FLF) << "LLDP received - no matching network interface " << interface_id << " to update";
                    return;
                }

                const auto& interface_name = interface->second.name;

                using web::json::value_of;

                auto lock = model.write_lock();
                auto& resources = model.node_resources;

                auto resource = nmos::find_self_resource(resources);
                if (resources.end() != resource)
                {
                    auto& json_interfaces = nmos::fields::interfaces(resource->data);

                    const auto json_interface = std::find_if(json_interfaces.begin(), json_interfaces.end(), [&](const web::json::value& nv)
                    {
                        return nmos::fields::name(nv) == interface_name;
                    });

                    if (json_interfaces.end() == json_interface)
                    {
                        slog::log<slog::severities::error>(gate, SLOG_FLF) << "LLDP received - no matching network interface " << interface_id << " to update";
                        return;
                    }

                    const auto attached_network_device = value_of({
                        { nmos::fields::chassis_id, utility::s2us(lldp::parse_chassis_id(lldpdu.chassis_id)) },
                        { nmos::fields::port_id, utility::s2us(lldp::parse_port_id(lldpdu.port_id)) }
                    });

                    // has the attached_network_device info changed?
                    if (nmos::fields::attached_network_device(*json_interface) != attached_network_device)
                    {
                        slog::log<slog::severities::more_info>(gate, SLOG_FLF) << "LLDP received - updating attached network device info for network interface " << interface->second.name;

                        nmos::modify_resource(resources, resource->id, [&interface_name, &attached_network_device](nmos::resource& resource)
                        {
                            resource.data[nmos::fields::version] = web::json::value::string(nmos::make_version());

                            auto& json_interfaces = nmos::fields::interfaces(resource.data);

                            const auto json_interface = std::find_if(json_interfaces.begin(), json_interfaces.end(), [&](const web::json::value& nv)
                            {
                                return nmos::fields::name(nv) == interface_name;
                            });

                            if (json_interfaces.end() != json_interface)
                            {
                                (*json_interface)[nmos::fields::attached_network_device] = attached_network_device;
                            }
                        });

                        slog::log<slog::severities::too_much_info>(gate, SLOG_FLF) << "Notifying attached network device info updated";
                        model.notify();
                    }
                }
                else
                {
                    slog::log<slog::severities::error>(gate, SLOG_FLF) << "Self resource not found!";
                }
            };
        }
    }
}

#endif