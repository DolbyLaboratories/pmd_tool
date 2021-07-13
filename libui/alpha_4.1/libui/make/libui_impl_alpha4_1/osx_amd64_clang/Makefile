#-*-makefile-*-

BASE=../../../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
ARFLAGS = -rc -s
CCDEP = $(CC)
CFLAGS_debug = -g -O0 -arch x86_64 -fPIC -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wno-overlength-strings -Wdeclaration-after-statement -Wconditional-uninitialized
CFLAGS_release = -O3 -arch x86_64 -fPIC -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wno-overlength-strings -Wdeclaration-after-statement -Wconditional-uninitialized
DEFINES_debug = -DELPP_NO_DEFAULT_LOG_FILE=1
DEFINES_release = -DELPP_NO_DEFAULT_LOG_FILE=1 -DNDEBUG=1
INCLUDES = -I$(BASE)libui/alpha_4.1/libui/darwin -I$(BASE)libui/alpha_4.1/libui/common -I$(BASE)libui/alpha_4.1/libui


INPUTS_ARCHIVE_debug = $(TMP)libui/alpha_4.1/libui/common/areaevents.libui_impl_alpha4_1_osx_amd64_clang_debug.o $(TMP)libui/alpha_4.1/libui/common/attrlist.libui_impl_alpha4_1_osx_amd64_clang_debug.o\
                       $(TMP)libui/alpha_4.1/libui/common/attrstr.libui_impl_alpha4_1_osx_amd64_clang_debug.o $(TMP)libui/alpha_4.1/libui/common/control.libui_impl_alpha4_1_osx_amd64_clang_debug.o\
                       $(TMP)libui/alpha_4.1/libui/common/matrix.libui_impl_alpha4_1_osx_amd64_clang_debug.o $(TMP)libui/alpha_4.1/libui/common/opentype.libui_impl_alpha4_1_osx_amd64_clang_debug.o\
                       $(TMP)libui/alpha_4.1/libui/common/tablemodel.libui_impl_alpha4_1_osx_amd64_clang_debug.o $(TMP)libui/alpha_4.1/libui/common/userbugs.libui_impl_alpha4_1_osx_amd64_clang_debug.o\
                       $(TMP)libui/alpha_4.1/libui/common/utf.libui_impl_alpha4_1_osx_amd64_clang_debug.o $(TMP)libui/alpha_4.1/libui/darwin/aat.libui_impl_alpha4_1_osx_amd64_clang_debug.o\
                       $(TMP)libui/alpha_4.1/libui/darwin/alloc.libui_impl_alpha4_1_osx_amd64_clang_debug.o $(TMP)libui/alpha_4.1/libui/darwin/area.libui_impl_alpha4_1_osx_amd64_clang_debug.o\
                       $(TMP)libui/alpha_4.1/libui/darwin/areaevents.libui_impl_alpha4_1_osx_amd64_clang_debug.o $(TMP)libui/alpha_4.1/libui/darwin/attrstr.libui_impl_alpha4_1_osx_amd64_clang_debug.o\
                       $(TMP)libui/alpha_4.1/libui/darwin/autolayout.libui_impl_alpha4_1_osx_amd64_clang_debug.o $(TMP)libui/alpha_4.1/libui/darwin/box.libui_impl_alpha4_1_osx_amd64_clang_debug.o\
                       $(TMP)libui/alpha_4.1/libui/darwin/button.libui_impl_alpha4_1_osx_amd64_clang_debug.o $(TMP)libui/alpha_4.1/libui/darwin/checkbox.libui_impl_alpha4_1_osx_amd64_clang_debug.o\
                       $(TMP)libui/alpha_4.1/libui/darwin/colorbutton.libui_impl_alpha4_1_osx_amd64_clang_debug.o $(TMP)libui/alpha_4.1/libui/darwin/combobox.libui_impl_alpha4_1_osx_amd64_clang_debug.o\
                       $(TMP)libui/alpha_4.1/libui/darwin/control.libui_impl_alpha4_1_osx_amd64_clang_debug.o $(TMP)libui/alpha_4.1/libui/darwin/datetimepicker.libui_impl_alpha4_1_osx_amd64_clang_debug.o\
                       $(TMP)libui/alpha_4.1/libui/darwin/debug.libui_impl_alpha4_1_osx_amd64_clang_debug.o $(TMP)libui/alpha_4.1/libui/darwin/draw.libui_impl_alpha4_1_osx_amd64_clang_debug.o\
                       $(TMP)libui/alpha_4.1/libui/darwin/drawtext.libui_impl_alpha4_1_osx_amd64_clang_debug.o $(TMP)libui/alpha_4.1/libui/darwin/editablecombo.libui_impl_alpha4_1_osx_amd64_clang_debug.o\
                       $(TMP)libui/alpha_4.1/libui/darwin/entry.libui_impl_alpha4_1_osx_amd64_clang_debug.o $(TMP)libui/alpha_4.1/libui/darwin/fontbutton.libui_impl_alpha4_1_osx_amd64_clang_debug.o\
                       $(TMP)libui/alpha_4.1/libui/darwin/fontmatch.libui_impl_alpha4_1_osx_amd64_clang_debug.o $(TMP)libui/alpha_4.1/libui/darwin/fonttraits.libui_impl_alpha4_1_osx_amd64_clang_debug.o\
                       $(TMP)libui/alpha_4.1/libui/darwin/fontvariation.libui_impl_alpha4_1_osx_amd64_clang_debug.o $(TMP)libui/alpha_4.1/libui/darwin/form.libui_impl_alpha4_1_osx_amd64_clang_debug.o\
                       $(TMP)libui/alpha_4.1/libui/darwin/future.libui_impl_alpha4_1_osx_amd64_clang_debug.o $(TMP)libui/alpha_4.1/libui/darwin/graphemes.libui_impl_alpha4_1_osx_amd64_clang_debug.o\
                       $(TMP)libui/alpha_4.1/libui/darwin/grid.libui_impl_alpha4_1_osx_amd64_clang_debug.o $(TMP)libui/alpha_4.1/libui/darwin/group.libui_impl_alpha4_1_osx_amd64_clang_debug.o\
                       $(TMP)libui/alpha_4.1/libui/darwin/image.libui_impl_alpha4_1_osx_amd64_clang_debug.o $(TMP)libui/alpha_4.1/libui/darwin/label.libui_impl_alpha4_1_osx_amd64_clang_debug.o\
                       $(TMP)libui/alpha_4.1/libui/darwin/main.libui_impl_alpha4_1_osx_amd64_clang_debug.o $(TMP)libui/alpha_4.1/libui/darwin/map.libui_impl_alpha4_1_osx_amd64_clang_debug.o\
                       $(TMP)libui/alpha_4.1/libui/darwin/menu.libui_impl_alpha4_1_osx_amd64_clang_debug.o $(TMP)libui/alpha_4.1/libui/darwin/multilineentry.libui_impl_alpha4_1_osx_amd64_clang_debug.o\
                       $(TMP)libui/alpha_4.1/libui/darwin/opentype.libui_impl_alpha4_1_osx_amd64_clang_debug.o $(TMP)libui/alpha_4.1/libui/darwin/progressbar.libui_impl_alpha4_1_osx_amd64_clang_debug.o\
                       $(TMP)libui/alpha_4.1/libui/darwin/radiobuttons.libui_impl_alpha4_1_osx_amd64_clang_debug.o $(TMP)libui/alpha_4.1/libui/darwin/scrollview.libui_impl_alpha4_1_osx_amd64_clang_debug.o\
                       $(TMP)libui/alpha_4.1/libui/darwin/separator.libui_impl_alpha4_1_osx_amd64_clang_debug.o $(TMP)libui/alpha_4.1/libui/darwin/slider.libui_impl_alpha4_1_osx_amd64_clang_debug.o\
                       $(TMP)libui/alpha_4.1/libui/darwin/spinbox.libui_impl_alpha4_1_osx_amd64_clang_debug.o $(TMP)libui/alpha_4.1/libui/darwin/stddialogs.libui_impl_alpha4_1_osx_amd64_clang_debug.o\
                       $(TMP)libui/alpha_4.1/libui/darwin/tab.libui_impl_alpha4_1_osx_amd64_clang_debug.o $(TMP)libui/alpha_4.1/libui/darwin/table.libui_impl_alpha4_1_osx_amd64_clang_debug.o\
                       $(TMP)libui/alpha_4.1/libui/darwin/tablecolumn.libui_impl_alpha4_1_osx_amd64_clang_debug.o $(TMP)libui/alpha_4.1/libui/darwin/text.libui_impl_alpha4_1_osx_amd64_clang_debug.o\
                       $(TMP)libui/alpha_4.1/libui/darwin/undocumented.libui_impl_alpha4_1_osx_amd64_clang_debug.o $(TMP)libui/alpha_4.1/libui/darwin/util.libui_impl_alpha4_1_osx_amd64_clang_debug.o\
                       $(TMP)libui/alpha_4.1/libui/darwin/window.libui_impl_alpha4_1_osx_amd64_clang_debug.o $(TMP)libui/alpha_4.1/libui/darwin/winmoveresize.libui_impl_alpha4_1_osx_amd64_clang_debug.o
INPUTS_ARCHIVE_release = $(TMP)libui/alpha_4.1/libui/common/areaevents.libui_impl_alpha4_1_osx_amd64_clang_release.o $(TMP)libui/alpha_4.1/libui/common/attrlist.libui_impl_alpha4_1_osx_amd64_clang_release.o\
                         $(TMP)libui/alpha_4.1/libui/common/attrstr.libui_impl_alpha4_1_osx_amd64_clang_release.o $(TMP)libui/alpha_4.1/libui/common/control.libui_impl_alpha4_1_osx_amd64_clang_release.o\
                         $(TMP)libui/alpha_4.1/libui/common/matrix.libui_impl_alpha4_1_osx_amd64_clang_release.o $(TMP)libui/alpha_4.1/libui/common/opentype.libui_impl_alpha4_1_osx_amd64_clang_release.o\
                         $(TMP)libui/alpha_4.1/libui/common/tablemodel.libui_impl_alpha4_1_osx_amd64_clang_release.o $(TMP)libui/alpha_4.1/libui/common/userbugs.libui_impl_alpha4_1_osx_amd64_clang_release.o\
                         $(TMP)libui/alpha_4.1/libui/common/utf.libui_impl_alpha4_1_osx_amd64_clang_release.o $(TMP)libui/alpha_4.1/libui/darwin/aat.libui_impl_alpha4_1_osx_amd64_clang_release.o\
                         $(TMP)libui/alpha_4.1/libui/darwin/alloc.libui_impl_alpha4_1_osx_amd64_clang_release.o $(TMP)libui/alpha_4.1/libui/darwin/area.libui_impl_alpha4_1_osx_amd64_clang_release.o\
                         $(TMP)libui/alpha_4.1/libui/darwin/areaevents.libui_impl_alpha4_1_osx_amd64_clang_release.o $(TMP)libui/alpha_4.1/libui/darwin/attrstr.libui_impl_alpha4_1_osx_amd64_clang_release.o\
                         $(TMP)libui/alpha_4.1/libui/darwin/autolayout.libui_impl_alpha4_1_osx_amd64_clang_release.o $(TMP)libui/alpha_4.1/libui/darwin/box.libui_impl_alpha4_1_osx_amd64_clang_release.o\
                         $(TMP)libui/alpha_4.1/libui/darwin/button.libui_impl_alpha4_1_osx_amd64_clang_release.o $(TMP)libui/alpha_4.1/libui/darwin/checkbox.libui_impl_alpha4_1_osx_amd64_clang_release.o\
                         $(TMP)libui/alpha_4.1/libui/darwin/colorbutton.libui_impl_alpha4_1_osx_amd64_clang_release.o $(TMP)libui/alpha_4.1/libui/darwin/combobox.libui_impl_alpha4_1_osx_amd64_clang_release.o\
                         $(TMP)libui/alpha_4.1/libui/darwin/control.libui_impl_alpha4_1_osx_amd64_clang_release.o $(TMP)libui/alpha_4.1/libui/darwin/datetimepicker.libui_impl_alpha4_1_osx_amd64_clang_release.o\
                         $(TMP)libui/alpha_4.1/libui/darwin/debug.libui_impl_alpha4_1_osx_amd64_clang_release.o $(TMP)libui/alpha_4.1/libui/darwin/draw.libui_impl_alpha4_1_osx_amd64_clang_release.o\
                         $(TMP)libui/alpha_4.1/libui/darwin/drawtext.libui_impl_alpha4_1_osx_amd64_clang_release.o $(TMP)libui/alpha_4.1/libui/darwin/editablecombo.libui_impl_alpha4_1_osx_amd64_clang_release.o\
                         $(TMP)libui/alpha_4.1/libui/darwin/entry.libui_impl_alpha4_1_osx_amd64_clang_release.o $(TMP)libui/alpha_4.1/libui/darwin/fontbutton.libui_impl_alpha4_1_osx_amd64_clang_release.o\
                         $(TMP)libui/alpha_4.1/libui/darwin/fontmatch.libui_impl_alpha4_1_osx_amd64_clang_release.o $(TMP)libui/alpha_4.1/libui/darwin/fonttraits.libui_impl_alpha4_1_osx_amd64_clang_release.o\
                         $(TMP)libui/alpha_4.1/libui/darwin/fontvariation.libui_impl_alpha4_1_osx_amd64_clang_release.o\
                         $(TMP)libui/alpha_4.1/libui/darwin/form.libui_impl_alpha4_1_osx_amd64_clang_release.o $(TMP)libui/alpha_4.1/libui/darwin/future.libui_impl_alpha4_1_osx_amd64_clang_release.o\
                         $(TMP)libui/alpha_4.1/libui/darwin/graphemes.libui_impl_alpha4_1_osx_amd64_clang_release.o $(TMP)libui/alpha_4.1/libui/darwin/grid.libui_impl_alpha4_1_osx_amd64_clang_release.o\
                         $(TMP)libui/alpha_4.1/libui/darwin/group.libui_impl_alpha4_1_osx_amd64_clang_release.o $(TMP)libui/alpha_4.1/libui/darwin/image.libui_impl_alpha4_1_osx_amd64_clang_release.o\
                         $(TMP)libui/alpha_4.1/libui/darwin/label.libui_impl_alpha4_1_osx_amd64_clang_release.o $(TMP)libui/alpha_4.1/libui/darwin/main.libui_impl_alpha4_1_osx_amd64_clang_release.o\
                         $(TMP)libui/alpha_4.1/libui/darwin/map.libui_impl_alpha4_1_osx_amd64_clang_release.o $(TMP)libui/alpha_4.1/libui/darwin/menu.libui_impl_alpha4_1_osx_amd64_clang_release.o\
                         $(TMP)libui/alpha_4.1/libui/darwin/multilineentry.libui_impl_alpha4_1_osx_amd64_clang_release.o\
                         $(TMP)libui/alpha_4.1/libui/darwin/opentype.libui_impl_alpha4_1_osx_amd64_clang_release.o $(TMP)libui/alpha_4.1/libui/darwin/progressbar.libui_impl_alpha4_1_osx_amd64_clang_release.o\
                         $(TMP)libui/alpha_4.1/libui/darwin/radiobuttons.libui_impl_alpha4_1_osx_amd64_clang_release.o $(TMP)libui/alpha_4.1/libui/darwin/scrollview.libui_impl_alpha4_1_osx_amd64_clang_release.o\
                         $(TMP)libui/alpha_4.1/libui/darwin/separator.libui_impl_alpha4_1_osx_amd64_clang_release.o $(TMP)libui/alpha_4.1/libui/darwin/slider.libui_impl_alpha4_1_osx_amd64_clang_release.o\
                         $(TMP)libui/alpha_4.1/libui/darwin/spinbox.libui_impl_alpha4_1_osx_amd64_clang_release.o $(TMP)libui/alpha_4.1/libui/darwin/stddialogs.libui_impl_alpha4_1_osx_amd64_clang_release.o\
                         $(TMP)libui/alpha_4.1/libui/darwin/tab.libui_impl_alpha4_1_osx_amd64_clang_release.o $(TMP)libui/alpha_4.1/libui/darwin/table.libui_impl_alpha4_1_osx_amd64_clang_release.o\
                         $(TMP)libui/alpha_4.1/libui/darwin/tablecolumn.libui_impl_alpha4_1_osx_amd64_clang_release.o $(TMP)libui/alpha_4.1/libui/darwin/text.libui_impl_alpha4_1_osx_amd64_clang_release.o\
                         $(TMP)libui/alpha_4.1/libui/darwin/undocumented.libui_impl_alpha4_1_osx_amd64_clang_release.o $(TMP)libui/alpha_4.1/libui/darwin/util.libui_impl_alpha4_1_osx_amd64_clang_release.o\
                         $(TMP)libui/alpha_4.1/libui/darwin/window.libui_impl_alpha4_1_osx_amd64_clang_release.o $(TMP)libui/alpha_4.1/libui/darwin/winmoveresize.libui_impl_alpha4_1_osx_amd64_clang_release.o


default: libui_impl_alpha4_1_debug.a libui_impl_alpha4_1_release.a

libui_impl_alpha4_1_debug.a: $(INPUTS_ARCHIVE_debug)
	$(AR) $(ARFLAGS)   $@ $^

libui_impl_alpha4_1_release.a: $(INPUTS_ARCHIVE_release)
	$(AR) $(ARFLAGS)   $@ $^

$(TMP)%.libui_impl_alpha4_1_osx_amd64_clang_debug.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -arch x86_64 -MM $(DEFINES_debug) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_debug) $(DEFINES_debug) $(INCLUDES) -o $@ $<

$(TMP)%.libui_impl_alpha4_1_osx_amd64_clang_debug.o: $(BASE)%.m | $(TMP)%/..
	$(CCDEP) -arch x86_64 -MM $(DEFINES_debug) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_debug) $(DEFINES_debug) $(INCLUDES) -o $@ $<

$(TMP)%.libui_impl_alpha4_1_osx_amd64_clang_release.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -arch x86_64 -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

$(TMP)%.libui_impl_alpha4_1_osx_amd64_clang_release.o: $(BASE)%.m | $(TMP)%/..
	$(CCDEP) -arch x86_64 -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMPILE_debug = $(TMP)libui/alpha_4.1/libui/common/areaevents.libui_impl_alpha4_1_osx_amd64_clang_debug.d $(TMP)libui/alpha_4.1/libui/common/attrlist.libui_impl_alpha4_1_osx_amd64_clang_debug.d\
                      $(TMP)libui/alpha_4.1/libui/common/attrstr.libui_impl_alpha4_1_osx_amd64_clang_debug.d $(TMP)libui/alpha_4.1/libui/common/control.libui_impl_alpha4_1_osx_amd64_clang_debug.d\
                      $(TMP)libui/alpha_4.1/libui/common/matrix.libui_impl_alpha4_1_osx_amd64_clang_debug.d $(TMP)libui/alpha_4.1/libui/common/opentype.libui_impl_alpha4_1_osx_amd64_clang_debug.d\
                      $(TMP)libui/alpha_4.1/libui/common/tablemodel.libui_impl_alpha4_1_osx_amd64_clang_debug.d $(TMP)libui/alpha_4.1/libui/common/userbugs.libui_impl_alpha4_1_osx_amd64_clang_debug.d\
                      $(TMP)libui/alpha_4.1/libui/common/utf.libui_impl_alpha4_1_osx_amd64_clang_debug.d $(TMP)libui/alpha_4.1/libui/darwin/aat.libui_impl_alpha4_1_osx_amd64_clang_debug.d\
                      $(TMP)libui/alpha_4.1/libui/darwin/alloc.libui_impl_alpha4_1_osx_amd64_clang_debug.d $(TMP)libui/alpha_4.1/libui/darwin/area.libui_impl_alpha4_1_osx_amd64_clang_debug.d\
                      $(TMP)libui/alpha_4.1/libui/darwin/areaevents.libui_impl_alpha4_1_osx_amd64_clang_debug.d $(TMP)libui/alpha_4.1/libui/darwin/attrstr.libui_impl_alpha4_1_osx_amd64_clang_debug.d\
                      $(TMP)libui/alpha_4.1/libui/darwin/autolayout.libui_impl_alpha4_1_osx_amd64_clang_debug.d $(TMP)libui/alpha_4.1/libui/darwin/box.libui_impl_alpha4_1_osx_amd64_clang_debug.d\
                      $(TMP)libui/alpha_4.1/libui/darwin/button.libui_impl_alpha4_1_osx_amd64_clang_debug.d $(TMP)libui/alpha_4.1/libui/darwin/checkbox.libui_impl_alpha4_1_osx_amd64_clang_debug.d\
                      $(TMP)libui/alpha_4.1/libui/darwin/colorbutton.libui_impl_alpha4_1_osx_amd64_clang_debug.d $(TMP)libui/alpha_4.1/libui/darwin/combobox.libui_impl_alpha4_1_osx_amd64_clang_debug.d\
                      $(TMP)libui/alpha_4.1/libui/darwin/control.libui_impl_alpha4_1_osx_amd64_clang_debug.d $(TMP)libui/alpha_4.1/libui/darwin/datetimepicker.libui_impl_alpha4_1_osx_amd64_clang_debug.d\
                      $(TMP)libui/alpha_4.1/libui/darwin/debug.libui_impl_alpha4_1_osx_amd64_clang_debug.d $(TMP)libui/alpha_4.1/libui/darwin/draw.libui_impl_alpha4_1_osx_amd64_clang_debug.d\
                      $(TMP)libui/alpha_4.1/libui/darwin/drawtext.libui_impl_alpha4_1_osx_amd64_clang_debug.d $(TMP)libui/alpha_4.1/libui/darwin/editablecombo.libui_impl_alpha4_1_osx_amd64_clang_debug.d\
                      $(TMP)libui/alpha_4.1/libui/darwin/entry.libui_impl_alpha4_1_osx_amd64_clang_debug.d $(TMP)libui/alpha_4.1/libui/darwin/fontbutton.libui_impl_alpha4_1_osx_amd64_clang_debug.d\
                      $(TMP)libui/alpha_4.1/libui/darwin/fontmatch.libui_impl_alpha4_1_osx_amd64_clang_debug.d $(TMP)libui/alpha_4.1/libui/darwin/fonttraits.libui_impl_alpha4_1_osx_amd64_clang_debug.d\
                      $(TMP)libui/alpha_4.1/libui/darwin/fontvariation.libui_impl_alpha4_1_osx_amd64_clang_debug.d $(TMP)libui/alpha_4.1/libui/darwin/form.libui_impl_alpha4_1_osx_amd64_clang_debug.d\
                      $(TMP)libui/alpha_4.1/libui/darwin/future.libui_impl_alpha4_1_osx_amd64_clang_debug.d $(TMP)libui/alpha_4.1/libui/darwin/graphemes.libui_impl_alpha4_1_osx_amd64_clang_debug.d\
                      $(TMP)libui/alpha_4.1/libui/darwin/grid.libui_impl_alpha4_1_osx_amd64_clang_debug.d $(TMP)libui/alpha_4.1/libui/darwin/group.libui_impl_alpha4_1_osx_amd64_clang_debug.d\
                      $(TMP)libui/alpha_4.1/libui/darwin/image.libui_impl_alpha4_1_osx_amd64_clang_debug.d $(TMP)libui/alpha_4.1/libui/darwin/label.libui_impl_alpha4_1_osx_amd64_clang_debug.d\
                      $(TMP)libui/alpha_4.1/libui/darwin/main.libui_impl_alpha4_1_osx_amd64_clang_debug.d $(TMP)libui/alpha_4.1/libui/darwin/map.libui_impl_alpha4_1_osx_amd64_clang_debug.d\
                      $(TMP)libui/alpha_4.1/libui/darwin/menu.libui_impl_alpha4_1_osx_amd64_clang_debug.d $(TMP)libui/alpha_4.1/libui/darwin/multilineentry.libui_impl_alpha4_1_osx_amd64_clang_debug.d\
                      $(TMP)libui/alpha_4.1/libui/darwin/opentype.libui_impl_alpha4_1_osx_amd64_clang_debug.d $(TMP)libui/alpha_4.1/libui/darwin/progressbar.libui_impl_alpha4_1_osx_amd64_clang_debug.d\
                      $(TMP)libui/alpha_4.1/libui/darwin/radiobuttons.libui_impl_alpha4_1_osx_amd64_clang_debug.d $(TMP)libui/alpha_4.1/libui/darwin/scrollview.libui_impl_alpha4_1_osx_amd64_clang_debug.d\
                      $(TMP)libui/alpha_4.1/libui/darwin/separator.libui_impl_alpha4_1_osx_amd64_clang_debug.d $(TMP)libui/alpha_4.1/libui/darwin/slider.libui_impl_alpha4_1_osx_amd64_clang_debug.d\
                      $(TMP)libui/alpha_4.1/libui/darwin/spinbox.libui_impl_alpha4_1_osx_amd64_clang_debug.d $(TMP)libui/alpha_4.1/libui/darwin/stddialogs.libui_impl_alpha4_1_osx_amd64_clang_debug.d\
                      $(TMP)libui/alpha_4.1/libui/darwin/tab.libui_impl_alpha4_1_osx_amd64_clang_debug.d $(TMP)libui/alpha_4.1/libui/darwin/table.libui_impl_alpha4_1_osx_amd64_clang_debug.d\
                      $(TMP)libui/alpha_4.1/libui/darwin/tablecolumn.libui_impl_alpha4_1_osx_amd64_clang_debug.d $(TMP)libui/alpha_4.1/libui/darwin/text.libui_impl_alpha4_1_osx_amd64_clang_debug.d\
                      $(TMP)libui/alpha_4.1/libui/darwin/undocumented.libui_impl_alpha4_1_osx_amd64_clang_debug.d $(TMP)libui/alpha_4.1/libui/darwin/util.libui_impl_alpha4_1_osx_amd64_clang_debug.d\
                      $(TMP)libui/alpha_4.1/libui/darwin/window.libui_impl_alpha4_1_osx_amd64_clang_debug.d $(TMP)libui/alpha_4.1/libui/darwin/winmoveresize.libui_impl_alpha4_1_osx_amd64_clang_debug.d
CLEAN_COMPILE_release = $(TMP)libui/alpha_4.1/libui/common/areaevents.libui_impl_alpha4_1_osx_amd64_clang_release.d $(TMP)libui/alpha_4.1/libui/common/attrlist.libui_impl_alpha4_1_osx_amd64_clang_release.d\
                        $(TMP)libui/alpha_4.1/libui/common/attrstr.libui_impl_alpha4_1_osx_amd64_clang_release.d $(TMP)libui/alpha_4.1/libui/common/control.libui_impl_alpha4_1_osx_amd64_clang_release.d\
                        $(TMP)libui/alpha_4.1/libui/common/matrix.libui_impl_alpha4_1_osx_amd64_clang_release.d $(TMP)libui/alpha_4.1/libui/common/opentype.libui_impl_alpha4_1_osx_amd64_clang_release.d\
                        $(TMP)libui/alpha_4.1/libui/common/tablemodel.libui_impl_alpha4_1_osx_amd64_clang_release.d $(TMP)libui/alpha_4.1/libui/common/userbugs.libui_impl_alpha4_1_osx_amd64_clang_release.d\
                        $(TMP)libui/alpha_4.1/libui/common/utf.libui_impl_alpha4_1_osx_amd64_clang_release.d $(TMP)libui/alpha_4.1/libui/darwin/aat.libui_impl_alpha4_1_osx_amd64_clang_release.d\
                        $(TMP)libui/alpha_4.1/libui/darwin/alloc.libui_impl_alpha4_1_osx_amd64_clang_release.d $(TMP)libui/alpha_4.1/libui/darwin/area.libui_impl_alpha4_1_osx_amd64_clang_release.d\
                        $(TMP)libui/alpha_4.1/libui/darwin/areaevents.libui_impl_alpha4_1_osx_amd64_clang_release.d $(TMP)libui/alpha_4.1/libui/darwin/attrstr.libui_impl_alpha4_1_osx_amd64_clang_release.d\
                        $(TMP)libui/alpha_4.1/libui/darwin/autolayout.libui_impl_alpha4_1_osx_amd64_clang_release.d $(TMP)libui/alpha_4.1/libui/darwin/box.libui_impl_alpha4_1_osx_amd64_clang_release.d\
                        $(TMP)libui/alpha_4.1/libui/darwin/button.libui_impl_alpha4_1_osx_amd64_clang_release.d $(TMP)libui/alpha_4.1/libui/darwin/checkbox.libui_impl_alpha4_1_osx_amd64_clang_release.d\
                        $(TMP)libui/alpha_4.1/libui/darwin/colorbutton.libui_impl_alpha4_1_osx_amd64_clang_release.d $(TMP)libui/alpha_4.1/libui/darwin/combobox.libui_impl_alpha4_1_osx_amd64_clang_release.d\
                        $(TMP)libui/alpha_4.1/libui/darwin/control.libui_impl_alpha4_1_osx_amd64_clang_release.d $(TMP)libui/alpha_4.1/libui/darwin/datetimepicker.libui_impl_alpha4_1_osx_amd64_clang_release.d\
                        $(TMP)libui/alpha_4.1/libui/darwin/debug.libui_impl_alpha4_1_osx_amd64_clang_release.d $(TMP)libui/alpha_4.1/libui/darwin/draw.libui_impl_alpha4_1_osx_amd64_clang_release.d\
                        $(TMP)libui/alpha_4.1/libui/darwin/drawtext.libui_impl_alpha4_1_osx_amd64_clang_release.d $(TMP)libui/alpha_4.1/libui/darwin/editablecombo.libui_impl_alpha4_1_osx_amd64_clang_release.d\
                        $(TMP)libui/alpha_4.1/libui/darwin/entry.libui_impl_alpha4_1_osx_amd64_clang_release.d $(TMP)libui/alpha_4.1/libui/darwin/fontbutton.libui_impl_alpha4_1_osx_amd64_clang_release.d\
                        $(TMP)libui/alpha_4.1/libui/darwin/fontmatch.libui_impl_alpha4_1_osx_amd64_clang_release.d $(TMP)libui/alpha_4.1/libui/darwin/fonttraits.libui_impl_alpha4_1_osx_amd64_clang_release.d\
                        $(TMP)libui/alpha_4.1/libui/darwin/fontvariation.libui_impl_alpha4_1_osx_amd64_clang_release.d $(TMP)libui/alpha_4.1/libui/darwin/form.libui_impl_alpha4_1_osx_amd64_clang_release.d\
                        $(TMP)libui/alpha_4.1/libui/darwin/future.libui_impl_alpha4_1_osx_amd64_clang_release.d $(TMP)libui/alpha_4.1/libui/darwin/graphemes.libui_impl_alpha4_1_osx_amd64_clang_release.d\
                        $(TMP)libui/alpha_4.1/libui/darwin/grid.libui_impl_alpha4_1_osx_amd64_clang_release.d $(TMP)libui/alpha_4.1/libui/darwin/group.libui_impl_alpha4_1_osx_amd64_clang_release.d\
                        $(TMP)libui/alpha_4.1/libui/darwin/image.libui_impl_alpha4_1_osx_amd64_clang_release.d $(TMP)libui/alpha_4.1/libui/darwin/label.libui_impl_alpha4_1_osx_amd64_clang_release.d\
                        $(TMP)libui/alpha_4.1/libui/darwin/main.libui_impl_alpha4_1_osx_amd64_clang_release.d $(TMP)libui/alpha_4.1/libui/darwin/map.libui_impl_alpha4_1_osx_amd64_clang_release.d\
                        $(TMP)libui/alpha_4.1/libui/darwin/menu.libui_impl_alpha4_1_osx_amd64_clang_release.d $(TMP)libui/alpha_4.1/libui/darwin/multilineentry.libui_impl_alpha4_1_osx_amd64_clang_release.d\
                        $(TMP)libui/alpha_4.1/libui/darwin/opentype.libui_impl_alpha4_1_osx_amd64_clang_release.d $(TMP)libui/alpha_4.1/libui/darwin/progressbar.libui_impl_alpha4_1_osx_amd64_clang_release.d\
                        $(TMP)libui/alpha_4.1/libui/darwin/radiobuttons.libui_impl_alpha4_1_osx_amd64_clang_release.d $(TMP)libui/alpha_4.1/libui/darwin/scrollview.libui_impl_alpha4_1_osx_amd64_clang_release.d\
                        $(TMP)libui/alpha_4.1/libui/darwin/separator.libui_impl_alpha4_1_osx_amd64_clang_release.d $(TMP)libui/alpha_4.1/libui/darwin/slider.libui_impl_alpha4_1_osx_amd64_clang_release.d\
                        $(TMP)libui/alpha_4.1/libui/darwin/spinbox.libui_impl_alpha4_1_osx_amd64_clang_release.d $(TMP)libui/alpha_4.1/libui/darwin/stddialogs.libui_impl_alpha4_1_osx_amd64_clang_release.d\
                        $(TMP)libui/alpha_4.1/libui/darwin/tab.libui_impl_alpha4_1_osx_amd64_clang_release.d $(TMP)libui/alpha_4.1/libui/darwin/table.libui_impl_alpha4_1_osx_amd64_clang_release.d\
                        $(TMP)libui/alpha_4.1/libui/darwin/tablecolumn.libui_impl_alpha4_1_osx_amd64_clang_release.d $(TMP)libui/alpha_4.1/libui/darwin/text.libui_impl_alpha4_1_osx_amd64_clang_release.d\
                        $(TMP)libui/alpha_4.1/libui/darwin/undocumented.libui_impl_alpha4_1_osx_amd64_clang_release.d $(TMP)libui/alpha_4.1/libui/darwin/util.libui_impl_alpha4_1_osx_amd64_clang_release.d\
                        $(TMP)libui/alpha_4.1/libui/darwin/window.libui_impl_alpha4_1_osx_amd64_clang_release.d $(TMP)libui/alpha_4.1/libui/darwin/winmoveresize.libui_impl_alpha4_1_osx_amd64_clang_release.d

cleandeps:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)

clean: cleanself

cleanself:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)
	$(RM) $(INPUTS_ARCHIVE_debug)
	$(RM) $(INPUTS_ARCHIVE_release)
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

include $(wildcard $(TMP)libui/alpha_4.1/libui/common/areaevents.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/attrlist.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/attrstr.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/control.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/matrix.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/opentype.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/tablemodel.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/userbugs.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/utf.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/aat.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/alloc.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/area.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/areaevents.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/attrstr.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/autolayout.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/box.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/button.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/checkbox.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/colorbutton.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/combobox.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/control.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/datetimepicker.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/debug.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/draw.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/drawtext.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/editablecombo.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/entry.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/fontbutton.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/fontmatch.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/fonttraits.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/fontvariation.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/form.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/future.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/graphemes.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/grid.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/group.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/image.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/label.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/main.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/map.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/menu.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/multilineentry.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/opentype.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/progressbar.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/radiobuttons.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/scrollview.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/separator.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/slider.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/spinbox.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/stddialogs.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/tab.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/table.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/tablecolumn.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/text.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/undocumented.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/util.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/window.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/winmoveresize.libui_impl_alpha4_1_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/areaevents.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/attrlist.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/attrstr.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/control.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/matrix.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/opentype.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/tablemodel.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/userbugs.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/utf.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/aat.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/alloc.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/area.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/areaevents.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/attrstr.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/autolayout.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/box.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/button.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/checkbox.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/colorbutton.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/combobox.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/control.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/datetimepicker.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/debug.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/draw.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/drawtext.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/editablecombo.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/entry.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/fontbutton.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/fontmatch.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/fonttraits.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/fontvariation.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/form.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/future.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/graphemes.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/grid.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/group.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/image.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/label.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/main.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/map.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/menu.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/multilineentry.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/opentype.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/progressbar.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/radiobuttons.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/scrollview.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/separator.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/slider.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/spinbox.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/stddialogs.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/tab.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/table.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/tablecolumn.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/text.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/undocumented.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/util.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/window.libui_impl_alpha4_1_osx_amd64_clang_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/darwin/winmoveresize.libui_impl_alpha4_1_osx_amd64_clang_release.d)


