#-*-makefile-*-

BASE=../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
ARFLAGS = -rc
CCDEP = $(CC)
CFLAGS_debug = -D_GNU_SOURCE -msse2 -g -ggdb3 -O0 -fPIC -m32 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
CFLAGS_release = -D_GNU_SOURCE -msse2 -O3 -ftree-vectorize -fPIC -m32 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
DEFINES_debug = -DELPP_NO_DEFAULT_LOG_FILE=1 -DHAVE_HIDDEN=1 -D_LARGEFILE64_SOURCE=1
DEFINES_release = -DELPP_NO_DEFAULT_LOG_FILE=1 -DHAVE_HIDDEN=1 -DNDEBUG=1 -D_LARGEFILE64_SOURCE=1
INCLUDES = -I$(BASE)dlb_buffer/1.3 -I$(BASE)dlb_buffer/1.3 -I$(BASE)dlb_octfile/1.1 -I$(BASE)dlb_pmd/frontend/pmd_tool -I$(BASE)dlb_pmd/frontend -I$(BASE)dlb_pmd/include -I$(BASE)dlb_pmd -I$(BASE)dlb_pmd/os -I$(BASE)dlb_pmd/src/model -I$(BASE)dlb_pmd/src/modules/klv -I$(BASE)dlb_pmd/src/modules/pcm -I$(BASE)dlb_pmd/src/modules/sadm -I$(BASE)dlb_pmd/src/modules/xml -I$(BASE)dlb_wave/1.6 -I$(BASE)dlb_xmllib/1.0.4 -I$(BASE). -I$(BASE). -I$(BASE)zlib/1.2.11/zlib-1.2.11

COMMON_FILES = $(TMP)dlb_octfile/1.1/dlb_octfile/src/dlb_octfile $(TMP)dlb_octfile/1.1/dlb_octfile/src/dlb_octfile_disk $(TMP)dlb_octfile/1.1/dlb_octfile/src/dlb_octfile_memory\
               $(TMP)dlb_pmd/frontend/pmd_tool/pcm $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool_klv\
               $(TMP)dlb_pmd/frontend/pmd_tool/xml $(TMP)dlb_pmd/os/linux/pmd_ctrl_c $(TMP)dlb_pmd/src/api/dlb_pmd $(TMP)dlb_pmd/src/api/dlb_pmd_api_read\
               $(TMP)dlb_pmd/src/api/dlb_pmd_api_write $(TMP)dlb_pmd/src/api/dlb_pmd_equal $(TMP)dlb_pmd/src/api/dlb_pmd_metadata_set\
               $(TMP)dlb_pmd/src/model/pmd_language $(TMP)dlb_pmd/src/modules/capture/pmd_capture $(TMP)dlb_pmd/src/modules/generate/pmd_random_generator\
               $(TMP)dlb_pmd/src/modules/klv/pmd_crc32 $(TMP)dlb_pmd/src/modules/klv/pmd_klv_reader $(TMP)dlb_pmd/src/modules/klv/pmd_klv_writer\
               $(TMP)dlb_pmd/src/modules/pcm/pmd_bitstream $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_augmentor $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_extractor\
               $(TMP)dlb_pmd/src/modules/pcm/pmd_smpte_337m $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_decoder $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_encoder\
               $(TMP)dlb_pmd/src/modules/sadm/idmap $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_generator $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_ingester\
               $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_file $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_string $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_reader\
               $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_file $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_string $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_writer\
               $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_common_definitions $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_model\
               $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_read_file $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_read_string\
               $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_reader $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_write_file\
               $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_write_string $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_writer\
               $(TMP)dlb_pmd/src/modules/sadm/sadm/idrefs $(TMP)dlb_pmd/src/modules/sadm/sadm/lookup3 $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_file\
               $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_string $(TMP)dlb_pmd/src/modules/xml/pmd_xml_reader $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_file\
               $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_string $(TMP)dlb_pmd/src/modules/xml/pmd_xml_writer $(TMP)dlb_wave/1.6/dlb_wave/src/dlb_riff\
               $(TMP)dlb_wave/1.6/dlb_wave/src/dlb_wave $(TMP)dlb_wave/1.6/dlb_wave/src/dlb_wave_float $(TMP)dlb_wave/1.6/dlb_wave/src/dlb_wave_int\
               $(TMP)dlb_xmllib/1.0.4/dlb_xml/src/dlb_xml $(TMP)zlib/1.2.11/zlib-1.2.11/adler32 $(TMP)zlib/1.2.11/zlib-1.2.11/compress\
               $(TMP)zlib/1.2.11/zlib-1.2.11/crc32 $(TMP)zlib/1.2.11/zlib-1.2.11/deflate $(TMP)zlib/1.2.11/zlib-1.2.11/gzclose\
               $(TMP)zlib/1.2.11/zlib-1.2.11/gzlib $(TMP)zlib/1.2.11/zlib-1.2.11/gzread $(TMP)zlib/1.2.11/zlib-1.2.11/gzwrite\
               $(TMP)zlib/1.2.11/zlib-1.2.11/infback $(TMP)zlib/1.2.11/zlib-1.2.11/inffast $(TMP)zlib/1.2.11/zlib-1.2.11/inflate\
               $(TMP)zlib/1.2.11/zlib-1.2.11/inftrees $(TMP)zlib/1.2.11/zlib-1.2.11/trees $(TMP)zlib/1.2.11/zlib-1.2.11/uncompr\
               $(TMP)zlib/1.2.11/zlib-1.2.11/zutil

INPUTS_COMMON_ARCHIVE_debug = $(addsuffix .dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.o,$(COMMON_FILES))
INPUTS_COMMON_ARCHIVE_release = $(addsuffix .dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.o,$(COMMON_FILES))


default: dlb_pmd_tool_lib_dynamic_debug.so dlb_pmd_tool_lib_dynamic_release.so

dlb_pmd_tool_lib_dynamic_debug.so: $(INPUTS_COMMON_ARCHIVE_debug)
	$(AR) $(ARFLAGS)  $@ $^

dlb_pmd_tool_lib_dynamic_release.so: $(INPUTS_COMMON_ARCHIVE_release)
	$(AR) $(ARFLAGS)  $@ $^

$(TMP)%.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -msse2 -MM $(DEFINES_debug) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_debug) $(DEFINES_debug) $(INCLUDES) -o $@ $<

$(TMP)%.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -msse2 -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMMON_COMPILE_debug = $(addsuffix .dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d,$(COMMON_FILES))
CLEAN_COMMON_COMPILE_release = $(addsuffix .dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d,$(COMMON_FILES))

cleandeps:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)

clean: cleanself

cleanself:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)
	$(RM) $(INPUTS_COMMON_ARCHIVE_debug)
	$(RM) $(INPUTS_COMMON_ARCHIVE_release)
	$(RM) dlb_pmd_tool_lib_dynamic_debug.so
	$(RM) dlb_pmd_tool_lib_dynamic_release.so

.PHONY: default FORCE clean cleandeps cleanself help

FORCE:

help:
	@echo "This makefile has the following targets:"
	@echo " dlb_pmd_tool_lib_dynamic_debug.so"
	@echo " dlb_pmd_tool_lib_dynamic_release.so"
	@echo " default"
	@echo " clean"
	@echo " cleandeps"
	@echo " help"

include $(wildcard $(TMP)dlb_octfile/1.1/dlb_octfile/src/dlb_octfile.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_octfile/1.1/dlb_octfile/src/dlb_octfile_disk.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_octfile/1.1/dlb_octfile/src/dlb_octfile_memory.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pcm.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool_klv.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/xml.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/os/linux/pmd_ctrl_c.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_api_read.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_api_write.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_equal.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_metadata_set.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/model/pmd_language.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/capture/pmd_capture.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/generate/pmd_random_generator.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/klv/pmd_crc32.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/klv/pmd_klv_reader.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/klv/pmd_klv_writer.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_bitstream.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_augmentor.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_extractor.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_smpte_337m.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_decoder.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_encoder.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/idmap.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_generator.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_ingester.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_file.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_string.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_reader.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_file.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_string.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_writer.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_common_definitions.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_model.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_read_file.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_read_string.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_reader.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_write_file.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_write_string.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_writer.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/idrefs.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/lookup3.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_file.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_string.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_reader.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_file.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_string.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_writer.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_wave/1.6/dlb_wave/src/dlb_riff.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_wave/1.6/dlb_wave/src/dlb_wave.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_wave/1.6/dlb_wave/src/dlb_wave_float.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_wave/1.6/dlb_wave/src/dlb_wave_int.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_xmllib/1.0.4/dlb_xml/src/dlb_xml.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/adler32.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/compress.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/crc32.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/deflate.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/gzclose.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/gzlib.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/gzread.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/gzwrite.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/infback.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/inffast.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/inflate.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/inftrees.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/trees.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/uncompr.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/zutil.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_octfile/1.1/dlb_octfile/src/dlb_octfile.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_octfile/1.1/dlb_octfile/src/dlb_octfile_disk.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_octfile/1.1/dlb_octfile/src/dlb_octfile_memory.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pcm.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool_klv.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/xml.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/os/linux/pmd_ctrl_c.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_api_read.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_api_write.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_equal.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_metadata_set.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/model/pmd_language.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/capture/pmd_capture.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/generate/pmd_random_generator.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/klv/pmd_crc32.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/klv/pmd_klv_reader.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/klv/pmd_klv_writer.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_bitstream.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_augmentor.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_extractor.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_smpte_337m.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_decoder.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_encoder.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/idmap.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_generator.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_ingester.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_file.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_string.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_reader.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_file.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_string.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_writer.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_common_definitions.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_model.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_read_file.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_read_string.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_reader.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_write_file.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_write_string.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_writer.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/idrefs.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/lookup3.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_file.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_string.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_reader.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_file.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_string.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_writer.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_wave/1.6/dlb_wave/src/dlb_riff.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_wave/1.6/dlb_wave/src/dlb_wave.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_wave/1.6/dlb_wave/src/dlb_wave_float.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_wave/1.6/dlb_wave/src/dlb_wave_int.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_xmllib/1.0.4/dlb_xml/src/dlb_xml.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/adler32.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/compress.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/crc32.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/deflate.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/gzclose.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/gzlib.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/gzread.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/gzwrite.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/infback.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/inffast.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/inflate.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/inftrees.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/trees.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/uncompr.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/zutil.dlb_pmd_tool_lib_dynamic_linux_x86_gnu_release.d)


