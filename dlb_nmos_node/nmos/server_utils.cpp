/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Removed OCSP (Online Certificate Status Protocol) support from
 * SSL context configuration; simplified certificate loading.
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

#include "nmos/server_utils.h"

#include <algorithm>
#if !defined(_WIN32) || !defined(__cplusplus_winrt) || defined(CPPREST_FORCE_HTTP_CLIENT_ASIO)
#include "boost/asio/ssl/set_cipher_list.hpp"
#include "boost/asio/ssl/use_tmp_ecdh.hpp"
#endif
#include "cpprest/basic_utils.h"
#include "nmos/certificate_handlers.h"
#include "cpprest/details/system_error.h"
#include "cpprest/http_listener.h"
#include "cpprest/ws_listener.h"
#include "nmos/slog.h"
#include "nmos/ssl_context_options.h"

// Utility types, constants and functions for implementing NMOS REST API servers
namespace nmos
{
    namespace details
    {
#if !defined(_WIN32) || !defined(__cplusplus_winrt) || defined(CPPREST_FORCE_HTTP_CLIENT_ASIO)
        template <typename ExceptionType>
        inline std::function<void(boost::asio::ssl::context&)> make_listener_ssl_context_callback(const nmos::settings& settings, load_server_certificates_handler load_server_certificates, load_dh_param_handler load_dh_param, slog::base_gate& gate)
        {
            if (!load_server_certificates)
            {
                load_server_certificates = make_load_server_certificates_handler(settings, gate);
            }

            if (!load_dh_param)
            {
                load_dh_param = make_load_dh_param_handler(settings, gate);
            }

            return [load_server_certificates, load_dh_param](boost::asio::ssl::context& ctx)
            {
                try
                {
                    ctx.set_options(nmos::details::ssl_context_options);

                    const auto server_certificates = load_server_certificates();

                    if (server_certificates.empty())
                    {
                        throw ExceptionType({}, "Missing server certificates");
                    }

                    for (const auto& server_certificate : server_certificates)
                    {
                        const auto key = utility::us2s(server_certificate.private_key);
                        if (0 == key.size())
                        {
                            throw ExceptionType({}, "Missing private key");
                        }
                        const auto cert_chain = utility::us2s(server_certificate.certificate_chain);
                        if (0 == cert_chain.size())
                        {
                            throw ExceptionType({}, "Missing certificate chain");
                        }
                        ctx.use_private_key(boost::asio::buffer(key.data(), key.size()), boost::asio::ssl::context_base::pem);
                        ctx.use_certificate_chain(boost::asio::buffer(cert_chain.data(), cert_chain.size()));

                        const auto key_algorithm = server_certificate.key_algorithm;
                        if (key_algorithm.name.empty() || key_algorithm == key_algorithms::ECDSA)
                        {
                            // certificates may not have ECDH parameters, so ignore errors...
                            boost::system::error_code ec;
                            use_tmp_ecdh(ctx, boost::asio::buffer(cert_chain.data(), cert_chain.size()), ec);
                        }
                    }

                    set_cipher_list(ctx, nmos::details::ssl_cipher_list);

                    const auto dh_param = utility::us2s(load_dh_param());
                    if (dh_param.size())
                    {
                        ctx.use_tmp_dh(boost::asio::buffer(dh_param.data(), dh_param.size()));
                    }
                }
                catch (const boost::system::system_error& e)
                {
                    throw web::details::from_boost_system_system_error<ExceptionType>(e);
                }
            };
        }
#endif
    }

    // construct listener config based on settings
    web::http::experimental::listener::http_listener_config make_http_listener_config(const nmos::settings& settings, load_server_certificates_handler load_server_certificates, load_dh_param_handler load_dh_param, slog::base_gate& gate)
    {
        web::http::experimental::listener::http_listener_config config;
        config.set_backlog(nmos::fields::listen_backlog(settings));
#if !defined(_WIN32) || defined(CPPREST_FORCE_HTTP_LISTENER_ASIO)
        // hmm, hostport_listener::on_accept(...) in http_server_asio.cpp
        // only expects boost::system::system_error to be thrown, so for now
        // don't use web::http::http_exception
        config.set_ssl_context_callback(details::make_listener_ssl_context_callback<boost::system::system_error>(settings, load_server_certificates, load_dh_param, gate));
#endif

        return config;
    }

    // construct listener config based on settings
    web::websockets::experimental::listener::websocket_listener_config make_websocket_listener_config(const nmos::settings& settings, load_server_certificates_handler load_server_certificates, load_dh_param_handler load_dh_param, slog::base_gate& gate)
    {
        web::websockets::experimental::listener::websocket_listener_config config;
        config.set_backlog(nmos::fields::listen_backlog(settings));
#if !defined(_WIN32) || !defined(__cplusplus_winrt)
        config.set_ssl_context_callback(details::make_listener_ssl_context_callback<web::websockets::websocket_exception>(settings, load_server_certificates, load_dh_param, gate));
#endif

        return config;
    }

    namespace experimental
    {
        // map the configured client port to the server port on which to listen
        int server_port(int client_port, const nmos::settings& settings)
        {
            const auto& port_map = nmos::experimental::fields::proxy_map(settings).as_array();
            const auto found = std::find_if(port_map.begin(), port_map.end(), [&](const web::json::value& m)
            {
                return client_port == m.at(U("client_port")).as_integer();
            });
            return port_map.end() != found ? found->at(U("server_port")).as_integer() : client_port;
        }
    }
}
