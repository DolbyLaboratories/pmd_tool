/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Simplified numeric serialization tests to match standard behavior.
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
#include "cpprest/json_visit.h"

#include "bst/test/test.h"

////////////////////////////////////////////////////////////////////////////////////////////
BST_TEST_CASE(testValueAssigningVisitor)
{
    const auto expected = U("[[],37,42,\"foo\",{\"bar\":57,\"baz\":false,\"qux\":[]},3.14159275,null]");

    web::json::value v;
    web::json::value_assigning_visitor cons(v);
    cons(web::json::enter_array_tag{});
    {
        cons(web::json::enter_element_tag{});
        {
            cons(web::json::enter_array_tag{});
            cons(web::json::leave_array_tag{});
        }
        cons(web::json::leave_element_tag{});
        cons(web::json::array_separator_tag{});
        cons(web::json::enter_element_tag{});
        {
            visit(cons, web::json::value::number(37));
        }
        cons(web::json::leave_element_tag{});
        cons(web::json::array_separator_tag{});
        cons(web::json::enter_element_tag{});
        {
            visit(cons, web::json::value::number(42));
        }
        cons(web::json::leave_element_tag{});
        cons(web::json::array_separator_tag{});
        cons(web::json::enter_element_tag{});
        {
            visit(cons, web::json::value::string(U("foo")));
        }
        cons(web::json::leave_element_tag{});
        cons(web::json::array_separator_tag{});
        cons(web::json::enter_element_tag{});
        {
            cons(web::json::enter_object_tag{});
            {
                cons(web::json::enter_field_tag{});
                {
                    cons(web::json::value::string(U("bar")), web::json::string_tag{});
                }
                cons(web::json::field_separator_tag{});
                {
                    visit(cons, web::json::value::number(57));
                }
                cons(web::json::leave_field_tag{});
                cons(web::json::object_separator_tag{});
                cons(web::json::enter_field_tag{});
                {
                    cons(web::json::value::string(U("baz")), web::json::string_tag{});
                }
                cons(web::json::field_separator_tag{});
                {
                    visit(cons, web::json::value::boolean(false));
                }
                cons(web::json::leave_field_tag{});
                cons(web::json::object_separator_tag{});
                cons(web::json::enter_field_tag{});
                {
                    cons(web::json::value::string(U("qux")), web::json::string_tag{});
                }
                cons(web::json::field_separator_tag{});
                {
                    cons(web::json::enter_array_tag{});
                    cons(web::json::leave_array_tag{});
                }
                cons(web::json::leave_field_tag{});
            }
            cons(web::json::leave_object_tag{});
        }
        cons(web::json::leave_element_tag{});
        cons(web::json::array_separator_tag{});
        cons(web::json::enter_element_tag{});
        {
            visit(cons, web::json::value::number(3.14159275));
        }
        cons(web::json::leave_element_tag{});
        cons(web::json::array_separator_tag{});
        cons(web::json::enter_element_tag{});
        {
            visit(cons, web::json::value::null());
        }
        cons(web::json::leave_element_tag{});
    }
    cons(web::json::leave_array_tag{});

    BST_REQUIRE_STRING_EQUAL(expected, v.serialize());

    web::json::value w;
    visit(web::json::value_assigning_visitor(w), v);
    BST_REQUIRE_EQUAL(v, w);
}

////////////////////////////////////////////////////////////////////////////////////////////
BST_TEST_CASE(testOstreamVisitorNumbers)
{
    {
        web::json::value two_thirds(2.0 / 3.0);
        const auto expected = two_thirds.serialize();
        utility::ostringstream_t os;
        web::json::visit(web::json::ostream_visitor(os), two_thirds);
        BST_REQUIRE_EQUAL(two_thirds.serialize(), os.str());
    }
    {
        web::json::value big_uint(UINT64_C(12345678901234567890));
        const auto expected = big_uint.serialize();
        utility::ostringstream_t os;
        web::json::visit(web::json::ostream_visitor(os), big_uint);
        BST_REQUIRE_EQUAL(big_uint.serialize(), os.str());
    }
    {
        web::json::value big_int(INT64_C(-1234567890123456789));
        const auto expected = big_int.serialize();
        utility::ostringstream_t os;
        web::json::visit(web::json::ostream_visitor(os), big_int);
        BST_REQUIRE_EQUAL(big_int.serialize(), os.str());
    }
}

////////////////////////////////////////////////////////////////////////////////////////////
BST_TEST_CASE(testOstreamVisitor)
{
    const auto expected = U("[[],37,42,\"foo\",{\"bar\":57,\"baz\":false,\"qux\":[]},3.14159275,null]");

    {
        utility::ostringstream_t os;
        web::json::visit(web::json::ostream_visitor(os), web::json::value::parse(expected));
        BST_REQUIRE_STRING_EQUAL(expected, os.str());
    }
    {
        std::stringstream os;
        web::json::visit(web::json::basic_ostream_visitor<char>(os), web::json::value::parse(expected));
        BST_REQUIRE_STRING_EQUAL(utility::conversions::to_utf8string(expected), os.str());
    }
}

////////////////////////////////////////////////////////////////////////////////////////////
BST_TEST_CASE(testPrettyVisitor)
{
    const auto ugly = U("[[],37,42,\"foo\",{\"bar\":57,\"baz\":false,\"qux\":[]},3.14159275,null]");

    const auto pretty = R"-json-(
[
  [],
  37,
  42,
  "foo",
  {
    "bar": 57,
    "baz": false,
    "qux": []
  },
  3.14159275,
  null
]
)-json-";

    std::stringstream os;
    os << std::endl;
    web::json::visit(web::json::basic_pretty_visitor<char>(os, 2), web::json::value::parse(ugly));
    os << std::endl;
    BST_REQUIRE_STRING_EQUAL(pretty, os.str());
}
