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
CXXFLAGS_debug = -g -ggdb3 -O0 -m64 -c -Wall
CXXFLAGS_release = -O3 -ftree-vectorize -m64 -c -Wall
DEFINES_debug = -DWIN32=1 -DWIN64=1 -D_LIB=1
DEFINES_release = -DNDEBUG=1 -DWIN32=1 -DWIN64=1 -D_LIB=1
INCLUDES = -I$(BASE)libui/alpha_4.1/libui/windows -I$(BASE)libui/alpha_4.1/libui/common -I$(BASE)libui/alpha_4.1/libui


INPUTS_ARCHIVE_debug = $(TMP)libui/alpha_4.1/libui/common/areaevents.libui_impl_alpha4_1_windows_amd64_gnu_debug.o $(TMP)libui/alpha_4.1/libui/common/attrlist.libui_impl_alpha4_1_windows_amd64_gnu_debug.o\
                       $(TMP)libui/alpha_4.1/libui/common/attrstr.libui_impl_alpha4_1_windows_amd64_gnu_debug.o $(TMP)libui/alpha_4.1/libui/common/control.libui_impl_alpha4_1_windows_amd64_gnu_debug.o\
                       $(TMP)libui/alpha_4.1/libui/common/matrix.libui_impl_alpha4_1_windows_amd64_gnu_debug.o $(TMP)libui/alpha_4.1/libui/common/opentype.libui_impl_alpha4_1_windows_amd64_gnu_debug.o\
                       $(TMP)libui/alpha_4.1/libui/common/tablemodel.libui_impl_alpha4_1_windows_amd64_gnu_debug.o $(TMP)libui/alpha_4.1/libui/common/userbugs.libui_impl_alpha4_1_windows_amd64_gnu_debug.o\
                       $(TMP)libui/alpha_4.1/libui/common/utf.libui_impl_alpha4_1_windows_amd64_gnu_debug.o $(TMP)libui/alpha_4.1/libui/windows/alloc.libui_impl_alpha4_1_windows_amd64_gnu_debug.o\
                       $(TMP)libui/alpha_4.1/libui/windows/areadraw.libui_impl_alpha4_1_windows_amd64_gnu_debug.o $(TMP)libui/alpha_4.1/libui/windows/areascroll.libui_impl_alpha4_1_windows_amd64_gnu_debug.o\
                       $(TMP)libui/alpha_4.1/libui/windows/areautil.libui_impl_alpha4_1_windows_amd64_gnu_debug.o $(TMP)libui/alpha_4.1/libui/windows/box.libui_impl_alpha4_1_windows_amd64_gnu_debug.o\
                       $(TMP)libui/alpha_4.1/libui/windows/button.libui_impl_alpha4_1_windows_amd64_gnu_debug.o $(TMP)libui/alpha_4.1/libui/windows/checkbox.libui_impl_alpha4_1_windows_amd64_gnu_debug.o\
                       $(TMP)libui/alpha_4.1/libui/windows/colorbutton.libui_impl_alpha4_1_windows_amd64_gnu_debug.o $(TMP)libui/alpha_4.1/libui/windows/colordialog.libui_impl_alpha4_1_windows_amd64_gnu_debug.o\
                       $(TMP)libui/alpha_4.1/libui/windows/combobox.libui_impl_alpha4_1_windows_amd64_gnu_debug.o $(TMP)libui/alpha_4.1/libui/windows/container.libui_impl_alpha4_1_windows_amd64_gnu_debug.o\
                       $(TMP)libui/alpha_4.1/libui/windows/d2dscratch.libui_impl_alpha4_1_windows_amd64_gnu_debug.o $(TMP)libui/alpha_4.1/libui/windows/datetimepicker.libui_impl_alpha4_1_windows_amd64_gnu_debug.o\
                       $(TMP)libui/alpha_4.1/libui/windows/debug.libui_impl_alpha4_1_windows_amd64_gnu_debug.o $(TMP)libui/alpha_4.1/libui/windows/draw.libui_impl_alpha4_1_windows_amd64_gnu_debug.o\
                       $(TMP)libui/alpha_4.1/libui/windows/drawmatrix.libui_impl_alpha4_1_windows_amd64_gnu_debug.o $(TMP)libui/alpha_4.1/libui/windows/drawpath.libui_impl_alpha4_1_windows_amd64_gnu_debug.o\
                       $(TMP)libui/alpha_4.1/libui/windows/dwrite.libui_impl_alpha4_1_windows_amd64_gnu_debug.o $(TMP)libui/alpha_4.1/libui/windows/editablecombo.libui_impl_alpha4_1_windows_amd64_gnu_debug.o\
                       $(TMP)libui/alpha_4.1/libui/windows/entry.libui_impl_alpha4_1_windows_amd64_gnu_debug.o $(TMP)libui/alpha_4.1/libui/windows/events.libui_impl_alpha4_1_windows_amd64_gnu_debug.o\
                       $(TMP)libui/alpha_4.1/libui/windows/fontdialog.libui_impl_alpha4_1_windows_amd64_gnu_debug.o $(TMP)libui/alpha_4.1/libui/windows/form.libui_impl_alpha4_1_windows_amd64_gnu_debug.o\
                       $(TMP)libui/alpha_4.1/libui/windows/graphemes.libui_impl_alpha4_1_windows_amd64_gnu_debug.o $(TMP)libui/alpha_4.1/libui/windows/grid.libui_impl_alpha4_1_windows_amd64_gnu_debug.o\
                       $(TMP)libui/alpha_4.1/libui/windows/group.libui_impl_alpha4_1_windows_amd64_gnu_debug.o $(TMP)libui/alpha_4.1/libui/windows/label.libui_impl_alpha4_1_windows_amd64_gnu_debug.o\
                       $(TMP)libui/alpha_4.1/libui/windows/main.libui_impl_alpha4_1_windows_amd64_gnu_debug.o $(TMP)libui/alpha_4.1/libui/windows/menu.libui_impl_alpha4_1_windows_amd64_gnu_debug.o\
                       $(TMP)libui/alpha_4.1/libui/windows/multilineentry.libui_impl_alpha4_1_windows_amd64_gnu_debug.o\
                       $(TMP)libui/alpha_4.1/libui/windows/parent.libui_impl_alpha4_1_windows_amd64_gnu_debug.o $(TMP)libui/alpha_4.1/libui/windows/progressbar.libui_impl_alpha4_1_windows_amd64_gnu_debug.o\
                       $(TMP)libui/alpha_4.1/libui/windows/radiobuttons.libui_impl_alpha4_1_windows_amd64_gnu_debug.o $(TMP)libui/alpha_4.1/libui/windows/separator.libui_impl_alpha4_1_windows_amd64_gnu_debug.o\
                       $(TMP)libui/alpha_4.1/libui/windows/sizing.libui_impl_alpha4_1_windows_amd64_gnu_debug.o $(TMP)libui/alpha_4.1/libui/windows/slider.libui_impl_alpha4_1_windows_amd64_gnu_debug.o\
                       $(TMP)libui/alpha_4.1/libui/windows/spinbox.libui_impl_alpha4_1_windows_amd64_gnu_debug.o $(TMP)libui/alpha_4.1/libui/windows/stddialogs.libui_impl_alpha4_1_windows_amd64_gnu_debug.o\
                       $(TMP)libui/alpha_4.1/libui/windows/tab.libui_impl_alpha4_1_windows_amd64_gnu_debug.o $(TMP)libui/alpha_4.1/libui/windows/table.libui_impl_alpha4_1_windows_amd64_gnu_debug.o\
                       $(TMP)libui/alpha_4.1/libui/windows/tabledispinfo.libui_impl_alpha4_1_windows_amd64_gnu_debug.o $(TMP)libui/alpha_4.1/libui/windows/tabledraw.libui_impl_alpha4_1_windows_amd64_gnu_debug.o\
                       $(TMP)libui/alpha_4.1/libui/windows/tableediting.libui_impl_alpha4_1_windows_amd64_gnu_debug.o $(TMP)libui/alpha_4.1/libui/windows/tablemetrics.libui_impl_alpha4_1_windows_amd64_gnu_debug.o\
                       $(TMP)libui/alpha_4.1/libui/windows/tabpage.libui_impl_alpha4_1_windows_amd64_gnu_debug.o $(TMP)libui/alpha_4.1/libui/windows/text.libui_impl_alpha4_1_windows_amd64_gnu_debug.o\
                       $(TMP)libui/alpha_4.1/libui/windows/utf16.libui_impl_alpha4_1_windows_amd64_gnu_debug.o $(TMP)libui/alpha_4.1/libui/windows/utilwin.libui_impl_alpha4_1_windows_amd64_gnu_debug.o\
                       $(TMP)libui/alpha_4.1/libui/windows/window.libui_impl_alpha4_1_windows_amd64_gnu_debug.o $(TMP)libui/alpha_4.1/libui/windows/winpublic.libui_impl_alpha4_1_windows_amd64_gnu_debug.o\
                       $(TMP)libui/alpha_4.1/libui/windows/winutil.libui_impl_alpha4_1_windows_amd64_gnu_debug.o
INPUTS_ARCHIVE_release = $(TMP)libui/alpha_4.1/libui/common/areaevents.libui_impl_alpha4_1_windows_amd64_gnu_release.o $(TMP)libui/alpha_4.1/libui/common/attrlist.libui_impl_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/common/attrstr.libui_impl_alpha4_1_windows_amd64_gnu_release.o $(TMP)libui/alpha_4.1/libui/common/control.libui_impl_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/common/matrix.libui_impl_alpha4_1_windows_amd64_gnu_release.o $(TMP)libui/alpha_4.1/libui/common/opentype.libui_impl_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/common/tablemodel.libui_impl_alpha4_1_windows_amd64_gnu_release.o $(TMP)libui/alpha_4.1/libui/common/userbugs.libui_impl_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/common/utf.libui_impl_alpha4_1_windows_amd64_gnu_release.o $(TMP)libui/alpha_4.1/libui/windows/alloc.libui_impl_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/windows/areadraw.libui_impl_alpha4_1_windows_amd64_gnu_release.o $(TMP)libui/alpha_4.1/libui/windows/areascroll.libui_impl_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/windows/areautil.libui_impl_alpha4_1_windows_amd64_gnu_release.o $(TMP)libui/alpha_4.1/libui/windows/box.libui_impl_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/windows/button.libui_impl_alpha4_1_windows_amd64_gnu_release.o $(TMP)libui/alpha_4.1/libui/windows/checkbox.libui_impl_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/windows/colorbutton.libui_impl_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/windows/colordialog.libui_impl_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/windows/combobox.libui_impl_alpha4_1_windows_amd64_gnu_release.o $(TMP)libui/alpha_4.1/libui/windows/container.libui_impl_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/windows/d2dscratch.libui_impl_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/windows/datetimepicker.libui_impl_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/windows/debug.libui_impl_alpha4_1_windows_amd64_gnu_release.o $(TMP)libui/alpha_4.1/libui/windows/draw.libui_impl_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/windows/drawmatrix.libui_impl_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/windows/drawpath.libui_impl_alpha4_1_windows_amd64_gnu_release.o $(TMP)libui/alpha_4.1/libui/windows/dwrite.libui_impl_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/windows/editablecombo.libui_impl_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/windows/entry.libui_impl_alpha4_1_windows_amd64_gnu_release.o $(TMP)libui/alpha_4.1/libui/windows/events.libui_impl_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/windows/fontdialog.libui_impl_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/windows/form.libui_impl_alpha4_1_windows_amd64_gnu_release.o $(TMP)libui/alpha_4.1/libui/windows/graphemes.libui_impl_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/windows/grid.libui_impl_alpha4_1_windows_amd64_gnu_release.o $(TMP)libui/alpha_4.1/libui/windows/group.libui_impl_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/windows/label.libui_impl_alpha4_1_windows_amd64_gnu_release.o $(TMP)libui/alpha_4.1/libui/windows/main.libui_impl_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/windows/menu.libui_impl_alpha4_1_windows_amd64_gnu_release.o $(TMP)libui/alpha_4.1/libui/windows/multilineentry.libui_impl_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/windows/parent.libui_impl_alpha4_1_windows_amd64_gnu_release.o $(TMP)libui/alpha_4.1/libui/windows/progressbar.libui_impl_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/windows/radiobuttons.libui_impl_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/windows/separator.libui_impl_alpha4_1_windows_amd64_gnu_release.o $(TMP)libui/alpha_4.1/libui/windows/sizing.libui_impl_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/windows/slider.libui_impl_alpha4_1_windows_amd64_gnu_release.o $(TMP)libui/alpha_4.1/libui/windows/spinbox.libui_impl_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/windows/stddialogs.libui_impl_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/windows/tab.libui_impl_alpha4_1_windows_amd64_gnu_release.o $(TMP)libui/alpha_4.1/libui/windows/table.libui_impl_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/windows/tabledispinfo.libui_impl_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/windows/tabledraw.libui_impl_alpha4_1_windows_amd64_gnu_release.o $(TMP)libui/alpha_4.1/libui/windows/tableediting.libui_impl_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/windows/tablemetrics.libui_impl_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/windows/tabpage.libui_impl_alpha4_1_windows_amd64_gnu_release.o $(TMP)libui/alpha_4.1/libui/windows/text.libui_impl_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/windows/utf16.libui_impl_alpha4_1_windows_amd64_gnu_release.o $(TMP)libui/alpha_4.1/libui/windows/utilwin.libui_impl_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/windows/window.libui_impl_alpha4_1_windows_amd64_gnu_release.o $(TMP)libui/alpha_4.1/libui/windows/winpublic.libui_impl_alpha4_1_windows_amd64_gnu_release.o\
                         $(TMP)libui/alpha_4.1/libui/windows/winutil.libui_impl_alpha4_1_windows_amd64_gnu_release.o


default: libui_impl_alpha4_1_debug.lib libui_impl_alpha4_1_release.lib

libui_impl_alpha4_1_debug.lib: $(INPUTS_ARCHIVE_debug)
	$(AR) $(ARFLAGS)  $@ $^

libui_impl_alpha4_1_release.lib: $(INPUTS_ARCHIVE_release)
	$(AR) $(ARFLAGS)  $@ $^

$(TMP)%.libui_impl_alpha4_1_windows_amd64_gnu_debug.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -MM $(DEFINES_debug) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_debug) $(DEFINES_debug) $(INCLUDES) -o $@ $<

$(TMP)%.libui_impl_alpha4_1_windows_amd64_gnu_debug.o: $(BASE)%.cpp | $(TMP)%/..
	$(CXXDEP) -MM $(DEFINES_debug) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS_debug) $(DEFINES_debug) $(INCLUDES) -o $@ $<

$(TMP)%.libui_impl_alpha4_1_windows_amd64_gnu_release.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

$(TMP)%.libui_impl_alpha4_1_windows_amd64_gnu_release.o: $(BASE)%.cpp | $(TMP)%/..
	$(CXXDEP) -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMPILE_debug = $(TMP)libui/alpha_4.1/libui/common/areaevents.libui_impl_alpha4_1_windows_amd64_gnu_debug.d $(TMP)libui/alpha_4.1/libui/common/attrlist.libui_impl_alpha4_1_windows_amd64_gnu_debug.d\
                      $(TMP)libui/alpha_4.1/libui/common/attrstr.libui_impl_alpha4_1_windows_amd64_gnu_debug.d $(TMP)libui/alpha_4.1/libui/common/control.libui_impl_alpha4_1_windows_amd64_gnu_debug.d\
                      $(TMP)libui/alpha_4.1/libui/common/matrix.libui_impl_alpha4_1_windows_amd64_gnu_debug.d $(TMP)libui/alpha_4.1/libui/common/opentype.libui_impl_alpha4_1_windows_amd64_gnu_debug.d\
                      $(TMP)libui/alpha_4.1/libui/common/tablemodel.libui_impl_alpha4_1_windows_amd64_gnu_debug.d $(TMP)libui/alpha_4.1/libui/common/userbugs.libui_impl_alpha4_1_windows_amd64_gnu_debug.d\
                      $(TMP)libui/alpha_4.1/libui/common/utf.libui_impl_alpha4_1_windows_amd64_gnu_debug.d $(TMP)libui/alpha_4.1/libui/windows/alloc.libui_impl_alpha4_1_windows_amd64_gnu_debug.d\
                      $(TMP)libui/alpha_4.1/libui/windows/areadraw.libui_impl_alpha4_1_windows_amd64_gnu_debug.d $(TMP)libui/alpha_4.1/libui/windows/areascroll.libui_impl_alpha4_1_windows_amd64_gnu_debug.d\
                      $(TMP)libui/alpha_4.1/libui/windows/areautil.libui_impl_alpha4_1_windows_amd64_gnu_debug.d $(TMP)libui/alpha_4.1/libui/windows/box.libui_impl_alpha4_1_windows_amd64_gnu_debug.d\
                      $(TMP)libui/alpha_4.1/libui/windows/button.libui_impl_alpha4_1_windows_amd64_gnu_debug.d $(TMP)libui/alpha_4.1/libui/windows/checkbox.libui_impl_alpha4_1_windows_amd64_gnu_debug.d\
                      $(TMP)libui/alpha_4.1/libui/windows/colorbutton.libui_impl_alpha4_1_windows_amd64_gnu_debug.d $(TMP)libui/alpha_4.1/libui/windows/colordialog.libui_impl_alpha4_1_windows_amd64_gnu_debug.d\
                      $(TMP)libui/alpha_4.1/libui/windows/combobox.libui_impl_alpha4_1_windows_amd64_gnu_debug.d $(TMP)libui/alpha_4.1/libui/windows/container.libui_impl_alpha4_1_windows_amd64_gnu_debug.d\
                      $(TMP)libui/alpha_4.1/libui/windows/d2dscratch.libui_impl_alpha4_1_windows_amd64_gnu_debug.d $(TMP)libui/alpha_4.1/libui/windows/datetimepicker.libui_impl_alpha4_1_windows_amd64_gnu_debug.d\
                      $(TMP)libui/alpha_4.1/libui/windows/debug.libui_impl_alpha4_1_windows_amd64_gnu_debug.d $(TMP)libui/alpha_4.1/libui/windows/draw.libui_impl_alpha4_1_windows_amd64_gnu_debug.d\
                      $(TMP)libui/alpha_4.1/libui/windows/drawmatrix.libui_impl_alpha4_1_windows_amd64_gnu_debug.d $(TMP)libui/alpha_4.1/libui/windows/drawpath.libui_impl_alpha4_1_windows_amd64_gnu_debug.d\
                      $(TMP)libui/alpha_4.1/libui/windows/dwrite.libui_impl_alpha4_1_windows_amd64_gnu_debug.d $(TMP)libui/alpha_4.1/libui/windows/editablecombo.libui_impl_alpha4_1_windows_amd64_gnu_debug.d\
                      $(TMP)libui/alpha_4.1/libui/windows/entry.libui_impl_alpha4_1_windows_amd64_gnu_debug.d $(TMP)libui/alpha_4.1/libui/windows/events.libui_impl_alpha4_1_windows_amd64_gnu_debug.d\
                      $(TMP)libui/alpha_4.1/libui/windows/fontdialog.libui_impl_alpha4_1_windows_amd64_gnu_debug.d $(TMP)libui/alpha_4.1/libui/windows/form.libui_impl_alpha4_1_windows_amd64_gnu_debug.d\
                      $(TMP)libui/alpha_4.1/libui/windows/graphemes.libui_impl_alpha4_1_windows_amd64_gnu_debug.d $(TMP)libui/alpha_4.1/libui/windows/grid.libui_impl_alpha4_1_windows_amd64_gnu_debug.d\
                      $(TMP)libui/alpha_4.1/libui/windows/group.libui_impl_alpha4_1_windows_amd64_gnu_debug.d $(TMP)libui/alpha_4.1/libui/windows/label.libui_impl_alpha4_1_windows_amd64_gnu_debug.d\
                      $(TMP)libui/alpha_4.1/libui/windows/main.libui_impl_alpha4_1_windows_amd64_gnu_debug.d $(TMP)libui/alpha_4.1/libui/windows/menu.libui_impl_alpha4_1_windows_amd64_gnu_debug.d\
                      $(TMP)libui/alpha_4.1/libui/windows/multilineentry.libui_impl_alpha4_1_windows_amd64_gnu_debug.d $(TMP)libui/alpha_4.1/libui/windows/parent.libui_impl_alpha4_1_windows_amd64_gnu_debug.d\
                      $(TMP)libui/alpha_4.1/libui/windows/progressbar.libui_impl_alpha4_1_windows_amd64_gnu_debug.d $(TMP)libui/alpha_4.1/libui/windows/radiobuttons.libui_impl_alpha4_1_windows_amd64_gnu_debug.d\
                      $(TMP)libui/alpha_4.1/libui/windows/separator.libui_impl_alpha4_1_windows_amd64_gnu_debug.d $(TMP)libui/alpha_4.1/libui/windows/sizing.libui_impl_alpha4_1_windows_amd64_gnu_debug.d\
                      $(TMP)libui/alpha_4.1/libui/windows/slider.libui_impl_alpha4_1_windows_amd64_gnu_debug.d $(TMP)libui/alpha_4.1/libui/windows/spinbox.libui_impl_alpha4_1_windows_amd64_gnu_debug.d\
                      $(TMP)libui/alpha_4.1/libui/windows/stddialogs.libui_impl_alpha4_1_windows_amd64_gnu_debug.d $(TMP)libui/alpha_4.1/libui/windows/tab.libui_impl_alpha4_1_windows_amd64_gnu_debug.d\
                      $(TMP)libui/alpha_4.1/libui/windows/table.libui_impl_alpha4_1_windows_amd64_gnu_debug.d $(TMP)libui/alpha_4.1/libui/windows/tabledispinfo.libui_impl_alpha4_1_windows_amd64_gnu_debug.d\
                      $(TMP)libui/alpha_4.1/libui/windows/tabledraw.libui_impl_alpha4_1_windows_amd64_gnu_debug.d $(TMP)libui/alpha_4.1/libui/windows/tableediting.libui_impl_alpha4_1_windows_amd64_gnu_debug.d\
                      $(TMP)libui/alpha_4.1/libui/windows/tablemetrics.libui_impl_alpha4_1_windows_amd64_gnu_debug.d $(TMP)libui/alpha_4.1/libui/windows/tabpage.libui_impl_alpha4_1_windows_amd64_gnu_debug.d\
                      $(TMP)libui/alpha_4.1/libui/windows/text.libui_impl_alpha4_1_windows_amd64_gnu_debug.d $(TMP)libui/alpha_4.1/libui/windows/utf16.libui_impl_alpha4_1_windows_amd64_gnu_debug.d\
                      $(TMP)libui/alpha_4.1/libui/windows/utilwin.libui_impl_alpha4_1_windows_amd64_gnu_debug.d $(TMP)libui/alpha_4.1/libui/windows/window.libui_impl_alpha4_1_windows_amd64_gnu_debug.d\
                      $(TMP)libui/alpha_4.1/libui/windows/winpublic.libui_impl_alpha4_1_windows_amd64_gnu_debug.d $(TMP)libui/alpha_4.1/libui/windows/winutil.libui_impl_alpha4_1_windows_amd64_gnu_debug.d
CLEAN_COMPILE_release = $(TMP)libui/alpha_4.1/libui/common/areaevents.libui_impl_alpha4_1_windows_amd64_gnu_release.d $(TMP)libui/alpha_4.1/libui/common/attrlist.libui_impl_alpha4_1_windows_amd64_gnu_release.d\
                        $(TMP)libui/alpha_4.1/libui/common/attrstr.libui_impl_alpha4_1_windows_amd64_gnu_release.d $(TMP)libui/alpha_4.1/libui/common/control.libui_impl_alpha4_1_windows_amd64_gnu_release.d\
                        $(TMP)libui/alpha_4.1/libui/common/matrix.libui_impl_alpha4_1_windows_amd64_gnu_release.d $(TMP)libui/alpha_4.1/libui/common/opentype.libui_impl_alpha4_1_windows_amd64_gnu_release.d\
                        $(TMP)libui/alpha_4.1/libui/common/tablemodel.libui_impl_alpha4_1_windows_amd64_gnu_release.d $(TMP)libui/alpha_4.1/libui/common/userbugs.libui_impl_alpha4_1_windows_amd64_gnu_release.d\
                        $(TMP)libui/alpha_4.1/libui/common/utf.libui_impl_alpha4_1_windows_amd64_gnu_release.d $(TMP)libui/alpha_4.1/libui/windows/alloc.libui_impl_alpha4_1_windows_amd64_gnu_release.d\
                        $(TMP)libui/alpha_4.1/libui/windows/areadraw.libui_impl_alpha4_1_windows_amd64_gnu_release.d $(TMP)libui/alpha_4.1/libui/windows/areascroll.libui_impl_alpha4_1_windows_amd64_gnu_release.d\
                        $(TMP)libui/alpha_4.1/libui/windows/areautil.libui_impl_alpha4_1_windows_amd64_gnu_release.d $(TMP)libui/alpha_4.1/libui/windows/box.libui_impl_alpha4_1_windows_amd64_gnu_release.d\
                        $(TMP)libui/alpha_4.1/libui/windows/button.libui_impl_alpha4_1_windows_amd64_gnu_release.d $(TMP)libui/alpha_4.1/libui/windows/checkbox.libui_impl_alpha4_1_windows_amd64_gnu_release.d\
                        $(TMP)libui/alpha_4.1/libui/windows/colorbutton.libui_impl_alpha4_1_windows_amd64_gnu_release.d\
                        $(TMP)libui/alpha_4.1/libui/windows/colordialog.libui_impl_alpha4_1_windows_amd64_gnu_release.d\
                        $(TMP)libui/alpha_4.1/libui/windows/combobox.libui_impl_alpha4_1_windows_amd64_gnu_release.d $(TMP)libui/alpha_4.1/libui/windows/container.libui_impl_alpha4_1_windows_amd64_gnu_release.d\
                        $(TMP)libui/alpha_4.1/libui/windows/d2dscratch.libui_impl_alpha4_1_windows_amd64_gnu_release.d $(TMP)libui/alpha_4.1/libui/windows/datetimepicker.libui_impl_alpha4_1_windows_amd64_gnu_release.d\
                        $(TMP)libui/alpha_4.1/libui/windows/debug.libui_impl_alpha4_1_windows_amd64_gnu_release.d $(TMP)libui/alpha_4.1/libui/windows/draw.libui_impl_alpha4_1_windows_amd64_gnu_release.d\
                        $(TMP)libui/alpha_4.1/libui/windows/drawmatrix.libui_impl_alpha4_1_windows_amd64_gnu_release.d $(TMP)libui/alpha_4.1/libui/windows/drawpath.libui_impl_alpha4_1_windows_amd64_gnu_release.d\
                        $(TMP)libui/alpha_4.1/libui/windows/dwrite.libui_impl_alpha4_1_windows_amd64_gnu_release.d $(TMP)libui/alpha_4.1/libui/windows/editablecombo.libui_impl_alpha4_1_windows_amd64_gnu_release.d\
                        $(TMP)libui/alpha_4.1/libui/windows/entry.libui_impl_alpha4_1_windows_amd64_gnu_release.d $(TMP)libui/alpha_4.1/libui/windows/events.libui_impl_alpha4_1_windows_amd64_gnu_release.d\
                        $(TMP)libui/alpha_4.1/libui/windows/fontdialog.libui_impl_alpha4_1_windows_amd64_gnu_release.d $(TMP)libui/alpha_4.1/libui/windows/form.libui_impl_alpha4_1_windows_amd64_gnu_release.d\
                        $(TMP)libui/alpha_4.1/libui/windows/graphemes.libui_impl_alpha4_1_windows_amd64_gnu_release.d $(TMP)libui/alpha_4.1/libui/windows/grid.libui_impl_alpha4_1_windows_amd64_gnu_release.d\
                        $(TMP)libui/alpha_4.1/libui/windows/group.libui_impl_alpha4_1_windows_amd64_gnu_release.d $(TMP)libui/alpha_4.1/libui/windows/label.libui_impl_alpha4_1_windows_amd64_gnu_release.d\
                        $(TMP)libui/alpha_4.1/libui/windows/main.libui_impl_alpha4_1_windows_amd64_gnu_release.d $(TMP)libui/alpha_4.1/libui/windows/menu.libui_impl_alpha4_1_windows_amd64_gnu_release.d\
                        $(TMP)libui/alpha_4.1/libui/windows/multilineentry.libui_impl_alpha4_1_windows_amd64_gnu_release.d\
                        $(TMP)libui/alpha_4.1/libui/windows/parent.libui_impl_alpha4_1_windows_amd64_gnu_release.d $(TMP)libui/alpha_4.1/libui/windows/progressbar.libui_impl_alpha4_1_windows_amd64_gnu_release.d\
                        $(TMP)libui/alpha_4.1/libui/windows/radiobuttons.libui_impl_alpha4_1_windows_amd64_gnu_release.d\
                        $(TMP)libui/alpha_4.1/libui/windows/separator.libui_impl_alpha4_1_windows_amd64_gnu_release.d $(TMP)libui/alpha_4.1/libui/windows/sizing.libui_impl_alpha4_1_windows_amd64_gnu_release.d\
                        $(TMP)libui/alpha_4.1/libui/windows/slider.libui_impl_alpha4_1_windows_amd64_gnu_release.d $(TMP)libui/alpha_4.1/libui/windows/spinbox.libui_impl_alpha4_1_windows_amd64_gnu_release.d\
                        $(TMP)libui/alpha_4.1/libui/windows/stddialogs.libui_impl_alpha4_1_windows_amd64_gnu_release.d $(TMP)libui/alpha_4.1/libui/windows/tab.libui_impl_alpha4_1_windows_amd64_gnu_release.d\
                        $(TMP)libui/alpha_4.1/libui/windows/table.libui_impl_alpha4_1_windows_amd64_gnu_release.d $(TMP)libui/alpha_4.1/libui/windows/tabledispinfo.libui_impl_alpha4_1_windows_amd64_gnu_release.d\
                        $(TMP)libui/alpha_4.1/libui/windows/tabledraw.libui_impl_alpha4_1_windows_amd64_gnu_release.d $(TMP)libui/alpha_4.1/libui/windows/tableediting.libui_impl_alpha4_1_windows_amd64_gnu_release.d\
                        $(TMP)libui/alpha_4.1/libui/windows/tablemetrics.libui_impl_alpha4_1_windows_amd64_gnu_release.d\
                        $(TMP)libui/alpha_4.1/libui/windows/tabpage.libui_impl_alpha4_1_windows_amd64_gnu_release.d $(TMP)libui/alpha_4.1/libui/windows/text.libui_impl_alpha4_1_windows_amd64_gnu_release.d\
                        $(TMP)libui/alpha_4.1/libui/windows/utf16.libui_impl_alpha4_1_windows_amd64_gnu_release.d $(TMP)libui/alpha_4.1/libui/windows/utilwin.libui_impl_alpha4_1_windows_amd64_gnu_release.d\
                        $(TMP)libui/alpha_4.1/libui/windows/window.libui_impl_alpha4_1_windows_amd64_gnu_release.d $(TMP)libui/alpha_4.1/libui/windows/winpublic.libui_impl_alpha4_1_windows_amd64_gnu_release.d\
                        $(TMP)libui/alpha_4.1/libui/windows/winutil.libui_impl_alpha4_1_windows_amd64_gnu_release.d

cleandeps:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)

clean: cleanself

cleanself:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)
	$(RM) $(INPUTS_ARCHIVE_debug)
	$(RM) $(INPUTS_ARCHIVE_release)
	$(RM) libui_impl_alpha4_1_debug.lib
	$(RM) libui_impl_alpha4_1_release.lib

.PHONY: default FORCE clean cleandeps cleanself help

FORCE:

help:
	@echo "This makefile has the following targets:"
	@echo " libui_impl_alpha4_1_debug.lib"
	@echo " libui_impl_alpha4_1_release.lib"
	@echo " default"
	@echo " clean"
	@echo " cleandeps"
	@echo " help"

include $(wildcard $(TMP)libui/alpha_4.1/libui/common/areaevents.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/attrlist.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/attrstr.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/control.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/matrix.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/opentype.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/tablemodel.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/userbugs.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/utf.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/alloc.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/areadraw.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/areascroll.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/areautil.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/box.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/button.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/checkbox.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/colorbutton.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/colordialog.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/combobox.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/container.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/d2dscratch.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/datetimepicker.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/debug.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/draw.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/drawmatrix.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/drawpath.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/dwrite.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/editablecombo.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/entry.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/events.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/fontdialog.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/form.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/graphemes.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/grid.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/group.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/label.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/main.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/menu.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/multilineentry.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/parent.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/progressbar.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/radiobuttons.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/separator.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/sizing.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/slider.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/spinbox.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/stddialogs.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/tab.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/table.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/tabledispinfo.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/tabledraw.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/tableediting.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/tablemetrics.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/tabpage.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/text.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/utf16.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/utilwin.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/window.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/winpublic.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/winutil.libui_impl_alpha4_1_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/areaevents.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/attrlist.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/attrstr.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/control.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/matrix.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/opentype.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/tablemodel.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/userbugs.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/common/utf.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/alloc.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/areadraw.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/areascroll.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/areautil.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/box.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/button.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/checkbox.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/colorbutton.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/colordialog.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/combobox.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/container.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/d2dscratch.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/datetimepicker.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/debug.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/draw.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/drawmatrix.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/drawpath.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/dwrite.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/editablecombo.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/entry.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/events.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/fontdialog.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/form.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/graphemes.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/grid.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/group.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/label.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/main.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/menu.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/multilineentry.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/parent.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/progressbar.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/radiobuttons.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/separator.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/sizing.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/slider.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/spinbox.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/stddialogs.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/tab.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/table.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/tabledispinfo.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/tabledraw.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/tableediting.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/tablemetrics.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/tabpage.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/text.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/utf16.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/utilwin.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/window.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/winpublic.libui_impl_alpha4_1_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)libui/alpha_4.1/libui/windows/winutil.libui_impl_alpha4_1_windows_amd64_gnu_release.d)


