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
add_executable(adm_unit_tests)

target_sources(adm_unit_tests
    PRIVATE
        CoreModelTest.cpp
        dlb_adm_data.h
        dlb_adm_emission_profile_data.h
        dlb_adm_01.cpp
        dlb_adm_02.cpp
        dlb_adm_03.cpp
        dlb_adm_04.cpp
        dlb_adm_05.cpp
        dlb_adm_06.cpp
        dlb_adm_emission_profile.cpp
        dlb_adm_fast_api_tests.cpp
        dlb_adm_fast_sadm_tests.cpp
        dlb_adm_analyze_content.cpp
        dlb_adm_dolbye_sadm_tests.cpp
        dlb_adm_dolbye_core_model_tests.cpp
        dlb_adm_dolbye_xml_check.cpp
        dlb_adm_dolbye_sadm_ref_tests.cpp
        dlb_adm_dolbye_api_to_api.cpp
        dlb_adm_dolbye_xml_to_xml.cpp
        dlb_adm_api.cpp
        unit_test_main.cpp
        UnitTestCommon.h
        TestUtilities.h
        TestUtilities.cpp
        AnalyzeContentXMLBuffers.h
        DolbyEProfileXMLBuffers.h
        DolbyEToSADMReferenceFiles.h
)

set_target_properties(adm_unit_tests PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS OFF
)

# Include private headers
target_include_directories(adm_unit_tests
    PUBLIC
        $<TARGET_PROPERTY:dlb_adm,INCLUDE_DIRECTORIES>
)

target_link_libraries(adm_unit_tests
    PRIVATE
        gtest
        dlb_adm
)

if (CMAKE_SYSTEM_PROCESSOR STREQUAL CMAKE_HOST_SYSTEM_PROCESSOR)
    gtest_discover_tests(adm_unit_tests
        PROPERTIES
            LABELS dlb_adm
            DISCOVERY_TIMEOUT 10
    )
endif()
