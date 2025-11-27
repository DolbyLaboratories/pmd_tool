# Derived from: sony/nmos-cpp (Apache-2.0)
# Modifications: Restructured build system to create nmos-cpp-node
# and dlb_nmos_node_main executables with simplified dependencies.
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# 
# Copyright (c) 2019-2025, Dolby Laboratories Inc.
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 
# 3. Neither the name of the copyright holder nor the names of its
#    contributors may be used to endorse or promote products derived from
#    this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# CMake 3.9 is required due to cpprestsdk-config.cmake using find_dependency with COMPONENTS
cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

# project name
project(nmos-cpp)

# The default nmos-cpp root directory
set(NMOS_CPP_DIR ${PROJECT_SOURCE_DIR})

# Conan
if (${USE_CONAN}) # download conan wrapper from it's repo
    include (${NMOS_CPP_DIR}/cmake/NmosCppConan.cmake)
else() # in case of building via conan itself or if `conan install` was called first
    include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
endif()

# Common setup and dependency checking
include (${NMOS_CPP_DIR}/cmake/NmosCppCommon.cmake)

# Setup for the libraries
include (${NMOS_CPP_DIR}/cmake/NmosCppLibraries.cmake)

# nmos-cpp-node executable

set(NMOS_CPP_NODE_SOURCES
    ${NMOS_CPP_DIR}/nmos-cpp-node/main.cpp
    ${NMOS_CPP_DIR}/nmos-cpp-node/node_implementation.cpp
    )
set(NMOS_CPP_NODE_HEADERS
    ${NMOS_CPP_DIR}/nmos-cpp-node/node_implementation.h
    )

add_executable(
    nmos-cpp-node
    ${NMOS_CPP_NODE_SOURCES}
    ${NMOS_CPP_NODE_HEADERS}
    ${NMOS_CPP_DIR}/nmos-cpp-node/config.json
    )

source_group("Source Files" FILES ${NMOS_CPP_NODE_SOURCES})
source_group("Header Files" FILES ${NMOS_CPP_NODE_HEADERS})

target_link_libraries(
    nmos-cpp-node
    nmos-cpp_static
    ${CPPRESTSDK_TARGET}
    ${PLATFORM_LIBS}
    ${Boost_LIBRARIES}
    )

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # Conan packages usually don't include PDB files so suppress the resulting warning
    set_target_properties(
        nmos-cpp-node
        PROPERTIES
        LINK_FLAGS "/ignore:4099"
        )
endif()


# dlb-nmos-cpp-node executable

#find_library(Avahi_COMMON_LIBRARY NAMES avahi-common)
#find_library(Avahi_CLIENT_LIBRARY NAMES avahi-client)

set(DLB_NMOS_CPP_NODE_SOURCES
    ${NMOS_CPP_DIR}/dlb-cpp-node/dlb_nmos_node_main.cpp
    ${NMOS_CPP_DIR}/dlb-nmos/dlb_nmos_node_api.cpp
    ${NMOS_CPP_DIR}/dlb-nmos/node_implementation.cpp
    )
set(DLB_NMOS_CPP_NODE_HEADERS
    ${NMOS_CPP_DIR}/dlb-nmos/dlb_nmos_node_api.h
    ${NMOS_CPP_DIR}/dlb-nmos/node_implementation.h
    ${NMOS_CPP_DIR}/dlb-nmos/dlb_nmos_node_api.h
    ${NMOS_CPP_DIR}/dlb-nmos/ws_endpoint.h
    )

add_executable(
    dlb_nmos_node_main
    ${DLB_NMOS_CPP_NODE_SOURCES}
    ${DLB_NMOS_CPP_NODE_HEADERS}
    )

target_include_directories(dlb_nmos_node_main
          PRIVATE ${NMOS_CPP_DIR}/dlb-nmos
          )

source_group("Source Files" FILES ${DLB_NMOS_CPP_NODE_SOURCES})
source_group("Header Files" FILES ${DLB_NMOS_CPP_NODE_HEADERS})

target_link_libraries(
    dlb_nmos_node_main
    nmos-cpp_static
    ${CPPRESTSDK_TARGET}
    ${PLATFORM_LIBS}
    ${Boost_LIBRARIES}
    avahi-client
    avahi-common
    )

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # Conan packages usually don't include PDB files so suppress the resulting warning
    set_target_properties(
        dlb_nmos_node_main
        PROPERTIES
        LINK_FLAGS "/ignore:4099"
        )
endif()

# nmos-cpp-registry executable

#set(NMOS_CPP_REGISTRY_SOURCES
#    ${NMOS_CPP_DIR}/nmos-cpp-registry/main.cpp
#    ${NMOS_CPP_DIR}/nmos-cpp-registry/registry_implementation.cpp
#    )
#set(NMOS_CPP_REGISTRY_HEADERS
#    ${NMOS_CPP_DIR}/nmos-cpp-registry/registry_implementation.h
#    )

#add_executable(
#    nmos-cpp-registry
#    ${NMOS_CPP_REGISTRY_SOURCES}
#    ${NMOS_CPP_REGISTRY_HEADERS}
#    ${NMOS_CPP_DIR}/nmos-cpp-registry/config.json
#    )

#source_group("Source Files" FILES ${NMOS_CPP_REGISTRY_SOURCES})
#source_group("Header Files" FILES ${NMOS_CPP_REGISTRY_HEADERS})

#target_link_libraries(
#    nmos-cpp-registry
#    nmos-cpp_static
#    ${CPPRESTSDK_TARGET}
#    ${PLATFORM_LIBS}
#    ${Boost_LIBRARIES}
#    )

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # Conan packages usually don't include PDB files so suppress the resulting warning
    set_target_properties(
        nmos-cpp-registry
        PROPERTIES
        LINK_FLAGS "/ignore:4099"
        )
endif()

# nmos-cpp-test executable
#include (${NMOS_CPP_DIR}/cmake/NmosCppTest.cmake)
