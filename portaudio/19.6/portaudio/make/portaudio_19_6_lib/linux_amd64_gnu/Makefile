#-*-makefile-*-

BASE=../../../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
ARFLAGS = -rc
CCDEP = $(CC)
CFLAGS_debug = -g -ggdb3 -O0 -m64 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
CFLAGS_release = -O3 -ftree-vectorize -m64 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
DEFINES_debug = -DDHAVE_STRING_H=1 -DDSTDC_HEADERS=1 -DHAVE_CLOCK_GETTIME=1 -DHAVE_DLFCN_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_LINUX_SOUNDCARD_H=1 -DHAVE_MEMORY_H=1 -DHAVE_NANOSLEEP=1 -DHAVE_STDINT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRINGS_H=1 -DHAVE_SYS_SOUNDCARD_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_UNISTD_H=1 -DPACKAGE_BUGREPORT='""' -DPACKAGE_NAME='""' -DPACKAGE_STRING='""' -DPACKAGE_TARNAME='""' -DPACKAGE_URL='""' -DPACKAGE_VERSION='""' -DPA_LITTLE_ENDIAN=1 -DPA_USE_ALSA=1 -DPA_USE_CALLBACK=1 -DPA_USE_JACK=1 -DPA_USE_OSS=1 -DSIZEOF_INT=4 -DSIZEOF_LONG=8 -DSIZEOF_SHORT=2
DEFINES_release = -DDHAVE_STRING_H=1 -DDSTDC_HEADERS=1 -DHAVE_CLOCK_GETTIME=1 -DHAVE_DLFCN_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_LINUX_SOUNDCARD_H=1 -DHAVE_MEMORY_H=1 -DHAVE_NANOSLEEP=1 -DHAVE_STDINT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRINGS_H=1 -DHAVE_SYS_SOUNDCARD_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_UNISTD_H=1 -DNDEBUG=1 -DPACKAGE_BUGREPORT='""' -DPACKAGE_NAME='""' -DPACKAGE_STRING='""' -DPACKAGE_TARNAME='""' -DPACKAGE_URL='""' -DPACKAGE_VERSION='""' -DPA_LITTLE_ENDIAN=1 -DPA_USE_ALSA=1 -DPA_USE_CALLBACK=1 -DPA_USE_JACK=1 -DPA_USE_OSS=1 -DSIZEOF_INT=4 -DSIZEOF_LONG=8 -DSIZEOF_SHORT=2
INCLUDES = -I$(BASE)portaudio/19.6/portaudio/include -I$(BASE)portaudio/19.6/portaudio/src/os/unix -I$(BASE)portaudio/19.6/portaudio -I$(BASE)portaudio/19.6/portaudio/src/common -I$(BASE)portaudio/19.6/portaudio/src/common

COMMON_FILES = $(TMP)portaudio/19.6/portaudio/src/common/pa_allocation $(TMP)portaudio/19.6/portaudio/src/common/pa_converters\
               $(TMP)portaudio/19.6/portaudio/src/common/pa_cpuload $(TMP)portaudio/19.6/portaudio/src/common/pa_debugprint\
               $(TMP)portaudio/19.6/portaudio/src/common/pa_dither $(TMP)portaudio/19.6/portaudio/src/common/pa_front $(TMP)portaudio/19.6/portaudio/src/common/pa_process\
               $(TMP)portaudio/19.6/portaudio/src/common/pa_ringbuffer $(TMP)portaudio/19.6/portaudio/src/common/pa_stream\
               $(TMP)portaudio/19.6/portaudio/src/common/pa_trace $(TMP)portaudio/19.6/portaudio/src/hostapi/alsa/pa_linux_alsa\
               $(TMP)portaudio/19.6/portaudio/src/hostapi/jack/pa_jack $(TMP)portaudio/19.6/portaudio/src/hostapi/oss/pa_unix_oss\
               $(TMP)portaudio/19.6/portaudio/src/os/unix/pa_unix_hostapis $(TMP)portaudio/19.6/portaudio/src/os/unix/pa_unix_util

INPUTS_COMMON_ARCHIVE_debug = $(addsuffix .portaudio_19_6_lib_linux_amd64_gnu_debug.o,$(COMMON_FILES))
INPUTS_COMMON_ARCHIVE_release = $(addsuffix .portaudio_19_6_lib_linux_amd64_gnu_release.o,$(COMMON_FILES))


default: portaudio_19_6_lib_debug.a portaudio_19_6_lib_release.a

portaudio_19_6_lib_debug.a: $(INPUTS_COMMON_ARCHIVE_debug)
	$(AR) $(ARFLAGS)  $@ $^

portaudio_19_6_lib_release.a: $(INPUTS_COMMON_ARCHIVE_release)
	$(AR) $(ARFLAGS)  $@ $^

$(TMP)%.portaudio_19_6_lib_linux_amd64_gnu_debug.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -MM $(DEFINES_debug) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_debug) $(DEFINES_debug) $(INCLUDES) -o $@ $<

$(TMP)%.portaudio_19_6_lib_linux_amd64_gnu_release.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMMON_COMPILE_debug = $(addsuffix .portaudio_19_6_lib_linux_amd64_gnu_debug.d,$(COMMON_FILES))
CLEAN_COMMON_COMPILE_release = $(addsuffix .portaudio_19_6_lib_linux_amd64_gnu_release.d,$(COMMON_FILES))

cleandeps:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)

clean: cleanself

cleanself:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)
	$(RM) $(INPUTS_COMMON_ARCHIVE_debug)
	$(RM) $(INPUTS_COMMON_ARCHIVE_release)
	$(RM) portaudio_19_6_lib_debug.a
	$(RM) portaudio_19_6_lib_release.a

.PHONY: default FORCE clean cleandeps cleanself help

FORCE:

help:
	@echo "This makefile has the following targets:"
	@echo " portaudio_19_6_lib_debug.a"
	@echo " portaudio_19_6_lib_release.a"
	@echo " default"
	@echo " clean"
	@echo " cleandeps"
	@echo " help"

include $(wildcard $(TMP)portaudio/19.6/portaudio/src/common/pa_allocation.portaudio_19_6_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/common/pa_converters.portaudio_19_6_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/common/pa_cpuload.portaudio_19_6_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/common/pa_debugprint.portaudio_19_6_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/common/pa_dither.portaudio_19_6_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/common/pa_front.portaudio_19_6_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/common/pa_process.portaudio_19_6_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/common/pa_ringbuffer.portaudio_19_6_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/common/pa_stream.portaudio_19_6_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/common/pa_trace.portaudio_19_6_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/hostapi/alsa/pa_linux_alsa.portaudio_19_6_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/hostapi/jack/pa_jack.portaudio_19_6_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/hostapi/oss/pa_unix_oss.portaudio_19_6_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/os/unix/pa_unix_hostapis.portaudio_19_6_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/os/unix/pa_unix_util.portaudio_19_6_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/common/pa_allocation.portaudio_19_6_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/common/pa_converters.portaudio_19_6_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/common/pa_cpuload.portaudio_19_6_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/common/pa_debugprint.portaudio_19_6_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/common/pa_dither.portaudio_19_6_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/common/pa_front.portaudio_19_6_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/common/pa_process.portaudio_19_6_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/common/pa_ringbuffer.portaudio_19_6_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/common/pa_stream.portaudio_19_6_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/common/pa_trace.portaudio_19_6_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/hostapi/alsa/pa_linux_alsa.portaudio_19_6_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/hostapi/jack/pa_jack.portaudio_19_6_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/hostapi/oss/pa_unix_oss.portaudio_19_6_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/os/unix/pa_unix_hostapis.portaudio_19_6_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)portaudio/19.6/portaudio/src/os/unix/pa_unix_util.portaudio_19_6_lib_linux_amd64_gnu_release.d)


