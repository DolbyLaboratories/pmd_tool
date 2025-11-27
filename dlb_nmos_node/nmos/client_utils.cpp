/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Removed OAuth 2.0 bearer token support, network interface binding,
 * and client address configuration; simplified HTTP/WebSocket client configuration;
 * replaced boost::chrono with std::chrono.
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

#include "nmos/client_utils.h"

#if !defined(_WIN32) || !defined(__cplusplus_winrt) || defined(CPPREST_FORCE_HTTP_CLIENT_ASIO)
#include "boost/asio/ssl/set_cipher_list.hpp"
#endif
#include "cpprest/basic_utils.h"
#include "cpprest/details/system_error.h"
#include "cpprest/http_utils.h"
#include "cpprest/ws_client.h"
#include "nmos/certificate_settings.h"
#include "nmos/slog.h"
#include "nmos/ssl_context_options.h"

// Utility types, constants and functions for implementing NMOS REST API clients
namespace nmos
{
    web::uri proxy_uri(const nmos::settings& settings)
    {
        const auto& proxy_address = nmos::experimental::fields::proxy_address(settings);
        if (proxy_address.empty()) return{};
        return web::uri_builder()
            .set_scheme(U("http"))
            .set_host(proxy_address)
            .set_port(nmos::experimental::fields::proxy_port(settings))
            .to_uri();
    }

    namespace details
    {
#if !defined(_WIN32) || !defined(__cplusplus_winrt) || defined(CPPREST_FORCE_HTTP_CLIENT_ASIO)
        template <typename ExceptionType>
        inline std::function<void(boost::asio::ssl::context&)> make_client_ssl_context_callback(const nmos::settings& settings, load_ca_certificates_handler load_ca_certificates, slog::base_gate& gate)
        {
            if (!load_ca_certificates)
            {
                load_ca_certificates = make_load_ca_certificates_handler(settings, gate);
            }

            return [load_ca_certificates](boost::asio::ssl::context& ctx)
            {
                try
                {
                    ctx.set_options(nmos::details::ssl_context_options);

                    const auto cacerts = utility::us2s(load_ca_certificates());
                    ctx.add_certificate_authority(boost::asio::buffer(cacerts.data(), cacerts.size()));

                    set_cipher_list(ctx, nmos::details::ssl_cipher_list);
                }
                catch (const boost::system::system_error& e)
                {
                    throw web::details::from_boost_system_system_error<ExceptionType>(e);
                }
            };
        }
#endif
    }

    // construct client config based on settings, e.g. using the specified proxy
    // with the remaining options defaulted, e.g. request timeout
    web::http::client::http_client_config make_http_client_config(const nmos::settings& settings, load_ca_certificates_handler load_ca_certificates, slog::base_gate& gate)
    {
        web::http::client::http_client_config config;
        const auto proxy = proxy_uri(settings);
        if (!proxy.is_empty()) config.set_proxy(proxy);
        config.set_validate_certificates(nmos::experimental::fields::validate_certificates(settings));
#if !defined(_WIN32) && !defined(__cplusplus_winrt) || defined(CPPREST_FORCE_HTTP_CLIENT_ASIO)
        config.set_ssl_context_callback(details::make_client_ssl_context_callback<web::http::http_exception>(settings, load_ca_certificates, gate));
#endif

        return config;
    }

    // construct client config based on settings, e.g. using the specified proxy
    // with the remaining options defaulted
    web::websockets::client::websocket_client_config make_websocket_client_config(const nmos::settings& settings, load_ca_certificates_handler load_ca_certificates, slog::base_gate& gate)
    {
        web::websockets::client::websocket_client_config config;
        const auto proxy = proxy_uri(settings);
        if (!proxy.is_empty()) config.set_proxy(proxy);
        config.set_validate_certificates(nmos::experimental::fields::validate_certificates(settings));
#if !defined(_WIN32) || !defined(__cplusplus_winrt)
        config.set_ssl_context_callback(details::make_client_ssl_context_callback<web::websockets::client::websocket_exception>(settings, load_ca_certificates, gate));
#endif

        return config;
    }

    // make a request with logging
    pplx::task<web::http::http_response> api_request(web::http::client::http_client client, web::http::http_request request, slog::base_gate& gate, const pplx::cancellation_token& token)
    {
        slog::log<slog::severities::too_much_info>(gate, SLOG_FLF) << "Sending request";
        // see https://developer.mozilla.org/en-US/docs/Web/API/Resource_Timing_API#Resource_loading_timestamps
        const auto start_time = std::chrono::system_clock::now();
        return client.request(request, token).then([start_time, &gate](web::http::http_response res)
        {
            const auto response_start = std::chrono::system_clock::now();
            const auto request_dur = std::chrono::duration_cast<std::chrono::microseconds>(response_start - start_time).count() / 1000.0;

            // see https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Server-Timing
            const auto server_timing = res.headers().find(web::http::experimental::header_names::server_timing);

            if (res.headers().end() != server_timing)
            {
                slog::log<slog::severities::too_much_info>(gate, SLOG_FLF) << "Response received after " << request_dur << "ms (server-timing: " << server_timing->second << ")";
            }
            else
            {
                slog::log<slog::severities::too_much_info>(gate, SLOG_FLF) << "Response received after " << request_dur << "ms";
            }

            return res;
        });
    }

    pplx::task<web::http::http_response> api_request(web::http::client::http_client client, const web::http::method& mtd, slog::base_gate& gate, const pplx::cancellation_token& token)
    {
        web::http::http_request msg(mtd);
        return api_request(client, msg, gate, token);
    }

    pplx::task<web::http::http_response> api_request(web::http::client::http_client client, const web::http::method& mtd, const utility::string_t& path_query_fragment, slog::base_gate& gate, const pplx::cancellation_token& token)
    {
        web::http::http_request msg(mtd);
        msg.set_request_uri(path_query_fragment);
        return api_request(client, msg, gate, token);
    }

    pplx::task<web::http::http_response> api_request(web::http::client::http_client client, const web::http::method& mtd, const utility::string_t& path_query_fragment, const web::json::value& body_data, slog::base_gate& gate, const pplx::cancellation_token& token)
    {
        web::http::http_request msg(mtd);
        msg.set_request_uri(path_query_fragment);
        msg.set_body(body_data);
        return api_request(client, msg, gate, token);
    }
}
