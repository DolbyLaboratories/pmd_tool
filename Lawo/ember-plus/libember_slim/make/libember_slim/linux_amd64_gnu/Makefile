#-*-makefile-*-

BASE=../../../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
ARFLAGS = -rc
CCDEP = $(CC)
CFLAGS_debug = -g -ggdb3 -O0 -m64 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
CFLAGS_release = -O3 -ftree-vectorize -m64 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
DEFINES_release = -DNDEBUG=1
INCLUDES = -I$(BASE)Lawo/ember-plus/libember_slim/Source

COMMON_FILES = $(TMP)Lawo/ember-plus/libember_slim/Source/ber $(TMP)Lawo/ember-plus/libember_slim/Source/berio $(TMP)Lawo/ember-plus/libember_slim/Source/berreader\
               $(TMP)Lawo/ember-plus/libember_slim/Source/bertag $(TMP)Lawo/ember-plus/libember_slim/Source/bytebuffer $(TMP)Lawo/ember-plus/libember_slim/Source/ember\
               $(TMP)Lawo/ember-plus/libember_slim/Source/emberasyncreader $(TMP)Lawo/ember-plus/libember_slim/Source/emberframing\
               $(TMP)Lawo/ember-plus/libember_slim/Source/emberinternal $(TMP)Lawo/ember-plus/libember_slim/Source/glow\
               $(TMP)Lawo/ember-plus/libember_slim/Source/glowrx $(TMP)Lawo/ember-plus/libember_slim/Source/glowtx

INPUTS_COMMON_ARCHIVE_debug = $(addsuffix .libember_slim_linux_amd64_gnu_debug.o,$(COMMON_FILES))
INPUTS_COMMON_ARCHIVE_release = $(addsuffix .libember_slim_linux_amd64_gnu_release.o,$(COMMON_FILES))


default: libember_slim_debug.a libember_slim_release.a

libember_slim_debug.a: $(INPUTS_COMMON_ARCHIVE_debug)
	$(AR) $(ARFLAGS)  $@ $^

libember_slim_release.a: $(INPUTS_COMMON_ARCHIVE_release)
	$(AR) $(ARFLAGS)  $@ $^

$(TMP)%.libember_slim_linux_amd64_gnu_debug.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -MM $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_debug) $(INCLUDES) -o $@ $<

$(TMP)%.libember_slim_linux_amd64_gnu_release.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMMON_COMPILE_debug = $(addsuffix .libember_slim_linux_amd64_gnu_debug.d,$(COMMON_FILES))
CLEAN_COMMON_COMPILE_release = $(addsuffix .libember_slim_linux_amd64_gnu_release.d,$(COMMON_FILES))

cleandeps:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)

clean: cleanself

cleanself:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)
	$(RM) $(INPUTS_COMMON_ARCHIVE_debug)
	$(RM) $(INPUTS_COMMON_ARCHIVE_release)
	$(RM) libember_slim_debug.a
	$(RM) libember_slim_release.a

.PHONY: default FORCE clean cleandeps cleanself help

FORCE:

help:
	@echo "This makefile has the following targets:"
	@echo " libember_slim_debug.a"
	@echo " libember_slim_release.a"
	@echo " default"
	@echo " clean"
	@echo " cleandeps"
	@echo " help"

include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/ber.libember_slim_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/berio.libember_slim_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/berreader.libember_slim_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/bertag.libember_slim_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/bytebuffer.libember_slim_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/ember.libember_slim_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/emberasyncreader.libember_slim_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/emberframing.libember_slim_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/emberinternal.libember_slim_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/glow.libember_slim_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/glowrx.libember_slim_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/glowtx.libember_slim_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/ber.libember_slim_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/berio.libember_slim_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/berreader.libember_slim_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/bertag.libember_slim_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/bytebuffer.libember_slim_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/ember.libember_slim_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/emberasyncreader.libember_slim_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/emberframing.libember_slim_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/emberinternal.libember_slim_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/glow.libember_slim_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/glowrx.libember_slim_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/glowtx.libember_slim_linux_amd64_gnu_release.d)


