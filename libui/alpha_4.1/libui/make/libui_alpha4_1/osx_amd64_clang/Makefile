#-*-makefile-*-

BASE=../../../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
ARFLAGS = -rc -s
CCDEP = $(CC)
CFLAGS_debug = -g -O0 -arch x86_64 -fPIC -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wno-overlength-strings -Wdeclaration-after-statement -Wconditional-uninitialized
CFLAGS_release = -O3 -arch x86_64 -fPIC -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wno-overlength-strings -Wdeclaration-after-statement -Wconditional-uninitialized
DEFINES_release = -DNDEBUG=1
INCLUDES = -I$(BASE)libui/alpha_4.1/libui/common -I$(BASE)libui/alpha_4.1/libui

COMMON_FILES = $(TMP)libui/alpha_4.1/libui/common/attribute $(TMP)libui/alpha_4.1/libui/common/debug $(TMP)libui/alpha_4.1/libui/common/shouldquit\
               $(TMP)libui/alpha_4.1/libui/common/tablevalue

INPUTS_COMMON_ARCHIVE_debug = $(addsuffix .libui_alpha4_1_osx_amd64_clang_debug.o,$(COMMON_FILES))
INPUTS_COMMON_ARCHIVE_release = $(addsuffix .libui_alpha4_1_osx_amd64_clang_release.o,$(COMMON_FILES))

DEP_PROJ_ARCHIVE_debug = $(BASE)libui/alpha_4.1/libui/make/libui_impl_alpha4_1/osx_amd64_clang/libui_impl_alpha4_1_debug.a
DEP_PROJ_ARCHIVE_release = $(BASE)libui/alpha_4.1/libui/make/libui_impl_alpha4_1/osx_amd64_clang/libui_impl_alpha4_1_release.a

default: libui_alpha4_1_debug.a libui_alpha4_1_release.a

libui_alpha4_1_debug.a: $(INPUTS_COMMON_ARCHIVE_debug)
	$(AR) $(ARFLAGS)   $@ $^

libui_alpha4_1_release.a: $(INPUTS_COMMON_ARCHIVE_release)
	$(AR) $(ARFLAGS)   $@ $^

$(TMP)%.libui_alpha4_1_osx_amd64_clang_debug.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -arch x86_64 -MM $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_debug) $(INCLUDES) -o $@ $<

$(TMP)%.libui_alpha4_1_osx_amd64_clang_release.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -arch x86_64 -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMMON_COMPILE_debug = $(addsuffix .libui_alpha4_1_osx_amd64_clang_debug.d,$(COMMON_FILES))
CLEAN_COMMON_COMPILE_release = $(addsuffix .libui_alpha4_1_osx_amd64_clang_release.d,$(COMMON_FILES))

cleandeps:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)

clean: cleanself
	$(MAKE) -C $(BASE)libui/alpha_4.1/libui/make/libui_impl_alpha4_1/osx_amd64_clang cleanself

cleanself:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)
	$(RM) $(INPUTS_COMMON_ARCHIVE_debug)
	$(RM) $(INPUTS_COMMON_ARCHIVE_release)
	$(RM) libui_alpha4_1_debug.a
	$(RM) libui_alpha4_1_release.a

.PHONY: default FORCE clean cleandeps cleanself help

FORCE:

help:
	@echo "This makefile has the following targets:"
	@echo " libui_alpha4_1_debug.a"
	@echo " libui_alpha4_1_release.a"
	@echo " default"
	@echo " clean"
	@echo " cleandeps"
	@echo " help"

include $(wildcard $(TMP)libui/alpha_4.1/libui/common/attribute.libui_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/debug.libui_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/shouldquit.libui_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/tablevalue.libui_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/attribute.libui_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/debug.libui_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/shouldquit.libui_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/tablevalue.libui_alpha4_1_osx_amd64_clang_release.d)

$(BASE)libui/alpha_4.1/libui/make/libui_impl_alpha4_1/osx_amd64_clang/libui_impl_alpha4_1_debug.a \
$(BASE)libui/alpha_4.1/libui/make/libui_impl_alpha4_1/osx_amd64_clang/libui_impl_alpha4_1_release.a: %.a: FORCE
	$(MAKE) -C $(@D) $(@F)


