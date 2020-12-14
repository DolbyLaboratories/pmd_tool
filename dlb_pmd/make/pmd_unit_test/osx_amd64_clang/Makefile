#-*-makefile-*-

BASE=../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
CCDEP = $(CC)
CFLAGS_debug = -g -O0 -arch x86_64 -fPIC -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wno-overlength-strings -Wdeclaration-after-statement -Wconditional-uninitialized
CFLAGS_release = -O3 -arch x86_64 -fPIC -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wno-overlength-strings -Wdeclaration-after-statement -Wconditional-uninitialized
CXXDEP = $(CXX)
CXXFLAGS_debug = -std=c++11 -g -O0 -arch x86_64 -fPIC -c -Wall
CXXFLAGS_release = -std=c++11 -O3 -arch x86_64 -fPIC -c -Wall
DEFINES_release = -DNDEBUG=1
INCLUDES = -I$(BASE)Lawo/ember-plus/libember_slim/Source -I$(BASE)dlb_buffer/1.3 -I$(BASE)dlb_octfile/1.1 -I$(BASE)dlb_pmd/include -I$(BASE)dlb_pmd/include -I$(BASE)dlb_pmd/os -I$(BASE)dlb_pmd/os -I$(BASE)dlb_pmd/src/model -I$(BASE)dlb_pmd/src/modules/pcm -I$(BASE)dlb_pmd -I$(BASE)dlb_wave/1.6 -I$(BASE)googletest/1.8.0/include -I$(BASE)googletest/1.8.0/include -I$(BASE). -I$(BASE). -I$(BASE)zlib/1.2.11/zlib-1.2.11
LD = $(CXX)
LDFLAGS_debug = -arch x86_64 -dead_strip
LDFLAGS_release = -arch x86_64 -dead_strip -O2
LDLIBS = -lpthread


INPUTS_LINK_debug = $(TMP)dlb_pmd/os/osx/pmd_ctrl_c.pmd_unit_test_osx_amd64_clang_debug.o $(TMP)dlb_pmd/unit_test/dlb_pmd_capture_01.pmd_unit_test_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/unit_test/dlb_pmd_capture_02.pmd_unit_test_osx_amd64_clang_debug.o $(TMP)dlb_pmd/unit_test/dlb_pmd_pcm_01.pmd_unit_test_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/unit_test/dlb_pmd_sadm_01.pmd_unit_test_osx_amd64_clang_debug.o $(TMP)dlb_pmd/unit_test/dlb_pmd_sadm_02.pmd_unit_test_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/unit_test/libember_slim_01.pmd_unit_test_osx_amd64_clang_debug.o $(TMP)dlb_pmd/unit_test/pmd_unit_test.pmd_unit_test_osx_amd64_clang_debug.o
INPUTS_LINK_release = $(TMP)dlb_pmd/os/osx/pmd_ctrl_c.pmd_unit_test_osx_amd64_clang_release.o $(TMP)dlb_pmd/unit_test/dlb_pmd_capture_01.pmd_unit_test_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/unit_test/dlb_pmd_capture_02.pmd_unit_test_osx_amd64_clang_release.o $(TMP)dlb_pmd/unit_test/dlb_pmd_pcm_01.pmd_unit_test_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/unit_test/dlb_pmd_sadm_01.pmd_unit_test_osx_amd64_clang_release.o $(TMP)dlb_pmd/unit_test/dlb_pmd_sadm_02.pmd_unit_test_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/unit_test/libember_slim_01.pmd_unit_test_osx_amd64_clang_release.o $(TMP)dlb_pmd/unit_test/pmd_unit_test.pmd_unit_test_osx_amd64_clang_release.o

DEP_PROJ_LINK_debug = $(BASE)dlb_pmd/make/dlb_wave_lib/osx_amd64_clang/dlb_wave_lib_debug.a $(BASE)Lawo/ember-plus/libember_slim/make/libember_slim/osx_amd64_clang/libember_slim_debug.a $(BASE)dlb_pmd/make/dlb_pmd_lib/osx_amd64_clang/dlb_pmd_lib_debug.a $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/osx_amd64_clang/dlb_xml_debug.a $(BASE)zlib/1.2.11/make/zlib/osx_amd64_clang/zlib_debug.a $(BASE)googletest/1.8.0/make/googletest/osx_amd64_clang/googletest_debug.a
DEP_PROJ_LINK_release = $(BASE)dlb_pmd/make/dlb_wave_lib/osx_amd64_clang/dlb_wave_lib_release.a $(BASE)Lawo/ember-plus/libember_slim/make/libember_slim/osx_amd64_clang/libember_slim_release.a $(BASE)dlb_pmd/make/dlb_pmd_lib/osx_amd64_clang/dlb_pmd_lib_release.a $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/osx_amd64_clang/dlb_xml_release.a $(BASE)zlib/1.2.11/make/zlib/osx_amd64_clang/zlib_release.a $(BASE)googletest/1.8.0/make/googletest/osx_amd64_clang/googletest_release.a

default: pmd_unit_test_debug pmd_unit_test_release

pmd_unit_test_debug: $(INPUTS_LINK_debug) $(DEP_PROJ_LINK_debug)
	$(LD) $(LDFLAGS_debug) -o $@ $+ $(LDLIBS)

pmd_unit_test_release: $(INPUTS_LINK_release) $(DEP_PROJ_LINK_release)
	$(LD) $(LDFLAGS_release) -o $@ $+ $(LDLIBS)

$(TMP)%.pmd_unit_test_osx_amd64_clang_debug.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -arch x86_64 -MM $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_debug) $(INCLUDES) -o $@ $<

$(TMP)%.pmd_unit_test_osx_amd64_clang_debug.o: $(BASE)%.cc | $(TMP)%/..
	$(CXXDEP) -std=c++11 -arch x86_64 -MM $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS_debug) $(INCLUDES) -o $@ $<

$(TMP)%.pmd_unit_test_osx_amd64_clang_release.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -arch x86_64 -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

$(TMP)%.pmd_unit_test_osx_amd64_clang_release.o: $(BASE)%.cc | $(TMP)%/..
	$(CXXDEP) -std=c++11 -arch x86_64 -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMPILE_debug = $(TMP)dlb_pmd/os/osx/pmd_ctrl_c.pmd_unit_test_osx_amd64_clang_debug.d $(TMP)dlb_pmd/unit_test/dlb_pmd_capture_01.pmd_unit_test_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/unit_test/dlb_pmd_capture_02.pmd_unit_test_osx_amd64_clang_debug.d $(TMP)dlb_pmd/unit_test/dlb_pmd_pcm_01.pmd_unit_test_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/unit_test/dlb_pmd_sadm_01.pmd_unit_test_osx_amd64_clang_debug.d $(TMP)dlb_pmd/unit_test/dlb_pmd_sadm_02.pmd_unit_test_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/unit_test/libember_slim_01.pmd_unit_test_osx_amd64_clang_debug.d $(TMP)dlb_pmd/unit_test/pmd_unit_test.pmd_unit_test_osx_amd64_clang_debug.d
CLEAN_COMPILE_release = $(TMP)dlb_pmd/os/osx/pmd_ctrl_c.pmd_unit_test_osx_amd64_clang_release.d $(TMP)dlb_pmd/unit_test/dlb_pmd_capture_01.pmd_unit_test_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/unit_test/dlb_pmd_capture_02.pmd_unit_test_osx_amd64_clang_release.d $(TMP)dlb_pmd/unit_test/dlb_pmd_pcm_01.pmd_unit_test_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/unit_test/dlb_pmd_sadm_01.pmd_unit_test_osx_amd64_clang_release.d $(TMP)dlb_pmd/unit_test/dlb_pmd_sadm_02.pmd_unit_test_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/unit_test/libember_slim_01.pmd_unit_test_osx_amd64_clang_release.d $(TMP)dlb_pmd/unit_test/pmd_unit_test.pmd_unit_test_osx_amd64_clang_release.d

cleandeps:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)

clean: cleanself
	$(MAKE) -C $(BASE)Lawo/ember-plus/libember_slim/make/libember_slim/osx_amd64_clang cleanself
	$(MAKE) -C $(BASE)dlb_pmd/make/dlb_pmd_lib/osx_amd64_clang cleanself
	$(MAKE) -C $(BASE)dlb_pmd/make/dlb_wave_lib/osx_amd64_clang cleanself
	$(MAKE) -C $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/osx_amd64_clang cleanself
	$(MAKE) -C $(BASE)googletest/1.8.0/make/googletest/osx_amd64_clang cleanself
	$(MAKE) -C $(BASE)zlib/1.2.11/make/zlib/osx_amd64_clang cleanself

cleanself:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)
	$(RM) $(INPUTS_LINK_debug)
	$(RM) $(INPUTS_LINK_release)
	$(RM) pmd_unit_test_debug
	$(RM) pmd_unit_test_release

.PHONY: default FORCE clean cleandeps cleanself help

FORCE:

help:
	@echo "This makefile has the following targets:"
	@echo " pmd_unit_test_debug"
	@echo " pmd_unit_test_release"
	@echo " default"
	@echo " clean"
	@echo " cleandeps"
	@echo " help"

include $(wildcard $(TMP)dlb_pmd/os/osx/pmd_ctrl_c.pmd_unit_test_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/unit_test/dlb_pmd_capture_01.pmd_unit_test_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/unit_test/dlb_pmd_capture_02.pmd_unit_test_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/unit_test/dlb_pmd_pcm_01.pmd_unit_test_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/unit_test/dlb_pmd_sadm_01.pmd_unit_test_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/unit_test/dlb_pmd_sadm_02.pmd_unit_test_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/unit_test/libember_slim_01.pmd_unit_test_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/unit_test/pmd_unit_test.pmd_unit_test_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/os/osx/pmd_ctrl_c.pmd_unit_test_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/unit_test/dlb_pmd_capture_01.pmd_unit_test_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/unit_test/dlb_pmd_capture_02.pmd_unit_test_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/unit_test/dlb_pmd_pcm_01.pmd_unit_test_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/unit_test/dlb_pmd_sadm_01.pmd_unit_test_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/unit_test/dlb_pmd_sadm_02.pmd_unit_test_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/unit_test/libember_slim_01.pmd_unit_test_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/unit_test/pmd_unit_test.pmd_unit_test_osx_amd64_clang_release.d)

$(BASE)Lawo/ember-plus/libember_slim/make/libember_slim/osx_amd64_clang/libember_slim_debug.a \
$(BASE)Lawo/ember-plus/libember_slim/make/libember_slim/osx_amd64_clang/libember_slim_release.a \
$(BASE)dlb_pmd/make/dlb_pmd_lib/osx_amd64_clang/dlb_pmd_lib_debug.a \
$(BASE)dlb_pmd/make/dlb_pmd_lib/osx_amd64_clang/dlb_pmd_lib_release.a \
$(BASE)dlb_pmd/make/dlb_wave_lib/osx_amd64_clang/dlb_wave_lib_debug.a \
$(BASE)dlb_pmd/make/dlb_wave_lib/osx_amd64_clang/dlb_wave_lib_release.a \
$(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/osx_amd64_clang/dlb_xml_debug.a \
$(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/osx_amd64_clang/dlb_xml_release.a \
$(BASE)googletest/1.8.0/make/googletest/osx_amd64_clang/googletest_debug.a \
$(BASE)googletest/1.8.0/make/googletest/osx_amd64_clang/googletest_release.a \
$(BASE)zlib/1.2.11/make/zlib/osx_amd64_clang/zlib_debug.a \
$(BASE)zlib/1.2.11/make/zlib/osx_amd64_clang/zlib_release.a: %.a: FORCE
	$(MAKE) -C $(@D) $(@F)


