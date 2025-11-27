/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Removed pop_back() method declaration from api_router class.
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

#ifndef CPPREST_API_ROUTER_H
#define CPPREST_API_ROUTER_H

#include <functional>
#include <list>
#include <unordered_map>
#include "cpprest/http_utils.h"
#include "cpprest/json_ops.h" // hmm, only for names used in using declarations
#include "cpprest/regex_utils.h" // hmm, only for types used in details functions

// api_router is an extension to http_listener that uses regexes to define route patterns
namespace web
{
    namespace http
    {
        namespace experimental
        {
            namespace listener
            {
                // a using-directive with the following namespace makes defining routers less verbose
                // using namespace api_router_using_declarations;
                namespace api_router_using_declarations {}

                typedef std::unordered_map<utility::string_t, utility::string_t> route_parameters;

                // route handlers have access to the request, and a mutable response object, the route path and parameters extracted from it by the matched route pattern;
                // a handler may e.g. reply to the request or initiate asynchronous processing, and returns a flag indicating whether to continue matching routes or not
                typedef std::function<pplx::task<bool>(web::http::http_request req, web::http::http_response res, const utility::string_t& route_path, const route_parameters& parameters)> route_handler;

                // api router implementation
                namespace details
                {
                    class api_router_impl;

                    enum match_flag_type { match_entire = 0, match_prefix = 1 };

                    utility::string_t get_route_relative_path(const web::http::http_request& req, const utility::string_t& route_path);
                    route_parameters get_parameters(const utility::named_sub_matches_t& parameter_sub_matches, const utility::smatch_t& route_match);
                    bool route_regex_match(const utility::string_t& path, utility::smatch_t& route_match, const utility::regex_t& route_regex, match_flag_type flags);
                }

                class api_router
                {
                public:
                    api_router();

                    // allow use as a handler with http_listener
                    void operator()(web::http::http_request req);
                    // allow use as a mounted handler in another api_router
                    pplx::task<bool> operator()(web::http::http_request req, web::http::http_response res, const utility::string_t& route_path, const route_parameters& parameters);

                    // add a method-specific handler to support requests for this route
                    void support(const utility::string_t& route_pattern, const web::http::method& method, route_handler handler);
                    // add a handler to support all other requests for this route (must be added after any method-specific handlers)
                    void support(const utility::string_t& route_pattern, route_handler all_handler);

                    // add a method-specific handler to support requests for this route and sub-routes
                    void mount(const utility::string_t& route_pattern, const web::http::method& method, route_handler handler);
                    // add a handler to support all other requests for this route and sub-routes (must be added after any method-specific handlers)
                    void mount(const utility::string_t& route_pattern, route_handler all_handler);

                    // provide an exception handler for this route and sub-routes (using std::current_exception, etc.)
                    void set_exception_handler(route_handler handler);

                private:
                    std::shared_ptr<details::api_router_impl> impl;
                };

                // convenient using declarations to make defining routers less verbose
                namespace api_router_using_declarations
                {
                    using utility::string_t;
                    using web::http::experimental::listener::api_router;
                    using web::http::experimental::listener::route_parameters;
                    using web::http::http_request;
                    using web::http::http_response;
                    using web::http::methods;
                    using web::http::set_reply;
                    using web::http::status_codes;
                    using web::json::value;
                    using web::json::value_of;
                }
            }
        }
    }
}

#endif
