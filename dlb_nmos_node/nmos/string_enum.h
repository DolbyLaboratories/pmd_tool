/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Removed forward declaration include and empty() method from string_enum base class.
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

#ifndef NMOS_STRING_ENUM_H
#define NMOS_STRING_ENUM_H

#include "cpprest/details/basic_types.h"

namespace nmos
{
    // Many of the JSON fields in the NMOS specifications are strings with an enumerated set of values.
    // Sometimes these enumerations are extensible (i.e. not a closed set), such as those for media types.
    // string_enum is a base class using CRTP to implement type safe enums with simple conversion to string.
    // See nmos/type.h for a usage example.
    template <class Derived>
    struct string_enum
    {
        utility::string_t name;
        // could add explicit string conversion operator?

        // totally_ordered rather than just equality_comparable only to allow use of type as a key
        // in associative containers; an alternative would be adding a std::hash override so that
        // unordered associative containers could be used instead?
        friend bool operator==(const Derived& lhs, const Derived& rhs) { return lhs.name == rhs.name; }
        friend bool operator< (const Derived& lhs, const Derived& rhs) { return lhs.name <  rhs.name; }
        friend bool operator> (const Derived& lhs, const Derived& rhs) { return rhs < lhs; }
        friend bool operator!=(const Derived& lhs, const Derived& rhs) { return !(lhs == rhs); }
        friend bool operator<=(const Derived& lhs, const Derived& rhs) { return !(rhs < lhs); }
        friend bool operator>=(const Derived& lhs, const Derived& rhs) { return !(lhs < rhs); }
    };
}

#define DEFINE_STRING_ENUM(Type) \
    struct Type : public nmos::string_enum<Type> \
    { \
        Type() {} \
        explicit Type(utility::string_t name) : string_enum{ std::move(name) } {} \
    };

#endif
