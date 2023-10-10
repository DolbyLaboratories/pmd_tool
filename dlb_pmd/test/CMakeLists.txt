set(PMD_TEST_CORE_OBJS
    TestModel.hh
    TestModel.cc
    TestModelRandom.hh
    TestMdset.hh
    TestMdset.cc
    TestXmlWriter.hh	
    TestXml.hh
    TestXml.cc
    TestKlv.hh
    TestKlv.cc
    TestPcm.hh
    TestPcm.cc
    TestSadm.hh
    TestSadm.cc
    XmlSchema.hh
    XmlSchema.cc
)


# ========================
# ---- pmd test tools ----
# ========================

# ---- pmd_test ----

add_executable(pmd_test)

target_link_libraries(pmd_test
    PRIVATE
        dlb_pmd
        dlb_adm
        dlb_wave
        dlb_octfile
        dlb_buffer
        xerces_3_2_0_lib
        gtest
)


set(PMD_TEST_CTRL_PATH ../os/linux)

if(APPLE)
    set(PMD_TEST_CTRL_PATH ../os/osx)
elseif(WIN32)
    set(PMD_TEST_CTRL_PATH ../os/windows)
else()
    add_compile_definitions(_GNU_SOURCE)
endif()

target_sources(pmd_test
    PRIVATE
        ${PMD_TEST_CORE_OBJS}
        ${PMD_TEST_CTRL_PATH}/pmd_ctrl_c.c
        pmd_test.cc
        Test_API.cc
        Test_ABD_AOD_APD.cc
        Test_Characters.cc
        Test_EEP.cc
        Test_ETD.cc
        Test_Floatvals.cc
        Test_HED.cc
        Test_IAT.cc
        Test_Languages.cc
        Test_PLD.cc
        Test_PresentationConfig.cc
        Test_Profiles.cc
        Test_Smpte2109.cc
        Test_Versions.cc
        Test_XYZ.cc
)

target_include_directories(pmd_test
    PRIVATE
        .
        ..
        ../include
        ../os
)

