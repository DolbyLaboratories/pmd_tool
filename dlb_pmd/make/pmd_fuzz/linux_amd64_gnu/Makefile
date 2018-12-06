#-*-makefile-*-

BASE=../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
CXXDEP = $(CXX)
CXXFLAGS_debug = -g -ggdb3 -O0 -m64 -c -Wall
CXXFLAGS_release = -O3 -ftree-vectorize -m64 -c -Wall
DEFINES_debug = -DHAVE_CONFIG_H=1 -DXERCES_BUILDING_LIBRARY=1
DEFINES_release = -DHAVE_CONFIG_H=1 -DNDEBUG=1 -DXERCES_BUILDING_LIBRARY=1
INCLUDES = -I$(BASE)dlb_bitbuf/2.0 -I$(BASE)dlb_buffer/1.3 -I$(BASE)dlb_octfile/1.1 -I$(BASE)dlb_pmd/include -I$(BASE)dlb_pmd -I$(BASE)dlb_wave/1.6 -I$(BASE). -I$(BASE). -I$(BASE)xerces/xerces-c-3.2.0/linux -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/FileManagers -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/MsgLoaders/ICU -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/MsgLoaders/MsgCatalog -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/MutexManagers -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/Curl -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/Socket -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/Transcoders/ICU -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/Transcoders/Iconv -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/Transcoders/IconvGNU -I$(BASE)xerces/xerces-c-3.2.0/src -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/dom/impl -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/dom -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/framework -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/internal -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/parsers -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/sax -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/sax2 -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/MsgLoaders/InMemory -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/regx -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/validators/common -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/validators/schema -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/xinclude
LD = $(CXX)
LDFLAGS_debug = -m64
LDFLAGS_release = -O2 -m64
LDLIBS = -lpthread -lnsl -lpthread -lcurl -licui18n -licuuc -licudata


INPUTS_LINK_debug = $(TMP)dlb_pmd/test/TestKlv.pmd_fuzz_linux_amd64_gnu_debug.o $(TMP)dlb_pmd/test/TestMdset.pmd_fuzz_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/test/TestModel.pmd_fuzz_linux_amd64_gnu_debug.o $(TMP)dlb_pmd/test/TestPcm.pmd_fuzz_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/test/TestXml.pmd_fuzz_linux_amd64_gnu_debug.o $(TMP)dlb_pmd/test/XmlSchema.pmd_fuzz_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/test/pmd_fuzz.pmd_fuzz_linux_amd64_gnu_debug.o
INPUTS_LINK_release = $(TMP)dlb_pmd/test/TestKlv.pmd_fuzz_linux_amd64_gnu_release.o $(TMP)dlb_pmd/test/TestMdset.pmd_fuzz_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/test/TestModel.pmd_fuzz_linux_amd64_gnu_release.o $(TMP)dlb_pmd/test/TestPcm.pmd_fuzz_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/test/TestXml.pmd_fuzz_linux_amd64_gnu_release.o $(TMP)dlb_pmd/test/XmlSchema.pmd_fuzz_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/test/pmd_fuzz.pmd_fuzz_linux_amd64_gnu_release.o

DEP_PROJ_LINK_debug = $(BASE)dlb_pmd/make/dlb_pmd_lib/linux_amd64_gnu/dlb_pmd_lib_debug.a $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_amd64_gnu/dlb_xml_debug.a $(BASE)dlb_pmd/make/dlb_wave_lib/linux_amd64_gnu/dlb_wave_lib_debug.a $(BASE)xerces/xerces-c-3.2.0/make/xerces_3_2_0_lib/linux_amd64_gnu/xerces_3_2_0_lib_debug.a
DEP_PROJ_LINK_release = $(BASE)dlb_pmd/make/dlb_pmd_lib/linux_amd64_gnu/dlb_pmd_lib_release.a $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_amd64_gnu/dlb_xml_release.a $(BASE)dlb_pmd/make/dlb_wave_lib/linux_amd64_gnu/dlb_wave_lib_release.a $(BASE)xerces/xerces-c-3.2.0/make/xerces_3_2_0_lib/linux_amd64_gnu/xerces_3_2_0_lib_release.a

default: pmd_fuzz_debug pmd_fuzz_release

pmd_fuzz_debug: $(INPUTS_LINK_debug) $(DEP_PROJ_LINK_debug)
	$(LD) $(LDFLAGS_debug) -o $@ $+ $(LDLIBS)

pmd_fuzz_release: $(INPUTS_LINK_release) $(DEP_PROJ_LINK_release)
	$(LD) $(LDFLAGS_release) -o $@ $+ $(LDLIBS)

$(TMP)%.pmd_fuzz_linux_amd64_gnu_debug.o: $(BASE)%.cc | $(TMP)%/..
	$(CXXDEP) -MM $(DEFINES_debug) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS_debug) $(DEFINES_debug) $(INCLUDES) -o $@ $<

$(TMP)%.pmd_fuzz_linux_amd64_gnu_release.o: $(BASE)%.cc | $(TMP)%/..
	$(CXXDEP) -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMPILE_debug = $(TMP)dlb_pmd/test/TestKlv.pmd_fuzz_linux_amd64_gnu_debug.d $(TMP)dlb_pmd/test/TestMdset.pmd_fuzz_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/test/TestModel.pmd_fuzz_linux_amd64_gnu_debug.d $(TMP)dlb_pmd/test/TestPcm.pmd_fuzz_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/test/TestXml.pmd_fuzz_linux_amd64_gnu_debug.d $(TMP)dlb_pmd/test/XmlSchema.pmd_fuzz_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/test/pmd_fuzz.pmd_fuzz_linux_amd64_gnu_debug.d
CLEAN_COMPILE_release = $(TMP)dlb_pmd/test/TestKlv.pmd_fuzz_linux_amd64_gnu_release.d $(TMP)dlb_pmd/test/TestMdset.pmd_fuzz_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/test/TestModel.pmd_fuzz_linux_amd64_gnu_release.d $(TMP)dlb_pmd/test/TestPcm.pmd_fuzz_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/test/TestXml.pmd_fuzz_linux_amd64_gnu_release.d $(TMP)dlb_pmd/test/XmlSchema.pmd_fuzz_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/test/pmd_fuzz.pmd_fuzz_linux_amd64_gnu_release.d

cleandeps:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)

clean: cleanself
	$(MAKE) -C $(BASE)dlb_pmd/make/dlb_pmd_lib/linux_amd64_gnu cleanself
	$(MAKE) -C $(BASE)dlb_pmd/make/dlb_wave_lib/linux_amd64_gnu cleanself
	$(MAKE) -C $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_amd64_gnu cleanself
	$(MAKE) -C $(BASE)xerces/xerces-c-3.2.0/make/xerces_3_2_0_lib/linux_amd64_gnu cleanself

cleanself:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)
	$(RM) $(INPUTS_LINK_debug)
	$(RM) $(INPUTS_LINK_release)
	$(RM) pmd_fuzz_debug
	$(RM) pmd_fuzz_release

.PHONY: default FORCE clean cleandeps cleanself help

FORCE:

help:
	@echo "This makefile has the following targets:"
	@echo " pmd_fuzz_debug"
	@echo " pmd_fuzz_release"
	@echo " default"
	@echo " clean"
	@echo " cleandeps"
	@echo " help"

include $(wildcard $(TMP)dlb_pmd/test/TestKlv.pmd_fuzz_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/TestMdset.pmd_fuzz_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/TestModel.pmd_fuzz_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/TestPcm.pmd_fuzz_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/TestXml.pmd_fuzz_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/XmlSchema.pmd_fuzz_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/pmd_fuzz.pmd_fuzz_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/test/TestKlv.pmd_fuzz_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/test/TestMdset.pmd_fuzz_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/test/TestModel.pmd_fuzz_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/test/TestPcm.pmd_fuzz_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/test/TestXml.pmd_fuzz_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/test/XmlSchema.pmd_fuzz_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/test/pmd_fuzz.pmd_fuzz_linux_amd64_gnu_release.d)

$(BASE)dlb_pmd/make/dlb_pmd_lib/linux_amd64_gnu/dlb_pmd_lib_debug.a \
$(BASE)dlb_pmd/make/dlb_pmd_lib/linux_amd64_gnu/dlb_pmd_lib_release.a \
$(BASE)dlb_pmd/make/dlb_wave_lib/linux_amd64_gnu/dlb_wave_lib_debug.a \
$(BASE)dlb_pmd/make/dlb_wave_lib/linux_amd64_gnu/dlb_wave_lib_release.a \
$(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_amd64_gnu/dlb_xml_debug.a \
$(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_amd64_gnu/dlb_xml_release.a \
$(BASE)xerces/xerces-c-3.2.0/make/xerces_3_2_0_lib/linux_amd64_gnu/xerces_3_2_0_lib_debug.a \
$(BASE)xerces/xerces-c-3.2.0/make/xerces_3_2_0_lib/linux_amd64_gnu/xerces_3_2_0_lib_release.a: %.a: FORCE
	$(MAKE) -C $(@D) $(@F)


