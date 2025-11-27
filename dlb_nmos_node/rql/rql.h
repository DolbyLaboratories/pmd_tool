/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Removed query validation functionality.
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

#ifndef RQL_RQL_H
#define RQL_RQL_H

#include <functional>
#include <unordered_map>
#include "cpprest/json.h"

namespace rql
{
    // Resource Query Language
    // See https://github.com/persvr/rql for the extensible grammar
    // and https://doc.apsstandard.org/6.0/spec/rql/ for sometimes better documentation for a particular domain

    // Helper for parsing a Resource Query Language string into a json representation of the abstract syntax tree
    // The resulting json value follows the structure described in the RQL reference implementation for json-native
    // simple types (strings, numbers and booleans) and arrays, as well as in the representation of call-operators
    // being a json object with "name" and "args" fields, the latter an array containing at least one value (since
    // the ABNF for RQL doesn't support empty arrays or argument lists).
    // See https://github.com/persvr/rql#rqlparser
    // This implementation has one extension in that unrecognized RQL types are transformed to json objects with
    // "type" and (string) "value" fields. This allows typed values to be processed as appropriate by implementations.

    web::json::value parse_query(const utility::string_t& query);

    bool is_call_operator(const web::json::value& arg);
    bool is_typed_value(const web::json::value& arg);

    // Evaluate an RQL query using a resource property extractor function, and set of RQL call-operators

    struct evaluator;

    typedef std::function<bool(web::json::value& results, const web::json::value& key)> extractor;
    typedef std::unordered_map<utility::string_t, std::function<web::json::value(const evaluator& eval, const web::json::value& args)>> operators;

    struct evaluator
    {
        explicit evaluator(extractor extract); // with default call-operators
        evaluator(extractor extract, operators operators);

        web::json::value operator()(const web::json::value& arg, bool extract_value = false) const;

        extractor extract;
        rql::operators operators;
    };

    // Construct a set of RQL call-operators, using default json value comparison

    operators default_operators();
    operators default_any_operators(); // array-friendly variant

    // Construct a set of RQL call-operators, using specified json value comparison
    // This allows typed values to be processed as appropriate by implementations.

    // result should be value_true, value_false or value_indeterminate
    typedef std::function<web::json::value(const web::json::value& lhs, const web::json::value& rhs)> comparator;

    operators default_operators(comparator equal_to, comparator less);
    operators default_any_operators(comparator equal_to, comparator less); // array-friendly variant

    // Helpers for json value comparison, implementing three-valued (tribool) logic

    const web::json::value value_indeterminate = web::json::value::null();
    const web::json::value value_false = web::json::value::boolean(false);
    const web::json::value value_true = web::json::value::boolean(true);

    web::json::value default_equal_to(const web::json::value& lhs, const web::json::value& rhs);
    web::json::value default_less(const web::json::value& lhs, const web::json::value& rhs);
}

#endif
