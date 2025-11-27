/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Removed RSA private key handler support.
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

#ifndef NMOS_CERTIFICATE_HANDLERS_H
#define NMOS_CERTIFICATE_HANDLERS_H

#include <functional>
#include <vector>
#include "cpprest/details/basic_types.h"
#include "nmos/settings.h"
#include "nmos/string_enum.h"

namespace slog
{
    class base_gate;
}

namespace nmos
{
    // callback to supply trusted root CA certificate(s) in PEM format
    // this callback is executed when opening the HTTP or WebSocket client
    // this callback should not throw exceptions
    // on Windows, if C++ REST SDK is built with CPPREST_HTTP_CLIENT_IMPL=winhttp (reported as "client=winhttp" by nmos::get_build_settings_info)
    // the trusted root CA certificates must also be imported into the certificate store
    typedef std::function<utility::string_t()> load_ca_certificates_handler;

    // common key algorithms
    DEFINE_STRING_ENUM(key_algorithm)
    namespace key_algorithms
    {
        const key_algorithm ECDSA{ U("ECDSA") };
        const key_algorithm RSA{ U("RSA") };
    }

    // certificate details including the private key and the certificate chain in PEM format
    // the key algorithm may also be specified
    struct certificate
    {
        certificate() {}

        certificate(utility::string_t private_key, utility::string_t certificate_chain)
            : private_key(std::move(private_key))
            , certificate_chain(std::move(certificate_chain))
        {}

        certificate(nmos::key_algorithm key_algorithm, utility::string_t private_key, utility::string_t certificate_chain)
            : key_algorithm(std::move(key_algorithm))
            , private_key(std::move(private_key))
            , certificate_chain(std::move(certificate_chain))
        {}

        nmos::key_algorithm key_algorithm;
        utility::string_t private_key;
        // the chain should be sorted starting with the end entity's certificate, followed by any intermediate CA certificates, and ending with the highest level (root) CA
        utility::string_t certificate_chain;
    };

    // callback to supply a list of server certificates
    // this callback is executed when opening the HTTP or WebSocket listener
    // this callback should not throw exceptions
    // on Windows, if C++ REST SDK is built with CPPREST_HTTP_LISTENER_IMPL=httpsys (reported as "listener=httpsys" by nmos::get_build_settings_info)
    // one of the certificates must also be bound to each port e.g. using 'netsh add sslcert'
    typedef std::function<std::vector<certificate>()> load_server_certificates_handler;

    // callback to supply Diffie-Hellman parameters for ephemeral key exchange support, in PEM format or empty string for no support
    // see e.g. https://wiki.openssl.org/index.php/Diffie-Hellman_parameters
    // this callback is executed when opening the HTTP or WebSocket listener
    // this callback should not throw exceptions
    typedef std::function<utility::string_t()> load_dh_param_handler;

    // construct callback to load certification authorities from file based on settings, see nmos/certificate_settings.h
    load_ca_certificates_handler make_load_ca_certificates_handler(const nmos::settings& settings, slog::base_gate& gate);

    // construct callback to load server certificates from files based on settings, see nmos/certificate_settings.h
    load_server_certificates_handler make_load_server_certificates_handler(const nmos::settings& settings, slog::base_gate& gate);

    // construct callback to load Diffie-Hellman parameters for ephemeral key exchange support from file based on settings, see nmos/certificate_settings.h
    load_dh_param_handler make_load_dh_param_handler(const nmos::settings& settings, slog::base_gate& gate);
}

#endif
