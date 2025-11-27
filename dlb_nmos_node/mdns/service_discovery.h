/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Removed address_result struct and getaddrinfo method declarations from public interface.
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

#ifndef MDNS_SERVICE_DISCOVERY_H
#define MDNS_SERVICE_DISCOVERY_H

#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include "pplx/pplxtasks.h"
#include "mdns/core.h"

namespace slog
{
    class base_gate;
}

// An interface for straightforward DNS Service Discovery (DNS-SD) browsing
// via unicast DNS, or multicast DNS (mDNS)
namespace mdns
{
    // service discovery implementation
    namespace details
    {
        class service_discovery_impl;
    }

    struct browse_result
    {
        browse_result() : interface_id(0) {}
        browse_result(const std::string& name, const std::string& type, const std::string& domain, std::uint32_t interface_id = 0) : name(name), type(type), domain(domain), interface_id(interface_id) {}

        std::string name;
        std::string type;
        std::string domain;
        std::uint32_t interface_id;
    };

    // return true from the browse result callback if the operation should be ended before its specified timeout once no more results are "imminent"
    // the callback must not throw
    typedef std::function<bool(const browse_result&)> browse_handler;

    struct resolve_result
    {
        resolve_result() {}
        resolve_result(const std::string& host_name, std::uint16_t port, const mdns::txt_records& txt_records, std::uint32_t interface_id = 0) : host_name(host_name), port(port), txt_records(txt_records), interface_id(interface_id) {}

        std::string host_name;
        std::uint16_t port;
        mdns::txt_records txt_records;
        std::uint32_t interface_id;

        std::vector<std::string> ip_addresses;
    };

    // return true from the resolve result callback if the operation should be ended before its specified timeout once no more results are "imminent"
    // the callback must not throw
    typedef std::function<bool(const resolve_result&)> resolve_handler;

    class service_discovery
    {
    public:
        explicit service_discovery(slog::base_gate& gate); // or web::logging::experimental::log_handler to avoid the dependency on slog?
        ~service_discovery(); // do not destroy this object with outstanding tasks!

        pplx::task<bool> browse(const browse_handler& handler, const std::string& type, const std::string& domain, std::uint32_t interface_id, const std::chrono::steady_clock::duration& timeout, const pplx::cancellation_token& token = pplx::cancellation_token::none());
        pplx::task<bool> resolve(const resolve_handler& handler, const std::string& name, const std::string& type, const std::string& domain, std::uint32_t interface_id, const std::chrono::steady_clock::duration& timeout, const pplx::cancellation_token& token = pplx::cancellation_token::none());

        template <typename Rep = std::chrono::seconds::rep, typename Period = std::chrono::seconds::period>
        pplx::task<bool> browse(const browse_handler& handler, const std::string& type, const std::string& domain = {}, std::uint32_t interface_id = 0, const std::chrono::duration<Rep, Period>& timeout = std::chrono::seconds(default_timeout_seconds), const pplx::cancellation_token& token = pplx::cancellation_token::none())
        {
            return browse(handler, type, domain, interface_id, std::chrono::duration_cast<std::chrono::steady_clock::duration>(timeout), token);
        }
        template <typename Rep = std::chrono::seconds::rep, typename Period = std::chrono::seconds::period>
        pplx::task<bool> resolve(const resolve_handler& handler, const std::string& name, const std::string& type, const std::string& domain, std::uint32_t interface_id = 0, const std::chrono::duration<Rep, Period>& timeout = std::chrono::seconds(default_timeout_seconds), const pplx::cancellation_token& token = pplx::cancellation_token::none())
        {
            return resolve(handler, name, type, domain, interface_id, std::chrono::duration_cast<std::chrono::steady_clock::duration>(timeout), token);
        }

        template <typename Rep = std::chrono::seconds::rep, typename Period = std::chrono::seconds::period>
        pplx::task<std::vector<browse_result>> browse(const std::string& type, const std::string& domain = {}, std::uint32_t interface_id = 0, const std::chrono::duration<Rep, Period>& timeout = std::chrono::seconds(default_timeout_seconds), const pplx::cancellation_token& token = pplx::cancellation_token::none())
        {
            std::shared_ptr<std::vector<browse_result>> results(new std::vector<browse_result>());
            return browse([results](const browse_result& result) { results->push_back(result); return true; }, type, domain, interface_id, std::chrono::duration_cast<std::chrono::steady_clock::duration>(timeout), token)
                .then([results](bool) { return std::move(*results); });
        }
        template <typename Rep = std::chrono::seconds::rep, typename Period = std::chrono::seconds::period>
        pplx::task<std::vector<resolve_result>> resolve(const std::string& name, const std::string& type, const std::string& domain, std::uint32_t interface_id = 0, const std::chrono::duration<Rep, Period>& timeout = std::chrono::seconds(default_timeout_seconds), const pplx::cancellation_token& token = pplx::cancellation_token::none())
        {
            std::shared_ptr<std::vector<resolve_result>> results(new std::vector<resolve_result>());
            return resolve([results](const resolve_result& result) { results->push_back(result); return true; }, name, type, domain, interface_id, std::chrono::duration_cast<std::chrono::steady_clock::duration>(timeout), token)
                .then([results](bool) { return std::move(*results); });
        }

        service_discovery(service_discovery&& other);
        service_discovery& operator=(service_discovery&& other);

        service_discovery(std::unique_ptr<details::service_discovery_impl> impl);

    private:
        service_discovery(const service_discovery& other);
        service_discovery& operator=(const service_discovery& other);

        std::unique_ptr<details::service_discovery_impl> impl;
    };
}

#endif
