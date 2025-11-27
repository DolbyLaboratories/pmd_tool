# 
# Derived from: sony/nmos-cpp (Apache-2.0)
# Modifications: Major restructure to build static libraries with simplified naming
# and dependency management.
# 
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
# 
#  Copyright (c) 2019-2025, Dolby Laboratories Inc.
#  All rights reserved.
# 
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
# 
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
# 
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
# 
#  3. Neither the name of the copyright holder nor the names of its
#     contributors may be used to endorse or promote products derived from
#     this software without specific prior written permission.
# 
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
#  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
#  HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
#  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
#  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
#  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
#  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
#  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# 
# CMake instructions for making the nmos-cpp test program

# caller can set NMOS_CPP_DIR if the project is different
if (NOT DEFINED NMOS_CPP_DIR)
    set (NMOS_CPP_DIR ${PROJECT_SOURCE_DIR})
endif()

# nmos-cpp-test

set(NMOS_CPP_TEST_SOURCES
    ${NMOS_CPP_DIR}/nmos-cpp-test/main.cpp
    )
set(NMOS_CPP_TEST_HEADERS
    )

set(NMOS_CPP_TEST_BST_TEST_SOURCES
    )
set(NMOS_CPP_TEST_BST_TEST_HEADERS
    ${NMOS_CPP_DIR}/bst/test/test.h
    )

set(NMOS_CPP_TEST_CPPREST_TEST_SOURCES
    ${NMOS_CPP_DIR}/cpprest/test/api_router_test.cpp
    ${NMOS_CPP_DIR}/cpprest/test/http_utils_test.cpp
    ${NMOS_CPP_DIR}/cpprest/test/json_utils_test.cpp
    ${NMOS_CPP_DIR}/cpprest/test/json_visit_test.cpp
    ${NMOS_CPP_DIR}/cpprest/test/regex_utils_test.cpp
    ${NMOS_CPP_DIR}/cpprest/test/ws_listener_test.cpp
    )
set(NMOS_CPP_TEST_CPPREST_TEST_HEADERS
    )

if (BUILD_LLDP)
    set(NMOS_CPP_TEST_LLDP_TEST_SOURCES
        ${NMOS_CPP_DIR}/lldp/test/lldp_test.cpp
        )
    set(NMOS_CPP_TEST_LLDP_TEST_HEADERS
        )
endif()

set(NMOS_CPP_TEST_MDNS_TEST_SOURCES
    ${NMOS_CPP_DIR}/mdns/test/core_test.cpp
    ${NMOS_CPP_DIR}/mdns/test/mdns_test.cpp
    )
set(NMOS_CPP_TEST_MDNS_TEST_HEADERS
    )

set(NMOS_CPP_TEST_NMOS_TEST_SOURCES
    ${NMOS_CPP_DIR}/nmos/test/api_utils_test.cpp
    ${NMOS_CPP_DIR}/nmos/test/channels_test.cpp
    ${NMOS_CPP_DIR}/nmos/test/did_sdid_test.cpp
    ${NMOS_CPP_DIR}/nmos/test/event_type_test.cpp
    ${NMOS_CPP_DIR}/nmos/test/json_validator_test.cpp
    ${NMOS_CPP_DIR}/nmos/test/paging_utils_test.cpp
    ${NMOS_CPP_DIR}/nmos/test/query_api_test.cpp
    ${NMOS_CPP_DIR}/nmos/test/system_resources_test.cpp
    )
set(NMOS_CPP_TEST_NMOS_TEST_HEADERS
    )

set(NMOS_CPP_TEST_SDP_TEST_SOURCES
    ${NMOS_CPP_DIR}/sdp/test/sdp_test.cpp
    )
set(NMOS_CPP_TEST_SDP_TEST_HEADERS
    )

add_executable(
    nmos-cpp-test
    ${NMOS_CPP_TEST_SOURCES}
    ${NMOS_CPP_TEST_HEADERS}
    ${NMOS_CPP_TEST_BST_TEST_SOURCES}
    ${NMOS_CPP_TEST_BST_TEST_HEADERS}
    ${NMOS_CPP_TEST_CPPREST_TEST_SOURCES}
    ${NMOS_CPP_TEST_CPPREST_TEST_HEADERS}
    ${NMOS_CPP_TEST_LLDP_TEST_SOURCES}
    ${NMOS_CPP_TEST_LLDP_TEST_HEADERS}
    ${NMOS_CPP_TEST_MDNS_TEST_SOURCES}
    ${NMOS_CPP_TEST_MDNS_TEST_HEADERS}
    ${NMOS_CPP_TEST_NMOS_TEST_SOURCES}
    ${NMOS_CPP_TEST_NMOS_TEST_HEADERS}
    ${NMOS_CPP_TEST_SDP_TEST_SOURCES}
    ${NMOS_CPP_TEST_SDP_TEST_HEADERS}
    )

source_group("Source Files" FILES ${NMOS_CPP_TEST_SOURCES})
source_group("bst\\test\\Source Files" FILES ${NMOS_CPP_TEST_BST_TEST_SOURCES})
source_group("cpprest\\test\\Source Files" FILES ${NMOS_CPP_TEST_CPPREST_TEST_SOURCES})
source_group("lldp\\test\\Source Files" FILES ${NMOS_CPP_TEST_LLDP_TEST_SOURCES})
source_group("mdns\\test\\Source Files" FILES ${NMOS_CPP_TEST_MDNS_TEST_SOURCES})
source_group("nmos\\test\\Source Files" FILES ${NMOS_CPP_TEST_NMOS_TEST_SOURCES})
source_group("sdp\\test\\Source Files" FILES ${NMOS_CPP_TEST_SDP_TEST_SOURCES})

source_group("Header Files" FILES ${NMOS_CPP_TEST_HEADERS})
source_group("bst\\test\\Header Files" FILES ${NMOS_CPP_TEST_BST_TEST_HEADERS})
source_group("cpprest\\test\\Header Files" FILES ${NMOS_CPP_TEST_CPPREST_TEST_HEADERS})
source_group("lldp\\test\\Header Files" FILES ${NMOS_CPP_TEST_LLDP_TEST_HEADERS})
source_group("mdns\\test\\Header Files" FILES ${NMOS_CPP_TEST_MDNS_TEST_HEADERS})
source_group("nmos\\test\\Header Files" FILES ${NMOS_CPP_TEST_NMOS_TEST_HEADERS})
source_group("sdp\\test\\Header Files" FILES ${NMOS_CPP_TEST_SDP_TEST_HEADERS})

target_link_libraries(
    nmos-cpp-test
    nmos-cpp_static
    mdns_static
    ${CPPRESTSDK_TARGET}
    ${PLATFORM_LIBS}
    ${Boost_LIBRARIES}
    )
if (BUILD_LLDP)
    target_link_libraries(
        nmos-cpp-test
        lldp_static
        )
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # Conan packages usually don't include PDB files so suppress the resulting warning
    set_target_properties(
        nmos-cpp-test
        PROPERTIES
        LINK_FLAGS "/ignore:4099"
        )
endif()

include(Catch)

catch_discover_tests(nmos-cpp-test EXTRA_ARGS -r compact)
