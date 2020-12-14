#-*-makefile-*-

BASE=../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
CCDEP = $(CC)
CFLAGS_debug = -g -O0 -arch x86_64 -fPIC -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wno-overlength-strings -Wdeclaration-after-statement -Wconditional-uninitialized
CFLAGS_release = -O3 -arch x86_64 -fPIC -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wno-overlength-strings -Wdeclaration-after-statement -Wconditional-uninitialized
CXXDEP = $(CXX)
CXXFLAGS_debug = -std=c++11 -g -O0 -arch x86_64 -fPIC -c -Wall
CXXFLAGS_release = -std=c++11 -O3 -arch x86_64 -fPIC -c -Wall
DEFINES_release = -DNDEBUG=1
INCLUDES = -I$(BASE)Lawo/ember-plus/libember_slim/Source -I$(BASE)Lawo/ember-plus -I$(BASE)dlb_buffer/1.3 -I$(BASE)dlb_octfile/1.1 -I$(BASE)dlb_pmd/frontend -I$(BASE)dlb_pmd/frontend/pmd_studio/ember_connector -I$(BASE)dlb_pmd/frontend/pmd_studio/ember_connector/utils -I$(BASE)dlb_pmd/frontend/pmd_studio -I$(BASE)dlb_pmd/frontend/pmd_tool -I$(BASE)dlb_pmd/frontend -I$(BASE)dlb_pmd/include -I$(BASE)dlb_pmd -I$(BASE)dlb_pmd/os -I$(BASE)dlb_pmd/src/model -I$(BASE)dlb_pmd/src/modules/klv -I$(BASE)dlb_pmd/src/modules/pcm -I$(BASE)dlb_pmd/src/modules/sadm -I$(BASE)dlb_pmd/src/modules/xml -I$(BASE)dlb_socket/0.1/dlb_socket/include/osx -I$(BASE)dlb_socket/0.1/dlb_socket/include -I$(BASE)dlb_socket/0.1 -I$(BASE)dlb_wave/1.6 -I$(BASE)dlb_xmllib/1.0.4 -I$(BASE)libui/alpha_4.1/libui -I$(BASE). -I$(BASE). -I$(BASE)portaudio/19.6/portaudio/include -I$(BASE)zlib/1.2.11/zlib-1.2.11
LD = $(CXX)
LDFLAGS_debug = -arch x86_64 -dead_strip
LDFLAGS_release = -arch x86_64 -dead_strip -O2
LDLIBS = -lpthread -Wl,-framework,Foundation -Wl,-framework,AppKit -Wl,-framework,CoreAudio -Wl,-framework,AudioToolbox -Wl,-framework,AudioUnit -Wl,-framework,Cocoa


INPUTS_LINK_debug = $(TMP)Lawo/ember-plus/libember_slim/Source/ber.pmd_studio_osx_amd64_clang_debug.o $(TMP)Lawo/ember-plus/libember_slim/Source/berio.pmd_studio_osx_amd64_clang_debug.o\
                    $(TMP)Lawo/ember-plus/libember_slim/Source/berreader.pmd_studio_osx_amd64_clang_debug.o $(TMP)Lawo/ember-plus/libember_slim/Source/bertag.pmd_studio_osx_amd64_clang_debug.o\
                    $(TMP)Lawo/ember-plus/libember_slim/Source/bytebuffer.pmd_studio_osx_amd64_clang_debug.o $(TMP)Lawo/ember-plus/libember_slim/Source/ember.pmd_studio_osx_amd64_clang_debug.o\
                    $(TMP)Lawo/ember-plus/libember_slim/Source/emberasyncreader.pmd_studio_osx_amd64_clang_debug.o $(TMP)Lawo/ember-plus/libember_slim/Source/emberframing.pmd_studio_osx_amd64_clang_debug.o\
                    $(TMP)Lawo/ember-plus/libember_slim/Source/emberinternal.pmd_studio_osx_amd64_clang_debug.o $(TMP)Lawo/ember-plus/libember_slim/Source/glow.pmd_studio_osx_amd64_clang_debug.o\
                    $(TMP)Lawo/ember-plus/libember_slim/Source/glowrx.pmd_studio_osx_amd64_clang_debug.o $(TMP)Lawo/ember-plus/libember_slim/Source/glowtx.pmd_studio_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/frontend/pmd_tool/pcm.pmd_studio_osx_amd64_clang_debug.o $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool.pmd_studio_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool_klv.pmd_studio_osx_amd64_clang_debug.o $(TMP)dlb_pmd/frontend/pmd_tool/xml.pmd_studio_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/os/osx/pmd_ctrl_c.pmd_studio_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/api/dlb_pmd.pmd_studio_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/src/api/dlb_pmd_api_read.pmd_studio_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/api/dlb_pmd_api_write.pmd_studio_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/src/api/dlb_pmd_equal.pmd_studio_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/api/dlb_pmd_metadata_set.pmd_studio_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/src/model/pmd_language.pmd_studio_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/modules/capture/pmd_capture.pmd_studio_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/src/modules/generate/pmd_random_generator.pmd_studio_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/modules/klv/pmd_crc32.pmd_studio_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/src/modules/klv/pmd_klv_reader.pmd_studio_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/modules/klv/pmd_klv_writer.pmd_studio_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/src/modules/pcm/pmd_bitstream.pmd_studio_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_augmentor.pmd_studio_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_extractor.pmd_studio_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/modules/pcm/pmd_smpte_337m.pmd_studio_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_decoder.pmd_studio_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_encoder.pmd_studio_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/src/modules/sadm/idmap.pmd_studio_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_generator.pmd_studio_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_ingester.pmd_studio_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_file.pmd_studio_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_string.pmd_studio_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_reader.pmd_studio_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_file.pmd_studio_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_string.pmd_studio_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_writer.pmd_studio_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_common_definitions.pmd_studio_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_model.pmd_studio_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_read_file.pmd_studio_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_read_string.pmd_studio_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_reader.pmd_studio_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_write_file.pmd_studio_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_write_string.pmd_studio_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_writer.pmd_studio_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/modules/sadm/sadm/idrefs.pmd_studio_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/src/modules/sadm/sadm/lookup3.pmd_studio_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_file.pmd_studio_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_string.pmd_studio_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/modules/xml/pmd_xml_reader.pmd_studio_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_file.pmd_studio_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_string.pmd_studio_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/src/modules/xml/pmd_xml_writer.pmd_studio_osx_amd64_clang_debug.o $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/ember_connector.pmd_studio_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/ember_controller.pmd_studio_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/parameter_manager.pmd_studio_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/utils/utils.pmd_studio_osx_amd64_clang_debug.o $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio.pmd_studio_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_beds.pmd_studio_osx_amd64_clang_debug.o $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_objects.pmd_studio_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_outputs.pmd_studio_osx_amd64_clang_debug.o $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_presentations.pmd_studio_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_console.pmd_studio_osx_amd64_clang_debug.o $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_console_emberplus.pmd_studio_osx_amd64_clang_debug.o\
                    $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_device.pmd_studio_osx_amd64_clang_debug.o $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_settings.pmd_studio_osx_amd64_clang_debug.o
INPUTS_LINK_release = $(TMP)Lawo/ember-plus/libember_slim/Source/ber.pmd_studio_osx_amd64_clang_release.o $(TMP)Lawo/ember-plus/libember_slim/Source/berio.pmd_studio_osx_amd64_clang_release.o\
                      $(TMP)Lawo/ember-plus/libember_slim/Source/berreader.pmd_studio_osx_amd64_clang_release.o $(TMP)Lawo/ember-plus/libember_slim/Source/bertag.pmd_studio_osx_amd64_clang_release.o\
                      $(TMP)Lawo/ember-plus/libember_slim/Source/bytebuffer.pmd_studio_osx_amd64_clang_release.o $(TMP)Lawo/ember-plus/libember_slim/Source/ember.pmd_studio_osx_amd64_clang_release.o\
                      $(TMP)Lawo/ember-plus/libember_slim/Source/emberasyncreader.pmd_studio_osx_amd64_clang_release.o $(TMP)Lawo/ember-plus/libember_slim/Source/emberframing.pmd_studio_osx_amd64_clang_release.o\
                      $(TMP)Lawo/ember-plus/libember_slim/Source/emberinternal.pmd_studio_osx_amd64_clang_release.o $(TMP)Lawo/ember-plus/libember_slim/Source/glow.pmd_studio_osx_amd64_clang_release.o\
                      $(TMP)Lawo/ember-plus/libember_slim/Source/glowrx.pmd_studio_osx_amd64_clang_release.o $(TMP)Lawo/ember-plus/libember_slim/Source/glowtx.pmd_studio_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/frontend/pmd_tool/pcm.pmd_studio_osx_amd64_clang_release.o $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool.pmd_studio_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool_klv.pmd_studio_osx_amd64_clang_release.o $(TMP)dlb_pmd/frontend/pmd_tool/xml.pmd_studio_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/os/osx/pmd_ctrl_c.pmd_studio_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/api/dlb_pmd.pmd_studio_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/src/api/dlb_pmd_api_read.pmd_studio_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/api/dlb_pmd_api_write.pmd_studio_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/src/api/dlb_pmd_equal.pmd_studio_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/api/dlb_pmd_metadata_set.pmd_studio_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/src/model/pmd_language.pmd_studio_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/modules/capture/pmd_capture.pmd_studio_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/src/modules/generate/pmd_random_generator.pmd_studio_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/modules/klv/pmd_crc32.pmd_studio_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/src/modules/klv/pmd_klv_reader.pmd_studio_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/modules/klv/pmd_klv_writer.pmd_studio_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/src/modules/pcm/pmd_bitstream.pmd_studio_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_augmentor.pmd_studio_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_extractor.pmd_studio_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/modules/pcm/pmd_smpte_337m.pmd_studio_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_decoder.pmd_studio_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_encoder.pmd_studio_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/src/modules/sadm/idmap.pmd_studio_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_generator.pmd_studio_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_ingester.pmd_studio_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_file.pmd_studio_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_string.pmd_studio_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_reader.pmd_studio_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_file.pmd_studio_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_string.pmd_studio_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_writer.pmd_studio_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_common_definitions.pmd_studio_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_model.pmd_studio_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_read_file.pmd_studio_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_read_string.pmd_studio_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_reader.pmd_studio_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_write_file.pmd_studio_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_write_string.pmd_studio_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_writer.pmd_studio_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/modules/sadm/sadm/idrefs.pmd_studio_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/src/modules/sadm/sadm/lookup3.pmd_studio_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_file.pmd_studio_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_string.pmd_studio_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/modules/xml/pmd_xml_reader.pmd_studio_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_file.pmd_studio_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_string.pmd_studio_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/src/modules/xml/pmd_xml_writer.pmd_studio_osx_amd64_clang_release.o $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/ember_connector.pmd_studio_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/ember_controller.pmd_studio_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/parameter_manager.pmd_studio_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/utils/utils.pmd_studio_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio.pmd_studio_osx_amd64_clang_release.o $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_beds.pmd_studio_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_objects.pmd_studio_osx_amd64_clang_release.o $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_outputs.pmd_studio_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_presentations.pmd_studio_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_console.pmd_studio_osx_amd64_clang_release.o $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_console_emberplus.pmd_studio_osx_amd64_clang_release.o\
                      $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_device.pmd_studio_osx_amd64_clang_release.o $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_settings.pmd_studio_osx_amd64_clang_release.o

DEP_PROJ_LINK_debug = $(BASE)dlb_pmd/make/dlb_pmd_lib/osx_amd64_clang/dlb_pmd_lib_debug.a $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/osx_amd64_clang/dlb_xml_debug.a $(BASE)zlib/1.2.11/make/zlib/osx_amd64_clang/zlib_debug.a $(BASE)dlb_pmd/make/dlb_wave_lib/osx_amd64_clang/dlb_wave_lib_debug.a $(BASE)libui/alpha_4.1/libui/make/libui_alpha4_1/osx_amd64_clang/libui_alpha4_1_debug.a $(BASE)libui/alpha_4.1/libui/make/libui_impl_alpha4_1/osx_amd64_clang/libui_impl_alpha4_1_debug.a $(BASE)portaudio/19.6/portaudio/make/portaudio_19_6_lib/osx_amd64_clang/portaudio_19_6_lib_debug.a
DEP_PROJ_LINK_release = $(BASE)dlb_pmd/make/dlb_pmd_lib/osx_amd64_clang/dlb_pmd_lib_release.a $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/osx_amd64_clang/dlb_xml_release.a $(BASE)zlib/1.2.11/make/zlib/osx_amd64_clang/zlib_release.a $(BASE)dlb_pmd/make/dlb_wave_lib/osx_amd64_clang/dlb_wave_lib_release.a $(BASE)libui/alpha_4.1/libui/make/libui_alpha4_1/osx_amd64_clang/libui_alpha4_1_release.a $(BASE)libui/alpha_4.1/libui/make/libui_impl_alpha4_1/osx_amd64_clang/libui_impl_alpha4_1_release.a $(BASE)portaudio/19.6/portaudio/make/portaudio_19_6_lib/osx_amd64_clang/portaudio_19_6_lib_release.a

default: pmd_studio_debug pmd_studio_release

pmd_studio_debug: $(INPUTS_LINK_debug) $(DEP_PROJ_LINK_debug)
	$(LD) $(LDFLAGS_debug) -o $@ $+ $(LDLIBS)

pmd_studio_release: $(INPUTS_LINK_release) $(DEP_PROJ_LINK_release)
	$(LD) $(LDFLAGS_release) -o $@ $+ $(LDLIBS)

$(TMP)%.pmd_studio_osx_amd64_clang_debug.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -arch x86_64 -MM $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_debug) $(INCLUDES) -o $@ $<

$(TMP)%.pmd_studio_osx_amd64_clang_debug.o: $(BASE)%.cpp | $(TMP)%/..
	$(CXXDEP) -std=c++11 -arch x86_64 -MM $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS_debug) $(INCLUDES) -o $@ $<

$(TMP)%.pmd_studio_osx_amd64_clang_release.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -arch x86_64 -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

$(TMP)%.pmd_studio_osx_amd64_clang_release.o: $(BASE)%.cpp | $(TMP)%/..
	$(CXXDEP) -std=c++11 -arch x86_64 -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMPILE_debug = $(TMP)Lawo/ember-plus/libember_slim/Source/ber.pmd_studio_osx_amd64_clang_debug.d $(TMP)Lawo/ember-plus/libember_slim/Source/berio.pmd_studio_osx_amd64_clang_debug.d\
                      $(TMP)Lawo/ember-plus/libember_slim/Source/berreader.pmd_studio_osx_amd64_clang_debug.d $(TMP)Lawo/ember-plus/libember_slim/Source/bertag.pmd_studio_osx_amd64_clang_debug.d\
                      $(TMP)Lawo/ember-plus/libember_slim/Source/bytebuffer.pmd_studio_osx_amd64_clang_debug.d $(TMP)Lawo/ember-plus/libember_slim/Source/ember.pmd_studio_osx_amd64_clang_debug.d\
                      $(TMP)Lawo/ember-plus/libember_slim/Source/emberasyncreader.pmd_studio_osx_amd64_clang_debug.d $(TMP)Lawo/ember-plus/libember_slim/Source/emberframing.pmd_studio_osx_amd64_clang_debug.d\
                      $(TMP)Lawo/ember-plus/libember_slim/Source/emberinternal.pmd_studio_osx_amd64_clang_debug.d $(TMP)Lawo/ember-plus/libember_slim/Source/glow.pmd_studio_osx_amd64_clang_debug.d\
                      $(TMP)Lawo/ember-plus/libember_slim/Source/glowrx.pmd_studio_osx_amd64_clang_debug.d $(TMP)Lawo/ember-plus/libember_slim/Source/glowtx.pmd_studio_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/frontend/pmd_tool/pcm.pmd_studio_osx_amd64_clang_debug.d $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool.pmd_studio_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool_klv.pmd_studio_osx_amd64_clang_debug.d $(TMP)dlb_pmd/frontend/pmd_tool/xml.pmd_studio_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/os/osx/pmd_ctrl_c.pmd_studio_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/api/dlb_pmd.pmd_studio_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/api/dlb_pmd_api_read.pmd_studio_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/api/dlb_pmd_api_write.pmd_studio_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/api/dlb_pmd_equal.pmd_studio_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/api/dlb_pmd_metadata_set.pmd_studio_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/model/pmd_language.pmd_studio_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/modules/capture/pmd_capture.pmd_studio_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/modules/generate/pmd_random_generator.pmd_studio_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/modules/klv/pmd_crc32.pmd_studio_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/modules/klv/pmd_klv_reader.pmd_studio_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/modules/klv/pmd_klv_writer.pmd_studio_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/modules/pcm/pmd_bitstream.pmd_studio_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_augmentor.pmd_studio_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_extractor.pmd_studio_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/modules/pcm/pmd_smpte_337m.pmd_studio_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_decoder.pmd_studio_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_encoder.pmd_studio_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/modules/sadm/idmap.pmd_studio_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_generator.pmd_studio_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_ingester.pmd_studio_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_file.pmd_studio_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_string.pmd_studio_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_reader.pmd_studio_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_file.pmd_studio_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_string.pmd_studio_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_writer.pmd_studio_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_common_definitions.pmd_studio_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_model.pmd_studio_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_read_file.pmd_studio_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_read_string.pmd_studio_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_reader.pmd_studio_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_write_file.pmd_studio_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_write_string.pmd_studio_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_writer.pmd_studio_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/modules/sadm/sadm/idrefs.pmd_studio_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/modules/sadm/sadm/lookup3.pmd_studio_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_file.pmd_studio_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_string.pmd_studio_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/modules/xml/pmd_xml_reader.pmd_studio_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_file.pmd_studio_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_string.pmd_studio_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/modules/xml/pmd_xml_writer.pmd_studio_osx_amd64_clang_debug.d $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/ember_connector.pmd_studio_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/ember_controller.pmd_studio_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/parameter_manager.pmd_studio_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/utils/utils.pmd_studio_osx_amd64_clang_debug.d $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio.pmd_studio_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_beds.pmd_studio_osx_amd64_clang_debug.d $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_objects.pmd_studio_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_outputs.pmd_studio_osx_amd64_clang_debug.d $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_presentations.pmd_studio_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_console.pmd_studio_osx_amd64_clang_debug.d $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_console_emberplus.pmd_studio_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_device.pmd_studio_osx_amd64_clang_debug.d $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_settings.pmd_studio_osx_amd64_clang_debug.d
CLEAN_COMPILE_release = $(TMP)Lawo/ember-plus/libember_slim/Source/ber.pmd_studio_osx_amd64_clang_release.d $(TMP)Lawo/ember-plus/libember_slim/Source/berio.pmd_studio_osx_amd64_clang_release.d\
                        $(TMP)Lawo/ember-plus/libember_slim/Source/berreader.pmd_studio_osx_amd64_clang_release.d $(TMP)Lawo/ember-plus/libember_slim/Source/bertag.pmd_studio_osx_amd64_clang_release.d\
                        $(TMP)Lawo/ember-plus/libember_slim/Source/bytebuffer.pmd_studio_osx_amd64_clang_release.d $(TMP)Lawo/ember-plus/libember_slim/Source/ember.pmd_studio_osx_amd64_clang_release.d\
                        $(TMP)Lawo/ember-plus/libember_slim/Source/emberasyncreader.pmd_studio_osx_amd64_clang_release.d\
                        $(TMP)Lawo/ember-plus/libember_slim/Source/emberframing.pmd_studio_osx_amd64_clang_release.d $(TMP)Lawo/ember-plus/libember_slim/Source/emberinternal.pmd_studio_osx_amd64_clang_release.d\
                        $(TMP)Lawo/ember-plus/libember_slim/Source/glow.pmd_studio_osx_amd64_clang_release.d $(TMP)Lawo/ember-plus/libember_slim/Source/glowrx.pmd_studio_osx_amd64_clang_release.d\
                        $(TMP)Lawo/ember-plus/libember_slim/Source/glowtx.pmd_studio_osx_amd64_clang_release.d $(TMP)dlb_pmd/frontend/pmd_tool/pcm.pmd_studio_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool.pmd_studio_osx_amd64_clang_release.d $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool_klv.pmd_studio_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/frontend/pmd_tool/xml.pmd_studio_osx_amd64_clang_release.d $(TMP)dlb_pmd/os/osx/pmd_ctrl_c.pmd_studio_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/api/dlb_pmd.pmd_studio_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/api/dlb_pmd_api_read.pmd_studio_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/api/dlb_pmd_api_write.pmd_studio_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/api/dlb_pmd_equal.pmd_studio_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/api/dlb_pmd_metadata_set.pmd_studio_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/model/pmd_language.pmd_studio_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/capture/pmd_capture.pmd_studio_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/modules/generate/pmd_random_generator.pmd_studio_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/klv/pmd_crc32.pmd_studio_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/modules/klv/pmd_klv_reader.pmd_studio_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/klv/pmd_klv_writer.pmd_studio_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/modules/pcm/pmd_bitstream.pmd_studio_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_augmentor.pmd_studio_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_extractor.pmd_studio_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/pcm/pmd_smpte_337m.pmd_studio_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_decoder.pmd_studio_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_encoder.pmd_studio_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/modules/sadm/idmap.pmd_studio_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_generator.pmd_studio_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_ingester.pmd_studio_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_file.pmd_studio_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_string.pmd_studio_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_reader.pmd_studio_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_file.pmd_studio_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_string.pmd_studio_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_writer.pmd_studio_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_common_definitions.pmd_studio_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_model.pmd_studio_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_read_file.pmd_studio_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_read_string.pmd_studio_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_reader.pmd_studio_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_write_file.pmd_studio_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_write_string.pmd_studio_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_writer.pmd_studio_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/modules/sadm/sadm/idrefs.pmd_studio_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/sadm/sadm/lookup3.pmd_studio_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_file.pmd_studio_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_string.pmd_studio_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/modules/xml/pmd_xml_reader.pmd_studio_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_file.pmd_studio_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_string.pmd_studio_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/xml/pmd_xml_writer.pmd_studio_osx_amd64_clang_release.d $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/ember_connector.pmd_studio_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/ember_controller.pmd_studio_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/parameter_manager.pmd_studio_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/utils/utils.pmd_studio_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio.pmd_studio_osx_amd64_clang_release.d $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_beds.pmd_studio_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_objects.pmd_studio_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_outputs.pmd_studio_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_presentations.pmd_studio_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_console.pmd_studio_osx_amd64_clang_release.d $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_console_emberplus.pmd_studio_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_device.pmd_studio_osx_amd64_clang_release.d $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_settings.pmd_studio_osx_amd64_clang_release.d

cleandeps:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)

clean: cleanself
	$(MAKE) -C $(BASE)dlb_pmd/make/dlb_pmd_lib/osx_amd64_clang cleanself
	$(MAKE) -C $(BASE)dlb_pmd/make/dlb_wave_lib/osx_amd64_clang cleanself
	$(MAKE) -C $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/osx_amd64_clang cleanself
	$(MAKE) -C $(BASE)libui/alpha_4.1/libui/make/libui_alpha4_1/osx_amd64_clang cleanself
	$(MAKE) -C $(BASE)libui/alpha_4.1/libui/make/libui_impl_alpha4_1/osx_amd64_clang cleanself
	$(MAKE) -C $(BASE)portaudio/19.6/portaudio/make/portaudio_19_6_lib/osx_amd64_clang cleanself
	$(MAKE) -C $(BASE)zlib/1.2.11/make/zlib/osx_amd64_clang cleanself

cleanself:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)
	$(RM) $(INPUTS_LINK_debug)
	$(RM) $(INPUTS_LINK_release)
	$(RM) pmd_studio_debug
	$(RM) pmd_studio_release

.PHONY: default FORCE clean cleandeps cleanself help

FORCE:

help:
	@echo "This makefile has the following targets:"
	@echo " pmd_studio_debug"
	@echo " pmd_studio_release"
	@echo " default"
	@echo " clean"
	@echo " cleandeps"
	@echo " help"

include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/ber.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/berio.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/berreader.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/bertag.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/bytebuffer.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/ember.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/emberasyncreader.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/emberframing.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/emberinternal.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/glow.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/glowrx.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/glowtx.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pcm.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool_klv.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/xml.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/os/osx/pmd_ctrl_c.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_api_read.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_api_write.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_equal.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_metadata_set.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/model/pmd_language.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/capture/pmd_capture.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/generate/pmd_random_generator.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/klv/pmd_crc32.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/klv/pmd_klv_reader.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/klv/pmd_klv_writer.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_bitstream.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_augmentor.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_extractor.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_smpte_337m.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_decoder.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_encoder.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/idmap.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_generator.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_ingester.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_file.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_string.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_reader.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_file.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_string.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_writer.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_common_definitions.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_model.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_read_file.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_read_string.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_reader.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_write_file.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_write_string.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_writer.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/idrefs.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/lookup3.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_file.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_string.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_reader.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_file.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_string.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_writer.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/ember_connector.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/ember_controller.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/parameter_manager.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/utils/utils.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_beds.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_objects.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_outputs.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_presentations.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_console.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_console_emberplus.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_device.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_settings.pmd_studio_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/ber.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/berio.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/berreader.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/bertag.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/bytebuffer.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/ember.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/emberasyncreader.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/emberframing.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/emberinternal.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/glow.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/glowrx.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/glowtx.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pcm.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool_klv.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/xml.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/os/osx/pmd_ctrl_c.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_api_read.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_api_write.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_equal.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_metadata_set.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/model/pmd_language.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/capture/pmd_capture.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/generate/pmd_random_generator.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/klv/pmd_crc32.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/klv/pmd_klv_reader.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/klv/pmd_klv_writer.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_bitstream.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_augmentor.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_extractor.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_smpte_337m.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_decoder.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_encoder.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/idmap.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_generator.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_ingester.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_file.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_string.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_reader.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_file.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_string.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_writer.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_common_definitions.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_model.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_read_file.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_read_string.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_reader.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_write_file.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_write_string.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_writer.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/idrefs.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/lookup3.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_file.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_string.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_reader.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_file.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_string.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_writer.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/ember_connector.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/ember_controller.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/parameter_manager.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/utils/utils.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_beds.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_objects.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_outputs.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_presentations.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_console.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_console_emberplus.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_device.pmd_studio_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_settings.pmd_studio_osx_amd64_clang_release.d)

$(BASE)dlb_pmd/make/dlb_pmd_lib/osx_amd64_clang/dlb_pmd_lib_debug.a \
$(BASE)dlb_pmd/make/dlb_pmd_lib/osx_amd64_clang/dlb_pmd_lib_release.a \
$(BASE)dlb_pmd/make/dlb_wave_lib/osx_amd64_clang/dlb_wave_lib_debug.a \
$(BASE)dlb_pmd/make/dlb_wave_lib/osx_amd64_clang/dlb_wave_lib_release.a \
$(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/osx_amd64_clang/dlb_xml_debug.a \
$(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/osx_amd64_clang/dlb_xml_release.a \
$(BASE)libui/alpha_4.1/libui/make/libui_alpha4_1/osx_amd64_clang/libui_alpha4_1_debug.a \
$(BASE)libui/alpha_4.1/libui/make/libui_alpha4_1/osx_amd64_clang/libui_alpha4_1_release.a \
$(BASE)libui/alpha_4.1/libui/make/libui_impl_alpha4_1/osx_amd64_clang/libui_impl_alpha4_1_debug.a \
$(BASE)libui/alpha_4.1/libui/make/libui_impl_alpha4_1/osx_amd64_clang/libui_impl_alpha4_1_release.a \
$(BASE)portaudio/19.6/portaudio/make/portaudio_19_6_lib/osx_amd64_clang/portaudio_19_6_lib_debug.a \
$(BASE)portaudio/19.6/portaudio/make/portaudio_19_6_lib/osx_amd64_clang/portaudio_19_6_lib_release.a \
$(BASE)zlib/1.2.11/make/zlib/osx_amd64_clang/zlib_debug.a \
$(BASE)zlib/1.2.11/make/zlib/osx_amd64_clang/zlib_release.a: %.a: FORCE
	$(MAKE) -C $(@D) $(@F)


