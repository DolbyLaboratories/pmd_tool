/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Fixed preprocessor conditions for host wildcard testing.
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
// The first "test" is of course whether the header compiles standalone
#include "cpprest/api_router.h"

#include "bst/test/test.h"
#include "cpprest/basic_utils.h" // for utility::us2s, utility::s2us

////////////////////////////////////////////////////////////////////////////////////////////
BST_TEST_CASE(testMakeListenerUri)
{
#if defined(_WIN32) && !defined(CPPREST_FORCE_HTTP_LISTENER_ASIO)
    BST_REQUIRE_STRING_EQUAL("http://*:42/", utility::us2s(web::http::experimental::listener::make_listener_uri(42).to_string()));
#else
    BST_REQUIRE_STRING_EQUAL("http://0.0.0.0:42/", utility::us2s(web::http::experimental::listener::make_listener_uri(42).to_string()));
#endif

    BST_REQUIRE_STRING_EQUAL("http://203.0.113.42:42/", utility::us2s(web::http::experimental::listener::make_listener_uri(U("203.0.113.42"), 42).to_string()));

    BST_REQUIRE_STRING_EQUAL("https://203.0.113.42:42/", utility::us2s(web::http::experimental::listener::make_listener_uri(true, U("203.0.113.42"), 42).to_string()));
}

////////////////////////////////////////////////////////////////////////////////////////////
BST_TEST_CASE(testGetRouteRelativePath)
{
    using utility::us2s;
    using web::http::experimental::listener::details::get_route_relative_path;
    using web::http::http_exception;

    web::http::http_request req;
    req.set_request_uri(web::uri(U("http://host:123/foo/bar/baz?qux=quux#quuux")));

    // clear specification

    BST_REQUIRE_STRING_EQUAL("/foo/bar/baz", us2s(get_route_relative_path(req, U(""))));
    BST_REQUIRE_STRING_EQUAL("/bar/baz", us2s(get_route_relative_path(req, U("/foo"))));
    BST_REQUIRE_STRING_EQUAL("", us2s(get_route_relative_path(req, U("/foo/bar/baz"))));
    BST_REQUIRE_THROW(get_route_relative_path(req, U("/qux")), http_exception);

    // less clear specification

    // compatible with http_request::relative_uri(), but should it be "foo/bar/baz"?
    BST_CHECK_STRING_EQUAL("/foo/bar/baz", us2s(get_route_relative_path(req, U("/"))));

    // should it be "/bar/baz"?
    BST_CHECK_STRING_EQUAL("bar/baz", us2s(get_route_relative_path(req, U("/foo/"))));

    // should it throw, no match?
    BST_CHECK_STRING_EQUAL("ar/baz", us2s(get_route_relative_path(req, U("/foo/b"))));

    // should it be ""?
    BST_CHECK_THROW(get_route_relative_path(req, U("/foo/bar/baz/")), http_exception);
}

////////////////////////////////////////////////////////////////////////////////////////////
BST_TEST_CASE(testRouteRegexMatch)
{
    using web::http::experimental::listener::details::match_entire;
    using web::http::experimental::listener::details::match_prefix;
    using web::http::experimental::listener::details::route_regex_match;

    utility::smatch_t route_match;

    BST_REQUIRE(route_regex_match(U("/foo/bar/baz"), route_match, utility::regex_t(U("/f../b../b..")), match_entire));
    BST_REQUIRE(route_regex_match(U("/foo/bar/baz"), route_match, utility::regex_t(U("/f../b../b..")), match_prefix));

    BST_REQUIRE(!route_regex_match(U("/foo/bar/baz/qux"), route_match, utility::regex_t(U("/f../b../b..")), match_entire));
    BST_REQUIRE(route_regex_match(U("/foo/bar/baz/qux"), route_match, utility::regex_t(U("/f../b../b..")), match_prefix));

    BST_REQUIRE(!route_regex_match(U("/foo/bar/qux"), route_match, utility::regex_t(U("/f../b../b..")), match_prefix));
    BST_REQUIRE(!route_regex_match(U("/qux/foo/bar/baz"), route_match, utility::regex_t(U("/f../b../b..")), match_prefix));
}

////////////////////////////////////////////////////////////////////////////////////////////
BST_TEST_CASE(testGetParameters)
{
    using web::http::experimental::listener::details::get_parameters;
    using web::http::experimental::listener::route_parameters;

    const utility::string_t path{ U("ABCD") };
    const utility::regex_t route_regex{ U("A(B(?:C(x)?))((D)?)") };
    utility::named_sub_matches_t parameter_sub_matches;
    parameter_sub_matches[U("BCx")] = 1;
    parameter_sub_matches[U("x")] = 2;
    parameter_sub_matches[U("D")] = 3;

    route_parameters expected;
    expected[U("BCx")] = U("BC");
    expected[U("D")] = U("D");

    utility::smatch_t route_match;
    BST_REQUIRE(bst::regex_match(path, route_match, route_regex));
    BST_REQUIRE(expected == get_parameters(parameter_sub_matches, route_match));
}
