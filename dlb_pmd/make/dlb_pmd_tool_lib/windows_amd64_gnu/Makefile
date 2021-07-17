#-*-makefile-*-

BASE=../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
ARFLAGS = -rc
CCDEP = $(CC)
CFLAGS_debug = -g -ggdb3 -O0 -m64 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
CFLAGS_release = -O3 -ftree-vectorize -m64 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
DEFINES_debug = -DELPP_NO_DEFAULT_LOG_FILE=1 -DWIN32=1 -DWIN64=1 -D_LIB=1
DEFINES_release = -DELPP_NO_DEFAULT_LOG_FILE=1 -DNDEBUG=1 -DWIN32=1 -DWIN64=1 -D_LIB=1
INCLUDES = -I$(BASE)dlb_buffer/1.3 -I$(BASE)dlb_octfile/1.1 -I$(BASE)dlb_pmd/frontend/pmd_tool -I$(BASE)dlb_pmd/frontend -I$(BASE)dlb_pmd/include -I$(BASE)dlb_wave/1.6 -I$(BASE). -I$(BASE).

COMMON_FILES = $(TMP)dlb_pmd/frontend/pmd_tool/pcm $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool_klv\
               $(TMP)dlb_pmd/frontend/pmd_tool/xml

INPUTS_COMMON_ARCHIVE_debug = $(addsuffix .dlb_pmd_tool_lib_windows_amd64_gnu_debug.o,$(COMMON_FILES))
INPUTS_COMMON_ARCHIVE_release = $(addsuffix .dlb_pmd_tool_lib_windows_amd64_gnu_release.o,$(COMMON_FILES))

DEP_PROJ_ARCHIVE_debug = $(BASE)dlb_pmd/make/dlb_pmd_lib/windows_amd64_gnu/dlb_pmd_lib_debug.lib $(BASE)dlb_adm/make/dlb_adm_lib/windows_amd64_gnu/dlb_adm_lib_debug.lib $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/windows_amd64_gnu/dlb_xml_debug.lib $(BASE)zlib/1.2.11/make/zlib/windows_amd64_gnu/zlib_debug.lib $(BASE)dlb_pmd/make/dlb_wave_lib/windows_amd64_gnu/dlb_wave_lib_debug.lib
DEP_PROJ_ARCHIVE_release = $(BASE)dlb_pmd/make/dlb_pmd_lib/windows_amd64_gnu/dlb_pmd_lib_release.lib $(BASE)dlb_adm/make/dlb_adm_lib/windows_amd64_gnu/dlb_adm_lib_release.lib $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/windows_amd64_gnu/dlb_xml_release.lib $(BASE)zlib/1.2.11/make/zlib/windows_amd64_gnu/zlib_release.lib $(BASE)dlb_pmd/make/dlb_wave_lib/windows_amd64_gnu/dlb_wave_lib_release.lib

default: dlb_pmd_tool_lib_debug.lib dlb_pmd_tool_lib_release.lib

dlb_pmd_tool_lib_debug.lib: $(INPUTS_COMMON_ARCHIVE_debug)
	$(AR) $(ARFLAGS)  $@ $^

dlb_pmd_tool_lib_release.lib: $(INPUTS_COMMON_ARCHIVE_release)
	$(AR) $(ARFLAGS)  $@ $^

$(TMP)%.dlb_pmd_tool_lib_windows_amd64_gnu_debug.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -MM $(DEFINES_debug) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_debug) $(DEFINES_debug) $(INCLUDES) -o $@ $<

$(TMP)%.dlb_pmd_tool_lib_windows_amd64_gnu_release.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMMON_COMPILE_debug = $(addsuffix .dlb_pmd_tool_lib_windows_amd64_gnu_debug.d,$(COMMON_FILES))
CLEAN_COMMON_COMPILE_release = $(addsuffix .dlb_pmd_tool_lib_windows_amd64_gnu_release.d,$(COMMON_FILES))

cleandeps:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)

clean: cleanself
	$(MAKE) -C $(BASE)dlb_adm/make/dlb_adm_lib/windows_amd64_gnu cleanself
	$(MAKE) -C $(BASE)dlb_pmd/make/dlb_pmd_lib/windows_amd64_gnu cleanself
	$(MAKE) -C $(BASE)dlb_pmd/make/dlb_wave_lib/windows_amd64_gnu cleanself
	$(MAKE) -C $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/windows_amd64_gnu cleanself
	$(MAKE) -C $(BASE)zlib/1.2.11/make/zlib/windows_amd64_gnu cleanself

cleanself:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)
	$(RM) $(INPUTS_COMMON_ARCHIVE_debug)
	$(RM) $(INPUTS_COMMON_ARCHIVE_release)
	$(RM) dlb_pmd_tool_lib_debug.lib
	$(RM) dlb_pmd_tool_lib_release.lib

.PHONY: default FORCE clean cleandeps cleanself help

FORCE:

help:
	@echo "This makefile has the following targets:"
	@echo " dlb_pmd_tool_lib_debug.lib"
	@echo " dlb_pmd_tool_lib_release.lib"
	@echo " default"
	@echo " clean"
	@echo " cleandeps"
	@echo " help"

include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pcm.dlb_pmd_tool_lib_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool.dlb_pmd_tool_lib_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool_klv.dlb_pmd_tool_lib_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/xml.dlb_pmd_tool_lib_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pcm.dlb_pmd_tool_lib_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool.dlb_pmd_tool_lib_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool_klv.dlb_pmd_tool_lib_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/xml.dlb_pmd_tool_lib_windows_amd64_gnu_release.d)

$(BASE)dlb_adm/make/dlb_adm_lib/windows_amd64_gnu/dlb_adm_lib_debug.lib \
$(BASE)dlb_adm/make/dlb_adm_lib/windows_amd64_gnu/dlb_adm_lib_release.lib \
$(BASE)dlb_pmd/make/dlb_pmd_lib/windows_amd64_gnu/dlb_pmd_lib_debug.lib \
$(BASE)dlb_pmd/make/dlb_pmd_lib/windows_amd64_gnu/dlb_pmd_lib_release.lib \
$(BASE)dlb_pmd/make/dlb_wave_lib/windows_amd64_gnu/dlb_wave_lib_debug.lib \
$(BASE)dlb_pmd/make/dlb_wave_lib/windows_amd64_gnu/dlb_wave_lib_release.lib \
$(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/windows_amd64_gnu/dlb_xml_debug.lib \
$(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/windows_amd64_gnu/dlb_xml_release.lib \
$(BASE)zlib/1.2.11/make/zlib/windows_amd64_gnu/zlib_debug.lib \
$(BASE)zlib/1.2.11/make/zlib/windows_amd64_gnu/zlib_release.lib: %.lib: FORCE
	$(MAKE) -C $(@D) $(@F)


