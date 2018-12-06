#-*-makefile-*-

BASE=../../../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
ARFLAGS = -rc
CC = gcc
CCDEP = $(CC)
CFLAGS_debug = -g -ggdb3 -O0 -m64 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
CFLAGS_release = -O3 -ftree-vectorize -m64 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
DEFINES_debug = -DDLB_BITBUF_USE_READ_API=1 -DDLB_BITBUF_USE_WRITE_API=1
DEFINES_release = -DDLB_BITBUF_USE_READ_API=1 -DDLB_BITBUF_USE_WRITE_API=1 -DNDEBUG=1
INCLUDES = -I$(BASE)dlb_bitbuf/2.0/dlb_bitbuf/include -I$(BASE)dlb_bitbuf/2.0 -I$(BASE)dlb_buffer/1.3 -I$(BASE)dlb_octfile/1.1 -I$(BASE).

COMMON_FILES = $(TMP)dlb_bitbuf/2.0/dlb_bitbuf/src/bitbuf_common $(TMP)dlb_bitbuf/2.0/dlb_bitbuf/src/bitbuf_read $(TMP)dlb_bitbuf/2.0/dlb_bitbuf/src/bitbuf_write

INPUTS_COMMON_ARCHIVE_debug = $(addsuffix .dlb_bitbuf_lib_osx_amd64_gnu_debug.o,$(COMMON_FILES))
INPUTS_COMMON_ARCHIVE_release = $(addsuffix .dlb_bitbuf_lib_osx_amd64_gnu_release.o,$(COMMON_FILES))


default: dlb_bitbuf_lib_debug.a dlb_bitbuf_lib_release.a

dlb_bitbuf_lib_debug.a: $(INPUTS_COMMON_ARCHIVE_debug)
	$(AR) $(ARFLAGS)  $@ $^

dlb_bitbuf_lib_release.a: $(INPUTS_COMMON_ARCHIVE_release)
	$(AR) $(ARFLAGS)  $@ $^

$(TMP)%.dlb_bitbuf_lib_osx_amd64_gnu_debug.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -MM $(DEFINES_debug) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_debug) $(DEFINES_debug) $(INCLUDES) -o $@ $<

$(TMP)%.dlb_bitbuf_lib_osx_amd64_gnu_release.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMMON_COMPILE_debug = $(addsuffix .dlb_bitbuf_lib_osx_amd64_gnu_debug.d,$(COMMON_FILES))
CLEAN_COMMON_COMPILE_release = $(addsuffix .dlb_bitbuf_lib_osx_amd64_gnu_release.d,$(COMMON_FILES))

cleandeps:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)

clean: cleanself

cleanself:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)
	$(RM) $(INPUTS_COMMON_ARCHIVE_debug)
	$(RM) $(INPUTS_COMMON_ARCHIVE_release)
	$(RM) dlb_bitbuf_lib_debug.a
	$(RM) dlb_bitbuf_lib_release.a

.PHONY: default FORCE clean cleandeps cleanself help

FORCE:

help:
	@echo "This makefile has the following targets:"
	@echo " dlb_bitbuf_lib_debug.a"
	@echo " dlb_bitbuf_lib_release.a"
	@echo " default"
	@echo " clean"
	@echo " cleandeps"
	@echo " help"

include $(wildcard $(TMP)dlb_bitbuf/2.0/dlb_bitbuf/src/bitbuf_common.dlb_bitbuf_lib_osx_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_bitbuf/2.0/dlb_bitbuf/src/bitbuf_read.dlb_bitbuf_lib_osx_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_bitbuf/2.0/dlb_bitbuf/src/bitbuf_write.dlb_bitbuf_lib_osx_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_bitbuf/2.0/dlb_bitbuf/src/bitbuf_common.dlb_bitbuf_lib_osx_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_bitbuf/2.0/dlb_bitbuf/src/bitbuf_read.dlb_bitbuf_lib_osx_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_bitbuf/2.0/dlb_bitbuf/src/bitbuf_write.dlb_bitbuf_lib_osx_amd64_gnu_release.d)


