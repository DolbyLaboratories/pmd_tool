cmake_minimum_required(VERSION 3.18)

include(CMakePrintHelpers)

set(CMAKE_POSITION_INDEPENDENT_CODE ON) # enable position independent code (-fPIC) on all targets
set(BUILD_SHARED_LIBS OFF) # build all libraries as static libs unless explicitly specified as shared

set(PROJECT PMD)

project(${PROJECT})

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
