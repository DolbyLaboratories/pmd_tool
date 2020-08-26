#-*-makefile-*-

BASE=../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
ARFLAGS = -rc
CCDEP = $(CC)
CFLAGS_debug = -msse2 -g -ggdb3 -O0 -m32 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
CFLAGS_release = -msse2 -O3 -ftree-vectorize -m32 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
DEFINES_release = -DNDEBUG=1
INCLUDES = -I$(BASE)dlb_buffer/1.3 -I$(BASE)dlb_buffer/1.3 -I$(BASE)dlb_octfile/1.1 -I$(BASE)dlb_wave/1.6 -I$(BASE).

COMMON_FILES = $(TMP)dlb_octfile/1.1/dlb_octfile/src/dlb_octfile $(TMP)dlb_octfile/1.1/dlb_octfile/src/dlb_octfile_disk $(TMP)dlb_octfile/1.1/dlb_octfile/src/dlb_octfile_memory\
               $(TMP)dlb_wave/1.6/dlb_wave/src/dlb_riff $(TMP)dlb_wave/1.6/dlb_wave/src/dlb_wave $(TMP)dlb_wave/1.6/dlb_wave/src/dlb_wave_float\
               $(TMP)dlb_wave/1.6/dlb_wave/src/dlb_wave_int

INPUTS_COMMON_ARCHIVE_debug = $(addsuffix .dlb_wave_lib_linux_x86_gnu_debug.o,$(COMMON_FILES))
INPUTS_COMMON_ARCHIVE_release = $(addsuffix .dlb_wave_lib_linux_x86_gnu_release.o,$(COMMON_FILES))


default: dlb_wave_lib_debug.a dlb_wave_lib_release.a

dlb_wave_lib_debug.a: $(INPUTS_COMMON_ARCHIVE_debug)
	$(AR) $(ARFLAGS)  $@ $^

dlb_wave_lib_release.a: $(INPUTS_COMMON_ARCHIVE_release)
	$(AR) $(ARFLAGS)  $@ $^

$(TMP)%.dlb_wave_lib_linux_x86_gnu_debug.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -msse2 -MM $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_debug) $(INCLUDES) -o $@ $<

$(TMP)%.dlb_wave_lib_linux_x86_gnu_release.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -msse2 -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMMON_COMPILE_debug = $(addsuffix .dlb_wave_lib_linux_x86_gnu_debug.d,$(COMMON_FILES))
CLEAN_COMMON_COMPILE_release = $(addsuffix .dlb_wave_lib_linux_x86_gnu_release.d,$(COMMON_FILES))

cleandeps:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)

clean: cleanself

cleanself:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)
	$(RM) $(INPUTS_COMMON_ARCHIVE_debug)
	$(RM) $(INPUTS_COMMON_ARCHIVE_release)
	$(RM) dlb_wave_lib_debug.a
	$(RM) dlb_wave_lib_release.a

.PHONY: default FORCE clean cleandeps cleanself help

FORCE:

help:
	@echo "This makefile has the following targets:"
	@echo " dlb_wave_lib_debug.a"
	@echo " dlb_wave_lib_release.a"
	@echo " default"
	@echo " clean"
	@echo " cleandeps"
	@echo " help"

include $(wildcard $(TMP)dlb_octfile/1.1/dlb_octfile/src/dlb_octfile.dlb_wave_lib_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_octfile/1.1/dlb_octfile/src/dlb_octfile_disk.dlb_wave_lib_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_octfile/1.1/dlb_octfile/src/dlb_octfile_memory.dlb_wave_lib_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_wave/1.6/dlb_wave/src/dlb_riff.dlb_wave_lib_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_wave/1.6/dlb_wave/src/dlb_wave.dlb_wave_lib_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_wave/1.6/dlb_wave/src/dlb_wave_float.dlb_wave_lib_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_wave/1.6/dlb_wave/src/dlb_wave_int.dlb_wave_lib_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_octfile/1.1/dlb_octfile/src/dlb_octfile.dlb_wave_lib_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_octfile/1.1/dlb_octfile/src/dlb_octfile_disk.dlb_wave_lib_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_octfile/1.1/dlb_octfile/src/dlb_octfile_memory.dlb_wave_lib_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_wave/1.6/dlb_wave/src/dlb_riff.dlb_wave_lib_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_wave/1.6/dlb_wave/src/dlb_wave.dlb_wave_lib_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_wave/1.6/dlb_wave/src/dlb_wave_float.dlb_wave_lib_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_wave/1.6/dlb_wave/src/dlb_wave_int.dlb_wave_lib_linux_x86_gnu_release.d)


