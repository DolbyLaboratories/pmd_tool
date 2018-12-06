#-*-makefile-*-

BASE=../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
CCDEP = $(CC)
CFLAGS_debug = -g -ggdb3 -O0 -m64 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
CFLAGS_release = -O3 -ftree-vectorize -m64 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
DEFINES_release = -DNDEBUG=1
INCLUDES = -I$(BASE)dlb_bitbuf/2.0 -I$(BASE)dlb_buffer/1.3 -I$(BASE)dlb_octfile/1.1 -I$(BASE)dlb_pmd/frontend -I$(BASE)dlb_pmd -I$(BASE)dlb_pmd/include -I$(BASE)dlb_wave/1.6 -I$(BASE). -I$(BASE).
LD = $(CC)
LDFLAGS_debug = -m64
LDFLAGS_release = -O2 -m64
LDLIBS = -lpthread -lm

COMMON_FILES = $(TMP)dlb_pmd/frontend/klv $(TMP)dlb_pmd/frontend/pcm $(TMP)dlb_pmd/frontend/pmd_tool $(TMP)dlb_pmd/frontend/xml

INPUTS_COMMON_LINK_debug = $(addsuffix .pmd_tool_linux_amd64_gnu_debug.o,$(COMMON_FILES))
INPUTS_COMMON_LINK_release = $(addsuffix .pmd_tool_linux_amd64_gnu_release.o,$(COMMON_FILES))

DEP_PROJ_LINK_debug = $(BASE)dlb_pmd/make/dlb_pmd_lib/linux_amd64_gnu/dlb_pmd_lib_debug.a $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_amd64_gnu/dlb_xml_debug.a $(BASE)dlb_pmd/make/dlb_wave_lib/linux_amd64_gnu/dlb_wave_lib_debug.a
DEP_PROJ_LINK_release = $(BASE)dlb_pmd/make/dlb_pmd_lib/linux_amd64_gnu/dlb_pmd_lib_release.a $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_amd64_gnu/dlb_xml_release.a $(BASE)dlb_pmd/make/dlb_wave_lib/linux_amd64_gnu/dlb_wave_lib_release.a

default: pmd_tool_debug pmd_tool_release

pmd_tool_debug: $(INPUTS_COMMON_LINK_debug) $(DEP_PROJ_LINK_debug)
	$(LD) $(LDFLAGS_debug) -o $@ $+ $(LDLIBS)

pmd_tool_release: $(INPUTS_COMMON_LINK_release) $(DEP_PROJ_LINK_release)
	$(LD) $(LDFLAGS_release) -o $@ $+ $(LDLIBS)

$(TMP)%.pmd_tool_linux_amd64_gnu_debug.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -MM $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_debug) $(INCLUDES) -o $@ $<

$(TMP)%.pmd_tool_linux_amd64_gnu_release.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMMON_COMPILE_debug = $(addsuffix .pmd_tool_linux_amd64_gnu_debug.d,$(COMMON_FILES))
CLEAN_COMMON_COMPILE_release = $(addsuffix .pmd_tool_linux_amd64_gnu_release.d,$(COMMON_FILES))

cleandeps:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)

clean: cleanself
	$(MAKE) -C $(BASE)dlb_pmd/make/dlb_pmd_lib/linux_amd64_gnu cleanself
	$(MAKE) -C $(BASE)dlb_pmd/make/dlb_wave_lib/linux_amd64_gnu cleanself
	$(MAKE) -C $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_amd64_gnu cleanself

cleanself:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)
	$(RM) $(INPUTS_COMMON_LINK_debug)
	$(RM) $(INPUTS_COMMON_LINK_release)
	$(RM) pmd_tool_debug
	$(RM) pmd_tool_release

.PHONY: default FORCE clean cleandeps cleanself help

FORCE:

help:
	@echo "This makefile has the following targets:"
	@echo " pmd_tool_debug"
	@echo " pmd_tool_release"
	@echo " default"
	@echo " clean"
	@echo " cleandeps"
	@echo " help"

include $(wildcard $(TMP)dlb_pmd/frontend/klv.pmd_tool_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pcm.pmd_tool_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool.pmd_tool_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/xml.pmd_tool_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/klv.pmd_tool_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pcm.pmd_tool_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool.pmd_tool_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/xml.pmd_tool_linux_amd64_gnu_release.d)

$(BASE)dlb_pmd/make/dlb_pmd_lib/linux_amd64_gnu/dlb_pmd_lib_debug.a \
$(BASE)dlb_pmd/make/dlb_pmd_lib/linux_amd64_gnu/dlb_pmd_lib_release.a \
$(BASE)dlb_pmd/make/dlb_wave_lib/linux_amd64_gnu/dlb_wave_lib_debug.a \
$(BASE)dlb_pmd/make/dlb_wave_lib/linux_amd64_gnu/dlb_wave_lib_release.a \
$(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_amd64_gnu/dlb_xml_debug.a \
$(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_amd64_gnu/dlb_xml_release.a: %.a: FORCE
	$(MAKE) -C $(@D) $(@F)


