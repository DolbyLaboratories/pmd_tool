#-*-makefile-*-

BASE=../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
CXXDEP = $(CXX)
CXXFLAGS_debug = -g -ggdb3 -O0 -m64 -c -Wall
CXXFLAGS_release = -O3 -ftree-vectorize -m64 -c -Wall
DEFINES_debug = -DHAVE_CONFIG_H=1 -DWIN32=1 -DWIN64=1 -DXERCES_BUILDING_LIBRARY=1 -D_CONSOLE=1
DEFINES_release = -DHAVE_CONFIG_H=1 -DNDEBUG=1 -DWIN32=1 -DWIN64=1 -DXERCES_BUILDING_LIBRARY=1 -D_CONSOLE=1
INCLUDES = -I$(BASE)dlb_bitbuf/2.0 -I$(BASE)dlb_buffer/1.3 -I$(BASE)dlb_octfile/1.1 -I$(BASE)dlb_pmd/include -I$(BASE)dlb_pmd -I$(BASE)dlb_wave/1.6 -I$(BASE)googletest/1.8.0/include -I$(BASE)googletest/1.8.0/include -I$(BASE). -I$(BASE). -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/FileManagers -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/MsgLoaders/Win32 -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/MutexManagers -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/WinSock -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/Transcoders/Win32 -I$(BASE)xerces/xerces-c-3.2.0/windows -I$(BASE)xerces/xerces-c-3.2.0/src -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/dom/impl -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/dom -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/framework -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/internal -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/parsers -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/sax -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/sax2 -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/MsgLoaders/InMemory -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/regx -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/validators/common -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/validators/schema -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/xinclude
LD = $(CXX)
LDFLAGS_debug = -m64
LDFLAGS_release = -O2 -m64


INPUTS_LINK_debug = $(TMP)dlb_pmd/test/TestKlv.pmd_test_windows_amd64_gnu_debug.o $(TMP)dlb_pmd/test/TestMdset.pmd_test_windows_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/test/TestModel.pmd_test_windows_amd64_gnu_debug.o $(TMP)dlb_pmd/test/TestPcm.pmd_test_windows_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/test/TestXml.pmd_test_windows_amd64_gnu_debug.o $(TMP)dlb_pmd/test/Test_ABD_AOD_APD.pmd_test_windows_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/test/Test_API.pmd_test_windows_amd64_gnu_debug.o $(TMP)dlb_pmd/test/Test_Characters.pmd_test_windows_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/test/Test_EEP.pmd_test_windows_amd64_gnu_debug.o $(TMP)dlb_pmd/test/Test_ETD.pmd_test_windows_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/test/Test_Floatvals.pmd_test_windows_amd64_gnu_debug.o $(TMP)dlb_pmd/test/Test_HED.pmd_test_windows_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/test/Test_IAT.pmd_test_windows_amd64_gnu_debug.o $(TMP)dlb_pmd/test/Test_Languages.pmd_test_windows_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/test/Test_PLD.pmd_test_windows_amd64_gnu_debug.o $(TMP)dlb_pmd/test/Test_PresentationConfig.pmd_test_windows_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/test/Test_Profiles.pmd_test_windows_amd64_gnu_debug.o $(TMP)dlb_pmd/test/Test_Smpte2109.pmd_test_windows_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/test/Test_Versions.pmd_test_windows_amd64_gnu_debug.o $(TMP)dlb_pmd/test/Test_XYZ.pmd_test_windows_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/test/XmlSchema.pmd_test_windows_amd64_gnu_debug.o $(TMP)dlb_pmd/test/pmd_test.pmd_test_windows_amd64_gnu_debug.o
INPUTS_LINK_release = $(TMP)dlb_pmd/test/TestKlv.pmd_test_windows_amd64_gnu_release.o $(TMP)dlb_pmd/test/TestMdset.pmd_test_windows_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/test/TestModel.pmd_test_windows_amd64_gnu_release.o $(TMP)dlb_pmd/test/TestPcm.pmd_test_windows_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/test/TestXml.pmd_test_windows_amd64_gnu_release.o $(TMP)dlb_pmd/test/Test_ABD_AOD_APD.pmd_test_windows_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/test/Test_API.pmd_test_windows_amd64_gnu_release.o $(TMP)dlb_pmd/test/Test_Characters.pmd_test_windows_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/test/Test_EEP.pmd_test_windows_amd64_gnu_release.o $(TMP)dlb_pmd/test/Test_ETD.pmd_test_windows_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/test/Test_Floatvals.pmd_test_windows_amd64_gnu_release.o $(TMP)dlb_pmd/test/Test_HED.pmd_test_windows_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/test/Test_IAT.pmd_test_windows_amd64_gnu_release.o $(TMP)dlb_pmd/test/Test_Languages.pmd_test_windows_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/test/Test_PLD.pmd_test_windows_amd64_gnu_release.o $(TMP)dlb_pmd/test/Test_PresentationConfig.pmd_test_windows_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/test/Test_Profiles.pmd_test_windows_amd64_gnu_release.o $(TMP)dlb_pmd/test/Test_Smpte2109.pmd_test_windows_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/test/Test_Versions.pmd_test_windows_amd64_gnu_release.o $(TMP)dlb_pmd/test/Test_XYZ.pmd_test_windows_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/test/XmlSchema.pmd_test_windows_amd64_gnu_release.o $(TMP)dlb_pmd/test/pmd_test.pmd_test_windows_amd64_gnu_release.o

DEP_PROJ_LINK_debug = $(BASE)dlb_pmd/make/dlb_pmd_lib/windows_amd64_gnu/dlb_pmd_lib_debug.lib $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/windows_amd64_gnu/dlb_xml_debug.lib $(BASE)dlb_pmd/make/dlb_wave_lib/windows_amd64_gnu/dlb_wave_lib_debug.lib $(BASE)xerces/xerces-c-3.2.0/make/xerces_3_2_0_lib/windows_amd64_gnu/xerces_3_2_0_lib_debug.lib $(BASE)googletest/1.8.0/make/googletest/windows_amd64_gnu/googletest_debug.lib
DEP_PROJ_LINK_release = $(BASE)dlb_pmd/make/dlb_pmd_lib/windows_amd64_gnu/dlb_pmd_lib_release.lib $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/windows_amd64_gnu/dlb_xml_release.lib $(BASE)dlb_pmd/make/dlb_wave_lib/windows_amd64_gnu/dlb_wave_lib_release.lib $(BASE)xerces/xerces-c-3.2.0/make/xerces_3_2_0_lib/windows_amd64_gnu/xerces_3_2_0_lib_release.lib $(BASE)googletest/1.8.0/make/googletest/windows_amd64_gnu/googletest_release.lib

default: pmd_test_debug.exe pmd_test_release.exe

pmd_test_debug.exe: $(INPUTS_LINK_debug) $(DEP_PROJ_LINK_debug)
	$(LD) $(LDFLAGS_debug) -o $@ $+ 

pmd_test_release.exe: $(INPUTS_LINK_release) $(DEP_PROJ_LINK_release)
	$(LD) $(LDFLAGS_release) -o $@ $+ 

$(TMP)%.pmd_test_windows_amd64_gnu_debug.o: $(BASE)%.cc | $(TMP)%/..
	$(CXXDEP) -MM $(DEFINES_debug) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS_debug) $(DEFINES_debug) $(INCLUDES) -o $@ $<

$(TMP)%.pmd_test_windows_amd64_gnu_release.o: $(BASE)%.cc | $(TMP)%/..
	$(CXXDEP) -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMPILE_debug = $(TMP)dlb_pmd/test/TestKlv.pmd_test_windows_amd64_gnu_debug.d $(TMP)dlb_pmd/test/TestMdset.pmd_test_windows_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/test/TestModel.pmd_test_windows_amd64_gnu_debug.d $(TMP)dlb_pmd/test/TestPcm.pmd_test_windows_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/test/TestXml.pmd_test_windows_amd64_gnu_debug.d $(TMP)dlb_pmd/test/Test_ABD_AOD_APD.pmd_test_windows_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/test/Test_API.pmd_test_windows_amd64_gnu_debug.d $(TMP)dlb_pmd/test/Test_Characters.pmd_test_windows_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/test/Test_EEP.pmd_test_windows_amd64_gnu_debug.d $(TMP)dlb_pmd/test/Test_ETD.pmd_test_windows_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/test/Test_Floatvals.pmd_test_windows_amd64_gnu_debug.d $(TMP)dlb_pmd/test/Test_HED.pmd_test_windows_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/test/Test_IAT.pmd_test_windows_amd64_gnu_debug.d $(TMP)dlb_pmd/test/Test_Languages.pmd_test_windows_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/test/Test_PLD.pmd_test_windows_amd64_gnu_debug.d $(TMP)dlb_pmd/test/Test_PresentationConfig.pmd_test_windows_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/test/Test_Profiles.pmd_test_windows_amd64_gnu_debug.d $(TMP)dlb_pmd/test/Test_Smpte2109.pmd_test_windows_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/test/Test_Versions.pmd_test_windows_amd64_gnu_debug.d $(TMP)dlb_pmd/test/Test_XYZ.pmd_test_windows_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/test/XmlSchema.pmd_test_windows_amd64_gnu_debug.d $(TMP)dlb_pmd/test/pmd_test.pmd_test_windows_amd64_gnu_debug.d
CLEAN_COMPILE_release = $(TMP)dlb_pmd/test/TestKlv.pmd_test_windows_amd64_gnu_release.d $(TMP)dlb_pmd/test/TestMdset.pmd_test_windows_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/test/TestModel.pmd_test_windows_amd64_gnu_release.d $(TMP)dlb_pmd/test/TestPcm.pmd_test_windows_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/test/TestXml.pmd_test_windows_amd64_gnu_release.d $(TMP)dlb_pmd/test/Test_ABD_AOD_APD.pmd_test_windows_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/test/Test_API.pmd_test_windows_amd64_gnu_release.d $(TMP)dlb_pmd/test/Test_Characters.pmd_test_windows_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/test/Test_EEP.pmd_test_windows_amd64_gnu_release.d $(TMP)dlb_pmd/test/Test_ETD.pmd_test_windows_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/test/Test_Floatvals.pmd_test_windows_amd64_gnu_release.d $(TMP)dlb_pmd/test/Test_HED.pmd_test_windows_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/test/Test_IAT.pmd_test_windows_amd64_gnu_release.d $(TMP)dlb_pmd/test/Test_Languages.pmd_test_windows_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/test/Test_PLD.pmd_test_windows_amd64_gnu_release.d $(TMP)dlb_pmd/test/Test_PresentationConfig.pmd_test_windows_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/test/Test_Profiles.pmd_test_windows_amd64_gnu_release.d $(TMP)dlb_pmd/test/Test_Smpte2109.pmd_test_windows_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/test/Test_Versions.pmd_test_windows_amd64_gnu_release.d $(TMP)dlb_pmd/test/Test_XYZ.pmd_test_windows_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/test/XmlSchema.pmd_test_windows_amd64_gnu_release.d $(TMP)dlb_pmd/test/pmd_test.pmd_test_windows_amd64_gnu_release.d

cleandeps:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)

clean: cleanself
	$(MAKE) -C $(BASE)dlb_pmd/make/dlb_pmd_lib/windows_amd64_gnu cleanself
	$(MAKE) -C $(BASE)dlb_pmd/make/dlb_wave_lib/windows_amd64_gnu cleanself
	$(MAKE) -C $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/windows_amd64_gnu cleanself
	$(MAKE) -C $(BASE)googletest/1.8.0/make/googletest/windows_amd64_gnu cleanself
	$(MAKE) -C $(BASE)xerces/xerces-c-3.2.0/make/xerces_3_2_0_lib/windows_amd64_gnu cleanself

cleanself:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)
	$(RM) $(INPUTS_LINK_debug)
	$(RM) $(INPUTS_LINK_release)
	$(RM) pmd_test_debug.exe
	$(RM) pmd_test_release.exe

.PHONY: default FORCE clean cleandeps cleanself help

FORCE:

help:
	@echo "This makefile has the following targets:"
	@echo " pmd_test_debug.exe"
	@echo " pmd_test_release.exe"
	@echo " default"
	@echo " clean"
	@echo " cleandeps"
	@echo " help"

include $(wildcard $(TMP)dlb_pmd/test/TestKlv.pmd_test_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/TestMdset.pmd_test_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/TestModel.pmd_test_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/TestPcm.pmd_test_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/TestXml.pmd_test_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_ABD_AOD_APD.pmd_test_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_API.pmd_test_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_Characters.pmd_test_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_EEP.pmd_test_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_ETD.pmd_test_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_Floatvals.pmd_test_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_HED.pmd_test_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_IAT.pmd_test_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_Languages.pmd_test_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_PLD.pmd_test_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_PresentationConfig.pmd_test_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_Profiles.pmd_test_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_Smpte2109.pmd_test_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_Versions.pmd_test_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_XYZ.pmd_test_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/XmlSchema.pmd_test_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/pmd_test.pmd_test_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/TestKlv.pmd_test_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/test/TestMdset.pmd_test_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/test/TestModel.pmd_test_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/test/TestPcm.pmd_test_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/test/TestXml.pmd_test_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_ABD_AOD_APD.pmd_test_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_API.pmd_test_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_Characters.pmd_test_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_EEP.pmd_test_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_ETD.pmd_test_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_Floatvals.pmd_test_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_HED.pmd_test_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_IAT.pmd_test_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_Languages.pmd_test_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_PLD.pmd_test_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_PresentationConfig.pmd_test_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_Profiles.pmd_test_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_Smpte2109.pmd_test_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_Versions.pmd_test_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/test/Test_XYZ.pmd_test_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/test/XmlSchema.pmd_test_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/test/pmd_test.pmd_test_windows_amd64_gnu_release.d)

$(BASE)dlb_pmd/make/dlb_pmd_lib/windows_amd64_gnu/dlb_pmd_lib_debug.lib \
$(BASE)dlb_pmd/make/dlb_pmd_lib/windows_amd64_gnu/dlb_pmd_lib_release.lib \
$(BASE)dlb_pmd/make/dlb_wave_lib/windows_amd64_gnu/dlb_wave_lib_debug.lib \
$(BASE)dlb_pmd/make/dlb_wave_lib/windows_amd64_gnu/dlb_wave_lib_release.lib \
$(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/windows_amd64_gnu/dlb_xml_debug.lib \
$(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/windows_amd64_gnu/dlb_xml_release.lib \
$(BASE)googletest/1.8.0/make/googletest/windows_amd64_gnu/googletest_debug.lib \
$(BASE)googletest/1.8.0/make/googletest/windows_amd64_gnu/googletest_release.lib \
$(BASE)xerces/xerces-c-3.2.0/make/xerces_3_2_0_lib/windows_amd64_gnu/xerces_3_2_0_lib_debug.lib \
$(BASE)xerces/xerces-c-3.2.0/make/xerces_3_2_0_lib/windows_amd64_gnu/xerces_3_2_0_lib_release.lib: %.lib: FORCE
	$(MAKE) -C $(@D) $(@F)


