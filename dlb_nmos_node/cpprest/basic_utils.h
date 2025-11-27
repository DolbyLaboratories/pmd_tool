/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Removed base64url encoding/decoding utility functions.
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

#ifndef CPPREST_BASIC_UTILS_H
#define CPPREST_BASIC_UTILS_H

#include "cpprest/asyncrt_utils.h" // for cpprest/details/basic_types.h and utility::conversions

namespace utility
{
    namespace conversions
    {
        namespace details
        {
            // non-throwing overload of function in cpprest/asyncrt_utils.h
            template <typename Source>
            utility::string_t print_string(const Source& val, const utility::string_t& default_str)
            {
                utility::ostringstream_t oss;
                oss.imbue(std::locale::classic());
                oss << val;
                return !oss.fail() ? oss.str() : default_str;
            }

            // non-throwing overload of function in cpprest/asyncrt_utils.h
            template <typename Target>
            Target scan_string(const utility::string_t& str, const Target& default_val)
            {
                Target t;
                utility::istringstream_t iss(str);
                iss.imbue(std::locale::classic());
                iss >> t;
                return !iss.fail() ? t : default_val;
            }
        }
    }
}

#ifndef _TURN_OFF_PLATFORM_STRING
#define US(x) utility::string_t{_XPLATSTR(x)}
#endif

// more convenient utility functions dependent on utility::char_t
namespace utility
{
    inline std::string us2s(const string_t& us)
    {
        return conversions::to_utf8string(us);
    }

    inline string_t s2us(const std::string& s)
    {
        return conversions::to_string_t(s);
    }

    template <typename T>
    inline string_t ostringstreamed(const T& value)
    {
        return conversions::details::print_string(value, {});
    }

    template <typename T>
    inline T istringstreamed(const string_t& value, const T& default_value = {})
    {
        return conversions::details::scan_string(value, default_value);
    }
}

#endif
