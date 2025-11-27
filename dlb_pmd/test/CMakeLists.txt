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

