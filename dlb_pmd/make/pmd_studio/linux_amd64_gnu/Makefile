#-*-makefile-*-

BASE=../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
CCDEP = $(CC)
CFLAGS_debug = -D_GNU_SOURCE -g -ggdb3 -O0 -m64 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
CFLAGS_release = -D_GNU_SOURCE -O3 -ftree-vectorize -m64 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
DEFINES_release = -DNDEBUG=1
INCLUDES = -I$(BASE)dlb_buffer/1.3 -I$(BASE)dlb_octfile/1.1 -I$(BASE)dlb_pmd/frontend -I$(BASE)dlb_pmd/frontend/pmd_studio -I$(BASE)dlb_pmd/frontend/pmd_tool -I$(BASE)dlb_pmd/frontend -I$(BASE)dlb_pmd/include -I$(BASE)dlb_pmd/os -I$(BASE)dlb_socket/0.1/dlb_socket/include/posix -I$(BASE)dlb_socket/0.1/dlb_socket/include -I$(BASE)dlb_socket/0.1 -I$(BASE)dlb_wave/1.6 -I`pkg-config --cflags gtk+-3.0` -I$(BASE)libui/alpha_4.1/libui -I$(BASE). -I$(BASE). -I$(BASE)portaudio/19.6/portaudio/include
LD = $(CC)
LDFLAGS_debug = -m64
LDFLAGS_release = -O2 -m64
LDLIBS = -lpthread -lgtk-3 -lgdk-3 -lpangocairo-1.0 -lpango-1.0 -latk-1.0 -lcairo-gobject -lcairo -lgdk_pixbuf-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0 -lm -ldl -Wl,--no-undefined -Wl,--no-allow-shlib-undefined -lrt -lasound -ljack -lpthread -lm

COMMON_FILES = $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_beds $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_objects\
               $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_outputs $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_presentations\
               $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_device $(TMP)dlb_pmd/frontend/pmd_tool/pcm $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool\
               $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool_klv $(TMP)dlb_pmd/frontend/pmd_tool/xml $(TMP)dlb_pmd/os/linux/pmd_ctrl_c

INPUTS_COMMON_LINK_debug = $(addsuffix .pmd_studio_linux_amd64_gnu_debug.o,$(COMMON_FILES))
INPUTS_COMMON_LINK_release = $(addsuffix .pmd_studio_linux_amd64_gnu_release.o,$(COMMON_FILES))

DEP_PROJ_LINK_debug = $(BASE)dlb_pmd/make/dlb_pmd_lib/linux_amd64_gnu/dlb_pmd_lib_debug.a $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_amd64_gnu/dlb_xml_debug.a $(BASE)zlib/1.2.11/make/zlib/linux_amd64_gnu/zlib_debug.a $(BASE)dlb_pmd/make/dlb_wave_lib/linux_amd64_gnu/dlb_wave_lib_debug.a $(BASE)libui/alpha_4.1/libui/make/libui_alpha4_1/linux_amd64_gnu/libui_alpha4_1_debug.a $(BASE)libui/alpha_4.1/libui/make/libui_impl_alpha4_1/linux_amd64_gnu/libui_impl_alpha4_1_debug.a $(BASE)portaudio/19.6/portaudio/make/portaudio_19_6_lib/linux_amd64_gnu/portaudio_19_6_lib_debug.a
DEP_PROJ_LINK_release = $(BASE)dlb_pmd/make/dlb_pmd_lib/linux_amd64_gnu/dlb_pmd_lib_release.a $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_amd64_gnu/dlb_xml_release.a $(BASE)zlib/1.2.11/make/zlib/linux_amd64_gnu/zlib_release.a $(BASE)dlb_pmd/make/dlb_wave_lib/linux_amd64_gnu/dlb_wave_lib_release.a $(BASE)libui/alpha_4.1/libui/make/libui_alpha4_1/linux_amd64_gnu/libui_alpha4_1_release.a $(BASE)libui/alpha_4.1/libui/make/libui_impl_alpha4_1/linux_amd64_gnu/libui_impl_alpha4_1_release.a $(BASE)portaudio/19.6/portaudio/make/portaudio_19_6_lib/linux_amd64_gnu/portaudio_19_6_lib_release.a

default: pmd_studio_debug pmd_studio_release

pmd_studio_debug: $(INPUTS_COMMON_LINK_debug) $(DEP_PROJ_LINK_debug)
	$(LD) $(LDFLAGS_debug) -o $@ $+ $(LDLIBS)

pmd_studio_release: $(INPUTS_COMMON_LINK_release) $(DEP_PROJ_LINK_release)
	$(LD) $(LDFLAGS_release) -o $@ $+ $(LDLIBS)

$(TMP)%.pmd_studio_linux_amd64_gnu_debug.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -MM $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_debug) $(INCLUDES) -o $@ $<

$(TMP)%.pmd_studio_linux_amd64_gnu_release.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMMON_COMPILE_debug = $(addsuffix .pmd_studio_linux_amd64_gnu_debug.d,$(COMMON_FILES))
CLEAN_COMMON_COMPILE_release = $(addsuffix .pmd_studio_linux_amd64_gnu_release.d,$(COMMON_FILES))

cleandeps:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)

clean: cleanself
	$(MAKE) -C $(BASE)dlb_pmd/make/dlb_pmd_lib/linux_amd64_gnu cleanself
	$(MAKE) -C $(BASE)dlb_pmd/make/dlb_wave_lib/linux_amd64_gnu cleanself
	$(MAKE) -C $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_amd64_gnu cleanself
	$(MAKE) -C $(BASE)libui/alpha_4.1/libui/make/libui_alpha4_1/linux_amd64_gnu cleanself
	$(MAKE) -C $(BASE)libui/alpha_4.1/libui/make/libui_impl_alpha4_1/linux_amd64_gnu cleanself
	$(MAKE) -C $(BASE)portaudio/19.6/portaudio/make/portaudio_19_6_lib/linux_amd64_gnu cleanself
	$(MAKE) -C $(BASE)zlib/1.2.11/make/zlib/linux_amd64_gnu cleanself

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

include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio.pmd_studio_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_beds.pmd_studio_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_objects.pmd_studio_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_outputs.pmd_studio_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_presentations.pmd_studio_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_device.pmd_studio_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pcm.pmd_studio_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool.pmd_studio_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool_klv.pmd_studio_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/xml.pmd_studio_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/os/linux/pmd_ctrl_c.pmd_studio_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio.pmd_studio_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_beds.pmd_studio_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_objects.pmd_studio_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_outputs.pmd_studio_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_presentations.pmd_studio_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_device.pmd_studio_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pcm.pmd_studio_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool.pmd_studio_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool_klv.pmd_studio_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/xml.pmd_studio_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/os/linux/pmd_ctrl_c.pmd_studio_linux_amd64_gnu_release.d)

$(BASE)dlb_pmd/make/dlb_pmd_lib/linux_amd64_gnu/dlb_pmd_lib_debug.a \
$(BASE)dlb_pmd/make/dlb_pmd_lib/linux_amd64_gnu/dlb_pmd_lib_release.a \
$(BASE)dlb_pmd/make/dlb_wave_lib/linux_amd64_gnu/dlb_wave_lib_debug.a \
$(BASE)dlb_pmd/make/dlb_wave_lib/linux_amd64_gnu/dlb_wave_lib_release.a \
$(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_amd64_gnu/dlb_xml_debug.a \
$(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_amd64_gnu/dlb_xml_release.a \
$(BASE)libui/alpha_4.1/libui/make/libui_alpha4_1/linux_amd64_gnu/libui_alpha4_1_debug.a \
$(BASE)libui/alpha_4.1/libui/make/libui_alpha4_1/linux_amd64_gnu/libui_alpha4_1_release.a \
$(BASE)libui/alpha_4.1/libui/make/libui_impl_alpha4_1/linux_amd64_gnu/libui_impl_alpha4_1_debug.a \
$(BASE)libui/alpha_4.1/libui/make/libui_impl_alpha4_1/linux_amd64_gnu/libui_impl_alpha4_1_release.a \
$(BASE)portaudio/19.6/portaudio/make/portaudio_19_6_lib/linux_amd64_gnu/portaudio_19_6_lib_debug.a \
$(BASE)portaudio/19.6/portaudio/make/portaudio_19_6_lib/linux_amd64_gnu/portaudio_19_6_lib_release.a \
$(BASE)zlib/1.2.11/make/zlib/linux_amd64_gnu/zlib_debug.a \
$(BASE)zlib/1.2.11/make/zlib/linux_amd64_gnu/zlib_release.a: %.a: FORCE
	$(MAKE) -C $(@D) $(@F)


