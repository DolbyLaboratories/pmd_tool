#-*-makefile-*-

BASE=../../../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
ARFLAGS = -rc
CCDEP = $(CC)
CFLAGS_debug = -g -ggdb3 -O0 -m64 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
CFLAGS_release = -O3 -ftree-vectorize -m64 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
DEFINES_debug = -DPACKAGE_BUGREPORT='""' -DPACKAGE_NAME='""' -DPACKAGE_STRING='""' -DPACKAGE_TARNAME='""' -DPACKAGE_URL='""' -DPACKAGE_VERSION='""' -DPAWIN_USE_WDMKS_DEVICE_INFO=1 -DPA_LITTLE_ENDIAN=1 -DPA_USE_CALLBACK=1 -DPA_USE_DS=0 -DPA_USE_WASAPI=1 -DPA_USE_WDMKS=1 -DPA_USE_WMME=1 -DSIZEOF_INT=4 -DSIZEOF_LONG=4 -DSIZEOF_SHORT=2 -DWIN32=1 -DWIN64=1 -D_LIB=1
DEFINES_release = -DNDEBUG=1 -DPACKAGE_BUGREPORT='""' -DPACKAGE_NAME='""' -DPACKAGE_STRING='""' -DPACKAGE_TARNAME='""' -DPACKAGE_URL='""' -DPACKAGE_VERSION='""' -DPAWIN_USE_WDMKS_DEVICE_INFO=1 -DPA_LITTLE_ENDIAN=1 -DPA_USE_CALLBACK=1 -DPA_USE_DS=0 -DPA_USE_WASAPI=1 -DPA_USE_WDMKS=1 -DPA_USE_WMME=1 -DSIZEOF_INT=4 -DSIZEOF_LONG=4 -DSIZEOF_SHORT=2 -DWIN32=1 -DWIN64=1 -D_LIB=1
INCLUDES = -I$(BASE)portaudio/19.6/portaudio/include -I$(BASE)portaudio/19.6/portaudio/src/os/win -I$(BASE)portaudio/19.6/portaudio -I$(BASE)portaudio/19.6/portaudio/src/common -I$(BASE)portaudio/19.6/portaudio/src/common

COMMON_FILES = $(TMP)portaudio/19.6/portaudio/src/common/pa_allocation $(TMP)portaudio/19.6/portaudio/src/common/pa_converters\
               $(TMP)portaudio/19.6/portaudio/src/common/pa_cpuload $(TMP)portaudio/19.6/portaudio/src/common/pa_debugprint\
               $(TMP)portaudio/19.6/portaudio/src/common/pa_dither $(TMP)portaudio/19.6/portaudio/src/common/pa_front $(TMP)portaudio/19.6/portaudio/src/common/pa_process\
               $(TMP)portaudio/19.6/portaudio/src/common/pa_ringbuffer $(TMP)portaudio/19.6/portaudio/src/common/pa_stream\
               $(TMP)portaudio/19.6/portaudio/src/common/pa_trace $(TMP)portaudio/19.6/portaudio/src/hostapi/dsound/pa_win_ds\
               $(TMP)portaudio/19.6/portaudio/src/hostapi/dsound/pa_win_ds_dynlink $(TMP)portaudio/19.6/portaudio/src/hostapi/wasapi/pa_win_wasapi\
               $(TMP)portaudio/19.6/portaudio/src/hostapi/wdmks/pa_win_wdmks $(TMP)portaudio/19.6/portaudio/src/hostapi/wmme/pa_win_wmme\
               $(TMP)portaudio/19.6/portaudio/src/os/win/pa_win_coinitialize $(TMP)portaudio/19.6/portaudio/src/os/win/pa_win_hostapis\
               $(TMP)portaudio/19.6/portaudio/src/os/win/pa_win_util $(TMP)portaudio/19.6/portaudio/src/os/win/pa_win_waveformat\
               $(TMP)portaudio/19.6/portaudio/src/os/win/pa_win_wdmks_utils $(TMP)portaudio/19.6/portaudio/src/os/win/pa_x86_plain_converters

INPUTS_COMMON_ARCHIVE_debug = $(addsuffix .portaudio_19_6_lib_windows_amd64_gnu_debug.o,$(COMMON_FILES))
INPUTS_COMMON_ARCHIVE_release = $(addsuffix .portaudio_19_6_lib_windows_amd64_gnu_release.o,$(COMMON_FILES))


default: portaudio_19_6_lib_debug.lib portaudio_19_6_lib_release.lib

portaudio_19_6_lib_debug.lib: $(INPUTS_COMMON_ARCHIVE_debug)
	$(AR) $(ARFLAGS)  $@ $^

portaudio_19_6_lib_release.lib: $(INPUTS_COMMON_ARCHIVE_release)
	$(AR) $(ARFLAGS)  $@ $^

$(TMP)%.portaudio_19_6_lib_windows_amd64_gnu_debug.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -MM $(DEFINES_debug) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_debug) $(DEFINES_debug) $(INCLUDES) -o $@ $<

$(TMP)%.portaudio_19_6_lib_windows_amd64_gnu_release.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMMON_COMPILE_debug = $(addsuffix .portaudio_19_6_lib_windows_amd64_gnu_debug.d,$(COMMON_FILES))
CLEAN_COMMON_COMPILE_release = $(addsuffix .portaudio_19_6_lib_windows_amd64_gnu_release.d,$(COMMON_FILES))

cleandeps:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)

clean: cleanself

cleanself:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)
	$(RM) $(INPUTS_COMMON_ARCHIVE_debug)
	$(RM) $(INPUTS_COMMON_ARCHIVE_release)
	$(RM) portaudio_19_6_lib_debug.lib
	$(RM) portaudio_19_6_lib_release.lib

.PHONY: default FORCE clean cleandeps cleanself help

FORCE:

help:
	@echo "This makefile has the following targets:"
	@echo " portaudio_19_6_lib_debug.lib"
	@echo " portaudio_19_6_lib_release.lib"
	@echo " default"
	@echo " clean"
	@echo " cleandeps"
	@echo " help"

include $(wildcard $(TMP)portaudio/19.6/portaudio/src/common/pa_allocation.portaudio_19_6_lib_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/common/pa_converters.portaudio_19_6_lib_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/common/pa_cpuload.portaudio_19_6_lib_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/common/pa_debugprint.portaudio_19_6_lib_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/common/pa_dither.portaudio_19_6_lib_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/common/pa_front.portaudio_19_6_lib_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/common/pa_process.portaudio_19_6_lib_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/common/pa_ringbuffer.portaudio_19_6_lib_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/common/pa_stream.portaudio_19_6_lib_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/common/pa_trace.portaudio_19_6_lib_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/hostapi/dsound/pa_win_ds.portaudio_19_6_lib_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/hostapi/dsound/pa_win_ds_dynlink.portaudio_19_6_lib_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/hostapi/wasapi/pa_win_wasapi.portaudio_19_6_lib_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/hostapi/wdmks/pa_win_wdmks.portaudio_19_6_lib_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/hostapi/wmme/pa_win_wmme.portaudio_19_6_lib_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/os/win/pa_win_coinitialize.portaudio_19_6_lib_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/os/win/pa_win_hostapis.portaudio_19_6_lib_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/os/win/pa_win_util.portaudio_19_6_lib_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/os/win/pa_win_waveformat.portaudio_19_6_lib_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/os/win/pa_win_wdmks_utils.portaudio_19_6_lib_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/os/win/pa_x86_plain_converters.portaudio_19_6_lib_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/common/pa_allocation.portaudio_19_6_lib_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/common/pa_converters.portaudio_19_6_lib_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/common/pa_cpuload.portaudio_19_6_lib_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/common/pa_debugprint.portaudio_19_6_lib_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/common/pa_dither.portaudio_19_6_lib_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/common/pa_front.portaudio_19_6_lib_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/common/pa_process.portaudio_19_6_lib_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/common/pa_ringbuffer.portaudio_19_6_lib_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/common/pa_stream.portaudio_19_6_lib_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/common/pa_trace.portaudio_19_6_lib_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/hostapi/dsound/pa_win_ds.portaudio_19_6_lib_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/hostapi/dsound/pa_win_ds_dynlink.portaudio_19_6_lib_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/hostapi/wasapi/pa_win_wasapi.portaudio_19_6_lib_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/hostapi/wdmks/pa_win_wdmks.portaudio_19_6_lib_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/hostapi/wmme/pa_win_wmme.portaudio_19_6_lib_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/os/win/pa_win_coinitialize.portaudio_19_6_lib_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/os/win/pa_win_hostapis.portaudio_19_6_lib_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/os/win/pa_win_util.portaudio_19_6_lib_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/os/win/pa_win_waveformat.portaudio_19_6_lib_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/os/win/pa_win_wdmks_utils.portaudio_19_6_lib_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/os/win/pa_x86_plain_converters.portaudio_19_6_lib_windows_amd64_gnu_release.d)


