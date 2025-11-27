/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Removed tests for vector-based key path insert/extract functionality.
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
#include "cpprest/json_utils.h"

#include <map>
#include "boost/range/adaptor/transformed.hpp"
#include "bst/test/test.h"

namespace
{
    web::json::value merged_original(web::json::value target, const web::json::value& source)
    {
        web::json::merge_patch(target, source, false);
        return target;
    }

    web::json::value merged_permissive(web::json::value target, const web::json::value& source)
    {
        web::json::merge_patch(target, source, true);
        return target;
    }

    const auto non_composites = web::json::value_of({
        _XPLATSTR("What do you get if you multiply six by nine?"),
        57,
        3.1419275,
        false,
        web::json::value::null()
    });

    const auto empty_composites = web::json::value_of({
        web::json::value::array(),
        web::json::value::object()
    });

    const utility::string_t foo = _XPLATSTR("foo");
    const utility::string_t bar = _XPLATSTR("bar");
    const utility::string_t baz = _XPLATSTR("baz");
}

////////////////////////////////////////////////////////////////////////////////////////////
BST_TEST_CASE(testMergePatch)
{
    const auto merged = merged_original;

    using web::json::value;
    using web::json::value_of;
    using web::json::json_exception;

    for (const auto& source : non_composites.as_array())
    {
        for (const auto& target : non_composites.as_array())
        {
            BST_REQUIRE_EQUAL(source, merged(target, source));
        }
        for (const auto& target : empty_composites.as_array())
        {
            BST_REQUIRE_THROW(merged(target, source), json_exception);
        }
    }

    for (const auto& source : empty_composites.as_array())
    {
        for (const auto& target : non_composites.as_array())
        {
            BST_REQUIRE_THROW(merged(target, source), json_exception);
        }
    }

    BST_REQUIRE_EQUAL(value::array(), merged(value::array(), value::array()));
    BST_REQUIRE_EQUAL(value::object(), merged(value::object(), value::object()));
    BST_REQUIRE_THROW(merged(value::object(), value::array()), json_exception);
    BST_REQUIRE_THROW(merged(value::array(), value::object()), json_exception);

    BST_REQUIRE_EQUAL(value_of({ 3, 4 }), merged(value_of({ 1, 2 }), value_of({ 3, 4 })));
    BST_REQUIRE_THROW(merged(value_of({ 1, 2 }), value_of({ 3 })), json_exception);
    BST_REQUIRE_THROW(merged(value_of({ 1 }), value_of({ 2, 3 })), json_exception);

    BST_REQUIRE_EQUAL(value_of({ { foo, 3 }, { bar, 2 } }), merged(value_of({ { foo, 1 }, { bar, 2 } }), value_of({ { foo, 3 } })));
    BST_REQUIRE_THROW(merged(value_of({ { foo, 1 }, { bar, 2 } }), value_of({ { baz, 3 } })), json_exception);
}

////////////////////////////////////////////////////////////////////////////////////////////
BST_TEST_CASE(testMergePatchPermissive)
{
    const auto merged = merged_permissive;

    using web::json::value;
    using web::json::value_of;
    using web::json::json_exception;

    for (const auto& source : non_composites.as_array())
    {
        for (const auto& target : non_composites.as_array())
        {
            BST_REQUIRE_EQUAL(source, merged(target, source));
        }
        for (const auto& target : empty_composites.as_array())
        {
            BST_REQUIRE_EQUAL(source, merged(target, source));
        }
    }

    for (const auto& source : empty_composites.as_array())
    {
        for (const auto& target : non_composites.as_array())
        {
            BST_REQUIRE_EQUAL(source, merged(target, source));
        }
    }

    BST_REQUIRE_EQUAL(value::array(), merged(value::array(), value::array()));
    BST_REQUIRE_EQUAL(value::object(), merged(value::object(), value::object()));
    BST_REQUIRE_EQUAL(value::array(), merged(value::object(), value::array()));
    BST_REQUIRE_EQUAL(value::object(), merged(value::array(), value::object()));

    BST_REQUIRE_EQUAL(value_of({ 3, 4 }), merged(value_of({ 1, 2 }), value_of({ 3, 4 })));
    BST_REQUIRE_EQUAL(value_of({ 3 }), merged(value_of({ 1, 2 }), value_of({ 3 })));
    BST_REQUIRE_EQUAL(value_of({ 2, 3 }), merged(value_of({ 1 }), value_of({ 2, 3 })));

    BST_REQUIRE_EQUAL(value_of({ { foo, 3 }, { bar, 2 } }), merged(value_of({ { foo, 1 }, { bar, 2 } }), value_of({ { foo, 3 } })));
    BST_REQUIRE_EQUAL(value_of({ { foo, 1 }, { bar, 2 }, { baz, 3 } }), merged(value_of({ { foo, 1 }, { bar, 2 } }), value_of({ { baz, 3 } })));

    // additional tests for permissive mode

    const auto ofoo = value_of({ { foo, 1 } });
    const auto obar = value_of({ { bar, 2 } });
    const auto ofoobar = value_of({ { foo, 1 }, { bar, 2 } });

    BST_REQUIRE_EQUAL(value_of({ ofoo, obar }), merged(value_of({ ofoo, obar }), value_of({ value::object(), value::object() })));
    BST_REQUIRE_EQUAL(value_of({ ofoo }), merged(value_of({ ofoo, obar }), value_of({ value::object() })));
    BST_REQUIRE_EQUAL(value_of({ ofoo }), merged(value_of({ ofoo, obar }), value_of({ value::object(), value::null() })));
    BST_REQUIRE_EQUAL(value_of({ obar }), merged(value_of({ ofoo, obar }), value_of({ value::null(), value::object() })));
    BST_REQUIRE_EQUAL(value::array(), merged(value_of({ ofoo, obar }), value_of({ value::null() })));
    BST_REQUIRE_EQUAL(value_of({ ofoo, obar }), merged(value_of({ ofoobar, ofoobar }), value_of({ value_of({ { bar, value::null() } }), value_of({ { foo, value::null() } }) })));
}

////////////////////////////////////////////////////////////////////////////////////////////
BST_TEST_CASE(testMergePatchRFC7386)
{
    // See https://tools.ietf.org/html/rfc7386#section-3

    const char* example = R"-json-(
       {
         "title": "Goodbye!",
         "author" : {
       "givenName" : "John",
       "familyName" : "Doe"
         },
         "tags":[ "example", "sample" ],
         "content": "This will be unchanged"
       }
    )-json-";

    const char* request = R"-json-(
       {
         "title": "Hello!",
         "phoneNumber": "+01-123-456-7890",
         "author": {
       "familyName": null
         },
         "tags": [ "example" ]
       }
    )-json-";

    const char* result = R"-json-(
       {
         "title": "Hello!",
         "author" : {
       "givenName" : "John"
         },
         "tags": [ "example" ],
         "content": "This will be unchanged",
         "phoneNumber": "+01-123-456-7890"
       }
    )-json-";

    const auto target = web::json::value::parse(utility::conversions::to_string_t(example));
    const auto source = web::json::value::parse(utility::conversions::to_string_t(request));
    const auto expected = web::json::value::parse(utility::conversions::to_string_t(result));

    BST_REQUIRE_EQUAL(expected, merged_permissive(target, source));
}

////////////////////////////////////////////////////////////////////////////////////////////
BST_TEST_CASE(testPreprocess)
{
    const auto example = utility::conversions::to_string_t(R"({ //" foo /*
  "bar": /*
baz
qux */ 42,
  "quux": "/*//\r\n\"quuux\\"
/** /* **/ }
)");

    const auto stripped = utility::conversions::to_string_t(R"({  )" R"(
   "bar":   42,
   "quux":  "/*//\r\n\"quuux\\"
  }
)");

    BST_REQUIRE_STRING_EQUAL(stripped, web::json::experimental::preprocess(example));

    const auto parsed = web::json::value_of({
        { U("bar"), 42 },
        { U("quux"), U("/*//\r\n\"quuux\\") }
    });

    BST_REQUIRE_EQUAL(parsed, web::json::value::parse(web::json::experimental::preprocess(example)));

    // ho hum, turns out web::json::value::parse doesn't advertise the fact, but it actually handles single- and multi-line comments already...
    BST_REQUIRE_EQUAL(parsed, web::json::value::parse(example));
}

////////////////////////////////////////////////////////////////////////////////////////////
BST_TEST_CASE(testSerialize)
{
    {
        auto ints = { 1, 2, 4, 8, 16, 32 };

        auto expected = web::json::value::array(std::vector<web::json::value>(ints.begin(), ints.end())).serialize();
        auto actual = web::json::serialize_array(ints | boost::adaptors::transformed([](int i) { return web::json::value(i); }));
        BST_REQUIRE_EQUAL(expected, actual);
    }

    {
        std::vector<utility::string_t> strs{ U("meow"), U("purr"), U("hiss"), U("yowl") };

        auto expected = web::json::value::array(std::vector<web::json::value>(strs.begin(), strs.end())).serialize();
        auto actual = web::json::serialize_array(strs | boost::adaptors::transformed([](const utility::string_t& s) { return web::json::value(s); }));
        BST_REQUIRE_EQUAL(expected, actual);
    }

    {
        std::map<utility::string_t, web::json::value> fields{
            { U("meow"), web::json::value::string(U("foo")) },
            { U("purr"), web::json::value::boolean(true) },
            { U("hiss"), web::json::value::number(42) },
            { U("yowl"), web::json::value::array() }
        };

        auto expected = web::json::value::object(std::vector<std::pair<utility::string_t, web::json::value>>(fields.begin(), fields.end())).serialize();
        auto actual = web::json::serialize_object(fields);
        BST_REQUIRE_EQUAL(expected, actual);
    }
}
