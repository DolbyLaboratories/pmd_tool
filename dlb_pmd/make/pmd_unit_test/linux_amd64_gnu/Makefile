#-*-makefile-*-

BASE=../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
CCDEP = $(CC)
CFLAGS_debug = -D_GNU_SOURCE -g -ggdb3 -O0 -m64 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
CFLAGS_release = -D_GNU_SOURCE -O3 -ftree-vectorize -m64 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
CXXDEP = $(CXX)
CXXFLAGS_debug = -std=c++11 -g -ggdb3 -O0 -m64 -c -Wall
CXXFLAGS_release = -std=c++11 -O3 -ftree-vectorize -m64 -c -Wall
DEFINES_release = -DNDEBUG=1
INCLUDES = -I$(BASE)dlb_buffer/1.3 -I$(BASE)dlb_octfile/1.1 -I$(BASE)dlb_pmd/include -I$(BASE)dlb_pmd/include -I$(BASE)dlb_pmd/os -I$(BASE)dlb_pmd/os -I$(BASE)dlb_pmd/src/model -I$(BASE)dlb_pmd/src/modules/pcm -I$(BASE)dlb_pmd -I$(BASE)dlb_wave/1.6 -I$(BASE)googletest/1.8.0/include -I$(BASE)googletest/1.8.0/include -I$(BASE). -I$(BASE). -I$(BASE)zlib/1.2.11/zlib-1.2.11
LD = $(CXX)
LDFLAGS_debug = -m64
LDFLAGS_release = -O2 -m64
LDLIBS = -lpthread


INPUTS_LINK_debug = $(TMP)dlb_pmd/os/linux/pmd_ctrl_c.pmd_unit_test_linux_amd64_gnu_debug.o $(TMP)dlb_pmd/unit_test/dlb_pmd_capture_01.pmd_unit_test_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/unit_test/dlb_pmd_capture_02.pmd_unit_test_linux_amd64_gnu_debug.o $(TMP)dlb_pmd/unit_test/dlb_pmd_pcm_01.pmd_unit_test_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/unit_test/dlb_pmd_sadm_01.pmd_unit_test_linux_amd64_gnu_debug.o $(TMP)dlb_pmd/unit_test/dlb_pmd_sadm_02.pmd_unit_test_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/unit_test/pmd_unit_test.pmd_unit_test_linux_amd64_gnu_debug.o
INPUTS_LINK_release = $(TMP)dlb_pmd/os/linux/pmd_ctrl_c.pmd_unit_test_linux_amd64_gnu_release.o $(TMP)dlb_pmd/unit_test/dlb_pmd_capture_01.pmd_unit_test_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/unit_test/dlb_pmd_capture_02.pmd_unit_test_linux_amd64_gnu_release.o $(TMP)dlb_pmd/unit_test/dlb_pmd_pcm_01.pmd_unit_test_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/unit_test/dlb_pmd_sadm_01.pmd_unit_test_linux_amd64_gnu_release.o $(TMP)dlb_pmd/unit_test/dlb_pmd_sadm_02.pmd_unit_test_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/unit_test/pmd_unit_test.pmd_unit_test_linux_amd64_gnu_release.o

DEP_PROJ_LINK_debug = $(BASE)dlb_pmd/make/dlb_wave_lib/linux_amd64_gnu/dlb_wave_lib_debug.a $(BASE)dlb_pmd/make/dlb_pmd_lib/linux_amd64_gnu/dlb_pmd_lib_debug.a $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_amd64_gnu/dlb_xml_debug.a $(BASE)zlib/1.2.11/make/zlib/linux_amd64_gnu/zlib_debug.a $(BASE)googletest/1.8.0/make/googletest/linux_amd64_gnu/googletest_debug.a
DEP_PROJ_LINK_release = $(BASE)dlb_pmd/make/dlb_wave_lib/linux_amd64_gnu/dlb_wave_lib_release.a $(BASE)dlb_pmd/make/dlb_pmd_lib/linux_amd64_gnu/dlb_pmd_lib_release.a $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_amd64_gnu/dlb_xml_release.a $(BASE)zlib/1.2.11/make/zlib/linux_amd64_gnu/zlib_release.a $(BASE)googletest/1.8.0/make/googletest/linux_amd64_gnu/googletest_release.a

default: pmd_unit_test_debug pmd_unit_test_release

pmd_unit_test_debug: $(INPUTS_LINK_debug) $(DEP_PROJ_LINK_debug)
	$(LD) $(LDFLAGS_debug) -o $@ $+ $(LDLIBS)

pmd_unit_test_release: $(INPUTS_LINK_release) $(DEP_PROJ_LINK_release)
	$(LD) $(LDFLAGS_release) -o $@ $+ $(LDLIBS)

$(TMP)%.pmd_unit_test_linux_amd64_gnu_debug.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -MM $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_debug) $(INCLUDES) -o $@ $<

$(TMP)%.pmd_unit_test_linux_amd64_gnu_debug.o: $(BASE)%.cc | $(TMP)%/..
	$(CXXDEP) -std=c++11 -MM $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS_debug) $(INCLUDES) -o $@ $<

$(TMP)%.pmd_unit_test_linux_amd64_gnu_release.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

$(TMP)%.pmd_unit_test_linux_amd64_gnu_release.o: $(BASE)%.cc | $(TMP)%/..
	$(CXXDEP) -std=c++11 -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMPILE_debug = $(TMP)dlb_pmd/os/linux/pmd_ctrl_c.pmd_unit_test_linux_amd64_gnu_debug.d $(TMP)dlb_pmd/unit_test/dlb_pmd_capture_01.pmd_unit_test_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/unit_test/dlb_pmd_capture_02.pmd_unit_test_linux_amd64_gnu_debug.d $(TMP)dlb_pmd/unit_test/dlb_pmd_pcm_01.pmd_unit_test_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/unit_test/dlb_pmd_sadm_01.pmd_unit_test_linux_amd64_gnu_debug.d $(TMP)dlb_pmd/unit_test/dlb_pmd_sadm_02.pmd_unit_test_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/unit_test/pmd_unit_test.pmd_unit_test_linux_amd64_gnu_debug.d
CLEAN_COMPILE_release = $(TMP)dlb_pmd/os/linux/pmd_ctrl_c.pmd_unit_test_linux_amd64_gnu_release.d $(TMP)dlb_pmd/unit_test/dlb_pmd_capture_01.pmd_unit_test_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/unit_test/dlb_pmd_capture_02.pmd_unit_test_linux_amd64_gnu_release.d $(TMP)dlb_pmd/unit_test/dlb_pmd_pcm_01.pmd_unit_test_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/unit_test/dlb_pmd_sadm_01.pmd_unit_test_linux_amd64_gnu_release.d $(TMP)dlb_pmd/unit_test/dlb_pmd_sadm_02.pmd_unit_test_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/unit_test/pmd_unit_test.pmd_unit_test_linux_amd64_gnu_release.d

cleandeps:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)

clean: cleanself
	$(MAKE) -C $(BASE)dlb_pmd/make/dlb_pmd_lib/linux_amd64_gnu cleanself
	$(MAKE) -C $(BASE)dlb_pmd/make/dlb_wave_lib/linux_amd64_gnu cleanself
	$(MAKE) -C $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_amd64_gnu cleanself
	$(MAKE) -C $(BASE)googletest/1.8.0/make/googletest/linux_amd64_gnu cleanself
	$(MAKE) -C $(BASE)zlib/1.2.11/make/zlib/linux_amd64_gnu cleanself

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

include $(wildcard $(TMP)dlb_pmd/os/linux/pmd_ctrl_c.pmd_unit_test_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/unit_test/dlb_pmd_capture_01.pmd_unit_test_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/unit_test/dlb_pmd_capture_02.pmd_unit_test_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/unit_test/dlb_pmd_pcm_01.pmd_unit_test_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/unit_test/dlb_pmd_sadm_01.pmd_unit_test_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/unit_test/dlb_pmd_sadm_02.pmd_unit_test_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/unit_test/pmd_unit_test.pmd_unit_test_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/os/linux/pmd_ctrl_c.pmd_unit_test_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/unit_test/dlb_pmd_capture_01.pmd_unit_test_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/unit_test/dlb_pmd_capture_02.pmd_unit_test_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/unit_test/dlb_pmd_pcm_01.pmd_unit_test_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/unit_test/dlb_pmd_sadm_01.pmd_unit_test_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/unit_test/dlb_pmd_sadm_02.pmd_unit_test_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/unit_test/pmd_unit_test.pmd_unit_test_linux_amd64_gnu_release.d)

$(BASE)dlb_pmd/make/dlb_pmd_lib/linux_amd64_gnu/dlb_pmd_lib_debug.a \
$(BASE)dlb_pmd/make/dlb_pmd_lib/linux_amd64_gnu/dlb_pmd_lib_release.a \
$(BASE)dlb_pmd/make/dlb_wave_lib/linux_amd64_gnu/dlb_wave_lib_debug.a \
$(BASE)dlb_pmd/make/dlb_wave_lib/linux_amd64_gnu/dlb_wave_lib_release.a \
$(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_amd64_gnu/dlb_xml_debug.a \
$(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_amd64_gnu/dlb_xml_release.a \
$(BASE)googletest/1.8.0/make/googletest/linux_amd64_gnu/googletest_debug.a \
$(BASE)googletest/1.8.0/make/googletest/linux_amd64_gnu/googletest_release.a \
$(BASE)zlib/1.2.11/make/zlib/linux_amd64_gnu/zlib_debug.a \
$(BASE)zlib/1.2.11/make/zlib/linux_amd64_gnu/zlib_release.a: %.a: FORCE
	$(MAKE) -C $(@D) $(@F)


