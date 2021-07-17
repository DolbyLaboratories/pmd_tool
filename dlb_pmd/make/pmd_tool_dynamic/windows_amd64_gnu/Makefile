#-*-makefile-*-

BASE=../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
CCDEP = $(CC)
CFLAGS_debug = -g -ggdb3 -O0 -m64 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
CFLAGS_release = -O3 -ftree-vectorize -m64 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
DEFINES_debug = -DELPP_NO_DEFAULT_LOG_FILE=1 -DWIN32=1 -DWIN64=1 -D_CONSOLE=1
DEFINES_release = -DELPP_NO_DEFAULT_LOG_FILE=1 -DNDEBUG=1 -DWIN32=1 -DWIN64=1 -D_CONSOLE=1
INCLUDES = -I$(BASE)dlb_pmd/frontend/pmd_tool -I$(BASE).
LD = $(CXX)
LDFLAGS_debug = -m64
LDFLAGS_release = -O2 -m64

COMMON_FILES = $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool_main

INPUTS_COMMON_LINK_debug = $(addsuffix .pmd_tool_dynamic_windows_amd64_gnu_debug.o,$(COMMON_FILES))
INPUTS_COMMON_LINK_release = $(addsuffix .pmd_tool_dynamic_windows_amd64_gnu_release.o,$(COMMON_FILES))

DEP_PROJ_LINK_debug = $(BASE)dlb_pmd/make/dlb_pmd_tool_lib_dynamic/windows_amd64_gnu/dlb_pmd_tool_lib_dynamic_debug.dll
DEP_PROJ_LINK_release = $(BASE)dlb_pmd/make/dlb_pmd_tool_lib_dynamic/windows_amd64_gnu/dlb_pmd_tool_lib_dynamic_release.dll

default: pmd_tool_dynamic_debug.exe pmd_tool_dynamic_release.exe

pmd_tool_dynamic_debug.exe: $(INPUTS_COMMON_LINK_debug) $(DEP_PROJ_LINK_debug)
	$(LD) $(LDFLAGS_debug) -o $@ $+ 

pmd_tool_dynamic_release.exe: $(INPUTS_COMMON_LINK_release) $(DEP_PROJ_LINK_release)
	$(LD) $(LDFLAGS_release) -o $@ $+ 

$(TMP)%.pmd_tool_dynamic_windows_amd64_gnu_debug.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -MM $(DEFINES_debug) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_debug) $(DEFINES_debug) $(INCLUDES) -o $@ $<

$(TMP)%.pmd_tool_dynamic_windows_amd64_gnu_release.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMMON_COMPILE_debug = $(addsuffix .pmd_tool_dynamic_windows_amd64_gnu_debug.d,$(COMMON_FILES))
CLEAN_COMMON_COMPILE_release = $(addsuffix .pmd_tool_dynamic_windows_amd64_gnu_release.d,$(COMMON_FILES))

cleandeps:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)

clean: cleanself
	$(MAKE) -C $(BASE)dlb_pmd/make/dlb_pmd_tool_lib_dynamic/windows_amd64_gnu cleanself

cleanself:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)
	$(RM) $(INPUTS_COMMON_LINK_debug)
	$(RM) $(INPUTS_COMMON_LINK_release)
	$(RM) pmd_tool_dynamic_debug.exe
	$(RM) pmd_tool_dynamic_release.exe

.PHONY: default FORCE clean cleandeps cleanself help

FORCE:

help:
	@echo "This makefile has the following targets:"
	@echo " pmd_tool_dynamic_debug.exe"
	@echo " pmd_tool_dynamic_release.exe"
	@echo " default"
	@echo " clean"
	@echo " cleandeps"
	@echo " help"

include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool_main.pmd_tool_dynamic_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool_main.pmd_tool_dynamic_windows_amd64_gnu_release.d)

$(BASE)dlb_pmd/make/dlb_pmd_tool_lib_dynamic/windows_amd64_gnu/dlb_pmd_tool_lib_dynamic_debug.dll \
$(BASE)dlb_pmd/make/dlb_pmd_tool_lib_dynamic/windows_amd64_gnu/dlb_pmd_tool_lib_dynamic_release.dll: %.dll: FORCE
	$(MAKE) -C $(@D) $(@F)


