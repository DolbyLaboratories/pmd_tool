<!--
Derived from: sony/nmos-cpp (Apache-2.0)
Modifications: Removed advanced features including IS-10 authorization,
IS-12 control protocol, JPEG XS support, SSL/certificate management,
and OCSP handling - reverted to core NMOS discovery and registration functionality
only.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

Copyright (c) 2019-2025, Dolby Laboratories Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
-->

# Third-party Source Files

Third-party source files used by the nmos-cpp libraries

- [catch](catch)  
  The [Catch](https://github.com/philsquared/Catch) (automated test framework) single header version
- [cmake](cmake)  
  CMake modules derived from third-party sources
- [mDNSResponder](mDNSResponder)  
  Patches and patched source files for the Bonjour DNS-SD implementation
- [nlohmann](nlohmann)  
  The [JSON for Modern C++](https://github.com/nlohmann/json) and [Modern C++ JSON schema validator](https://github.com/pboettch/json-schema-validator) libraries
- [nmos-audio-channel-mapping](nmos-audio-channel-mapping)  
  The JSON Schema files used for validation of Channel Mapping API requests and responses
- [nmos-device-connection-management](nmos-device-connection-management)  
  The JSON Schema files used for validation of Connection API requests and responses
- [nmos-discovery-registration](nmos-discovery-registration)  
  The JSON Schema files used for validation of e.g. Registration API requests and responses
- [nmos-system](nmos-system)  
  The JSON Schema files used for validation of System API requests and responses
- [WpdPack](WpdPack)  
  Libraries and header files from the [WinPcap](https://www.winpcap.org/) Developer's Pack
