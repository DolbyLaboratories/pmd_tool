/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Updated documentation URLs from specs.amwa.tv to GitHub repository links.
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

#ifndef NMOS_EVENT_TYPE_H
#define NMOS_EVENT_TYPE_H

#include "nmos/string_enum.h"

namespace nmos
{
    // IS-07 Event & Tally event types
    // See https://github.com/AMWA-TV/nmos-event-tally/blob/v1.0/docs/3.0.%20Event%20types.md
    DEFINE_STRING_ENUM(event_type)
    namespace event_types
    {
        // See https://github.com/AMWA-TV/nmos-event-tally/blob/v1.0/docs/3.0.%20Event%20types.md#21-boolean
        const event_type boolean{ U("boolean") };
        // See https://github.com/AMWA-TV/nmos-event-tally/blob/v1.0/docs/3.0.%20Event%20types.md#22-string
        const event_type string{ U("string") };
        // See https://github.com/AMWA-TV/nmos-event-tally/blob/v1.0/docs/3.0.%20Event%20types.md#23-number
        const event_type number{ U("number") };
        // See https://github.com/AMWA-TV/nmos-event-tally/blob/v1.0/docs/3.0.%20Event%20types.md#4-object
        // "The usage of the object event type is out of scope of this specification for version 1.0"
        const event_type object{ U("object") };

        // See https://github.com/AMWA-TV/nmos-event-tally/blob/v1.0/docs/3.0.%20Event%20types.md#231-measurements
        inline const event_type measurement(const utility::string_t& name, const utility::string_t& unit)
        {
            // specific measurement types are always "number/{Name}/{Unit}"
            // names and units should not be empty or contain the '/' character
            return event_type{ number.name + U('/') + name + U('/') + unit };
        }

        // See https://github.com/AMWA-TV/nmos-event-tally/blob/v1.0/docs/3.0.%20Event%20types.md#3-enum
        inline const event_type named_enum(const event_type& base_type, const utility::string_t& name)
        {
            return event_type{ base_type.name + U("/enum/") + name };
        }

        // See https://github.com/AMWA-TV/nmos-event-tally/blob/v1.0/docs/3.0.%20Event%20types.md#event-types-capability-management
        // "A wildcard (*) must replace a whole word and can only be used at the end of an event_type definition."
        struct wildcard_type
        {
            const event_type operator()(const event_type& base_type) const
            {
                return event_type{ base_type.name + U("/*") };
            }
        };
        const wildcard_type wildcard;

        // "any measurement unit" partial event type
        inline const event_type measurement(const utility::string_t& name, const wildcard_type&)
        {
            return wildcard(event_type{ number.name + U('/') + name });
        }

        // "any named enumeration" partial event type
        inline const event_type named_enum(const event_type& base_type, const wildcard_type&)
        {
            return wildcard(event_type{ base_type.name + U("/enum") });
        }

        // deprecated, provided for backwards compatibility, use measurement(name, unit) since a measurement must be a number
        inline const event_type measurement(const event_type& base_type, const utility::string_t& name, const utility::string_t& unit)
        {
            return event_type{ base_type.name + U('/') + name + U('/') + unit };
        }

        // deprecated, provided for backwards compatibility, use measurement(name, wildcard) since a measurement must be a number
        inline const event_type measurement(const event_type& base_type, const utility::string_t& name, const wildcard_type&)
        {
            return wildcard(event_type{ base_type.name + U('/') + name });
        }

        // deprecated, provided for backwards compatibility, use wildcard(base_type) since these wildcards do not only match measurements
        inline const event_type measurement(const event_type& base_type, const wildcard_type&)
        {
            return wildcard(base_type);
        }
    }

    // See https://github.com/AMWA-TV/nmos-event-tally/blob/v1.0/docs/3.0.%20Event%20types.md#event-types-capability-management
    inline bool is_matching_event_type(const event_type& capability, const event_type& type)
    {
        // "Comparisons between event_type values must be case sensitive."
        // See https://github.com/AMWA-TV/nmos-event-tally/blob/v1.0.1/docs/3.0.%20Event%20types.md#1-introduction
        auto& c = capability.name;
        auto& t = type.name;
        // The wildcard in a partial event type matches zero or more 'levels', e.g. "number/*" matches both "number" and "number/temperature/C".
        // A wildcard cannot be used at the top 'level', i.e. "*" is not a partial event type that matches any base type.
        if (2 < c.size() && c[c.size() - 2] == U('/') && c[c.size() - 1] == U('*'))
            return c.size() - 2 <= t.size() && std::equal(c.begin(), c.end() - 2, t.begin())
            && (c.size() - 2 == t.size() || t[c.size() - 2] == U('/'));
        else
            return c == t;
    }
}

#endif
