#-*-makefile-*-

BASE=../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
CCDEP = $(CC)
CFLAGS_debug = -g -O0 -arch x86_64 -fPIC -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wno-overlength-strings -Wdeclaration-after-statement -Wconditional-uninitialized
CFLAGS_release = -O3 -arch x86_64 -fPIC -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wno-overlength-strings -Wdeclaration-after-statement -Wconditional-uninitialized
DEFINES_release = -DNDEBUG=1
INCLUDES = -I$(BASE)dlb_buffer/1.3 -I$(BASE)dlb_octfile/1.1 -I$(BASE)dlb_pmd/frontend -I$(BASE)dlb_pmd/frontend/pmd_realtime -I$(BASE)dlb_pmd/include -I$(BASE)dlb_pmd/os -I$(BASE)dlb_socket/0.1/dlb_socket/include/osx -I$(BASE)dlb_socket/0.1/dlb_socket/include -I$(BASE)dlb_socket/0.1 -I$(BASE)dlb_wave/1.6 -I$(BASE). -I$(BASE). -I$(BASE)portaudio/19.6/portaudio/include
LD = $(CC)
LDFLAGS_debug = -arch x86_64 -dead_strip
LDFLAGS_release = -arch x86_64 -dead_strip -O2
LDLIBS = -lpthread -Wl,-framework,CoreAudio -Wl,-framework,AudioToolbox -Wl,-framework,AudioUnit -Wl,-framework,Cocoa

COMMON_FILES = $(TMP)dlb_pmd/frontend/pmd_realtime/args $(TMP)dlb_pmd/frontend/pmd_realtime/pa $(TMP)dlb_pmd/frontend/pmd_realtime/pa_capture\
               $(TMP)dlb_pmd/frontend/pmd_realtime/pa_pipe $(TMP)dlb_pmd/frontend/pmd_realtime/pa_play $(TMP)dlb_pmd/frontend/pmd_realtime/pmd_realtime\
               $(TMP)dlb_pmd/os/osx/pmd_ctrl_c

INPUTS_COMMON_LINK_debug = $(addsuffix .pmd_realtime_osx_amd64_clang_debug.o,$(COMMON_FILES))
INPUTS_COMMON_LINK_release = $(addsuffix .pmd_realtime_osx_amd64_clang_release.o,$(COMMON_FILES))

DEP_PROJ_LINK_debug = $(BASE)dlb_pmd/make/dlb_pmd_lib/osx_amd64_clang/dlb_pmd_lib_debug.a $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/osx_amd64_clang/dlb_xml_debug.a $(BASE)zlib/1.2.11/make/zlib/osx_amd64_clang/zlib_debug.a $(BASE)dlb_pmd/make/dlb_wave_lib/osx_amd64_clang/dlb_wave_lib_debug.a $(BASE)dlb_socket/0.1/dlb_socket/make/dlb_socket/osx_amd64_clang/dlb_socket_debug.a $(BASE)portaudio/19.6/portaudio/make/portaudio_19_6_lib/osx_amd64_clang/portaudio_19_6_lib_debug.a
DEP_PROJ_LINK_release = $(BASE)dlb_pmd/make/dlb_pmd_lib/osx_amd64_clang/dlb_pmd_lib_release.a $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/osx_amd64_clang/dlb_xml_release.a $(BASE)zlib/1.2.11/make/zlib/osx_amd64_clang/zlib_release.a $(BASE)dlb_pmd/make/dlb_wave_lib/osx_amd64_clang/dlb_wave_lib_release.a $(BASE)dlb_socket/0.1/dlb_socket/make/dlb_socket/osx_amd64_clang/dlb_socket_release.a $(BASE)portaudio/19.6/portaudio/make/portaudio_19_6_lib/osx_amd64_clang/portaudio_19_6_lib_release.a

default: pmd_realtime_debug pmd_realtime_release

pmd_realtime_debug: $(INPUTS_COMMON_LINK_debug) $(DEP_PROJ_LINK_debug)
	$(LD) $(LDFLAGS_debug) -o $@ $+ $(LDLIBS)

pmd_realtime_release: $(INPUTS_COMMON_LINK_release) $(DEP_PROJ_LINK_release)
	$(LD) $(LDFLAGS_release) -o $@ $+ $(LDLIBS)

$(TMP)%.pmd_realtime_osx_amd64_clang_debug.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -arch x86_64 -MM $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_debug) $(INCLUDES) -o $@ $<

$(TMP)%.pmd_realtime_osx_amd64_clang_release.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -arch x86_64 -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMMON_COMPILE_debug = $(addsuffix .pmd_realtime_osx_amd64_clang_debug.d,$(COMMON_FILES))
CLEAN_COMMON_COMPILE_release = $(addsuffix .pmd_realtime_osx_amd64_clang_release.d,$(COMMON_FILES))

cleandeps:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)

clean: cleanself
	$(MAKE) -C $(BASE)dlb_pmd/make/dlb_pmd_lib/osx_amd64_clang cleanself
	$(MAKE) -C $(BASE)dlb_pmd/make/dlb_wave_lib/osx_amd64_clang cleanself
	$(MAKE) -C $(BASE)dlb_socket/0.1/dlb_socket/make/dlb_socket/osx_amd64_clang cleanself
	$(MAKE) -C $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/osx_amd64_clang cleanself
	$(MAKE) -C $(BASE)portaudio/19.6/portaudio/make/portaudio_19_6_lib/osx_amd64_clang cleanself
	$(MAKE) -C $(BASE)zlib/1.2.11/make/zlib/osx_amd64_clang cleanself

cleanself:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)
	$(RM) $(INPUTS_COMMON_LINK_debug)
	$(RM) $(INPUTS_COMMON_LINK_release)
	$(RM) pmd_realtime_debug
	$(RM) pmd_realtime_release

.PHONY: default FORCE clean cleandeps cleanself help

FORCE:

help:
	@echo "This makefile has the following targets:"
	@echo " pmd_realtime_debug"
	@echo " pmd_realtime_release"
	@echo " default"
	@echo " clean"
	@echo " cleandeps"
	@echo " help"

include $(wildcard $(TMP)dlb_pmd/frontend/pmd_realtime/args.pmd_realtime_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_realtime/pa.pmd_realtime_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_realtime/pa_capture.pmd_realtime_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_realtime/pa_pipe.pmd_realtime_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_realtime/pa_play.pmd_realtime_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_realtime/pmd_realtime.pmd_realtime_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/os/osx/pmd_ctrl_c.pmd_realtime_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_realtime/args.pmd_realtime_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_realtime/pa.pmd_realtime_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_realtime/pa_capture.pmd_realtime_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_realtime/pa_pipe.pmd_realtime_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_realtime/pa_play.pmd_realtime_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_realtime/pmd_realtime.pmd_realtime_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/os/osx/pmd_ctrl_c.pmd_realtime_osx_amd64_clang_release.d)

$(BASE)dlb_pmd/make/dlb_pmd_lib/osx_amd64_clang/dlb_pmd_lib_debug.a \
$(BASE)dlb_pmd/make/dlb_pmd_lib/osx_amd64_clang/dlb_pmd_lib_release.a \
$(BASE)dlb_pmd/make/dlb_wave_lib/osx_amd64_clang/dlb_wave_lib_debug.a \
$(BASE)dlb_pmd/make/dlb_wave_lib/osx_amd64_clang/dlb_wave_lib_release.a \
$(BASE)dlb_socket/0.1/dlb_socket/make/dlb_socket/osx_amd64_clang/dlb_socket_debug.a \
$(BASE)dlb_socket/0.1/dlb_socket/make/dlb_socket/osx_amd64_clang/dlb_socket_release.a \
$(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/osx_amd64_clang/dlb_xml_debug.a \
$(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/osx_amd64_clang/dlb_xml_release.a \
$(BASE)portaudio/19.6/portaudio/make/portaudio_19_6_lib/osx_amd64_clang/portaudio_19_6_lib_debug.a \
$(BASE)portaudio/19.6/portaudio/make/portaudio_19_6_lib/osx_amd64_clang/portaudio_19_6_lib_release.a \
$(BASE)zlib/1.2.11/make/zlib/osx_amd64_clang/zlib_debug.a \
$(BASE)zlib/1.2.11/make/zlib/osx_amd64_clang/zlib_release.a: %.a: FORCE
	$(MAKE) -C $(@D) $(@F)


