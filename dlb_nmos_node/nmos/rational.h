/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Removed unused is_rational() function.
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

#ifndef NMOS_RATIONAL_H
#define NMOS_RATIONAL_H

#include <boost/rational.hpp>
#include "cpprest/json.h"

namespace nmos
{
    // A sub-object representing a rational is used in several of the NMOS IS-04 schemas
    // although unfortunately there are differences in the minimum and default specified
    // (uint64_t ought to work, but I can't be bothered to fix the resulting compile warnings...)
    typedef boost::rational<int64_t> rational; // defaults to { 0, 1 }

    namespace rationals
    {
        const rational rate60{ 60, 1 };
        const rational rate59_94{ 60000, 1001 };
        const rational rate50{ 50, 1 };
        const rational rate30{ 30, 1 };
        const rational rate29_97{ 30000, 1001 };
        const rational rate25{ 25, 1 };
        const rational rate24{ 24, 1 };
        const rational rate23_98{ 24000, 1001 };
    }
    namespace rates = rationals;

    web::json::value make_rational(const rational& value = {});

    inline web::json::value make_rational(int64_t numerator, int64_t denominator)
    {
        return make_rational({ numerator, denominator });
    }

    rational parse_rational(const web::json::value& value);
}

#endif
