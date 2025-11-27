/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Removed syntax error tests for ptokens and HSTS header parsing.
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
#include "cpprest/http_utils.h"

#include "bst/test/test.h"

////////////////////////////////////////////////////////////////////////////////////////////
BST_TEST_CASE(testGetHostPort)
{
    // no 'Host' header
    {
        web::http::http_request req;
        BST_REQUIRE_EQUAL(std::make_pair(utility::string_t{}, 0), web::http::get_host_port(req));
    }
    // empty 'Host' header
    {
        web::http::http_request req;
        req.headers().add(U("Host"), U(""));
        BST_REQUIRE_EQUAL(std::make_pair(utility::string_t{}, 0), web::http::get_host_port(req));
    }
    // host name alone
    {
        web::http::http_request req;
        req.headers().add(U("Host"), U("foobar"));
        BST_REQUIRE_EQUAL(std::make_pair(utility::string_t{ U("foobar") }, 0), web::http::get_host_port(req));
    }
    // host name and port
    {
        web::http::http_request req;
        req.headers().add(U("Host"), U("foobar:42"));
        BST_REQUIRE_EQUAL(std::make_pair(utility::string_t{ U("foobar") }, 42), web::http::get_host_port(req));
    }
    // host address and port
    {
        web::http::http_request req;
        req.headers().add(U("Host"), U("29.31.37.41:42"));
        BST_REQUIRE_EQUAL(std::make_pair(utility::string_t{ U("29.31.37.41") }, 42), web::http::get_host_port(req));
    }

    // 'X-Forwarded-Host'

    {
        web::http::http_request req;
        req.headers().add(U("Host"), U("foobar:42"));
        req.headers().add(U("X-Forwarded-Host"), U("baz, qux:57"));
        BST_REQUIRE_EQUAL(std::make_pair(utility::string_t{ U("baz") }, 0), web::http::get_host_port(req));
    }

    {
        web::http::http_request req;
        req.headers().add(U("X-Forwarded-Host"), U("baz:69, qux:57"));
        BST_REQUIRE_EQUAL(std::make_pair(utility::string_t{ U("baz") }, 69), web::http::get_host_port(req));
    }

    // suspect edge cases...

    {
        web::http::http_request req;
        req.headers().add(U("Host"), U("foobar:"));
        BST_REQUIRE_EQUAL(std::make_pair(utility::string_t{ U("foobar") }, 0), web::http::get_host_port(req));
    }
    {
        web::http::http_request req;
        req.headers().add(U("Host"), U(":42"));
        BST_REQUIRE_EQUAL(std::make_pair(utility::string_t{}, 42), web::http::get_host_port(req));
    }
    {
        web::http::http_request req;
        req.headers().add(U("Host"), U("foobar:baz"));
        BST_REQUIRE_EQUAL(std::make_pair(utility::string_t{ U("foobar") }, 0), web::http::get_host_port(req));
    }
}

////////////////////////////////////////////////////////////////////////////////////////////
BST_TEST_CASE(testHasHeaderValue)
{
    web::http::http_headers headers;
    BST_REQUIRE(!web::http::has_header_value(headers, U("foo"), 42));
    headers.add(U("foo"), 42);
    BST_REQUIRE(web::http::has_header_value(headers, U("foo"), 42));
    headers.add(U("foo"), 57);
    BST_REQUIRE(web::http::has_header_value(headers, U("foo"), 42));
    BST_REQUIRE(web::http::has_header_value(headers, U("foo"), 57));
}

////////////////////////////////////////////////////////////////////////////////////////////
namespace
{
    const std::pair<utility::string_t, web::http::experimental::ptokens> one{
        U("foo"),
        {
            { U("foo"), {} }
        }
    };

    const std::pair<utility::string_t, web::http::experimental::ptokens> two{
        U("foo, bar"),
        {
            { U("foo"), {} },
            { U("bar"), {} }
        }
    };

    const std::pair<utility::string_t, web::http::experimental::ptokens> params{
        U("foo;a=1;b=2, bar, baz;c=\"\";d=\"miaow, purr\", qux;e=\"\\\"\\\\\""),
        {
            { U("foo"), { { U("a"), U("1") }, { U("b"), U("2") } } },
            { U("bar"), {} },
            { U("baz"), { { U("c"), U("") }, { U("d"), U("miaow, purr") } } },
            { U("qux"), { { U("e"), U("\"\\") } } }
        }
    };
}

////////////////////////////////////////////////////////////////////////////////////////////
BST_TEST_CASE(testMakePtokensHeader)
{
    BST_REQUIRE(web::http::experimental::make_ptokens_header({}).empty());
    BST_REQUIRE_STRING_EQUAL(one.first, web::http::experimental::make_ptokens_header(one.second));
    BST_REQUIRE_STRING_EQUAL(two.first, web::http::experimental::make_ptokens_header(two.second));
    BST_REQUIRE_STRING_EQUAL(params.first, web::http::experimental::make_ptokens_header(params.second));
}

////////////////////////////////////////////////////////////////////////////////////////////
BST_TEST_CASE(testParsePtokensHeader)
{
    BST_REQUIRE(web::http::experimental::parse_ptokens_header({}).empty());
    BST_REQUIRE_EQUAL(one.second, web::http::experimental::parse_ptokens_header(one.first));
    BST_REQUIRE_EQUAL(two.second, web::http::experimental::parse_ptokens_header(two.first));
    BST_REQUIRE_EQUAL(params.second, web::http::experimental::parse_ptokens_header(params.first));
}

////////////////////////////////////////////////////////////////////////////////////////////
BST_TEST_CASE(testParsePtokensHeaderListRule)
{
    // see https://tools.ietf.org/html/rfc7230#section-7
    BST_REQUIRE_EQUAL(2, web::http::experimental::parse_ptokens_header(U("foo,bar")).size());
    BST_REQUIRE_EQUAL(2, web::http::experimental::parse_ptokens_header(U("foo ,bar,")).size());
    BST_REQUIRE_EQUAL(3, web::http::experimental::parse_ptokens_header(U("foo , ,bar,charlie   ")).size());
    BST_REQUIRE(web::http::experimental::parse_ptokens_header(U(",")).empty());
    BST_REQUIRE(web::http::experimental::parse_ptokens_header(U(",   ,")).empty());
}

////////////////////////////////////////////////////////////////////////////////////////////
BST_TEST_CASE(testParsePtokensHeaderOWS)
{
    BST_REQUIRE_EQUAL(params.second, web::http::experimental::parse_ptokens_header(U("  foo; a=  1 ;b  =2  ,bar, baz  ;c= \"\" ; d  = \"miaow, purr\",qux;e= \"\\\"\\\\\"  ")));
}

////////////////////////////////////////////////////////////////////////////////////////////
BST_TEST_CASE(testMakeTimingHeaderParseTimingHeader)
{
    BST_REQUIRE(web::http::experimental::make_timing_header({}).empty());

    // see https://w3c.github.io/server-timing/#examples
    std::vector<std::pair<utility::string_t, web::http::experimental::timing_metrics>> examples{
        { U("miss, db;dur=53, app;dur=47.2"), { { U("miss") }, { U("db"), 53 }, { U("app"), 47.2 } } },
        { U("customView, dc;desc=atl"), { { U("customView") }, { U("dc"), U("atl") } } },
        { U("cache;desc=\"Cache Read\";dur=23.2"), { { U("cache"), 23.2, U("Cache Read") } } },
        { U("total;dur=123.4"), { { U("total"), 123.4 } } }
    };

    for (const auto& example : examples)
    {
        BST_REQUIRE_EQUAL(example.second, web::http::experimental::parse_timing_header(example.first));
        BST_REQUIRE_EQUAL(example.second, web::http::experimental::parse_timing_header(web::http::experimental::make_timing_header(example.second)));
    }
}

////////////////////////////////////////////////////////////////////////////////////////////
BST_TEST_CASE(testParseTimingHeaderEdgeCases)
{
    BST_REQUIRE_EQUAL(42.0, web::http::experimental::parse_timing_header(U("foo;dur=42;desc=bar;dur=57")).front().duration);
    BST_REQUIRE(web::http::experimental::parse_timing_header(U("foo;desc=\"\";dur=42;desc=bar")).front().description.empty());
}
