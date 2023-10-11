# ---- pmd_unit_test ----

add_executable(pmd_unit_test)

target_link_libraries(pmd_unit_test
    PRIVATE
        dlb_pmd
        dlb_pmd_tool_lib
        dlb_wave
        dlb_octfile
        dlb_buffer
        ember_slim_lib
        gtest
)

target_include_directories(pmd_unit_test
    PRIVATE
        $<TARGET_PROPERTY:dlb_pmd,INCLUDE_DIRECTORIES>
)

set(PMD_UNIT_TEST_CTRL_PATH ../os/linux)

if(APPLE)
    set(PMD_UNIT_TEST_CTRL_PATH ../os/osx)
elseif(WIN32)
    set(PMD_UNIT_TEST_CTRL_PATH ../os/windows)
else()
    add_compile_definitions(_GNU_SOURCE)
endif()


target_sources(pmd_unit_test
    PRIVATE
        ${PMD_UNIT_TEST_CTRL_PATH}/pmd_ctrl_c.c
        combo_model.cc
        core_model_generator.cc
        core_model_ingester.cc
        dlb_pmd_capture_01.cc
        dlb_pmd_capture_02.cc
        dlb_pmd_pcm_01.cc
        #dlb_pmd_sadm_01.cc
        dlb_pmd_sadm_02.cc
        libember_slim_01.cc
        pmd_unit_test.cc
)

