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
# ---- main dlb_pmd ----

add_library(dlb_pmd)

target_include_directories(dlb_pmd
    PUBLIC
        include
        ..
    PRIVATE
        .
)

target_link_libraries(dlb_pmd
    PRIVATE
        dlb_adm
        dlb_xml
        zlib
)

# ---- pmd_realtime ---- (DEPRECATED)

# ---- pmd_tool ----

add_executable(dlb_pmd_tool)

# ---- pmd_tool_lib -----
add_library(dlb_pmd_tool_lib)

target_include_directories(dlb_pmd_tool_lib
    PUBLIC
        include
        ..
)

# ---- pmd_studio ----

add_executable(dlb_pmd_studio)

target_compile_features(dlb_pmd_studio PUBLIC cxx_std_17)

target_include_directories(dlb_pmd_studio
    PUBLIC
        include
        ..
        PRIVATE
        .
)

# ---- pmd_studio_rivermax (optional) ----

if(BUILD_PMD_STUDIO_RIVERMAX)
    add_executable(dlb_pmd_studio_rivermax)

    target_compile_features(dlb_pmd_studio_rivermax PUBLIC cxx_std_11)

    target_include_directories(dlb_pmd_studio_rivermax
        PUBLIC
            include
            ..
            PRIVATE
            .
    )
endif()

add_subdirectory(os)
add_subdirectory(src)
add_subdirectory(frontend)
add_subdirectory(test)
add_subdirectory(unit_test)