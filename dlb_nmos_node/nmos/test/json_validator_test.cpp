/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Added missing semicolon after function definition to resolve compilation error.
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
#include "cpprest/json_validator.h"

#include "bst/test/test.h"
#include "cpprest/basic_utils.h" // for utility::us2s, utility::s2us
#include "cpprest/json_utils.h"

namespace
{
    const auto id = web::uri{ U("/test") };

    web::json::experimental::json_validator make_validator(const web::json::value& schema, const web::uri& id)
    {
        return web::json::experimental::json_validator
        {
            [&](const web::uri&) { return schema; },
            { id }
        };
    };
}

////////////////////////////////////////////////////////////////////////////////////////////
BST_TEST_CASE(testInvalidTypeSchema)
{
    using web::json::value_of;

    const bool keep_order = true;

    const auto schema = value_of({
        { U("$schema"), U("http://json-schema.org/draft-04/schema#")},
        { U("type"), U("object")},
        { U("properties"), value_of({
            { U("foo"), value_of({
                { U("anyOf"), value_of({
                    value_of({
                        { U("type"), U("string") },
                        { U("pattern"), U("^auto$") }
                    }, keep_order),
                    U("bad")
                })
            }})
        }})
    }});

   // invalid JSON-type for schema
   BST_REQUIRE_THROW(make_validator(schema, id), std::invalid_argument);
}

////////////////////////////////////////////////////////////////////////////////////////////
BST_TEST_CASE(testEnumSchema)
{
    using web::json::value_of;

    const bool keep_order = true;

    const auto schema = value_of({
        { U("$schema"), U("http://json-schema.org/draft-04/schema#")},
        { U("type"), U("object")},
        { U("properties"), value_of({
            { U("foo"), value_of({
                { U("anyOf"), value_of({
                    value_of({
                        { U("type"), U("string") },
                        { U("pattern"), U("^auto$") }
                        }, keep_order),
                    value_of({
                        { U("enum"), value_of({
                            { U("good") }
                        })
                    }})
                })
            }})
        }})
    }});

    auto validator = make_validator(schema, id);

    // not in anyOf
    BST_REQUIRE_THROW(validator.validate(value_of({ { U("foo"), U("bad") } }), id), web::json::json_exception);

    // in anyOf, pattern
    validator.validate(value_of({ { U("foo"), U("auto") } }), id);
    BST_REQUIRE(true);

    // in anyOf, enum
    validator.validate(value_of({ { U("foo"), U("good") } }), id);
    BST_REQUIRE(true);
}
