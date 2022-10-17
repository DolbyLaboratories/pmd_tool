#-*-makefile-*-

BASE=../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
ARFLAGS = -rc -s
CCDEP = $(CC)
CFLAGS_debug = -g -O0 -arch x86_64 -fPIC -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wno-overlength-strings -Wdeclaration-after-statement -Wconditional-uninitialized
CFLAGS_release = -O3 -arch x86_64 -fPIC -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wno-overlength-strings -Wdeclaration-after-statement -Wconditional-uninitialized
CXXDEP = $(CXX)
CXXFLAGS_debug = -std=c++17 -g -O0 -arch x86_64 -fPIC -c -Wall
CXXFLAGS_release = -std=c++17 -O3 -arch x86_64 -fPIC -c -Wall
DEFINES_debug = -DELPP_NO_DEFAULT_LOG_FILE=1
DEFINES_release = -DELPP_NO_DEFAULT_LOG_FILE=1 -DNDEBUG=1
INCLUDES = -I$(BASE)boost/boost_1_73_0 -I$(BASE)dlb_adm/include -I$(BASE)dlb_pmd/include -I$(BASE)dlb_pmd -I$(BASE)dlb_pmd/os -I$(BASE)dlb_pmd/src/model -I$(BASE)dlb_pmd/src/modules/klv -I$(BASE)dlb_pmd/src/modules/pcm -I$(BASE)dlb_pmd/src/modules/sadm -I$(BASE)dlb_pmd/src/modules/xml -I$(BASE)dlb_xmllib/1.0.4 -I$(BASE). -I$(BASE). -I$(BASE)dlb_xmllib/1.0.4 -I$(BASE)zlib/1.2.11/zlib-1.2.11


INPUTS_ARCHIVE_debug = $(TMP)dlb_pmd/os/osx/pmd_ctrl_c.dlb_pmd_lib_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/api/dlb_pmd.dlb_pmd_lib_osx_amd64_clang_debug.o\
                       $(TMP)dlb_pmd/src/api/dlb_pmd_api_read.dlb_pmd_lib_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/api/dlb_pmd_api_write.dlb_pmd_lib_osx_amd64_clang_debug.o\
                       $(TMP)dlb_pmd/src/api/dlb_pmd_equal.dlb_pmd_lib_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/api/dlb_pmd_metadata_set.dlb_pmd_lib_osx_amd64_clang_debug.o\
                       $(TMP)dlb_pmd/src/model/pmd_language.dlb_pmd_lib_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/model/pmd_model.dlb_pmd_lib_osx_amd64_clang_debug.o\
                       $(TMP)dlb_pmd/src/model/pmd_model_combo.dlb_pmd_lib_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/modules/capture/pmd_capture.dlb_pmd_lib_osx_amd64_clang_debug.o\
                       $(TMP)dlb_pmd/src/modules/generate/pmd_random_generator.dlb_pmd_lib_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/modules/klv/pmd_crc32.dlb_pmd_lib_osx_amd64_clang_debug.o\
                       $(TMP)dlb_pmd/src/modules/klv/pmd_klv_reader.dlb_pmd_lib_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/modules/klv/pmd_klv_writer.dlb_pmd_lib_osx_amd64_clang_debug.o\
                       $(TMP)dlb_pmd/src/modules/pcm/pmd_bitstream.dlb_pmd_lib_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_augmentor.dlb_pmd_lib_osx_amd64_clang_debug.o\
                       $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_extractor.dlb_pmd_lib_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/modules/pcm/pmd_smpte_337m.dlb_pmd_lib_osx_amd64_clang_debug.o\
                       $(TMP)dlb_pmd/src/modules/sadm/pmd_core_model_generator.dlb_pmd_lib_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/modules/sadm/pmd_core_model_ingester.dlb_pmd_lib_osx_amd64_clang_debug.o\
                       $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_buffer.dlb_pmd_lib_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_file.dlb_pmd_lib_osx_amd64_clang_debug.o\
                       $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_buffer.dlb_pmd_lib_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_file.dlb_pmd_lib_osx_amd64_clang_debug.o\
                       $(TMP)dlb_pmd/src/modules/sadm/sadm_bitstream_decoder.dlb_pmd_lib_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/modules/sadm/sadm_bitstream_encoder.dlb_pmd_lib_osx_amd64_clang_debug.o\
                       $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_file.dlb_pmd_lib_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_string.dlb_pmd_lib_osx_amd64_clang_debug.o\
                       $(TMP)dlb_pmd/src/modules/xml/pmd_xml_reader.dlb_pmd_lib_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_file.dlb_pmd_lib_osx_amd64_clang_debug.o\
                       $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_string.dlb_pmd_lib_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/modules/xml/pmd_xml_writer.dlb_pmd_lib_osx_amd64_clang_debug.o\
                       $(TMP)dlb_pmd/src/model/DlbPmdModelWrapper.dlb_pmd_lib_osx_amd64_clang_debug.o
INPUTS_ARCHIVE_release = $(TMP)dlb_pmd/os/osx/pmd_ctrl_c.dlb_pmd_lib_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/api/dlb_pmd.dlb_pmd_lib_osx_amd64_clang_release.o\
                         $(TMP)dlb_pmd/src/api/dlb_pmd_api_read.dlb_pmd_lib_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/api/dlb_pmd_api_write.dlb_pmd_lib_osx_amd64_clang_release.o\
                         $(TMP)dlb_pmd/src/api/dlb_pmd_equal.dlb_pmd_lib_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/api/dlb_pmd_metadata_set.dlb_pmd_lib_osx_amd64_clang_release.o\
                         $(TMP)dlb_pmd/src/model/pmd_language.dlb_pmd_lib_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/model/pmd_model.dlb_pmd_lib_osx_amd64_clang_release.o\
                         $(TMP)dlb_pmd/src/model/pmd_model_combo.dlb_pmd_lib_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/modules/capture/pmd_capture.dlb_pmd_lib_osx_amd64_clang_release.o\
                         $(TMP)dlb_pmd/src/modules/generate/pmd_random_generator.dlb_pmd_lib_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/modules/klv/pmd_crc32.dlb_pmd_lib_osx_amd64_clang_release.o\
                         $(TMP)dlb_pmd/src/modules/klv/pmd_klv_reader.dlb_pmd_lib_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/modules/klv/pmd_klv_writer.dlb_pmd_lib_osx_amd64_clang_release.o\
                         $(TMP)dlb_pmd/src/modules/pcm/pmd_bitstream.dlb_pmd_lib_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_augmentor.dlb_pmd_lib_osx_amd64_clang_release.o\
                         $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_extractor.dlb_pmd_lib_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/modules/pcm/pmd_smpte_337m.dlb_pmd_lib_osx_amd64_clang_release.o\
                         $(TMP)dlb_pmd/src/modules/sadm/pmd_core_model_generator.dlb_pmd_lib_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/modules/sadm/pmd_core_model_ingester.dlb_pmd_lib_osx_amd64_clang_release.o\
                         $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_buffer.dlb_pmd_lib_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_file.dlb_pmd_lib_osx_amd64_clang_release.o\
                         $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_buffer.dlb_pmd_lib_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_file.dlb_pmd_lib_osx_amd64_clang_release.o\
                         $(TMP)dlb_pmd/src/modules/sadm/sadm_bitstream_decoder.dlb_pmd_lib_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/modules/sadm/sadm_bitstream_encoder.dlb_pmd_lib_osx_amd64_clang_release.o\
                         $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_file.dlb_pmd_lib_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_string.dlb_pmd_lib_osx_amd64_clang_release.o\
                         $(TMP)dlb_pmd/src/modules/xml/pmd_xml_reader.dlb_pmd_lib_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_file.dlb_pmd_lib_osx_amd64_clang_release.o\
                         $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_string.dlb_pmd_lib_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/modules/xml/pmd_xml_writer.dlb_pmd_lib_osx_amd64_clang_release.o\
                         $(TMP)dlb_pmd/src/model/DlbPmdModelWrapper.dlb_pmd_lib_osx_amd64_clang_release.o

DEP_PROJ_ARCHIVE_debug = $(BASE)dlb_adm/make/dlb_adm_lib/osx_amd64_clang/dlb_adm_lib_debug.a $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/osx_amd64_clang/dlb_xml_debug.a $(BASE)zlib/1.2.11/make/zlib/osx_amd64_clang/zlib_debug.a
DEP_PROJ_ARCHIVE_release = $(BASE)dlb_adm/make/dlb_adm_lib/osx_amd64_clang/dlb_adm_lib_release.a $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/osx_amd64_clang/dlb_xml_release.a $(BASE)zlib/1.2.11/make/zlib/osx_amd64_clang/zlib_release.a

default: dlb_pmd_lib_debug.a dlb_pmd_lib_release.a

dlb_pmd_lib_debug.a: $(INPUTS_ARCHIVE_debug)
	$(AR) $(ARFLAGS)   $@ $^

dlb_pmd_lib_release.a: $(INPUTS_ARCHIVE_release)
	$(AR) $(ARFLAGS)   $@ $^

$(TMP)%.dlb_pmd_lib_osx_amd64_clang_debug.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -arch x86_64 -MM $(DEFINES_debug) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_debug) $(DEFINES_debug) $(INCLUDES) -o $@ $<

$(TMP)%.dlb_pmd_lib_osx_amd64_clang_debug.o: $(BASE)%.cpp | $(TMP)%/..
	$(CXXDEP) -arch x86_64 -MM $(DEFINES_debug) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS_debug) $(DEFINES_debug) $(INCLUDES) -o $@ $<

$(TMP)%.dlb_pmd_lib_osx_amd64_clang_release.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -arch x86_64 -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

$(TMP)%.dlb_pmd_lib_osx_amd64_clang_release.o: $(BASE)%.cpp | $(TMP)%/..
	$(CXXDEP) -arch x86_64 -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMPILE_debug = $(TMP)dlb_pmd/os/osx/pmd_ctrl_c.dlb_pmd_lib_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/api/dlb_pmd.dlb_pmd_lib_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/api/dlb_pmd_api_read.dlb_pmd_lib_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/api/dlb_pmd_api_write.dlb_pmd_lib_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/api/dlb_pmd_equal.dlb_pmd_lib_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/api/dlb_pmd_metadata_set.dlb_pmd_lib_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/model/pmd_language.dlb_pmd_lib_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/model/pmd_model.dlb_pmd_lib_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/model/pmd_model_combo.dlb_pmd_lib_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/modules/capture/pmd_capture.dlb_pmd_lib_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/modules/generate/pmd_random_generator.dlb_pmd_lib_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/modules/klv/pmd_crc32.dlb_pmd_lib_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/modules/klv/pmd_klv_reader.dlb_pmd_lib_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/modules/klv/pmd_klv_writer.dlb_pmd_lib_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/modules/pcm/pmd_bitstream.dlb_pmd_lib_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_augmentor.dlb_pmd_lib_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_extractor.dlb_pmd_lib_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/modules/pcm/pmd_smpte_337m.dlb_pmd_lib_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/modules/sadm/pmd_core_model_generator.dlb_pmd_lib_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/modules/sadm/pmd_core_model_ingester.dlb_pmd_lib_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_buffer.dlb_pmd_lib_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_file.dlb_pmd_lib_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_buffer.dlb_pmd_lib_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_file.dlb_pmd_lib_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/modules/sadm/sadm_bitstream_decoder.dlb_pmd_lib_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/modules/sadm/sadm_bitstream_encoder.dlb_pmd_lib_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_file.dlb_pmd_lib_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_string.dlb_pmd_lib_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/modules/xml/pmd_xml_reader.dlb_pmd_lib_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_file.dlb_pmd_lib_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_string.dlb_pmd_lib_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/modules/xml/pmd_xml_writer.dlb_pmd_lib_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/model/DlbPmdModelWrapper.dlb_pmd_lib_osx_amd64_clang_debug.d
CLEAN_COMPILE_release = $(TMP)dlb_pmd/os/osx/pmd_ctrl_c.dlb_pmd_lib_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/api/dlb_pmd.dlb_pmd_lib_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/api/dlb_pmd_api_read.dlb_pmd_lib_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/api/dlb_pmd_api_write.dlb_pmd_lib_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/api/dlb_pmd_equal.dlb_pmd_lib_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/api/dlb_pmd_metadata_set.dlb_pmd_lib_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/model/pmd_language.dlb_pmd_lib_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/model/pmd_model.dlb_pmd_lib_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/model/pmd_model_combo.dlb_pmd_lib_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/modules/capture/pmd_capture.dlb_pmd_lib_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/generate/pmd_random_generator.dlb_pmd_lib_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/modules/klv/pmd_crc32.dlb_pmd_lib_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/klv/pmd_klv_reader.dlb_pmd_lib_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/modules/klv/pmd_klv_writer.dlb_pmd_lib_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/pcm/pmd_bitstream.dlb_pmd_lib_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_augmentor.dlb_pmd_lib_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_extractor.dlb_pmd_lib_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/modules/pcm/pmd_smpte_337m.dlb_pmd_lib_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/sadm/pmd_core_model_generator.dlb_pmd_lib_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/modules/sadm/pmd_core_model_ingester.dlb_pmd_lib_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_buffer.dlb_pmd_lib_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_file.dlb_pmd_lib_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_buffer.dlb_pmd_lib_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_file.dlb_pmd_lib_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/sadm/sadm_bitstream_decoder.dlb_pmd_lib_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/modules/sadm/sadm_bitstream_encoder.dlb_pmd_lib_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_file.dlb_pmd_lib_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_string.dlb_pmd_lib_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/xml/pmd_xml_reader.dlb_pmd_lib_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_file.dlb_pmd_lib_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_string.dlb_pmd_lib_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/modules/xml/pmd_xml_writer.dlb_pmd_lib_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/model/DlbPmdModelWrapper.dlb_pmd_lib_osx_amd64_clang_release.d

cleandeps:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)

clean: cleanself
	$(MAKE) -C $(BASE)dlb_adm/make/dlb_adm_lib/osx_amd64_clang cleanself
	$(MAKE) -C $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/osx_amd64_clang cleanself
	$(MAKE) -C $(BASE)zlib/1.2.11/make/zlib/osx_amd64_clang cleanself

cleanself:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)
	$(RM) $(INPUTS_ARCHIVE_debug)
	$(RM) $(INPUTS_ARCHIVE_release)
	$(RM) dlb_pmd_lib_debug.a
	$(RM) dlb_pmd_lib_release.a

.PHONY: default FORCE clean cleandeps cleanself help

FORCE:

help:
	@echo "This makefile has the following targets:"
	@echo " dlb_pmd_lib_debug.a"
	@echo " dlb_pmd_lib_release.a"
	@echo " default"
	@echo " clean"
	@echo " cleandeps"
	@echo " help"

include $(wildcard $(TMP)dlb_pmd/os/osx/pmd_ctrl_c.dlb_pmd_lib_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd.dlb_pmd_lib_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_api_read.dlb_pmd_lib_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_api_write.dlb_pmd_lib_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_equal.dlb_pmd_lib_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_metadata_set.dlb_pmd_lib_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/model/pmd_language.dlb_pmd_lib_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/model/pmd_model.dlb_pmd_lib_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/model/pmd_model_combo.dlb_pmd_lib_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/capture/pmd_capture.dlb_pmd_lib_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/generate/pmd_random_generator.dlb_pmd_lib_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/klv/pmd_crc32.dlb_pmd_lib_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/klv/pmd_klv_reader.dlb_pmd_lib_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/klv/pmd_klv_writer.dlb_pmd_lib_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_bitstream.dlb_pmd_lib_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_augmentor.dlb_pmd_lib_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_extractor.dlb_pmd_lib_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_smpte_337m.dlb_pmd_lib_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_core_model_generator.dlb_pmd_lib_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_core_model_ingester.dlb_pmd_lib_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_buffer.dlb_pmd_lib_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_file.dlb_pmd_lib_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_buffer.dlb_pmd_lib_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_file.dlb_pmd_lib_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm_bitstream_decoder.dlb_pmd_lib_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm_bitstream_encoder.dlb_pmd_lib_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_file.dlb_pmd_lib_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_string.dlb_pmd_lib_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_reader.dlb_pmd_lib_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_file.dlb_pmd_lib_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_string.dlb_pmd_lib_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_writer.dlb_pmd_lib_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/model/DlbPmdModelWrapper.dlb_pmd_lib_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/os/osx/pmd_ctrl_c.dlb_pmd_lib_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd.dlb_pmd_lib_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_api_read.dlb_pmd_lib_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_api_write.dlb_pmd_lib_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_equal.dlb_pmd_lib_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_metadata_set.dlb_pmd_lib_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/model/pmd_language.dlb_pmd_lib_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/model/pmd_model.dlb_pmd_lib_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/model/pmd_model_combo.dlb_pmd_lib_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/capture/pmd_capture.dlb_pmd_lib_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/generate/pmd_random_generator.dlb_pmd_lib_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/klv/pmd_crc32.dlb_pmd_lib_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/klv/pmd_klv_reader.dlb_pmd_lib_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/klv/pmd_klv_writer.dlb_pmd_lib_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_bitstream.dlb_pmd_lib_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_augmentor.dlb_pmd_lib_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_extractor.dlb_pmd_lib_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_smpte_337m.dlb_pmd_lib_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_core_model_generator.dlb_pmd_lib_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_core_model_ingester.dlb_pmd_lib_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_buffer.dlb_pmd_lib_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_file.dlb_pmd_lib_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_buffer.dlb_pmd_lib_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_file.dlb_pmd_lib_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm_bitstream_decoder.dlb_pmd_lib_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm_bitstream_encoder.dlb_pmd_lib_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_file.dlb_pmd_lib_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_string.dlb_pmd_lib_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_reader.dlb_pmd_lib_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_file.dlb_pmd_lib_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_string.dlb_pmd_lib_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_writer.dlb_pmd_lib_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/model/DlbPmdModelWrapper.dlb_pmd_lib_osx_amd64_clang_release.d)

$(BASE)dlb_adm/make/dlb_adm_lib/osx_amd64_clang/dlb_adm_lib_debug.a \
$(BASE)dlb_adm/make/dlb_adm_lib/osx_amd64_clang/dlb_adm_lib_release.a \
$(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/osx_amd64_clang/dlb_xml_debug.a \
$(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/osx_amd64_clang/dlb_xml_release.a \
$(BASE)zlib/1.2.11/make/zlib/osx_amd64_clang/zlib_debug.a \
$(BASE)zlib/1.2.11/make/zlib/osx_amd64_clang/zlib_release.a: %.a: FORCE
	$(MAKE) -C $(@D) $(@F)


