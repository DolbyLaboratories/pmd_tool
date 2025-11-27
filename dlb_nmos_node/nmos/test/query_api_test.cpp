/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Updated documentation references from specs.amwa.tv to GitHub
 * repository links for query API examples and schemas.
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
 *         Copyright (c) 2019-2025, Dolby Laboratories Inc.
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

#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <vector>
#include "bst/test/test.h"
#include "nmos/is04_versions.h"
#include "nmos/json_schema.h"

////////////////////////////////////////////////////////////////////////////////////////////
BST_TEST_CASE(testQueryAPISubscriptionsExtensionSchema)
{
    using web::json::value_of;
    using web::json::value;

    const web::json::experimental::json_validator validator
    {
        nmos::experimental::load_json_schema,
        boost::copy_range<std::vector<web::uri>>(nmos::is04_versions::all | boost::adaptors::transformed(nmos::experimental::make_queryapi_subscriptions_post_request_schema_uri))
    };

    // valid subscriptions post request data
    // see https://github.com/AMWA-TV/nmos-discovery-registration/blob/v1.3.x/examples/queryapi-subscriptions-post-request.json
    auto data = value_of({
        { U("max_update_rate_ms"), 100 },
        { U("resource_path"), U("/nodes") },
        { U("params"), value_of({
            { U("label"), U("host1") }
        }) },
        { U("persist"), false },
        { U("secure"), false }
    });

    // validate successfully, i.e. no exception
    // see https://github.com/AMWA-TV/nmos-discovery-registration/blob/v1.3.x/APIs/schemas/queryapi-subscriptions-post-request.json
    validator.validate(data, nmos::experimental::make_queryapi_subscriptions_post_request_schema_uri(nmos::is04_versions::v1_3));

    // empty path, for experimental extension
    data[U("resource_path")] = value::string(U(""));
    validator.validate(data, nmos::experimental::make_queryapi_subscriptions_post_request_schema_uri(nmos::is04_versions::v1_3));
}
