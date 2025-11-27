/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Updated preprocessor condition comments for consistency.
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

#ifndef NMOS_SSL_CONTEXT_OPTIONS_H
#define NMOS_SSL_CONTEXT_OPTIONS_H

#if !defined(_WIN32) || !defined(__cplusplus_winrt) || defined(CPPREST_FORCE_HTTP_CLIENT_ASIO)
#include "boost/asio/ssl.hpp"

namespace nmos
{
    namespace details
    {
        // AMWA BCP-003-01 states that "implementations SHOULD support TLS 1.3 and SHALL support TLS 1.2."
        // "Implementations SHALL NOT use TLS 1.0 or 1.1. These are deprecated."
        // "Implementations SHALL NOT use SSL. Although the SSL protocol has previously,
        // been used to secure HTTP traffic no version of SSL is now considered secure."
        // See https://github.com/AMWA-TV/nmos-secure-communication/blob/v1.0.x/docs/1.0.%20Secure%20Communication.md#tls
        const auto ssl_context_options =
            ( boost::asio::ssl::context::no_sslv2
            | boost::asio::ssl::context::no_sslv3
            | boost::asio::ssl::context::no_tlsv1
// TLS v1.2 support was added in Boost 1.54.0
// but the option to disable TLS v1.1 didn't arrive until Boost 1.58.0
#if BOOST_VERSION >= 105800
            | boost::asio::ssl::context::no_tlsv1_1
#endif
            );

        const auto ssl_cipher_list = "HIGH:!kRSA";
    }
}

#endif

#endif
