#-*-makefile-*-

BASE=../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
CCDEP = $(CC)
CFLAGS_debug = -g -O0 -arch x86_64 -fPIC -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wno-overlength-strings -Wdeclaration-after-statement -Wconditional-uninitialized
CFLAGS_release = -O3 -arch x86_64 -fPIC -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wno-overlength-strings -Wdeclaration-after-statement -Wconditional-uninitialized
CXXDEP = $(CXX)
CXXFLAGS_debug = -std=c++11 -g -O0 -arch x86_64 -fPIC -c -Wall -std=c++11
CXXFLAGS_release = -std=c++11 -O3 -arch x86_64 -fPIC -c -Wall -std=c++11
DEFINES_debug = -DHAVE_CONFIG_H=1 -DXERCES_BUILDING_LIBRARY=1
DEFINES_release = -DHAVE_CONFIG_H=1 -DNDEBUG=1 -DXERCES_BUILDING_LIBRARY=1
INCLUDES = -I$(BASE)dlb_buffer/1.3 -I$(BASE)dlb_octfile/1.1 -I$(BASE)dlb_pmd/include -I$(BASE)dlb_pmd/os -I$(BASE)dlb_pmd -I$(BASE)dlb_wave/1.6 -I$(BASE)googletest/1.8.0/include -I$(BASE)googletest/1.8.0/include -I$(BASE). -I$(BASE). -I$(BASE)xerces/xerces-c-3.2.0/osx -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/FileManagers -I$(BASE)xerces/xerces-c-3.2.0/src
LD = $(CXX)
LDFLAGS_debug = -arch x86_64 -dead_strip
LDFLAGS_release = -arch x86_64 -dead_strip -O2
LDLIBS = -lpthread -lpthread -lcurl -Wl,-framework -Wl,CoreServices


INPUTS_LINK_debug = $(TMP)dlb_pmd/os/osx/pmd_ctrl_c.pmd_test_osx_amd64_clang_debug.o $(TMP)dlb_pmd/test/TestKlv.pmd_test_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/test/TestMdset.pmd_test_osx_amd64_clang_debug.o $(TMP)dlb_pmd/test/TestModel.pmd_test_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/test/TestPcm.pmd_test_osx_amd64_clang_debug.o $(TMP)dlb_pmd/test/TestSadm.pmd_test_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/test/TestXml.pmd_test_osx_amd64_clang_debug.o $(TMP)dlb_pmd/test/Test_ABD_AOD_APD.pmd_test_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/test/Test_API.pmd_test_osx_amd64_clang_debug.o $(TMP)dlb_pmd/test/Test_Characters.pmd_test_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/test/Test_EEP.pmd_test_osx_amd64_clang_debug.o $(TMP)dlb_pmd/test/Test_ETD.pmd_test_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/test/Test_Floatvals.pmd_test_osx_amd64_clang_debug.o $(TMP)dlb_pmd/test/Test_HED.pmd_test_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/test/Test_IAT.pmd_test_osx_amd64_clang_debug.o $(TMP)dlb_pmd/test/Test_Languages.pmd_test_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/test/Test_PLD.pmd_test_osx_amd64_clang_debug.o $(TMP)dlb_pmd/test/Test_PresentationConfig.pmd_test_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/test/Test_Profiles.pmd_test_osx_amd64_clang_debug.o $(TMP)dlb_pmd/test/Test_Smpte2109.pmd_test_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/test/Test_Versions.pmd_test_osx_amd64_clang_debug.o $(TMP)dlb_pmd/test/Test_XYZ.pmd_test_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/test/XmlSchema.pmd_test_osx_amd64_clang_debug.o $(TMP)dlb_pmd/test/pmd_test.pmd_test_osx_amd64_clang_debug.o
INPUTS_LINK_release = $(TMP)dlb_pmd/os/osx/pmd_ctrl_c.pmd_test_osx_amd64_clang_release.o $(TMP)dlb_pmd/test/TestKlv.pmd_test_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/test/TestMdset.pmd_test_osx_amd64_clang_release.o $(TMP)dlb_pmd/test/TestModel.pmd_test_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/test/TestPcm.pmd_test_osx_amd64_clang_release.o $(TMP)dlb_pmd/test/TestSadm.pmd_test_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/test/TestXml.pmd_test_osx_amd64_clang_release.o $(TMP)dlb_pmd/test/Test_ABD_AOD_APD.pmd_test_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/test/Test_API.pmd_test_osx_amd64_clang_release.o $(TMP)dlb_pmd/test/Test_Characters.pmd_test_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/test/Test_EEP.pmd_test_osx_amd64_clang_release.o $(TMP)dlb_pmd/test/Test_ETD.pmd_test_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/test/Test_Floatvals.pmd_test_osx_amd64_clang_release.o $(TMP)dlb_pmd/test/Test_HED.pmd_test_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/test/Test_IAT.pmd_test_osx_amd64_clang_release.o $(TMP)dlb_pmd/test/Test_Languages.pmd_test_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/test/Test_PLD.pmd_test_osx_amd64_clang_release.o $(TMP)dlb_pmd/test/Test_PresentationConfig.pmd_test_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/test/Test_Profiles.pmd_test_osx_amd64_clang_release.o $(TMP)dlb_pmd/test/Test_Smpte2109.pmd_test_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/test/Test_Versions.pmd_test_osx_amd64_clang_release.o $(TMP)dlb_pmd/test/Test_XYZ.pmd_test_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/test/XmlSchema.pmd_test_osx_amd64_clang_release.o $(TMP)dlb_pmd/test/pmd_test.pmd_test_osx_amd64_clang_release.o

DEP_PROJ_LINK_debug = $(BASE)dlb_pmd/make/dlb_pmd_lib/osx_amd64_clang/dlb_pmd_lib_debug.a $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/osx_amd64_clang/dlb_xml_debug.a $(BASE)zlib/1.2.11/make/zlib/osx_amd64_clang/zlib_debug.a $(BASE)dlb_pmd/make/dlb_wave_lib/osx_amd64_clang/dlb_wave_lib_debug.a $(BASE)xerces/xerces-c-3.2.0/make/xerces_3_2_0_lib/osx_amd64_clang/xerces_3_2_0_lib_debug.a $(BASE)googletest/1.8.0/make/googletest/osx_amd64_clang/googletest_debug.a
DEP_PROJ_LINK_release = $(BASE)dlb_pmd/make/dlb_pmd_lib/osx_amd64_clang/dlb_pmd_lib_release.a $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/osx_amd64_clang/dlb_xml_release.a $(BASE)zlib/1.2.11/make/zlib/osx_amd64_clang/zlib_release.a $(BASE)dlb_pmd/make/dlb_wave_lib/osx_amd64_clang/dlb_wave_lib_release.a $(BASE)xerces/xerces-c-3.2.0/make/xerces_3_2_0_lib/osx_amd64_clang/xerces_3_2_0_lib_release.a $(BASE)googletest/1.8.0/make/googletest/osx_amd64_clang/googletest_release.a

default: pmd_test_debug pmd_test_release

pmd_test_debug: $(INPUTS_LINK_debug) $(DEP_PROJ_LINK_debug)
	$(LD) $(LDFLAGS_debug) -o $@ $+ $(LDLIBS)

pmd_test_release: $(INPUTS_LINK_release) $(DEP_PROJ_LINK_release)
	$(LD) $(LDFLAGS_release) -o $@ $+ $(LDLIBS)

$(TMP)%.pmd_test_osx_amd64_clang_debug.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -arch x86_64 -MM $(DEFINES_debug) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_debug) $(DEFINES_debug) $(INCLUDES) -o $@ $<

$(TMP)%.pmd_test_osx_amd64_clang_debug.o: $(BASE)%.cc | $(TMP)%/..
	$(CXXDEP) -std=c++11 -arch x86_64 -MM -std=c++11 $(DEFINES_debug) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS_debug) $(DEFINES_debug) $(INCLUDES) -o $@ $<

$(TMP)%.pmd_test_osx_amd64_clang_release.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -arch x86_64 -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

$(TMP)%.pmd_test_osx_amd64_clang_release.o: $(BASE)%.cc | $(TMP)%/..
	$(CXXDEP) -std=c++11 -arch x86_64 -MM -std=c++11 $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMPILE_debug = $(TMP)dlb_pmd/os/osx/pmd_ctrl_c.pmd_test_osx_amd64_clang_debug.d $(TMP)dlb_pmd/test/TestKlv.pmd_test_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/test/TestMdset.pmd_test_osx_amd64_clang_debug.d $(TMP)dlb_pmd/test/TestModel.pmd_test_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/test/TestPcm.pmd_test_osx_amd64_clang_debug.d $(TMP)dlb_pmd/test/TestSadm.pmd_test_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/test/TestXml.pmd_test_osx_amd64_clang_debug.d $(TMP)dlb_pmd/test/Test_ABD_AOD_APD.pmd_test_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/test/Test_API.pmd_test_osx_amd64_clang_debug.d $(TMP)dlb_pmd/test/Test_Characters.pmd_test_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/test/Test_EEP.pmd_test_osx_amd64_clang_debug.d $(TMP)dlb_pmd/test/Test_ETD.pmd_test_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/test/Test_Floatvals.pmd_test_osx_amd64_clang_debug.d $(TMP)dlb_pmd/test/Test_HED.pmd_test_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/test/Test_IAT.pmd_test_osx_amd64_clang_debug.d $(TMP)dlb_pmd/test/Test_Languages.pmd_test_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/test/Test_PLD.pmd_test_osx_amd64_clang_debug.d $(TMP)dlb_pmd/test/Test_PresentationConfig.pmd_test_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/test/Test_Profiles.pmd_test_osx_amd64_clang_debug.d $(TMP)dlb_pmd/test/Test_Smpte2109.pmd_test_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/test/Test_Versions.pmd_test_osx_amd64_clang_debug.d $(TMP)dlb_pmd/test/Test_XYZ.pmd_test_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/test/XmlSchema.pmd_test_osx_amd64_clang_debug.d $(TMP)dlb_pmd/test/pmd_test.pmd_test_osx_amd64_clang_debug.d
CLEAN_COMPILE_release = $(TMP)dlb_pmd/os/osx/pmd_ctrl_c.pmd_test_osx_amd64_clang_release.d $(TMP)dlb_pmd/test/TestKlv.pmd_test_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/test/TestMdset.pmd_test_osx_amd64_clang_release.d $(TMP)dlb_pmd/test/TestModel.pmd_test_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/test/TestPcm.pmd_test_osx_amd64_clang_release.d $(TMP)dlb_pmd/test/TestSadm.pmd_test_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/test/TestXml.pmd_test_osx_amd64_clang_release.d $(TMP)dlb_pmd/test/Test_ABD_AOD_APD.pmd_test_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/test/Test_API.pmd_test_osx_amd64_clang_release.d $(TMP)dlb_pmd/test/Test_Characters.pmd_test_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/test/Test_EEP.pmd_test_osx_amd64_clang_release.d $(TMP)dlb_pmd/test/Test_ETD.pmd_test_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/test/Test_Floatvals.pmd_test_osx_amd64_clang_release.d $(TMP)dlb_pmd/test/Test_HED.pmd_test_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/test/Test_IAT.pmd_test_osx_amd64_clang_release.d $(TMP)dlb_pmd/test/Test_Languages.pmd_test_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/test/Test_PLD.pmd_test_osx_amd64_clang_release.d $(TMP)dlb_pmd/test/Test_PresentationConfig.pmd_test_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/test/Test_Profiles.pmd_test_osx_amd64_clang_release.d $(TMP)dlb_pmd/test/Test_Smpte2109.pmd_test_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/test/Test_Versions.pmd_test_osx_amd64_clang_release.d $(TMP)dlb_pmd/test/Test_XYZ.pmd_test_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/test/XmlSchema.pmd_test_osx_amd64_clang_release.d $(TMP)dlb_pmd/test/pmd_test.pmd_test_osx_amd64_clang_release.d

cleandeps:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)

clean: cleanself
	$(MAKE) -C $(BASE)dlb_pmd/make/dlb_pmd_lib/osx_amd64_clang cleanself
	$(MAKE) -C $(BASE)dlb_pmd/make/dlb_wave_lib/osx_amd64_clang cleanself
	$(MAKE) -C $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/osx_amd64_clang cleanself
	$(MAKE) -C $(BASE)googletest/1.8.0/make/googletest/osx_amd64_clang cleanself
	$(MAKE) -C $(BASE)xerces/xerces-c-3.2.0/make/xerces_3_2_0_lib/osx_amd64_clang cleanself
	$(MAKE) -C $(BASE)zlib/1.2.11/make/zlib/osx_amd64_clang cleanself

cleanself:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)
	$(RM) $(INPUTS_LINK_debug)
	$(RM) $(INPUTS_LINK_release)
	$(RM) pmd_test_debug
	$(RM) pmd_test_release

.PHONY: default FORCE clean cleandeps cleanself help

FORCE:

help:
	@echo "This makefile has the following targets:"
	@echo " pmd_test_debug"
	@echo " pmd_test_release"
	@echo " default"
	@echo " clean"
	@echo " cleandeps"
	@echo " help"

include $(wildcard $(TMP)dlb_pmd/os/osx/pmd_ctrl_c.pmd_test_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/TestKlv.pmd_test_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/TestMdset.pmd_test_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/TestModel.pmd_test_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/TestPcm.pmd_test_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/TestSadm.pmd_test_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/TestXml.pmd_test_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_ABD_AOD_APD.pmd_test_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_API.pmd_test_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_Characters.pmd_test_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_EEP.pmd_test_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_ETD.pmd_test_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_Floatvals.pmd_test_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_HED.pmd_test_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_IAT.pmd_test_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_Languages.pmd_test_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_PLD.pmd_test_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_PresentationConfig.pmd_test_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_Profiles.pmd_test_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_Smpte2109.pmd_test_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_Versions.pmd_test_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_XYZ.pmd_test_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/XmlSchema.pmd_test_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/pmd_test.pmd_test_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/os/osx/pmd_ctrl_c.pmd_test_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/test/TestKlv.pmd_test_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/test/TestMdset.pmd_test_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/test/TestModel.pmd_test_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/test/TestPcm.pmd_test_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/test/TestSadm.pmd_test_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/test/TestXml.pmd_test_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_ABD_AOD_APD.pmd_test_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_API.pmd_test_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_Characters.pmd_test_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_EEP.pmd_test_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_ETD.pmd_test_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_Floatvals.pmd_test_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_HED.pmd_test_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_IAT.pmd_test_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_Languages.pmd_test_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_PLD.pmd_test_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_PresentationConfig.pmd_test_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_Profiles.pmd_test_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_Smpte2109.pmd_test_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_Versions.pmd_test_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_XYZ.pmd_test_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/test/XmlSchema.pmd_test_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/test/pmd_test.pmd_test_osx_amd64_clang_release.d)

$(BASE)dlb_pmd/make/dlb_pmd_lib/osx_amd64_clang/dlb_pmd_lib_debug.a \
$(BASE)dlb_pmd/make/dlb_pmd_lib/osx_amd64_clang/dlb_pmd_lib_release.a \
$(BASE)dlb_pmd/make/dlb_wave_lib/osx_amd64_clang/dlb_wave_lib_debug.a \
$(BASE)dlb_pmd/make/dlb_wave_lib/osx_amd64_clang/dlb_wave_lib_release.a \
$(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/osx_amd64_clang/dlb_xml_debug.a \
$(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/osx_amd64_clang/dlb_xml_release.a \
$(BASE)googletest/1.8.0/make/googletest/osx_amd64_clang/googletest_debug.a \
$(BASE)googletest/1.8.0/make/googletest/osx_amd64_clang/googletest_release.a \
$(BASE)xerces/xerces-c-3.2.0/make/xerces_3_2_0_lib/osx_amd64_clang/xerces_3_2_0_lib_debug.a \
$(BASE)xerces/xerces-c-3.2.0/make/xerces_3_2_0_lib/osx_amd64_clang/xerces_3_2_0_lib_release.a \
$(BASE)zlib/1.2.11/make/zlib/osx_amd64_clang/zlib_debug.a \
$(BASE)zlib/1.2.11/make/zlib/osx_amd64_clang/zlib_release.a: %.a: FORCE
	$(MAKE) -C $(@D) $(@F)


