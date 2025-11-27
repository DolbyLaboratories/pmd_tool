/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Fixed group hint generation - now properly checks
 * optional_group_scope.name.empty() instead of the object itself.
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

#ifndef NMOS_GROUP_HINT_H
#define NMOS_GROUP_HINT_H

#include "cpprest/json_utils.h"
#include "nmos/string_enum.h"

// Group Hint
// See https://github.com/AMWA-TV/nmos-parameter-registers/blob/main/tags/grouphint.md
namespace nmos
{
    namespace fields
    {
        const web::json::field_as_value_or group_hint{ U("urn:x-nmos:tag:grouphint/v1.0"), web::json::value::array() };
    }

    DEFINE_STRING_ENUM(group_scope)
    namespace group_scopes
    {
        const group_scope device{ U("device") };
        const group_scope node{ U("node") };
    }

    struct group_hint
    {
        utility::string_t group_name;
        utility::string_t role_in_group;
        group_scope optional_group_scope;

        group_hint(const utility::string_t& group_name, const utility::string_t& role_in_group, const group_scope& optional_group_scope = {})
            : group_name(group_name)
            , role_in_group(role_in_group)
            , optional_group_scope(optional_group_scope)
        {}
    };

    utility::string_t make_group_hint(const group_hint& group_hint);

    group_hint parse_group_hint(const utility::string_t& group_hint);
}

#endif
