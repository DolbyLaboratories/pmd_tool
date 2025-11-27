/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: 	Updated documentation URLs from specs.amwa.tv to GitHub repository links.
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

#ifndef NMOS_ID_H
#define NMOS_ID_H

#include <memory>
#include "cpprest/details/basic_types.h"

namespace nmos
{
    // "Each logical entity is identified by a UUID"
    // See https://github.com/AMWA-TV/nmos-discovery-registration/blob/v1.2/docs/5.1.%20Data%20Model%20-%20Identifier%20Mapping.md

    // Since identifiers are passed as strings in the APIs, and the formatting of identifiers has been a little
    // inconsistent between implementations in the past, they are currently stored simply as strings...
    typedef utility::string_t id;

    // a random number-based UUID (v4) generator
    // non-copyable, not thread-safe
    class id_generator
    {
    public:
        id_generator();
        ~id_generator();
        id operator()();

    private:
        struct impl_t;
        std::unique_ptr<impl_t> impl;
    };

    // generate a random number-based UUID (v4)
    // note, when creating multiple UUIDs, using an id_generator can be more efficient depending on platform and dependencies
    id make_id();

    // generate a name-based UUID (v5)
    id make_repeatable_id(id namespace_id, const utility::string_t& name);
}

#endif
