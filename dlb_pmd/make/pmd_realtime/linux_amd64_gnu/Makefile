#-*-makefile-*-

BASE=../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
CCDEP = $(CC)
CFLAGS_debug = -D_GNU_SOURCE -g -ggdb3 -O0 -m64 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
CFLAGS_release = -D_GNU_SOURCE -O3 -ftree-vectorize -m64 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
DEFINES_debug = -DELPP_NO_DEFAULT_LOG_FILE=1
DEFINES_release = -DELPP_NO_DEFAULT_LOG_FILE=1 -DNDEBUG=1
INCLUDES = -I$(BASE)dlb_buffer/1.3 -I$(BASE)dlb_octfile/1.1 -I$(BASE)dlb_pmd/frontend -I$(BASE)dlb_pmd/frontend/pmd_realtime -I$(BASE)dlb_pmd/include -I$(BASE)dlb_pmd/os -I$(BASE)dlb_socket/0.1/dlb_socket/include/posix -I$(BASE)dlb_socket/0.1/dlb_socket/include -I$(BASE)dlb_socket/0.1 -I$(BASE)dlb_wave/1.6 -I$(BASE). -I$(BASE). -I$(BASE)portaudio/19.6/portaudio/include
LD = $(CXX)
LDFLAGS_debug = -m64
LDFLAGS_release = -O2 -m64
LDLIBS = -lpthread -lrt -lasound -ljack -lpthread -lm

COMMON_FILES = $(TMP)dlb_pmd/frontend/pmd_realtime/args $(TMP)dlb_pmd/frontend/pmd_realtime/pa $(TMP)dlb_pmd/frontend/pmd_realtime/pa_capture\
               $(TMP)dlb_pmd/frontend/pmd_realtime/pa_pipe $(TMP)dlb_pmd/frontend/pmd_realtime/pa_play $(TMP)dlb_pmd/frontend/pmd_realtime/pmd_realtime\
               $(TMP)dlb_pmd/os/linux/pmd_ctrl_c

INPUTS_COMMON_LINK_debug = $(addsuffix .pmd_realtime_linux_amd64_gnu_debug.o,$(COMMON_FILES))
INPUTS_COMMON_LINK_release = $(addsuffix .pmd_realtime_linux_amd64_gnu_release.o,$(COMMON_FILES))

DEP_PROJ_LINK_debug = $(BASE)dlb_pmd/make/dlb_pmd_lib/linux_amd64_gnu/dlb_pmd_lib_debug.a $(BASE)dlb_adm/make/dlb_adm_lib/linux_amd64_gnu/dlb_adm_lib_debug.a $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_amd64_gnu/dlb_xml_debug.a $(BASE)zlib/1.2.11/make/zlib/linux_amd64_gnu/zlib_debug.a $(BASE)dlb_pmd/make/dlb_wave_lib/linux_amd64_gnu/dlb_wave_lib_debug.a $(BASE)dlb_socket/0.1/dlb_socket/make/dlb_socket/linux_amd64_gnu/dlb_socket_debug.a $(BASE)portaudio/19.6/portaudio/make/portaudio_19_6_lib/linux_amd64_gnu/portaudio_19_6_lib_debug.a
DEP_PROJ_LINK_release = $(BASE)dlb_pmd/make/dlb_pmd_lib/linux_amd64_gnu/dlb_pmd_lib_release.a $(BASE)dlb_adm/make/dlb_adm_lib/linux_amd64_gnu/dlb_adm_lib_release.a $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_amd64_gnu/dlb_xml_release.a $(BASE)zlib/1.2.11/make/zlib/linux_amd64_gnu/zlib_release.a $(BASE)dlb_pmd/make/dlb_wave_lib/linux_amd64_gnu/dlb_wave_lib_release.a $(BASE)dlb_socket/0.1/dlb_socket/make/dlb_socket/linux_amd64_gnu/dlb_socket_release.a $(BASE)portaudio/19.6/portaudio/make/portaudio_19_6_lib/linux_amd64_gnu/portaudio_19_6_lib_release.a

default: pmd_realtime_debug pmd_realtime_release

pmd_realtime_debug: $(INPUTS_COMMON_LINK_debug) $(DEP_PROJ_LINK_debug)
	$(LD) $(LDFLAGS_debug) -o $@ $+ $(LDLIBS)

pmd_realtime_release: $(INPUTS_COMMON_LINK_release) $(DEP_PROJ_LINK_release)
	$(LD) $(LDFLAGS_release) -o $@ $+ $(LDLIBS)

$(TMP)%.pmd_realtime_linux_amd64_gnu_debug.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -MM $(DEFINES_debug) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_debug) $(DEFINES_debug) $(INCLUDES) -o $@ $<

$(TMP)%.pmd_realtime_linux_amd64_gnu_release.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMMON_COMPILE_debug = $(addsuffix .pmd_realtime_linux_amd64_gnu_debug.d,$(COMMON_FILES))
CLEAN_COMMON_COMPILE_release = $(addsuffix .pmd_realtime_linux_amd64_gnu_release.d,$(COMMON_FILES))

cleandeps:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)

clean: cleanself
	$(MAKE) -C $(BASE)dlb_adm/make/dlb_adm_lib/linux_amd64_gnu cleanself
	$(MAKE) -C $(BASE)dlb_pmd/make/dlb_pmd_lib/linux_amd64_gnu cleanself
	$(MAKE) -C $(BASE)dlb_pmd/make/dlb_wave_lib/linux_amd64_gnu cleanself
	$(MAKE) -C $(BASE)dlb_socket/0.1/dlb_socket/make/dlb_socket/linux_amd64_gnu cleanself
	$(MAKE) -C $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_amd64_gnu cleanself
	$(MAKE) -C $(BASE)portaudio/19.6/portaudio/make/portaudio_19_6_lib/linux_amd64_gnu cleanself
	$(MAKE) -C $(BASE)zlib/1.2.11/make/zlib/linux_amd64_gnu cleanself

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

include $(wildcard $(TMP)dlb_pmd/frontend/pmd_realtime/args.pmd_realtime_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_realtime/pa.pmd_realtime_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_realtime/pa_capture.pmd_realtime_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_realtime/pa_pipe.pmd_realtime_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_realtime/pa_play.pmd_realtime_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_realtime/pmd_realtime.pmd_realtime_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/os/linux/pmd_ctrl_c.pmd_realtime_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_realtime/args.pmd_realtime_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_realtime/pa.pmd_realtime_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_realtime/pa_capture.pmd_realtime_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_realtime/pa_pipe.pmd_realtime_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_realtime/pa_play.pmd_realtime_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_realtime/pmd_realtime.pmd_realtime_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/os/linux/pmd_ctrl_c.pmd_realtime_linux_amd64_gnu_release.d)

$(BASE)dlb_adm/make/dlb_adm_lib/linux_amd64_gnu/dlb_adm_lib_debug.a \
$(BASE)dlb_adm/make/dlb_adm_lib/linux_amd64_gnu/dlb_adm_lib_release.a \
$(BASE)dlb_pmd/make/dlb_pmd_lib/linux_amd64_gnu/dlb_pmd_lib_debug.a \
$(BASE)dlb_pmd/make/dlb_pmd_lib/linux_amd64_gnu/dlb_pmd_lib_release.a \
$(BASE)dlb_pmd/make/dlb_wave_lib/linux_amd64_gnu/dlb_wave_lib_debug.a \
$(BASE)dlb_pmd/make/dlb_wave_lib/linux_amd64_gnu/dlb_wave_lib_release.a \
$(BASE)dlb_socket/0.1/dlb_socket/make/dlb_socket/linux_amd64_gnu/dlb_socket_debug.a \
$(BASE)dlb_socket/0.1/dlb_socket/make/dlb_socket/linux_amd64_gnu/dlb_socket_release.a \
$(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_amd64_gnu/dlb_xml_debug.a \
$(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_amd64_gnu/dlb_xml_release.a \
$(BASE)portaudio/19.6/portaudio/make/portaudio_19_6_lib/linux_amd64_gnu/portaudio_19_6_lib_debug.a \
$(BASE)portaudio/19.6/portaudio/make/portaudio_19_6_lib/linux_amd64_gnu/portaudio_19_6_lib_release.a \
$(BASE)zlib/1.2.11/make/zlib/linux_amd64_gnu/zlib_debug.a \
$(BASE)zlib/1.2.11/make/zlib/linux_amd64_gnu/zlib_release.a: %.a: FORCE
	$(MAKE) -C $(@D) $(@F)


