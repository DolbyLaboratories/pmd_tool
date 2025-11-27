#/************************************************************************
# * Copyright (c) 2023-2025, Dolby Laboratories Inc.
# * Copyright (c) 2025-2025, Dolby International AB.
# * All rights reserved.
# * 
# * Redistribution and use in source and binary forms, with or without
# * modification, are permitted provided that the following conditions
# * are met:
# * 
# * 1. Redistributions of source code must retain the above copyright
# *    notice, this list of conditions and the following disclaimer.
# *
# * 2. Redistributions in binary form must reproduce the above
# *    copyright notice, this list of conditions and the following
# *    disclaimer in the documentation and/or other materials provided
# *    with the distribution.
# *
# * 3. Neither the name of the copyright holder nor the names of its
# *    contributors may be used to endorse or promote products derived
# *    from this software without specific prior written permission.
# *
# * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# * 'AS IS' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
# * OF THE POSSIBILITY OF SUCH DAMAGE.
# **********************************************************************/
#/************************************************************************
# * dlb_adm
# * Copyright (c) 2023-2025, Dolby Laboratories Inc.
# * Copyright (c) 2025-2025, Dolby International AB.
# * All rights reserved.
# * 
# * Redistribution and use in source and binary forms, with or without
# * modification, are permitted provided that the following conditions
# * are met:
# * 
# * 1. Redistributions of source code must retain the above copyright
# *    notice, this list of conditions and the following disclaimer.
# *
# * 2. Redistributions in binary form must reproduce the above
# *    copyright notice, this list of conditions and the following
# *    disclaimer in the documentation and/or other materials provided
# *    with the distribution.
# *
# * 3. Neither the name of the copyright holder nor the names of its
# *    contributors may be used to endorse or promote products derived
# *    from this software without specific prior written permission.
# *
# * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
# * OF THE POSSIBILITY OF SUCH DAMAGE.
# **********************************************************************/

cmake_minimum_required(VERSION 3.18)

include(CMakePrintHelpers)

set(CMAKE_POSITION_INDEPENDENT_CODE ON) # enable position independent code (-fPIC) on all targets
set(BUILD_SHARED_LIBS OFF) # build all libraries as static libs unless explicitly specified as shared

set(PROJECT PMD)

project(${PROJECT})

set(USE_CONAN TRUE)

if(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE "Release" CACHE STRING
    "Choose the type of build: Debug Release RelWithDebInfo MinSizeRel."
    FORCE)
endif()


# Setup Rivermax support

if (NOT DEFINED BUILD_PMD_STUDIO_RIVERMAX)
    set(BUILD_PMD_STUDIO_RIVERMAX FALSE)
endif()

if(BUILD_PMD_STUDIO_RIVERMAX)
    message("-- PMD Studio Rivermax enabled.")

    # Setup Rivermax api dir

    if (NOT DEFINED RIVERMAX_API_INCLUDE_DIR)
        set(RIVERMAX_API_INCLUDE_DIR "/usr/include/mellanox/")
    endif()

    cmake_print_variables(RIVERMAX_API_INCLUDE_DIR)
else()
    message("-- PMD Studio Rivermax disabled.")
endif()

# Enable testing for CTests
enable_testing()
include(GoogleTest)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")


# Set OS and Architecture for lib building (helper variable)

set(ARCH other)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "AMD64|amd64")
    set(ARCH amd64)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "ARM64|arm64")
    set(ARCH x86)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86|X86")
    set(ARCH x86)
endif()


set(OS other)

if(WIN32)
    set(OS win32)
elseif(APPLE)
    set(OS osx)
else()
    set(OS linux)
endif()

# PMD libs, execs and its dependencies

add_subdirectory(googletest)
add_subdirectory(xerces)
add_subdirectory(dlb_xmllib)

add_subdirectory(dlb_pmd)
add_subdirectory(dlb_wave)
add_subdirectory(zlib)
add_subdirectory(dlb_octfile)
add_subdirectory(dlb_buffer)

add_subdirectory(portaudio)
add_subdirectory(dlb_socket)
add_subdirectory(libui)
add_subdirectory(Lawo)
add_subdirectory(boost)
add_subdirectory(dlb_adm)
add_subdirectory(dlb_nmos_node)

if(BUILD_PMD_STUDIO_RIVERMAX)
    add_subdirectory(dlb_st2110)
endif()

# Bundling libs

include(bundle_static_library)

# Bundling must occur after all targets lookup

bundle_static_library(dlb_pmd dlb_pmd_bundle)
bundle_static_library(dlb_pmd_tool_lib dlb_pmd_tool_bundle)

#test
add_test(
    NAME PMD_Unit_Tests
    COMMAND ./dlb_pmd/unit_test/pmd_unit_test
)

add_test(
    NAME PMD_tests
    COMMAND ./dlb_pmd/test/pmd_test
)
