#-*-makefile-*-

BASE=../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
CXXDEP = $(CXX)
CXXFLAGS_debug = -std=c++11 -std=c++17 -g -ggdb3 -O0 -m64 -c -Wall
CXXFLAGS_release = -std=c++11 -std=c++17 -O3 -ftree-vectorize -m64 -c -Wall
DEFINES_debug = -DELPP_NO_DEFAULT_LOG_FILE=1
DEFINES_release = -DELPP_NO_DEFAULT_LOG_FILE=1 -DNDEBUG=1
INCLUDES = -I$(BASE)boost/boost_1_73_0 -I$(BASE)dlb_adm/src/adm_identity -I$(BASE)dlb_adm/src/adm_transformer -I$(BASE)dlb_adm/src/adm_xml -I$(BASE)dlb_adm/src/core_model -I$(BASE)dlb_adm/src -I$(BASE)googletest/1.8.0/include -I$(BASE).
LD = $(CXX)
LDFLAGS_debug = -pthread -m64
LDFLAGS_release = -pthread -O2 -m64


INPUTS_LINK_debug = $(TMP)dlb_adm/test/unit/CoreModelTest.dlb_adm_unit_test_linux_amd64_gnu_debug.o $(TMP)dlb_adm/test/unit/dlb_adm_01.dlb_adm_unit_test_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_adm/test/unit/dlb_adm_02.dlb_adm_unit_test_linux_amd64_gnu_debug.o $(TMP)dlb_adm/test/unit/dlb_adm_03.dlb_adm_unit_test_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_adm/test/unit/dlb_adm_04.dlb_adm_unit_test_linux_amd64_gnu_debug.o $(TMP)dlb_adm/test/unit/dlb_adm_05.dlb_adm_unit_test_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_adm/test/unit/dlb_adm_06.dlb_adm_unit_test_linux_amd64_gnu_debug.o $(TMP)dlb_adm/test/unit/dlb_adm_emission_profile.dlb_adm_unit_test_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_adm/test/unit/unit_test_main.dlb_adm_unit_test_linux_amd64_gnu_debug.o
INPUTS_LINK_release = $(TMP)dlb_adm/test/unit/CoreModelTest.dlb_adm_unit_test_linux_amd64_gnu_release.o $(TMP)dlb_adm/test/unit/dlb_adm_01.dlb_adm_unit_test_linux_amd64_gnu_release.o\
                      $(TMP)dlb_adm/test/unit/dlb_adm_02.dlb_adm_unit_test_linux_amd64_gnu_release.o $(TMP)dlb_adm/test/unit/dlb_adm_03.dlb_adm_unit_test_linux_amd64_gnu_release.o\
                      $(TMP)dlb_adm/test/unit/dlb_adm_04.dlb_adm_unit_test_linux_amd64_gnu_release.o $(TMP)dlb_adm/test/unit/dlb_adm_05.dlb_adm_unit_test_linux_amd64_gnu_release.o\
                      $(TMP)dlb_adm/test/unit/dlb_adm_06.dlb_adm_unit_test_linux_amd64_gnu_release.o $(TMP)dlb_adm/test/unit/dlb_adm_emission_profile.dlb_adm_unit_test_linux_amd64_gnu_release.o\
                      $(TMP)dlb_adm/test/unit/unit_test_main.dlb_adm_unit_test_linux_amd64_gnu_release.o

DEP_PROJ_LINK_debug = $(BASE)dlb_adm/make/dlb_adm_lib/linux_amd64_gnu/dlb_adm_lib_debug.a $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_amd64_gnu/dlb_xml_debug.a $(BASE)googletest/1.8.0/make/googletest/linux_amd64_gnu/googletest_debug.a
DEP_PROJ_LINK_release = $(BASE)dlb_adm/make/dlb_adm_lib/linux_amd64_gnu/dlb_adm_lib_release.a $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_amd64_gnu/dlb_xml_release.a $(BASE)googletest/1.8.0/make/googletest/linux_amd64_gnu/googletest_release.a

default: dlb_adm_unit_test_debug dlb_adm_unit_test_release

dlb_adm_unit_test_debug: $(INPUTS_LINK_debug) $(DEP_PROJ_LINK_debug)
	$(LD) $(LDFLAGS_debug) -o $@ $+ 

dlb_adm_unit_test_release: $(INPUTS_LINK_release) $(DEP_PROJ_LINK_release)
	$(LD) $(LDFLAGS_release) -o $@ $+ 

$(TMP)%.dlb_adm_unit_test_linux_amd64_gnu_debug.o: $(BASE)%.cpp | $(TMP)%/..
	$(CXXDEP) -std=c++11 -MM $(DEFINES_debug) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS_debug) $(DEFINES_debug) $(INCLUDES) -o $@ $<

$(TMP)%.dlb_adm_unit_test_linux_amd64_gnu_release.o: $(BASE)%.cpp | $(TMP)%/..
	$(CXXDEP) -std=c++11 -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMPILE_debug = $(TMP)dlb_adm/test/unit/CoreModelTest.dlb_adm_unit_test_linux_amd64_gnu_debug.d $(TMP)dlb_adm/test/unit/dlb_adm_01.dlb_adm_unit_test_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_adm/test/unit/dlb_adm_02.dlb_adm_unit_test_linux_amd64_gnu_debug.d $(TMP)dlb_adm/test/unit/dlb_adm_03.dlb_adm_unit_test_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_adm/test/unit/dlb_adm_04.dlb_adm_unit_test_linux_amd64_gnu_debug.d $(TMP)dlb_adm/test/unit/dlb_adm_05.dlb_adm_unit_test_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_adm/test/unit/dlb_adm_06.dlb_adm_unit_test_linux_amd64_gnu_debug.d $(TMP)dlb_adm/test/unit/dlb_adm_emission_profile.dlb_adm_unit_test_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_adm/test/unit/unit_test_main.dlb_adm_unit_test_linux_amd64_gnu_debug.d
CLEAN_COMPILE_release = $(TMP)dlb_adm/test/unit/CoreModelTest.dlb_adm_unit_test_linux_amd64_gnu_release.d $(TMP)dlb_adm/test/unit/dlb_adm_01.dlb_adm_unit_test_linux_amd64_gnu_release.d\
                        $(TMP)dlb_adm/test/unit/dlb_adm_02.dlb_adm_unit_test_linux_amd64_gnu_release.d $(TMP)dlb_adm/test/unit/dlb_adm_03.dlb_adm_unit_test_linux_amd64_gnu_release.d\
                        $(TMP)dlb_adm/test/unit/dlb_adm_04.dlb_adm_unit_test_linux_amd64_gnu_release.d $(TMP)dlb_adm/test/unit/dlb_adm_05.dlb_adm_unit_test_linux_amd64_gnu_release.d\
                        $(TMP)dlb_adm/test/unit/dlb_adm_06.dlb_adm_unit_test_linux_amd64_gnu_release.d $(TMP)dlb_adm/test/unit/dlb_adm_emission_profile.dlb_adm_unit_test_linux_amd64_gnu_release.d\
                        $(TMP)dlb_adm/test/unit/unit_test_main.dlb_adm_unit_test_linux_amd64_gnu_release.d

cleandeps:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)

clean: cleanself
	$(MAKE) -C $(BASE)dlb_adm/make/dlb_adm_lib/linux_amd64_gnu cleanself
	$(MAKE) -C $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_amd64_gnu cleanself
	$(MAKE) -C $(BASE)googletest/1.8.0/make/googletest/linux_amd64_gnu cleanself

cleanself:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)
	$(RM) $(INPUTS_LINK_debug)
	$(RM) $(INPUTS_LINK_release)
	$(RM) dlb_adm_unit_test_debug
	$(RM) dlb_adm_unit_test_release

.PHONY: default FORCE clean cleandeps cleanself help

FORCE:

help:
	@echo "This makefile has the following targets:"
	@echo " dlb_adm_unit_test_debug"
	@echo " dlb_adm_unit_test_release"
	@echo " default"
	@echo " clean"
	@echo " cleandeps"
	@echo " help"

include $(wildcard $(TMP)dlb_adm/test/unit/CoreModelTest.dlb_adm_unit_test_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/test/unit/dlb_adm_01.dlb_adm_unit_test_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/test/unit/dlb_adm_02.dlb_adm_unit_test_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/test/unit/dlb_adm_03.dlb_adm_unit_test_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/test/unit/dlb_adm_04.dlb_adm_unit_test_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/test/unit/dlb_adm_05.dlb_adm_unit_test_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/test/unit/dlb_adm_06.dlb_adm_unit_test_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/test/unit/dlb_adm_emission_profile.dlb_adm_unit_test_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/test/unit/unit_test_main.dlb_adm_unit_test_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/test/unit/CoreModelTest.dlb_adm_unit_test_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/test/unit/dlb_adm_01.dlb_adm_unit_test_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/test/unit/dlb_adm_02.dlb_adm_unit_test_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/test/unit/dlb_adm_03.dlb_adm_unit_test_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/test/unit/dlb_adm_04.dlb_adm_unit_test_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/test/unit/dlb_adm_05.dlb_adm_unit_test_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/test/unit/dlb_adm_06.dlb_adm_unit_test_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/test/unit/dlb_adm_emission_profile.dlb_adm_unit_test_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/test/unit/unit_test_main.dlb_adm_unit_test_linux_amd64_gnu_release.d)

$(BASE)dlb_adm/make/dlb_adm_lib/linux_amd64_gnu/dlb_adm_lib_debug.a \
$(BASE)dlb_adm/make/dlb_adm_lib/linux_amd64_gnu/dlb_adm_lib_release.a \
$(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_amd64_gnu/dlb_xml_debug.a \
$(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_amd64_gnu/dlb_xml_release.a \
$(BASE)googletest/1.8.0/make/googletest/linux_amd64_gnu/googletest_debug.a \
$(BASE)googletest/1.8.0/make/googletest/linux_amd64_gnu/googletest_release.a: %.a: FORCE
	$(MAKE) -C $(@D) $(@F)


