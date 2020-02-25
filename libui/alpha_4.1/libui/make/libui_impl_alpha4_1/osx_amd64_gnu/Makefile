#-*-makefile-*-

BASE=../../../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
ARFLAGS = -rc
CC = gcc
CCDEP = $(CC)
CFLAGS_debug = -g -ggdb3 -O0 -m64 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
CFLAGS_release = -O3 -ftree-vectorize -m64 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
DEFINES_release = -DNDEBUG=1
INCLUDES = -I$(BASE)libui/alpha_4.1/libui/darwin -I$(BASE)libui/alpha_4.1/libui/common -I$(BASE)libui/alpha_4.1/libui

COMMON_FILES = $(TMP)libui/alpha_4.1/libui/common/areaevents $(TMP)libui/alpha_4.1/libui/common/attrlist $(TMP)libui/alpha_4.1/libui/common/attrstr\
               $(TMP)libui/alpha_4.1/libui/common/control $(TMP)libui/alpha_4.1/libui/common/matrix $(TMP)libui/alpha_4.1/libui/common/opentype\
               $(TMP)libui/alpha_4.1/libui/common/tablemodel $(TMP)libui/alpha_4.1/libui/common/userbugs $(TMP)libui/alpha_4.1/libui/common/utf

INPUTS_COMMON_ARCHIVE_debug = $(addsuffix .libui_impl_alpha4_1_osx_amd64_gnu_debug.o,$(COMMON_FILES))
INPUTS_COMMON_ARCHIVE_release = $(addsuffix .libui_impl_alpha4_1_osx_amd64_gnu_release.o,$(COMMON_FILES))


default: libui_impl_alpha4_1_debug.a libui_impl_alpha4_1_release.a

libui_impl_alpha4_1_debug.a: $(INPUTS_COMMON_ARCHIVE_debug)
	$(AR) $(ARFLAGS)  $@ $^

libui_impl_alpha4_1_release.a: $(INPUTS_COMMON_ARCHIVE_release)
	$(AR) $(ARFLAGS)  $@ $^

$(TMP)%.libui_impl_alpha4_1_osx_amd64_gnu_debug.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -MM $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_debug) $(INCLUDES) -o $@ $<

$(TMP)%.libui_impl_alpha4_1_osx_amd64_gnu_release.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMMON_COMPILE_debug = $(addsuffix .libui_impl_alpha4_1_osx_amd64_gnu_debug.d,$(COMMON_FILES))
CLEAN_COMMON_COMPILE_release = $(addsuffix .libui_impl_alpha4_1_osx_amd64_gnu_release.d,$(COMMON_FILES))

cleandeps:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)

clean: cleanself

cleanself:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)
	$(RM) $(INPUTS_COMMON_ARCHIVE_debug)
	$(RM) $(INPUTS_COMMON_ARCHIVE_release)
	$(RM) libui_impl_alpha4_1_debug.a
	$(RM) libui_impl_alpha4_1_release.a

.PHONY: default FORCE clean cleandeps cleanself help

FORCE:

help:
	@echo "This makefile has the following targets:"
	@echo " libui_impl_alpha4_1_debug.a"
	@echo " libui_impl_alpha4_1_release.a"
	@echo " default"
	@echo " clean"
	@echo " cleandeps"
	@echo " help"

include $(wildcard $(TMP)libui/alpha_4.1/libui/common/areaevents.libui_impl_alpha4_1_osx_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/attrlist.libui_impl_alpha4_1_osx_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/attrstr.libui_impl_alpha4_1_osx_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/control.libui_impl_alpha4_1_osx_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/matrix.libui_impl_alpha4_1_osx_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/opentype.libui_impl_alpha4_1_osx_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/tablemodel.libui_impl_alpha4_1_osx_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/userbugs.libui_impl_alpha4_1_osx_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/utf.libui_impl_alpha4_1_osx_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/areaevents.libui_impl_alpha4_1_osx_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/attrlist.libui_impl_alpha4_1_osx_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/attrstr.libui_impl_alpha4_1_osx_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/control.libui_impl_alpha4_1_osx_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/matrix.libui_impl_alpha4_1_osx_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/opentype.libui_impl_alpha4_1_osx_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/tablemodel.libui_impl_alpha4_1_osx_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/userbugs.libui_impl_alpha4_1_osx_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/utf.libui_impl_alpha4_1_osx_amd64_gnu_release.d)


