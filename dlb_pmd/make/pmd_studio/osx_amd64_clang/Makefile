#-*-makefile-*-

BASE=../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
CCDEP = $(CC)
CFLAGS_debug = -g -O0 -arch x86_64 -fPIC -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wno-overlength-strings -Wdeclaration-after-statement -Wconditional-uninitialized
CFLAGS_release = -O3 -arch x86_64 -fPIC -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wno-overlength-strings -Wdeclaration-after-statement -Wconditional-uninitialized
DEFINES_release = -DNDEBUG=1
INCLUDES = -I$(BASE)dlb_buffer/1.3 -I$(BASE)dlb_octfile/1.1 -I$(BASE)dlb_pmd/frontend -I$(BASE)dlb_pmd/frontend/pmd_studio -I$(BASE)dlb_pmd/frontend/pmd_tool -I$(BASE)dlb_pmd/frontend -I$(BASE)dlb_pmd/include -I$(BASE)dlb_pmd/os -I$(BASE)dlb_socket/0.1/dlb_socket/include/osx -I$(BASE)dlb_socket/0.1/dlb_socket/include -I$(BASE)dlb_socket/0.1 -I$(BASE)dlb_wave/1.6 -I$(BASE)libui/alpha_4.1/libui -I$(BASE). -I$(BASE). -I$(BASE)portaudio/19.6/portaudio/include
LD = $(CC)
LDFLAGS_debug = -arch x86_64 -dead_strip
LDFLAGS_release = -arch x86_64 -dead_strip -O2
LDLIBS = -lpthread -Wl,-framework,Foundation -Wl,-framework,AppKit -Wl,-framework,CoreAudio -Wl,-framework,AudioToolbox -Wl,-framework,AudioUnit -Wl,-framework,Cocoa

COMMON_FILES = $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_beds $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_objects\
               $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_outputs $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_presentations\
               $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_device $(TMP)dlb_pmd/frontend/pmd_tool/pcm $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool\
               $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool_klv $(TMP)dlb_pmd/frontend/pmd_tool/xml $(TMP)dlb_pmd/os/osx/pmd_ctrl_c

INPUTS_COMMON_LINK_debug = $(addsuffix .pmd_studio_osx_amd64_clang_debug.o,$(COMMON_FILES))
INPUTS_COMMON_LINK_release = $(addsuffix .pmd_studio_osx_amd64_clang_release.o,$(COMMON_FILES))

DEP_PROJ_LINK_debug = $(BASE)dlb_pmd/make/dlb_pmd_lib/osx_amd64_clang/dlb_pmd_lib_debug.a $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/osx_amd64_clang/dlb_xml_debug.a $(BASE)zlib/1.2.11/make/zlib/osx_amd64_clang/zlib_debug.a $(BASE)dlb_pmd/make/dlb_wave_lib/osx_amd64_clang/dlb_wave_lib_debug.a $(BASE)libui/alpha_4.1/libui/make/libui_alpha4_1/osx_amd64_clang/libui_alpha4_1_debug.a $(BASE)libui/alpha_4.1/libui/make/libui_impl_alpha4_1/osx_amd64_clang/libui_impl_alpha4_1_debug.a $(BASE)portaudio/19.6/portaudio/make/portaudio_19_6_lib/osx_amd64_clang/portaudio_19_6_lib_debug.a
DEP_PROJ_LINK_release = $(BASE)dlb_pmd/make/dlb_pmd_lib/osx_amd64_clang/dlb_pmd_lib_release.a $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/osx_amd64_clang/dlb_xml_release.a $(BASE)zlib/1.2.11/make/zlib/osx_amd64_clang/zlib_release.a $(BASE)dlb_pmd/make/dlb_wave_lib/osx_amd64_clang/dlb_wave_lib_release.a $(BASE)libui/alpha_4.1/libui/make/libui_alpha4_1/osx_amd64_clang/libui_alpha4_1_release.a $(BASE)libui/alpha_4.1/libui/make/libui_impl_alpha4_1/osx_amd64_clang/libui_impl_alpha4_1_release.a $(BASE)portaudio/19.6/portaudio/make/portaudio_19_6_lib/osx_amd64_clang/portaudio_19_6_lib_release.a

default: pmd_studio_debug pmd_studio_release

pmd_studio_debug: $(INPUTS_COMMON_LINK_debug) $(DEP_PROJ_LINK_debug)
	$(LD) $(LDFLAGS_debug) -o $@ $+ $(LDLIBS)

pmd_studio_release: $(INPUTS_COMMON_LINK_release) $(DEP_PROJ_LINK_release)
	$(LD) $(LDFLAGS_release) -o $@ $+ $(LDLIBS)

$(TMP)%.pmd_studio_osx_amd64_clang_debug.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -arch x86_64 -MM $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_debug) $(INCLUDES) -o $@ $<

$(TMP)%.pmd_studio_osx_amd64_clang_release.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -arch x86_64 -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMMON_COMPILE_debug = $(addsuffix .pmd_studio_osx_amd64_clang_debug.d,$(COMMON_FILES))
CLEAN_COMMON_COMPILE_release = $(addsuffix .pmd_studio_osx_amd64_clang_release.d,$(COMMON_FILES))

cleandeps:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)

clean: cleanself
	$(MAKE) -C $(BASE)dlb_pmd/make/dlb_pmd_lib/osx_amd64_clang cleanself
	$(MAKE) -C $(BASE)dlb_pmd/make/dlb_wave_lib/osx_amd64_clang cleanself
	$(MAKE) -C $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/osx_amd64_clang cleanself
	$(MAKE) -C $(BASE)libui/alpha_4.1/libui/make/libui_alpha4_1/osx_amd64_clang cleanself
	$(MAKE) -C $(BASE)libui/alpha_4.1/libui/make/libui_impl_alpha4_1/osx_amd64_clang cleanself
	$(MAKE) -C $(BASE)portaudio/19.6/portaudio/make/portaudio_19_6_lib/osx_amd64_clang cleanself
	$(MAKE) -C $(BASE)zlib/1.2.11/make/zlib/osx_amd64_clang cleanself

cleanself:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)
	$(RM) $(INPUTS_COMMON_LINK_debug)
	$(RM) $(INPUTS_COMMON_LINK_release)
	$(RM) pmd_studio_debug
	$(RM) pmd_studio_release

.PHONY: default FORCE clean cleandeps cleanself help

FORCE:

help:
	@echo "This makefile has the following targets:"
	@echo " pmd_studio_debug"
	@echo " pmd_studio_release"
	@echo " default"
	@echo " clean"
	@echo " cleandeps"
	@echo " help"

include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_beds.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_objects.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_outputs.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_presentations.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_device.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pcm.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool_klv.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/xml.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/os/osx/pmd_ctrl_c.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_beds.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_objects.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_outputs.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_presentations.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_device.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pcm.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool_klv.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/xml.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/os/osx/pmd_ctrl_c.pmd_studio_osx_amd64_clang_release.d)

$(BASE)dlb_pmd/make/dlb_pmd_lib/osx_amd64_clang/dlb_pmd_lib_debug.a \
$(BASE)dlb_pmd/make/dlb_pmd_lib/osx_amd64_clang/dlb_pmd_lib_release.a \
$(BASE)dlb_pmd/make/dlb_wave_lib/osx_amd64_clang/dlb_wave_lib_debug.a \
$(BASE)dlb_pmd/make/dlb_wave_lib/osx_amd64_clang/dlb_wave_lib_release.a \
$(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/osx_amd64_clang/dlb_xml_debug.a \
$(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/osx_amd64_clang/dlb_xml_release.a \
$(BASE)libui/alpha_4.1/libui/make/libui_alpha4_1/osx_amd64_clang/libui_alpha4_1_debug.a \
$(BASE)libui/alpha_4.1/libui/make/libui_alpha4_1/osx_amd64_clang/libui_alpha4_1_release.a \
$(BASE)libui/alpha_4.1/libui/make/libui_impl_alpha4_1/osx_amd64_clang/libui_impl_alpha4_1_debug.a \
$(BASE)libui/alpha_4.1/libui/make/libui_impl_alpha4_1/osx_amd64_clang/libui_impl_alpha4_1_release.a \
$(BASE)portaudio/19.6/portaudio/make/portaudio_19_6_lib/osx_amd64_clang/portaudio_19_6_lib_debug.a \
$(BASE)portaudio/19.6/portaudio/make/portaudio_19_6_lib/osx_amd64_clang/portaudio_19_6_lib_release.a \
$(BASE)zlib/1.2.11/make/zlib/osx_amd64_clang/zlib_debug.a \
$(BASE)zlib/1.2.11/make/zlib/osx_amd64_clang/zlib_release.a: %.a: FORCE
	$(MAKE) -C $(@D) $(@F)


