/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Consolidated VPID code definitions, merging separate 720-line
 * and 1080-line codes into a single 1.5Gbps code and updated payload descriptions.
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

#ifndef NMOS_VPID_CODE_H
#define NMOS_VPID_CODE_H

#include <cstdint>

namespace nmos
{
    // SMPTE ST 352 (Video) Payload Identification Codes for Serial Digital Interfaces
    // Byte 1: Payload and Digital Interface Identification
    typedef uint8_t vpid_code;

    // SMPTE ST 352 Video Payload ID Codes for Serial Digital Interfaces
    // See https://smpte-ra.org/video-payload-id-codes-serial-digital-interfaces
    namespace vpid_codes
    {
        // 483/576-line interlaced payloads on 270 Mb/s and 360 Mb/s serial digital interfaces
        const vpid_code vpid_270Mbps = 129;
        // 483/576-line extended payloads on 360 Mb/s single-link and 270 Mb/s dual-link serial digital interfaces
        const vpid_code vpid_360Mbps = 130;
        // 483/576-line payloads on a 540 Mb/s serial digital interface
        const vpid_code vpid_540Mbps = 131;
        // 483/576-line payloads on a 1.485 Gb/s (nominal) serial digital interface
        const vpid_code vpid_1_5Gbps = 132;

        // extensible enum
    }
}

#endif
