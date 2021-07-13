#-*-makefile-*-

BASE=../../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
ARFLAGS = -rc
CCDEP = $(CC)
CFLAGS_debug = -g -ggdb3 -O0 -m64 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
CFLAGS_release = -O3 -ftree-vectorize -m64 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
DEFINES_debug = -DELPP_NO_DEFAULT_LOG_FILE=1 -DHAVE_HIDDEN=1 -D_LARGEFILE64_SOURCE=1
DEFINES_release = -DELPP_NO_DEFAULT_LOG_FILE=1 -DHAVE_HIDDEN=1 -DNDEBUG=1 -D_LARGEFILE64_SOURCE=1
INCLUDES = -I$(BASE)zlib/1.2.11/zlib-1.2.11

COMMON_FILES = $(TMP)zlib/1.2.11/zlib-1.2.11/adler32 $(TMP)zlib/1.2.11/zlib-1.2.11/compress $(TMP)zlib/1.2.11/zlib-1.2.11/crc32\
               $(TMP)zlib/1.2.11/zlib-1.2.11/deflate $(TMP)zlib/1.2.11/zlib-1.2.11/gzclose $(TMP)zlib/1.2.11/zlib-1.2.11/gzlib\
               $(TMP)zlib/1.2.11/zlib-1.2.11/gzread $(TMP)zlib/1.2.11/zlib-1.2.11/gzwrite $(TMP)zlib/1.2.11/zlib-1.2.11/infback\
               $(TMP)zlib/1.2.11/zlib-1.2.11/inffast $(TMP)zlib/1.2.11/zlib-1.2.11/inflate $(TMP)zlib/1.2.11/zlib-1.2.11/inftrees\
               $(TMP)zlib/1.2.11/zlib-1.2.11/trees $(TMP)zlib/1.2.11/zlib-1.2.11/uncompr $(TMP)zlib/1.2.11/zlib-1.2.11/zutil

INPUTS_COMMON_ARCHIVE_debug = $(addsuffix .zlib_linux_amd64_gnu_debug.o,$(COMMON_FILES))
INPUTS_COMMON_ARCHIVE_release = $(addsuffix .zlib_linux_amd64_gnu_release.o,$(COMMON_FILES))


default: zlib_debug.a zlib_release.a

zlib_debug.a: $(INPUTS_COMMON_ARCHIVE_debug)
	$(AR) $(ARFLAGS)  $@ $^

zlib_release.a: $(INPUTS_COMMON_ARCHIVE_release)
	$(AR) $(ARFLAGS)  $@ $^

$(TMP)%.zlib_linux_amd64_gnu_debug.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -MM $(DEFINES_debug) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_debug) $(DEFINES_debug) $(INCLUDES) -o $@ $<

$(TMP)%.zlib_linux_amd64_gnu_release.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMMON_COMPILE_debug = $(addsuffix .zlib_linux_amd64_gnu_debug.d,$(COMMON_FILES))
CLEAN_COMMON_COMPILE_release = $(addsuffix .zlib_linux_amd64_gnu_release.d,$(COMMON_FILES))

cleandeps:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)

clean: cleanself

cleanself:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)
	$(RM) $(INPUTS_COMMON_ARCHIVE_debug)
	$(RM) $(INPUTS_COMMON_ARCHIVE_release)
	$(RM) zlib_debug.a
	$(RM) zlib_release.a

.PHONY: default FORCE clean cleandeps cleanself help

FORCE:

help:
	@echo "This makefile has the following targets:"
	@echo " zlib_debug.a"
	@echo " zlib_release.a"
	@echo " default"
	@echo " clean"
	@echo " cleandeps"
	@echo " help"

include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/adler32.zlib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/compress.zlib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/crc32.zlib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/deflate.zlib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/gzclose.zlib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/gzlib.zlib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/gzread.zlib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/gzwrite.zlib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/infback.zlib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/inffast.zlib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/inflate.zlib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/inftrees.zlib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/trees.zlib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/uncompr.zlib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/zutil.zlib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/adler32.zlib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/compress.zlib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/crc32.zlib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/deflate.zlib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/gzclose.zlib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/gzlib.zlib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/gzread.zlib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/gzwrite.zlib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/infback.zlib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/inffast.zlib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/inflate.zlib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/inftrees.zlib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/trees.zlib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/uncompr.zlib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/zutil.zlib_linux_amd64_gnu_release.d)


