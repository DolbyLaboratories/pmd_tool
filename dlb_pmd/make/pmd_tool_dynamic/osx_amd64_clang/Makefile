#-*-makefile-*-

BASE=../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
CCDEP = $(CC)
CFLAGS_debug = -g -O0 -arch x86_64 -fPIC -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wno-overlength-strings -Wdeclaration-after-statement -Wconditional-uninitialized
CFLAGS_release = -O3 -arch x86_64 -fPIC -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wno-overlength-strings -Wdeclaration-after-statement -Wconditional-uninitialized
DEFINES_debug = -DELPP_NO_DEFAULT_LOG_FILE=1
DEFINES_release = -DELPP_NO_DEFAULT_LOG_FILE=1 -DNDEBUG=1
INCLUDES = -I$(BASE)dlb_pmd/frontend/pmd_tool -I$(BASE).
LD = $(CXX)
LDFLAGS_debug = -arch x86_64 -dead_strip
LDFLAGS_release = -arch x86_64 -dead_strip -O2

COMMON_FILES = $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool_main

INPUTS_COMMON_LINK_debug = $(addsuffix .pmd_tool_dynamic_osx_amd64_clang_debug.o,$(COMMON_FILES))
INPUTS_COMMON_LINK_release = $(addsuffix .pmd_tool_dynamic_osx_amd64_clang_release.o,$(COMMON_FILES))

DEP_PROJ_LINK_debug = $(BASE)dlb_pmd/make/dlb_pmd_tool_lib_dynamic/osx_amd64_clang/dlb_pmd_tool_lib_dynamic_debug.dylib
DEP_PROJ_LINK_release = $(BASE)dlb_pmd/make/dlb_pmd_tool_lib_dynamic/osx_amd64_clang/dlb_pmd_tool_lib_dynamic_release.dylib

default: pmd_tool_dynamic_debug pmd_tool_dynamic_release

pmd_tool_dynamic_debug: $(INPUTS_COMMON_LINK_debug) $(DEP_PROJ_LINK_debug)
	$(LD) $(LDFLAGS_debug) -o $@ $+ 

pmd_tool_dynamic_release: $(INPUTS_COMMON_LINK_release) $(DEP_PROJ_LINK_release)
	$(LD) $(LDFLAGS_release) -o $@ $+ 

$(TMP)%.pmd_tool_dynamic_osx_amd64_clang_debug.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -arch x86_64 -MM $(DEFINES_debug) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_debug) $(DEFINES_debug) $(INCLUDES) -o $@ $<

$(TMP)%.pmd_tool_dynamic_osx_amd64_clang_release.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -arch x86_64 -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMMON_COMPILE_debug = $(addsuffix .pmd_tool_dynamic_osx_amd64_clang_debug.d,$(COMMON_FILES))
CLEAN_COMMON_COMPILE_release = $(addsuffix .pmd_tool_dynamic_osx_amd64_clang_release.d,$(COMMON_FILES))

cleandeps:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)

clean: cleanself
	$(MAKE) -C $(BASE)dlb_pmd/make/dlb_pmd_tool_lib_dynamic/osx_amd64_clang cleanself

cleanself:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)
	$(RM) $(INPUTS_COMMON_LINK_debug)
	$(RM) $(INPUTS_COMMON_LINK_release)
	$(RM) pmd_tool_dynamic_debug
	$(RM) pmd_tool_dynamic_release

.PHONY: default FORCE clean cleandeps cleanself help

FORCE:

help:
	@echo "This makefile has the following targets:"
	@echo " pmd_tool_dynamic_debug"
	@echo " pmd_tool_dynamic_release"
	@echo " default"
	@echo " clean"
	@echo " cleandeps"
	@echo " help"

include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool_main.pmd_tool_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool_main.pmd_tool_dynamic_osx_amd64_clang_release.d)

$(BASE)dlb_pmd/make/dlb_pmd_tool_lib_dynamic/osx_amd64_clang/dlb_pmd_tool_lib_dynamic_debug.dylib \
$(BASE)dlb_pmd/make/dlb_pmd_tool_lib_dynamic/osx_amd64_clang/dlb_pmd_tool_lib_dynamic_release.dylib: %.dylib: FORCE
	$(MAKE) -C $(@D) $(@F)


