/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Deleted testParseFmtpChannelOrder test case.
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
#include "nmos/channels.h"

#include "bst/test/test.h"

////////////////////////////////////////////////////////////////////////////////////////////
BST_TEST_CASE(testMakeFmtpChannelOrder)
{
    using namespace nmos::channel_symbols;

    // two simple examples

    const std::vector<nmos::channel_symbol> stereo{ L, R };
    BST_REQUIRE_EQUAL(U("SMPTE2110.(ST)"), nmos::make_fmtp_channel_order(stereo));

    const std::vector<nmos::channel_symbol> dual_mono{ M1, M2 };
    BST_REQUIRE_EQUAL(U("SMPTE2110.(DM)"), nmos::make_fmtp_channel_order(dual_mono));

    // two examples from ST 2110-30:2017 Section 6.2.2 Channel Order Convention

    const std::vector<nmos::channel_symbol> example_1{ L, R, C, LFE, Ls, Rs, L, R };
    BST_REQUIRE_EQUAL(U("SMPTE2110.(51,ST)"), nmos::make_fmtp_channel_order(example_1));

    const std::vector<nmos::channel_symbol> example_2{ M1, M1, M1, M1, L, R, Undefined(37), Undefined(42) };
    BST_REQUIRE_EQUAL(U("SMPTE2110.(M,M,M,M,ST,U02)"), nmos::make_fmtp_channel_order(example_2));

    // same result as above, because there's only a partial match to the 5.1 Surround grouping (but a complete match for Standard Stereo)
    const std::vector<nmos::channel_symbol> example_3{ M1, M1, M1, M1, L, R, C, LFE };
    BST_REQUIRE_EQUAL(U("SMPTE2110.(M,M,M,M,ST,U02)"), nmos::make_fmtp_channel_order(example_3));
}
