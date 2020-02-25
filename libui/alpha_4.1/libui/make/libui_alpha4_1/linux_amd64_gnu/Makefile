#-*-makefile-*-

BASE=../../../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
ARFLAGS = -rc
CCDEP = $(CC)
CFLAGS_debug = -std=c99 -fPIC -Wno-switch -fvisibility=hidden -g -ggdb3 -O0 -m64 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
CFLAGS_release = -std=c99 -fPIC -Wno-switch -fvisibility=hidden -O3 -ftree-vectorize -m64 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
DEFINES_release = -DNDEBUG=1
INCLUDES = -I$(BASE)libui/alpha_4.1/libui/unix -I$(BASE)libui/alpha_4.1/libui/common -I`pkg-config --cflags gtk+-3.0` -I/usr/include/gtk-3.0 -I/usr/include/at-spi2-atk/2.0 -I/usr/include/at-spi-2.0 -I/usr/include/dbus-1.0 -I/usr/lib/i386-linux-gnu/dbus-1.0/include -I/usr/include/gio-unix-2.0/ -I/usr/include/mirclient -I/usr/include/mircommon -I/usr/include/mircookie -I/usr/include/cairo -I/usr/include/pango-1.0 -I/usr/include/harfbuzz -I/usr/include/atk-1.0 -I/usr/include/pixman-1 -I/usr/include/freetype2 -I/usr/include/libpng12 -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/glib-2.0 -I/usr/lib/i386-linux-gnu/glib-2.0/include -I$(BASE)libui/alpha_4.1/libui

COMMON_FILES = $(TMP)libui/alpha_4.1/libui/common/attribute $(TMP)libui/alpha_4.1/libui/common/debug $(TMP)libui/alpha_4.1/libui/common/shouldquit\
               $(TMP)libui/alpha_4.1/libui/common/tablevalue $(TMP)libui/alpha_4.1/libui/unix/alloc $(TMP)libui/alpha_4.1/libui/unix/area\
               $(TMP)libui/alpha_4.1/libui/unix/attrstr $(TMP)libui/alpha_4.1/libui/unix/box $(TMP)libui/alpha_4.1/libui/unix/button\
               $(TMP)libui/alpha_4.1/libui/unix/cellrendererbutton $(TMP)libui/alpha_4.1/libui/unix/checkbox $(TMP)libui/alpha_4.1/libui/unix/child\
               $(TMP)libui/alpha_4.1/libui/unix/colorbutton $(TMP)libui/alpha_4.1/libui/unix/combobox $(TMP)libui/alpha_4.1/libui/unix/control\
               $(TMP)libui/alpha_4.1/libui/unix/datetimepicker $(TMP)libui/alpha_4.1/libui/unix/draw $(TMP)libui/alpha_4.1/libui/unix/drawmatrix\
               $(TMP)libui/alpha_4.1/libui/unix/drawpath $(TMP)libui/alpha_4.1/libui/unix/drawtext $(TMP)libui/alpha_4.1/libui/unix/editablecombo\
               $(TMP)libui/alpha_4.1/libui/unix/entry $(TMP)libui/alpha_4.1/libui/unix/fontbutton $(TMP)libui/alpha_4.1/libui/unix/fontmatch\
               $(TMP)libui/alpha_4.1/libui/unix/form $(TMP)libui/alpha_4.1/libui/unix/future $(TMP)libui/alpha_4.1/libui/unix/grid\
               $(TMP)libui/alpha_4.1/libui/unix/group $(TMP)libui/alpha_4.1/libui/unix/image $(TMP)libui/alpha_4.1/libui/unix/label\
               $(TMP)libui/alpha_4.1/libui/unix/main $(TMP)libui/alpha_4.1/libui/unix/menu $(TMP)libui/alpha_4.1/libui/unix/multilineentry\
               $(TMP)libui/alpha_4.1/libui/unix/opentype $(TMP)libui/alpha_4.1/libui/unix/progressbar $(TMP)libui/alpha_4.1/libui/unix/radiobuttons\
               $(TMP)libui/alpha_4.1/libui/unix/separator $(TMP)libui/alpha_4.1/libui/unix/slider $(TMP)libui/alpha_4.1/libui/unix/spinbox\
               $(TMP)libui/alpha_4.1/libui/unix/stddialogs $(TMP)libui/alpha_4.1/libui/unix/tab $(TMP)libui/alpha_4.1/libui/unix/table\
               $(TMP)libui/alpha_4.1/libui/unix/tablemodel $(TMP)libui/alpha_4.1/libui/unix/text $(TMP)libui/alpha_4.1/libui/unix/util\
               $(TMP)libui/alpha_4.1/libui/unix/window

INPUTS_COMMON_ARCHIVE_debug = $(addsuffix .libui_alpha4_1_linux_amd64_gnu_debug.o,$(COMMON_FILES))
INPUTS_COMMON_ARCHIVE_release = $(addsuffix .libui_alpha4_1_linux_amd64_gnu_release.o,$(COMMON_FILES))

DEP_PROJ_ARCHIVE_debug = $(BASE)libui/alpha_4.1/libui/make/libui_impl_alpha4_1/linux_amd64_gnu/libui_impl_alpha4_1_debug.a
DEP_PROJ_ARCHIVE_release = $(BASE)libui/alpha_4.1/libui/make/libui_impl_alpha4_1/linux_amd64_gnu/libui_impl_alpha4_1_release.a

default: libui_alpha4_1_debug.a libui_alpha4_1_release.a

libui_alpha4_1_debug.a: $(INPUTS_COMMON_ARCHIVE_debug)
	$(AR) $(ARFLAGS)  $@ $^

libui_alpha4_1_release.a: $(INPUTS_COMMON_ARCHIVE_release)
	$(AR) $(ARFLAGS)  $@ $^

$(TMP)%.libui_alpha4_1_linux_amd64_gnu_debug.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -MM $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_debug) $(INCLUDES) -o $@ $<

$(TMP)%.libui_alpha4_1_linux_amd64_gnu_release.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMMON_COMPILE_debug = $(addsuffix .libui_alpha4_1_linux_amd64_gnu_debug.d,$(COMMON_FILES))
CLEAN_COMMON_COMPILE_release = $(addsuffix .libui_alpha4_1_linux_amd64_gnu_release.d,$(COMMON_FILES))

cleandeps:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)

clean: cleanself
	$(MAKE) -C $(BASE)libui/alpha_4.1/libui/make/libui_impl_alpha4_1/linux_amd64_gnu cleanself

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

include $(wildcard $(TMP)libui/alpha_4.1/libui/common/attribute.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/debug.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/shouldquit.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/tablevalue.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/alloc.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/area.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/attrstr.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/box.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/button.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/cellrendererbutton.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/checkbox.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/child.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/colorbutton.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/combobox.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/control.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/datetimepicker.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/draw.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/drawmatrix.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/drawpath.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/drawtext.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/editablecombo.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/entry.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/fontbutton.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/fontmatch.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/form.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/future.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/grid.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/group.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/image.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/label.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/main.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/menu.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/multilineentry.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/opentype.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/progressbar.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/radiobuttons.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/separator.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/slider.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/spinbox.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/stddialogs.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/tab.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/table.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/tablemodel.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/text.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/util.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/window.libui_alpha4_1_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/attribute.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/debug.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/shouldquit.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/tablevalue.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/alloc.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/area.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/attrstr.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/box.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/button.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/cellrendererbutton.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/checkbox.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/child.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/colorbutton.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/combobox.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/control.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/datetimepicker.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/draw.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/drawmatrix.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/drawpath.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/drawtext.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/editablecombo.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/entry.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/fontbutton.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/fontmatch.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/form.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/future.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/grid.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/group.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/image.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/label.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/main.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/menu.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/multilineentry.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/opentype.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/progressbar.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/radiobuttons.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/separator.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/slider.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/spinbox.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/stddialogs.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/tab.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/table.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/tablemodel.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/text.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/util.libui_alpha4_1_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/unix/window.libui_alpha4_1_linux_amd64_gnu_release.d)

$(BASE)libui/alpha_4.1/libui/make/libui_impl_alpha4_1/linux_amd64_gnu/libui_impl_alpha4_1_debug.a \
$(BASE)libui/alpha_4.1/libui/make/libui_impl_alpha4_1/linux_amd64_gnu/libui_impl_alpha4_1_release.a: %.a: FORCE
	$(MAKE) -C $(@D) $(@F)


