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
INCLUDES = -I$(BASE)dlb_buffer/1.3 -I$(BASE)dlb_octfile/1.1 -I$(BASE)dlb_pmd/frontend -I$(BASE)dlb_pmd/frontend/pmd_realtime -I$(BASE)dlb_pmd/include -I$(BASE)dlb_pmd/os -I$(BASE)dlb_socket/0.1/dlb_socket/include/win32 -I$(BASE)dlb_socket/0.1/dlb_socket/include -I$(BASE)dlb_socket/0.1 -I$(BASE)dlb_wave/1.6 -I$(BASE). -I$(BASE). -I$(BASE)portaudio/19.6/portaudio/include
LD = $(CC)
LDFLAGS_debug = -m64
LDFLAGS_release = -O2 -m64

COMMON_FILES = $(TMP)dlb_pmd/frontend/pmd_realtime/args $(TMP)dlb_pmd/frontend/pmd_realtime/pa $(TMP)dlb_pmd/frontend/pmd_realtime/pa_capture\
               $(TMP)dlb_pmd/frontend/pmd_realtime/pa_pipe $(TMP)dlb_pmd/frontend/pmd_realtime/pa_play $(TMP)dlb_pmd/frontend/pmd_realtime/pmd_realtime\
               $(TMP)dlb_pmd/os/windows/pmd_ctrl_c

INPUTS_COMMON_LINK_debug = $(addsuffix .pmd_realtime_windows_amd64_gnu_debug.o,$(COMMON_FILES))
INPUTS_COMMON_LINK_release = $(addsuffix .pmd_realtime_windows_amd64_gnu_release.o,$(COMMON_FILES))

DEP_PROJ_LINK_debug = $(BASE)dlb_pmd/make/dlb_pmd_lib/windows_amd64_gnu/dlb_pmd_lib_debug.lib $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/windows_amd64_gnu/dlb_xml_debug.lib $(BASE)zlib/1.2.11/make/zlib/windows_amd64_gnu/zlib_debug.lib $(BASE)dlb_pmd/make/dlb_wave_lib/windows_amd64_gnu/dlb_wave_lib_debug.lib $(BASE)dlb_socket/0.1/dlb_socket/make/dlb_socket/windows_amd64_gnu/dlb_socket_debug.lib $(BASE)portaudio/19.6/portaudio/make/portaudio_19_6_lib/windows_amd64_gnu/portaudio_19_6_lib_debug.lib
DEP_PROJ_LINK_release = $(BASE)dlb_pmd/make/dlb_pmd_lib/windows_amd64_gnu/dlb_pmd_lib_release.lib $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/windows_amd64_gnu/dlb_xml_release.lib $(BASE)zlib/1.2.11/make/zlib/windows_amd64_gnu/zlib_release.lib $(BASE)dlb_pmd/make/dlb_wave_lib/windows_amd64_gnu/dlb_wave_lib_release.lib $(BASE)dlb_socket/0.1/dlb_socket/make/dlb_socket/windows_amd64_gnu/dlb_socket_release.lib $(BASE)portaudio/19.6/portaudio/make/portaudio_19_6_lib/windows_amd64_gnu/portaudio_19_6_lib_release.lib

default: pmd_realtime_debug.exe pmd_realtime_release.exe

pmd_realtime_debug.exe: $(INPUTS_COMMON_LINK_debug) $(DEP_PROJ_LINK_debug)
	$(LD) $(LDFLAGS_debug) -o $@ $+ 

pmd_realtime_release.exe: $(INPUTS_COMMON_LINK_release) $(DEP_PROJ_LINK_release)
	$(LD) $(LDFLAGS_release) -o $@ $+ 

$(TMP)%.pmd_realtime_windows_amd64_gnu_debug.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -MM $(DEFINES_debug) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_debug) $(DEFINES_debug) $(INCLUDES) -o $@ $<

$(TMP)%.pmd_realtime_windows_amd64_gnu_release.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMMON_COMPILE_debug = $(addsuffix .pmd_realtime_windows_amd64_gnu_debug.d,$(COMMON_FILES))
CLEAN_COMMON_COMPILE_release = $(addsuffix .pmd_realtime_windows_amd64_gnu_release.d,$(COMMON_FILES))

cleandeps:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)

clean: cleanself
	$(MAKE) -C $(BASE)dlb_pmd/make/dlb_pmd_lib/windows_amd64_gnu cleanself
	$(MAKE) -C $(BASE)dlb_pmd/make/dlb_wave_lib/windows_amd64_gnu cleanself
	$(MAKE) -C $(BASE)dlb_socket/0.1/dlb_socket/make/dlb_socket/windows_amd64_gnu cleanself
	$(MAKE) -C $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/windows_amd64_gnu cleanself
	$(MAKE) -C $(BASE)portaudio/19.6/portaudio/make/portaudio_19_6_lib/windows_amd64_gnu cleanself
	$(MAKE) -C $(BASE)zlib/1.2.11/make/zlib/windows_amd64_gnu cleanself

cleanself:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)
	$(RM) $(INPUTS_COMMON_LINK_debug)
	$(RM) $(INPUTS_COMMON_LINK_release)
	$(RM) pmd_realtime_debug.exe
	$(RM) pmd_realtime_release.exe

.PHONY: default FORCE clean cleandeps cleanself help

FORCE:

help:
	@echo "This makefile has the following targets:"
	@echo " pmd_realtime_debug.exe"
	@echo " pmd_realtime_release.exe"
	@echo " default"
	@echo " clean"
	@echo " cleandeps"
	@echo " help"

include $(wildcard $(TMP)dlb_pmd/frontend/pmd_realtime/args.pmd_realtime_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_realtime/pa.pmd_realtime_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_realtime/pa_capture.pmd_realtime_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_realtime/pa_pipe.pmd_realtime_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_realtime/pa_play.pmd_realtime_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_realtime/pmd_realtime.pmd_realtime_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/os/windows/pmd_ctrl_c.pmd_realtime_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_realtime/args.pmd_realtime_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_realtime/pa.pmd_realtime_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_realtime/pa_capture.pmd_realtime_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_realtime/pa_pipe.pmd_realtime_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_realtime/pa_play.pmd_realtime_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_realtime/pmd_realtime.pmd_realtime_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/os/windows/pmd_ctrl_c.pmd_realtime_windows_amd64_gnu_release.d)

$(BASE)dlb_pmd/make/dlb_pmd_lib/windows_amd64_gnu/dlb_pmd_lib_debug.lib \
$(BASE)dlb_pmd/make/dlb_pmd_lib/windows_amd64_gnu/dlb_pmd_lib_release.lib \
$(BASE)dlb_pmd/make/dlb_wave_lib/windows_amd64_gnu/dlb_wave_lib_debug.lib \
$(BASE)dlb_pmd/make/dlb_wave_lib/windows_amd64_gnu/dlb_wave_lib_release.lib \
$(BASE)dlb_socket/0.1/dlb_socket/make/dlb_socket/windows_amd64_gnu/dlb_socket_debug.lib \
$(BASE)dlb_socket/0.1/dlb_socket/make/dlb_socket/windows_amd64_gnu/dlb_socket_release.lib \
$(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/windows_amd64_gnu/dlb_xml_debug.lib \
$(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/windows_amd64_gnu/dlb_xml_release.lib \
$(BASE)portaudio/19.6/portaudio/make/portaudio_19_6_lib/windows_amd64_gnu/portaudio_19_6_lib_debug.lib \
$(BASE)portaudio/19.6/portaudio/make/portaudio_19_6_lib/windows_amd64_gnu/portaudio_19_6_lib_release.lib \
$(BASE)zlib/1.2.11/make/zlib/windows_amd64_gnu/zlib_debug.lib \
$(BASE)zlib/1.2.11/make/zlib/windows_amd64_gnu/zlib_release.lib: %.lib: FORCE
	$(MAKE) -C $(@D) $(@F)


