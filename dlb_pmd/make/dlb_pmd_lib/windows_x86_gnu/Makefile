#-*-makefile-*-

BASE=../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
ARFLAGS = -rc
CCDEP = $(CC)
CFLAGS_debug = -msse2 -g -ggdb3 -O0 -m32 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
CFLAGS_release = -msse2 -O3 -ftree-vectorize -m32 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
DEFINES_debug = -DWIN32=1 -D_LIB=1
DEFINES_release = -DNDEBUG=1 -DWIN32=1 -D_LIB=1
INCLUDES = -I$(BASE)dlb_pmd/include -I$(BASE)dlb_pmd -I$(BASE)dlb_pmd/src/model -I$(BASE)dlb_pmd/src/modules/klv -I$(BASE)dlb_pmd/src/modules/pcm -I$(BASE)dlb_pmd/src/modules/xml -I$(BASE)dlb_xmllib/1.0.4 -I$(BASE).

COMMON_FILES = $(TMP)dlb_pmd/src/api/dlb_pmd $(TMP)dlb_pmd/src/api/dlb_pmd_api_read $(TMP)dlb_pmd/src/api/dlb_pmd_api_write\
               $(TMP)dlb_pmd/src/api/dlb_pmd_equal $(TMP)dlb_pmd/src/api/dlb_pmd_metadata_set $(TMP)dlb_pmd/src/model/pmd_language\
               $(TMP)dlb_pmd/src/modules/generate/pmd_random_generator $(TMP)dlb_pmd/src/modules/klv/pmd_crc32 $(TMP)dlb_pmd/src/modules/klv/pmd_klv_reader\
               $(TMP)dlb_pmd/src/modules/klv/pmd_klv_writer $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_augmentor $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_extractor\
               $(TMP)dlb_pmd/src/modules/pcm/pmd_smpte_337m $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_file $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_string\
               $(TMP)dlb_pmd/src/modules/xml/pmd_xml_reader $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_file $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_string\
               $(TMP)dlb_pmd/src/modules/xml/pmd_xml_writer

INPUTS_COMMON_ARCHIVE_debug = $(addsuffix .dlb_pmd_lib_windows_x86_gnu_debug.o,$(COMMON_FILES))
INPUTS_COMMON_ARCHIVE_release = $(addsuffix .dlb_pmd_lib_windows_x86_gnu_release.o,$(COMMON_FILES))

DEP_PROJ_ARCHIVE_debug = $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/windows_x86_gnu/dlb_xml_debug.lib
DEP_PROJ_ARCHIVE_release = $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/windows_x86_gnu/dlb_xml_release.lib

default: dlb_pmd_lib_debug.lib dlb_pmd_lib_release.lib

dlb_pmd_lib_debug.lib: $(INPUTS_COMMON_ARCHIVE_debug)
	$(AR) $(ARFLAGS)  $@ $^

dlb_pmd_lib_release.lib: $(INPUTS_COMMON_ARCHIVE_release)
	$(AR) $(ARFLAGS)  $@ $^

$(TMP)%.dlb_pmd_lib_windows_x86_gnu_debug.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -msse2 -MM $(DEFINES_debug) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_debug) $(DEFINES_debug) $(INCLUDES) -o $@ $<

$(TMP)%.dlb_pmd_lib_windows_x86_gnu_release.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -msse2 -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMMON_COMPILE_debug = $(addsuffix .dlb_pmd_lib_windows_x86_gnu_debug.d,$(COMMON_FILES))
CLEAN_COMMON_COMPILE_release = $(addsuffix .dlb_pmd_lib_windows_x86_gnu_release.d,$(COMMON_FILES))

cleandeps:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)

clean: cleanself
	$(MAKE) -C $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/windows_x86_gnu cleanself

cleanself:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)
	$(RM) $(INPUTS_COMMON_ARCHIVE_debug)
	$(RM) $(INPUTS_COMMON_ARCHIVE_release)
	$(RM) dlb_pmd_lib_debug.lib
	$(RM) dlb_pmd_lib_release.lib

.PHONY: default FORCE clean cleandeps cleanself help

FORCE:

help:
	@echo "This makefile has the following targets:"
	@echo " dlb_pmd_lib_debug.lib"
	@echo " dlb_pmd_lib_release.lib"
	@echo " default"
	@echo " clean"
	@echo " cleandeps"
	@echo " help"

include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd.dlb_pmd_lib_windows_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_api_read.dlb_pmd_lib_windows_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_api_write.dlb_pmd_lib_windows_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_equal.dlb_pmd_lib_windows_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_metadata_set.dlb_pmd_lib_windows_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/model/pmd_language.dlb_pmd_lib_windows_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/generate/pmd_random_generator.dlb_pmd_lib_windows_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/klv/pmd_crc32.dlb_pmd_lib_windows_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/klv/pmd_klv_reader.dlb_pmd_lib_windows_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/klv/pmd_klv_writer.dlb_pmd_lib_windows_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_augmentor.dlb_pmd_lib_windows_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_extractor.dlb_pmd_lib_windows_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_smpte_337m.dlb_pmd_lib_windows_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_file.dlb_pmd_lib_windows_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_string.dlb_pmd_lib_windows_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_reader.dlb_pmd_lib_windows_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_file.dlb_pmd_lib_windows_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_string.dlb_pmd_lib_windows_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_writer.dlb_pmd_lib_windows_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd.dlb_pmd_lib_windows_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_api_read.dlb_pmd_lib_windows_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_api_write.dlb_pmd_lib_windows_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_equal.dlb_pmd_lib_windows_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_metadata_set.dlb_pmd_lib_windows_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/model/pmd_language.dlb_pmd_lib_windows_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/generate/pmd_random_generator.dlb_pmd_lib_windows_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/klv/pmd_crc32.dlb_pmd_lib_windows_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/klv/pmd_klv_reader.dlb_pmd_lib_windows_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/klv/pmd_klv_writer.dlb_pmd_lib_windows_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_augmentor.dlb_pmd_lib_windows_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_extractor.dlb_pmd_lib_windows_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_smpte_337m.dlb_pmd_lib_windows_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_file.dlb_pmd_lib_windows_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_string.dlb_pmd_lib_windows_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_reader.dlb_pmd_lib_windows_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_file.dlb_pmd_lib_windows_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_string.dlb_pmd_lib_windows_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_writer.dlb_pmd_lib_windows_x86_gnu_release.d)

$(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/windows_x86_gnu/dlb_xml_debug.lib \
$(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/windows_x86_gnu/dlb_xml_release.lib: %.lib: FORCE
	$(MAKE) -C $(@D) $(@F)


