#-*-makefile-*-

BASE=../../../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
ARFLAGS = -rc
CCDEP = $(CC)
CFLAGS_debug = -g -ggdb3 -O0 -m64 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
CFLAGS_release = -O3 -ftree-vectorize -m64 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
CXXDEP = $(CXX)
CXXFLAGS_debug = -std=c++17 -g -ggdb3 -O0 -m64 -c -Wall
CXXFLAGS_release = -std=c++17 -O3 -ftree-vectorize -m64 -c -Wall
DEFINES_debug = -DELPP_NO_DEFAULT_LOG_FILE=1 -DWIN32=1 -DWIN64=1 -D_LIB=1
DEFINES_release = -DELPP_NO_DEFAULT_LOG_FILE=1 -DNDEBUG=1 -DWIN32=1 -DWIN64=1 -D_LIB=1
INCLUDES = -I$(BASE)libui/alpha_4.1/libui/windows -I$(BASE)libui/alpha_4.1/libui/common -I$(BASE)libui/alpha_4.1/libui


INPUTS_ARCHIVE_debug = $(TMP)libui/alpha_4.1/libui/common/attribute.libui_alpha4_1_windows_amd64_gnu_debug.o $(TMP)libui/alpha_4.1/libui/common/debug.libui_alpha4_1_windows_amd64_gnu_debug.o\
                       $(TMP)libui/alpha_4.1/libui/common/shouldquit.libui_alpha4_1_windows_amd64_gnu_debug.o $(TMP)libui/alpha_4.1/libui/common/tablevalue.libui_alpha4_1_windows_amd64_gnu_debug.o\
                       $(TMP)libui/alpha_4.1/libui/windows/area.libui_alpha4_1_windows_amd64_gnu_debug.o $(TMP)libui/alpha_4.1/libui/windows/areaevents.libui_alpha4_1_windows_amd64_gnu_debug.o\
                       $(TMP)libui/alpha_4.1/libui/windows/attrstr.libui_alpha4_1_windows_amd64_gnu_debug.o $(TMP)libui/alpha_4.1/libui/windows/control.libui_alpha4_1_windows_amd64_gnu_debug.o\
                       $(TMP)libui/alpha_4.1/libui/windows/drawtext.libui_alpha4_1_windows_amd64_gnu_debug.o $(TMP)libui/alpha_4.1/libui/windows/fontbutton.libui_alpha4_1_windows_amd64_gnu_debug.o\
                       $(TMP)libui/alpha_4.1/libui/windows/fontmatch.libui_alpha4_1_windows_amd64_gnu_debug.o $(TMP)libui/alpha_4.1/libui/windows/image.libui_alpha4_1_windows_amd64_gnu_debug.o\
                       $(TMP)libui/alpha_4.1/libui/windows/init.libui_alpha4_1_windows_amd64_gnu_debug.o $(TMP)libui/alpha_4.1/libui/windows/opentype.libui_alpha4_1_windows_amd64_gnu_debug.o
INPUTS_ARCHIVE_release = $(TMP)libui/alpha_4.1/libui/common/attribute.libui_alpha4_1_windows_amd64_gnu_release.o $(TMP)libui/alpha_4.1/libui/common/debug.libui_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/common/shouldquit.libui_alpha4_1_windows_amd64_gnu_release.o $(TMP)libui/alpha_4.1/libui/common/tablevalue.libui_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/windows/area.libui_alpha4_1_windows_amd64_gnu_release.o $(TMP)libui/alpha_4.1/libui/windows/areaevents.libui_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/windows/attrstr.libui_alpha4_1_windows_amd64_gnu_release.o $(TMP)libui/alpha_4.1/libui/windows/control.libui_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/windows/drawtext.libui_alpha4_1_windows_amd64_gnu_release.o $(TMP)libui/alpha_4.1/libui/windows/fontbutton.libui_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/windows/fontmatch.libui_alpha4_1_windows_amd64_gnu_release.o $(TMP)libui/alpha_4.1/libui/windows/image.libui_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/windows/init.libui_alpha4_1_windows_amd64_gnu_release.o $(TMP)libui/alpha_4.1/libui/windows/opentype.libui_alpha4_1_windows_amd64_gnu_release.o

DEP_PROJ_ARCHIVE_debug = $(BASE)libui/alpha_4.1/libui/make/libui_impl_alpha4_1/windows_amd64_gnu/libui_impl_alpha4_1_debug.lib
DEP_PROJ_ARCHIVE_release = $(BASE)libui/alpha_4.1/libui/make/libui_impl_alpha4_1/windows_amd64_gnu/libui_impl_alpha4_1_release.lib

default: libui_alpha4_1_debug.lib libui_alpha4_1_release.lib

libui_alpha4_1_debug.lib: $(INPUTS_ARCHIVE_debug)
	$(AR) $(ARFLAGS)  $@ $^

libui_alpha4_1_release.lib: $(INPUTS_ARCHIVE_release)
	$(AR) $(ARFLAGS)  $@ $^

$(TMP)%.libui_alpha4_1_windows_amd64_gnu_debug.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -MM $(DEFINES_debug) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_debug) $(DEFINES_debug) $(INCLUDES) -o $@ $<

$(TMP)%.libui_alpha4_1_windows_amd64_gnu_debug.o: $(BASE)%.cpp | $(TMP)%/..
	$(CXXDEP) -MM $(DEFINES_debug) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS_debug) $(DEFINES_debug) $(INCLUDES) -o $@ $<

$(TMP)%.libui_alpha4_1_windows_amd64_gnu_release.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

$(TMP)%.libui_alpha4_1_windows_amd64_gnu_release.o: $(BASE)%.cpp | $(TMP)%/..
	$(CXXDEP) -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMPILE_debug = $(TMP)libui/alpha_4.1/libui/common/attribute.libui_alpha4_1_windows_amd64_gnu_debug.d $(TMP)libui/alpha_4.1/libui/common/debug.libui_alpha4_1_windows_amd64_gnu_debug.d\
                      $(TMP)libui/alpha_4.1/libui/common/shouldquit.libui_alpha4_1_windows_amd64_gnu_debug.d $(TMP)libui/alpha_4.1/libui/common/tablevalue.libui_alpha4_1_windows_amd64_gnu_debug.d\
                      $(TMP)libui/alpha_4.1/libui/windows/area.libui_alpha4_1_windows_amd64_gnu_debug.d $(TMP)libui/alpha_4.1/libui/windows/areaevents.libui_alpha4_1_windows_amd64_gnu_debug.d\
                      $(TMP)libui/alpha_4.1/libui/windows/attrstr.libui_alpha4_1_windows_amd64_gnu_debug.d $(TMP)libui/alpha_4.1/libui/windows/control.libui_alpha4_1_windows_amd64_gnu_debug.d\
                      $(TMP)libui/alpha_4.1/libui/windows/drawtext.libui_alpha4_1_windows_amd64_gnu_debug.d $(TMP)libui/alpha_4.1/libui/windows/fontbutton.libui_alpha4_1_windows_amd64_gnu_debug.d\
                      $(TMP)libui/alpha_4.1/libui/windows/fontmatch.libui_alpha4_1_windows_amd64_gnu_debug.d $(TMP)libui/alpha_4.1/libui/windows/image.libui_alpha4_1_windows_amd64_gnu_debug.d\
                      $(TMP)libui/alpha_4.1/libui/windows/init.libui_alpha4_1_windows_amd64_gnu_debug.d $(TMP)libui/alpha_4.1/libui/windows/opentype.libui_alpha4_1_windows_amd64_gnu_debug.d
CLEAN_COMPILE_release = $(TMP)libui/alpha_4.1/libui/common/attribute.libui_alpha4_1_windows_amd64_gnu_release.d $(TMP)libui/alpha_4.1/libui/common/debug.libui_alpha4_1_windows_amd64_gnu_release.d\
                        $(TMP)libui/alpha_4.1/libui/common/shouldquit.libui_alpha4_1_windows_amd64_gnu_release.d $(TMP)libui/alpha_4.1/libui/common/tablevalue.libui_alpha4_1_windows_amd64_gnu_release.d\
                        $(TMP)libui/alpha_4.1/libui/windows/area.libui_alpha4_1_windows_amd64_gnu_release.d $(TMP)libui/alpha_4.1/libui/windows/areaevents.libui_alpha4_1_windows_amd64_gnu_release.d\
                        $(TMP)libui/alpha_4.1/libui/windows/attrstr.libui_alpha4_1_windows_amd64_gnu_release.d $(TMP)libui/alpha_4.1/libui/windows/control.libui_alpha4_1_windows_amd64_gnu_release.d\
                        $(TMP)libui/alpha_4.1/libui/windows/drawtext.libui_alpha4_1_windows_amd64_gnu_release.d $(TMP)libui/alpha_4.1/libui/windows/fontbutton.libui_alpha4_1_windows_amd64_gnu_release.d\
                        $(TMP)libui/alpha_4.1/libui/windows/fontmatch.libui_alpha4_1_windows_amd64_gnu_release.d $(TMP)libui/alpha_4.1/libui/windows/image.libui_alpha4_1_windows_amd64_gnu_release.d\
                        $(TMP)libui/alpha_4.1/libui/windows/init.libui_alpha4_1_windows_amd64_gnu_release.d $(TMP)libui/alpha_4.1/libui/windows/opentype.libui_alpha4_1_windows_amd64_gnu_release.d

cleandeps:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)

clean: cleanself
	$(MAKE) -C $(BASE)libui/alpha_4.1/libui/make/libui_impl_alpha4_1/windows_amd64_gnu cleanself

cleanself:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)
	$(RM) $(INPUTS_ARCHIVE_debug)
	$(RM) $(INPUTS_ARCHIVE_release)
	$(RM) libui_alpha4_1_debug.lib
	$(RM) libui_alpha4_1_release.lib

.PHONY: default FORCE clean cleandeps cleanself help

FORCE:

help:
	@echo "This makefile has the following targets:"
	@echo " libui_alpha4_1_debug.lib"
	@echo " libui_alpha4_1_release.lib"
	@echo " default"
	@echo " clean"
	@echo " cleandeps"
	@echo " help"

include $(wildcard $(TMP)libui/alpha_4.1/libui/common/attribute.libui_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/debug.libui_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/shouldquit.libui_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/tablevalue.libui_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/area.libui_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/areaevents.libui_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/attrstr.libui_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/control.libui_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/drawtext.libui_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/fontbutton.libui_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/fontmatch.libui_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/image.libui_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/init.libui_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/opentype.libui_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/attribute.libui_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/debug.libui_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/shouldquit.libui_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/tablevalue.libui_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/area.libui_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/areaevents.libui_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/attrstr.libui_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/control.libui_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/drawtext.libui_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/fontbutton.libui_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/fontmatch.libui_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/image.libui_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/init.libui_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/opentype.libui_alpha4_1_windows_amd64_gnu_release.d)

$(BASE)libui/alpha_4.1/libui/make/libui_impl_alpha4_1/windows_amd64_gnu/libui_impl_alpha4_1_debug.lib \
$(BASE)libui/alpha_4.1/libui/make/libui_impl_alpha4_1/windows_amd64_gnu/libui_impl_alpha4_1_release.lib: %.lib: FORCE
	$(MAKE) -C $(@D) $(@F)


