add_executable(adm_unit_tests)

target_sources(adm_unit_tests
    PRIVATE
        CoreModelTest.cpp
        CoreModelTest.h
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
        unit_test_main.cpp
        AnalyzeContentXMLBuffers.h
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

if (NOT (CMAKE_SYSTEM_PROCESSOR STREQUAL aarch64 OR CMAKE_SYSTEM_PROCESSOR STREQUAL aarch32 OR CONAN_EXPORTED))
    gtest_discover_tests(adm_unit_tests
        PROPERTIES
            LABELS dlb_adm
            DISCOVERY_TIMEOUT 10
    )
endif()
