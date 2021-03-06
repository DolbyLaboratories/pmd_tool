#-*-makefile-*-

BASE=../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
ARFLAGS = -rc
CCDEP = $(CC)
CFLAGS_debug = -msse2 -g -ggdb3 -O0 -m32 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
CFLAGS_release = -msse2 -O3 -ftree-vectorize -m32 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
DEFINES_release = -DNDEBUG=1
INCLUDES = -I$(BASE)dlb_buffer/1.3 -I$(BASE)dlb_octfile/1.1 -I$(BASE)dlb_pmd/frontend/pmd_tool -I$(BASE)dlb_pmd/frontend -I$(BASE)dlb_pmd/include -I$(BASE)dlb_wave/1.6 -I$(BASE). -I$(BASE).

COMMON_FILES = $(TMP)dlb_pmd/frontend/pmd_tool/pcm $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool_klv\
               $(TMP)dlb_pmd/frontend/pmd_tool/xml

INPUTS_COMMON_ARCHIVE_debug = $(addsuffix .dlb_pmd_tool_lib_linux_x86_gnu_debug.o,$(COMMON_FILES))
INPUTS_COMMON_ARCHIVE_release = $(addsuffix .dlb_pmd_tool_lib_linux_x86_gnu_release.o,$(COMMON_FILES))

DEP_PROJ_ARCHIVE_debug = $(BASE)dlb_pmd/make/dlb_pmd_lib/linux_x86_gnu/dlb_pmd_lib_debug.a $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_x86_gnu/dlb_xml_debug.a $(BASE)zlib/1.2.11/make/zlib/linux_x86_gnu/zlib_debug.a $(BASE)dlb_pmd/make/dlb_wave_lib/linux_x86_gnu/dlb_wave_lib_debug.a
DEP_PROJ_ARCHIVE_release = $(BASE)dlb_pmd/make/dlb_pmd_lib/linux_x86_gnu/dlb_pmd_lib_release.a $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_x86_gnu/dlb_xml_release.a $(BASE)zlib/1.2.11/make/zlib/linux_x86_gnu/zlib_release.a $(BASE)dlb_pmd/make/dlb_wave_lib/linux_x86_gnu/dlb_wave_lib_release.a

default: dlb_pmd_tool_lib_debug.a dlb_pmd_tool_lib_release.a

dlb_pmd_tool_lib_debug.a: $(INPUTS_COMMON_ARCHIVE_debug)
	$(AR) $(ARFLAGS)  $@ $^

dlb_pmd_tool_lib_release.a: $(INPUTS_COMMON_ARCHIVE_release)
	$(AR) $(ARFLAGS)  $@ $^

$(TMP)%.dlb_pmd_tool_lib_linux_x86_gnu_debug.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -msse2 -MM $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_debug) $(INCLUDES) -o $@ $<

$(TMP)%.dlb_pmd_tool_lib_linux_x86_gnu_release.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -msse2 -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMMON_COMPILE_debug = $(addsuffix .dlb_pmd_tool_lib_linux_x86_gnu_debug.d,$(COMMON_FILES))
CLEAN_COMMON_COMPILE_release = $(addsuffix .dlb_pmd_tool_lib_linux_x86_gnu_release.d,$(COMMON_FILES))

cleandeps:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)

clean: cleanself
	$(MAKE) -C $(BASE)dlb_pmd/make/dlb_pmd_lib/linux_x86_gnu cleanself
	$(MAKE) -C $(BASE)dlb_pmd/make/dlb_wave_lib/linux_x86_gnu cleanself
	$(MAKE) -C $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_x86_gnu cleanself
	$(MAKE) -C $(BASE)zlib/1.2.11/make/zlib/linux_x86_gnu cleanself

cleanself:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)
	$(RM) $(INPUTS_COMMON_ARCHIVE_debug)
	$(RM) $(INPUTS_COMMON_ARCHIVE_release)
	$(RM) dlb_pmd_tool_lib_debug.a
	$(RM) dlb_pmd_tool_lib_release.a

.PHONY: default FORCE clean cleandeps cleanself help

FORCE:

help:
	@echo "This makefile has the following targets:"
	@echo " dlb_pmd_tool_lib_debug.a"
	@echo " dlb_pmd_tool_lib_release.a"
	@echo " default"
	@echo " clean"
	@echo " cleandeps"
	@echo " help"

include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pcm.dlb_pmd_tool_lib_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool.dlb_pmd_tool_lib_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool_klv.dlb_pmd_tool_lib_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/xml.dlb_pmd_tool_lib_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pcm.dlb_pmd_tool_lib_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool.dlb_pmd_tool_lib_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool_klv.dlb_pmd_tool_lib_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/xml.dlb_pmd_tool_lib_linux_x86_gnu_release.d)

$(BASE)dlb_pmd/make/dlb_pmd_lib/linux_x86_gnu/dlb_pmd_lib_debug.a \
$(BASE)dlb_pmd/make/dlb_pmd_lib/linux_x86_gnu/dlb_pmd_lib_release.a \
$(BASE)dlb_pmd/make/dlb_wave_lib/linux_x86_gnu/dlb_wave_lib_debug.a \
$(BASE)dlb_pmd/make/dlb_wave_lib/linux_x86_gnu/dlb_wave_lib_release.a \
$(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_x86_gnu/dlb_xml_debug.a \
$(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_x86_gnu/dlb_xml_release.a \
$(BASE)zlib/1.2.11/make/zlib/linux_x86_gnu/zlib_debug.a \
$(BASE)zlib/1.2.11/make/zlib/linux_x86_gnu/zlib_release.a: %.a: FORCE
	$(MAKE) -C $(@D) $(@F)


