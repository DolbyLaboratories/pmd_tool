#-*-makefile-*-

BASE=../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
CCDEP = $(CC)
CFLAGS_debug = -D_GNU_SOURCE -g -ggdb3 -O0 -m64 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
CFLAGS_release = -D_GNU_SOURCE -O3 -ftree-vectorize -m64 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
CXXDEP = $(CXX)
CXXFLAGS_debug = -std=c++11 -std=c++17 -g -ggdb3 -O0 -m64 -c -Wall
CXXFLAGS_release = -std=c++11 -std=c++17 -O3 -ftree-vectorize -m64 -c -Wall
DEFINES_debug = -DELPP_NO_DEFAULT_LOG_FILE=1
DEFINES_release = -DELPP_NO_DEFAULT_LOG_FILE=1 -DNDEBUG=1
INCLUDES = -I$(BASE)Lawo/ember-plus/libember_slim/Source -I$(BASE)Lawo/ember-plus -I$(BASE)dlb_buffer/1.3 -I$(BASE)dlb_octfile/1.1 -I$(BASE)dlb_pmd/frontend -I$(BASE)dlb_pmd/frontend/pmd_studio/ember_connector -I$(BASE)dlb_pmd/frontend/pmd_studio/ember_connector/utils -I$(BASE)dlb_pmd/frontend/pmd_studio -I$(BASE)dlb_pmd/frontend/pmd_studio/rivermax -I$(BASE)dlb_pmd/frontend/pmd_tool -I$(BASE)dlb_pmd/frontend -I$(BASE)dlb_pmd/include -I$(BASE)dlb_pmd -I$(BASE)dlb_pmd/os -I$(BASE)dlb_pmd/src/model -I$(BASE)dlb_pmd/src/modules/klv -I$(BASE)dlb_pmd/src/modules/pcm -I$(BASE)dlb_pmd/src/modules/sadm -I$(BASE)dlb_pmd/src/modules/xml -I$(BASE)dlb_socket/0.1/dlb_socket/include/posix -I$(BASE)dlb_socket/0.1/dlb_socket/include -I$(BASE)dlb_socket/0.1 -I$(BASE)dlb_st2110/0.9/dlb_st2110/include -I$(BASE)../../../../../../../usr/include/mellanox -I$(BASE)dlb_wave/1.6 -I$(BASE)dlb_xmllib/1.0.4 -I`pkg-config --cflags gtk+-3.0` -I$(BASE)libui/alpha_4.1/libui -I$(BASE). -I$(BASE). -I$(BASE)zlib/1.2.11/zlib-1.2.11
LD = $(CXX)
LDFLAGS_debug = -m64
LDFLAGS_release = -O2 -m64
LDLIBS = -lpthread -lavahi-client -lavahi-common -lrivermax -lpthread -lgtk-3 -lgdk-3 -lpangocairo-1.0 -lpango-1.0 -latk-1.0 -lcairo-gobject -lcairo -lgdk_pixbuf-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0 -lm -ldl -Wl,--no-undefined -Wl,--no-allow-shlib-undefined


INPUTS_LINK_debug = $(TMP)Lawo/ember-plus/libember_slim/Source/ber.pmd_studio_rivermax_linux_amd64_gnu_debug.o $(TMP)Lawo/ember-plus/libember_slim/Source/berio.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)Lawo/ember-plus/libember_slim/Source/berreader.pmd_studio_rivermax_linux_amd64_gnu_debug.o $(TMP)Lawo/ember-plus/libember_slim/Source/bertag.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)Lawo/ember-plus/libember_slim/Source/bytebuffer.pmd_studio_rivermax_linux_amd64_gnu_debug.o $(TMP)Lawo/ember-plus/libember_slim/Source/ember.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)Lawo/ember-plus/libember_slim/Source/emberasyncreader.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)Lawo/ember-plus/libember_slim/Source/emberframing.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)Lawo/ember-plus/libember_slim/Source/emberinternal.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)Lawo/ember-plus/libember_slim/Source/glow.pmd_studio_rivermax_linux_amd64_gnu_debug.o $(TMP)Lawo/ember-plus/libember_slim/Source/glowrx.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)Lawo/ember-plus/libember_slim/Source/glowtx.pmd_studio_rivermax_linux_amd64_gnu_debug.o $(TMP)dlb_pmd/frontend/pmd_tool/pcm.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool.pmd_studio_rivermax_linux_amd64_gnu_debug.o $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool_klv.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/frontend/pmd_tool/xml.pmd_studio_rivermax_linux_amd64_gnu_debug.o $(TMP)dlb_pmd/os/linux/pmd_ctrl_c.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/src/api/dlb_pmd.pmd_studio_rivermax_linux_amd64_gnu_debug.o $(TMP)dlb_pmd/src/api/dlb_pmd_api_read.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/src/api/dlb_pmd_api_write.pmd_studio_rivermax_linux_amd64_gnu_debug.o $(TMP)dlb_pmd/src/api/dlb_pmd_equal.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/src/api/dlb_pmd_metadata_set.pmd_studio_rivermax_linux_amd64_gnu_debug.o $(TMP)dlb_pmd/src/model/pmd_language.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/src/modules/capture/pmd_capture.pmd_studio_rivermax_linux_amd64_gnu_debug.o $(TMP)dlb_pmd/src/modules/generate/pmd_random_generator.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/src/modules/klv/pmd_crc32.pmd_studio_rivermax_linux_amd64_gnu_debug.o $(TMP)dlb_pmd/src/modules/klv/pmd_klv_reader.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/src/modules/klv/pmd_klv_writer.pmd_studio_rivermax_linux_amd64_gnu_debug.o $(TMP)dlb_pmd/src/modules/pcm/pmd_bitstream.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_augmentor.pmd_studio_rivermax_linux_amd64_gnu_debug.o $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_extractor.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/src/modules/pcm/pmd_smpte_337m.pmd_studio_rivermax_linux_amd64_gnu_debug.o $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_decoder.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_encoder.pmd_studio_rivermax_linux_amd64_gnu_debug.o $(TMP)dlb_pmd/src/modules/sadm/idmap.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_generator.pmd_studio_rivermax_linux_amd64_gnu_debug.o $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_ingester.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_file.pmd_studio_rivermax_linux_amd64_gnu_debug.o $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_string.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_reader.pmd_studio_rivermax_linux_amd64_gnu_debug.o $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_file.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_string.pmd_studio_rivermax_linux_amd64_gnu_debug.o $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_writer.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_common_definitions.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_model.pmd_studio_rivermax_linux_amd64_gnu_debug.o $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_read_file.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_read_string.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_reader.pmd_studio_rivermax_linux_amd64_gnu_debug.o $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_write_file.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_write_string.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_writer.pmd_studio_rivermax_linux_amd64_gnu_debug.o $(TMP)dlb_pmd/src/modules/sadm/sadm/idrefs.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/src/modules/sadm/sadm/lookup3.pmd_studio_rivermax_linux_amd64_gnu_debug.o $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_file.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_string.pmd_studio_rivermax_linux_amd64_gnu_debug.o $(TMP)dlb_pmd/src/modules/xml/pmd_xml_reader.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_file.pmd_studio_rivermax_linux_amd64_gnu_debug.o $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_string.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/src/modules/xml/pmd_xml_writer.pmd_studio_rivermax_linux_amd64_gnu_debug.o $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/ember_connector.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/ember_controller.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/parameter_manager.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/utils/utils.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio.pmd_studio_rivermax_linux_amd64_gnu_debug.o $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_beds.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_objects.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_outputs.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_presentations.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_console.pmd_studio_rivermax_linux_amd64_gnu_debug.o $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_console_emberplus.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_settings.pmd_studio_rivermax_linux_amd64_gnu_debug.o $(TMP)dlb_pmd/frontend/pmd_studio/rivermax/pmd_studio_device.pmd_studio_rivermax_linux_amd64_gnu_debug.o\
                    $(TMP)dlb_pmd/frontend/pmd_studio/rivermax/pmd_studio_device_settings.pmd_studio_rivermax_linux_amd64_gnu_debug.o
INPUTS_LINK_release = $(TMP)Lawo/ember-plus/libember_slim/Source/ber.pmd_studio_rivermax_linux_amd64_gnu_release.o $(TMP)Lawo/ember-plus/libember_slim/Source/berio.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)Lawo/ember-plus/libember_slim/Source/berreader.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)Lawo/ember-plus/libember_slim/Source/bertag.pmd_studio_rivermax_linux_amd64_gnu_release.o $(TMP)Lawo/ember-plus/libember_slim/Source/bytebuffer.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)Lawo/ember-plus/libember_slim/Source/ember.pmd_studio_rivermax_linux_amd64_gnu_release.o $(TMP)Lawo/ember-plus/libember_slim/Source/emberasyncreader.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)Lawo/ember-plus/libember_slim/Source/emberframing.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)Lawo/ember-plus/libember_slim/Source/emberinternal.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)Lawo/ember-plus/libember_slim/Source/glow.pmd_studio_rivermax_linux_amd64_gnu_release.o $(TMP)Lawo/ember-plus/libember_slim/Source/glowrx.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)Lawo/ember-plus/libember_slim/Source/glowtx.pmd_studio_rivermax_linux_amd64_gnu_release.o $(TMP)dlb_pmd/frontend/pmd_tool/pcm.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool.pmd_studio_rivermax_linux_amd64_gnu_release.o $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool_klv.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/frontend/pmd_tool/xml.pmd_studio_rivermax_linux_amd64_gnu_release.o $(TMP)dlb_pmd/os/linux/pmd_ctrl_c.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/src/api/dlb_pmd.pmd_studio_rivermax_linux_amd64_gnu_release.o $(TMP)dlb_pmd/src/api/dlb_pmd_api_read.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/src/api/dlb_pmd_api_write.pmd_studio_rivermax_linux_amd64_gnu_release.o $(TMP)dlb_pmd/src/api/dlb_pmd_equal.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/src/api/dlb_pmd_metadata_set.pmd_studio_rivermax_linux_amd64_gnu_release.o $(TMP)dlb_pmd/src/model/pmd_language.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/src/modules/capture/pmd_capture.pmd_studio_rivermax_linux_amd64_gnu_release.o $(TMP)dlb_pmd/src/modules/generate/pmd_random_generator.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/src/modules/klv/pmd_crc32.pmd_studio_rivermax_linux_amd64_gnu_release.o $(TMP)dlb_pmd/src/modules/klv/pmd_klv_reader.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/src/modules/klv/pmd_klv_writer.pmd_studio_rivermax_linux_amd64_gnu_release.o $(TMP)dlb_pmd/src/modules/pcm/pmd_bitstream.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_augmentor.pmd_studio_rivermax_linux_amd64_gnu_release.o $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_extractor.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/src/modules/pcm/pmd_smpte_337m.pmd_studio_rivermax_linux_amd64_gnu_release.o $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_decoder.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_encoder.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/src/modules/sadm/idmap.pmd_studio_rivermax_linux_amd64_gnu_release.o $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_generator.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_ingester.pmd_studio_rivermax_linux_amd64_gnu_release.o $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_file.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_string.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_reader.pmd_studio_rivermax_linux_amd64_gnu_release.o $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_file.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_string.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_writer.pmd_studio_rivermax_linux_amd64_gnu_release.o $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_common_definitions.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_model.pmd_studio_rivermax_linux_amd64_gnu_release.o $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_read_file.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_read_string.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_reader.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_write_file.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_write_string.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_writer.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/src/modules/sadm/sadm/idrefs.pmd_studio_rivermax_linux_amd64_gnu_release.o $(TMP)dlb_pmd/src/modules/sadm/sadm/lookup3.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_file.pmd_studio_rivermax_linux_amd64_gnu_release.o $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_string.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/src/modules/xml/pmd_xml_reader.pmd_studio_rivermax_linux_amd64_gnu_release.o $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_file.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_string.pmd_studio_rivermax_linux_amd64_gnu_release.o $(TMP)dlb_pmd/src/modules/xml/pmd_xml_writer.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/ember_connector.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/ember_controller.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/parameter_manager.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/utils/utils.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio.pmd_studio_rivermax_linux_amd64_gnu_release.o $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_beds.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_objects.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_outputs.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_presentations.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_console.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_console_emberplus.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_settings.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/frontend/pmd_studio/rivermax/pmd_studio_device.pmd_studio_rivermax_linux_amd64_gnu_release.o\
                      $(TMP)dlb_pmd/frontend/pmd_studio/rivermax/pmd_studio_device_settings.pmd_studio_rivermax_linux_amd64_gnu_release.o

DEP_PROJ_LINK_debug = $(BASE)dlb_pmd/make/dlb_pmd_lib/linux_amd64_gnu/dlb_pmd_lib_debug.a $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_amd64_gnu/dlb_xml_debug.a $(BASE)zlib/1.2.11/make/zlib/linux_amd64_gnu/zlib_debug.a $(BASE)dlb_pmd/make/dlb_wave_lib/linux_amd64_gnu/dlb_wave_lib_debug.a $(BASE)dlb_pmd/make/dlb_st2110_lib/linux_amd64_gnu/dlb_st2110_lib_debug.a $(BASE)libui/alpha_4.1/libui/make/libui_alpha4_1/linux_amd64_gnu/libui_alpha4_1_debug.a $(BASE)libui/alpha_4.1/libui/make/libui_impl_alpha4_1/linux_amd64_gnu/libui_impl_alpha4_1_debug.a
DEP_PROJ_LINK_release = $(BASE)dlb_pmd/make/dlb_pmd_lib/linux_amd64_gnu/dlb_pmd_lib_release.a $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_amd64_gnu/dlb_xml_release.a $(BASE)zlib/1.2.11/make/zlib/linux_amd64_gnu/zlib_release.a $(BASE)dlb_pmd/make/dlb_wave_lib/linux_amd64_gnu/dlb_wave_lib_release.a $(BASE)dlb_pmd/make/dlb_st2110_lib/linux_amd64_gnu/dlb_st2110_lib_release.a $(BASE)libui/alpha_4.1/libui/make/libui_alpha4_1/linux_amd64_gnu/libui_alpha4_1_release.a $(BASE)libui/alpha_4.1/libui/make/libui_impl_alpha4_1/linux_amd64_gnu/libui_impl_alpha4_1_release.a

default: pmd_studio_rivermax_debug pmd_studio_rivermax_release

pmd_studio_rivermax_debug: $(INPUTS_LINK_debug) $(DEP_PROJ_LINK_debug)
	$(LD) $(LDFLAGS_debug) -o $@ $+ $(LDLIBS)

pmd_studio_rivermax_release: $(INPUTS_LINK_release) $(DEP_PROJ_LINK_release)
	$(LD) $(LDFLAGS_release) -o $@ $+ $(LDLIBS)

$(TMP)%.pmd_studio_rivermax_linux_amd64_gnu_debug.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -MM $(DEFINES_debug) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_debug) $(DEFINES_debug) $(INCLUDES) -o $@ $<

$(TMP)%.pmd_studio_rivermax_linux_amd64_gnu_debug.o: $(BASE)%.cpp | $(TMP)%/..
	$(CXXDEP) -std=c++11 -MM $(DEFINES_debug) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS_debug) $(DEFINES_debug) $(INCLUDES) -o $@ $<

$(TMP)%.pmd_studio_rivermax_linux_amd64_gnu_release.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

$(TMP)%.pmd_studio_rivermax_linux_amd64_gnu_release.o: $(BASE)%.cpp | $(TMP)%/..
	$(CXXDEP) -std=c++11 -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMPILE_debug = $(TMP)Lawo/ember-plus/libember_slim/Source/ber.pmd_studio_rivermax_linux_amd64_gnu_debug.d $(TMP)Lawo/ember-plus/libember_slim/Source/berio.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)Lawo/ember-plus/libember_slim/Source/berreader.pmd_studio_rivermax_linux_amd64_gnu_debug.d $(TMP)Lawo/ember-plus/libember_slim/Source/bertag.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)Lawo/ember-plus/libember_slim/Source/bytebuffer.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)Lawo/ember-plus/libember_slim/Source/ember.pmd_studio_rivermax_linux_amd64_gnu_debug.d $(TMP)Lawo/ember-plus/libember_slim/Source/emberasyncreader.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)Lawo/ember-plus/libember_slim/Source/emberframing.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)Lawo/ember-plus/libember_slim/Source/emberinternal.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)Lawo/ember-plus/libember_slim/Source/glow.pmd_studio_rivermax_linux_amd64_gnu_debug.d $(TMP)Lawo/ember-plus/libember_slim/Source/glowrx.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)Lawo/ember-plus/libember_slim/Source/glowtx.pmd_studio_rivermax_linux_amd64_gnu_debug.d $(TMP)dlb_pmd/frontend/pmd_tool/pcm.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool.pmd_studio_rivermax_linux_amd64_gnu_debug.d $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool_klv.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/frontend/pmd_tool/xml.pmd_studio_rivermax_linux_amd64_gnu_debug.d $(TMP)dlb_pmd/os/linux/pmd_ctrl_c.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/src/api/dlb_pmd.pmd_studio_rivermax_linux_amd64_gnu_debug.d $(TMP)dlb_pmd/src/api/dlb_pmd_api_read.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/src/api/dlb_pmd_api_write.pmd_studio_rivermax_linux_amd64_gnu_debug.d $(TMP)dlb_pmd/src/api/dlb_pmd_equal.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/src/api/dlb_pmd_metadata_set.pmd_studio_rivermax_linux_amd64_gnu_debug.d $(TMP)dlb_pmd/src/model/pmd_language.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/src/modules/capture/pmd_capture.pmd_studio_rivermax_linux_amd64_gnu_debug.d $(TMP)dlb_pmd/src/modules/generate/pmd_random_generator.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/src/modules/klv/pmd_crc32.pmd_studio_rivermax_linux_amd64_gnu_debug.d $(TMP)dlb_pmd/src/modules/klv/pmd_klv_reader.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/src/modules/klv/pmd_klv_writer.pmd_studio_rivermax_linux_amd64_gnu_debug.d $(TMP)dlb_pmd/src/modules/pcm/pmd_bitstream.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_augmentor.pmd_studio_rivermax_linux_amd64_gnu_debug.d $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_extractor.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/src/modules/pcm/pmd_smpte_337m.pmd_studio_rivermax_linux_amd64_gnu_debug.d $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_decoder.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_encoder.pmd_studio_rivermax_linux_amd64_gnu_debug.d $(TMP)dlb_pmd/src/modules/sadm/idmap.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_generator.pmd_studio_rivermax_linux_amd64_gnu_debug.d $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_ingester.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_file.pmd_studio_rivermax_linux_amd64_gnu_debug.d $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_string.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_reader.pmd_studio_rivermax_linux_amd64_gnu_debug.d $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_file.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_string.pmd_studio_rivermax_linux_amd64_gnu_debug.d $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_writer.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_common_definitions.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_model.pmd_studio_rivermax_linux_amd64_gnu_debug.d $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_read_file.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_read_string.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_reader.pmd_studio_rivermax_linux_amd64_gnu_debug.d $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_write_file.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_write_string.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_writer.pmd_studio_rivermax_linux_amd64_gnu_debug.d $(TMP)dlb_pmd/src/modules/sadm/sadm/idrefs.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/src/modules/sadm/sadm/lookup3.pmd_studio_rivermax_linux_amd64_gnu_debug.d $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_file.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_string.pmd_studio_rivermax_linux_amd64_gnu_debug.d $(TMP)dlb_pmd/src/modules/xml/pmd_xml_reader.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_file.pmd_studio_rivermax_linux_amd64_gnu_debug.d $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_string.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/src/modules/xml/pmd_xml_writer.pmd_studio_rivermax_linux_amd64_gnu_debug.d $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/ember_connector.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/ember_controller.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/parameter_manager.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/utils/utils.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio.pmd_studio_rivermax_linux_amd64_gnu_debug.d $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_beds.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_objects.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_outputs.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_presentations.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_console.pmd_studio_rivermax_linux_amd64_gnu_debug.d $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_console_emberplus.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_settings.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/frontend/pmd_studio/rivermax/pmd_studio_device.pmd_studio_rivermax_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_pmd/frontend/pmd_studio/rivermax/pmd_studio_device_settings.pmd_studio_rivermax_linux_amd64_gnu_debug.d
CLEAN_COMPILE_release = $(TMP)Lawo/ember-plus/libember_slim/Source/ber.pmd_studio_rivermax_linux_amd64_gnu_release.d $(TMP)Lawo/ember-plus/libember_slim/Source/berio.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)Lawo/ember-plus/libember_slim/Source/berreader.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)Lawo/ember-plus/libember_slim/Source/bertag.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)Lawo/ember-plus/libember_slim/Source/bytebuffer.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)Lawo/ember-plus/libember_slim/Source/ember.pmd_studio_rivermax_linux_amd64_gnu_release.d $(TMP)Lawo/ember-plus/libember_slim/Source/emberasyncreader.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)Lawo/ember-plus/libember_slim/Source/emberframing.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)Lawo/ember-plus/libember_slim/Source/emberinternal.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)Lawo/ember-plus/libember_slim/Source/glow.pmd_studio_rivermax_linux_amd64_gnu_release.d $(TMP)Lawo/ember-plus/libember_slim/Source/glowrx.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)Lawo/ember-plus/libember_slim/Source/glowtx.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/frontend/pmd_tool/pcm.pmd_studio_rivermax_linux_amd64_gnu_release.d $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool_klv.pmd_studio_rivermax_linux_amd64_gnu_release.d $(TMP)dlb_pmd/frontend/pmd_tool/xml.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/os/linux/pmd_ctrl_c.pmd_studio_rivermax_linux_amd64_gnu_release.d $(TMP)dlb_pmd/src/api/dlb_pmd.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/src/api/dlb_pmd_api_read.pmd_studio_rivermax_linux_amd64_gnu_release.d $(TMP)dlb_pmd/src/api/dlb_pmd_api_write.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/src/api/dlb_pmd_equal.pmd_studio_rivermax_linux_amd64_gnu_release.d $(TMP)dlb_pmd/src/api/dlb_pmd_metadata_set.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/src/model/pmd_language.pmd_studio_rivermax_linux_amd64_gnu_release.d $(TMP)dlb_pmd/src/modules/capture/pmd_capture.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/src/modules/generate/pmd_random_generator.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/src/modules/klv/pmd_crc32.pmd_studio_rivermax_linux_amd64_gnu_release.d $(TMP)dlb_pmd/src/modules/klv/pmd_klv_reader.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/src/modules/klv/pmd_klv_writer.pmd_studio_rivermax_linux_amd64_gnu_release.d $(TMP)dlb_pmd/src/modules/pcm/pmd_bitstream.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_augmentor.pmd_studio_rivermax_linux_amd64_gnu_release.d $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_extractor.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/src/modules/pcm/pmd_smpte_337m.pmd_studio_rivermax_linux_amd64_gnu_release.d $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_decoder.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_encoder.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/src/modules/sadm/idmap.pmd_studio_rivermax_linux_amd64_gnu_release.d $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_generator.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_ingester.pmd_studio_rivermax_linux_amd64_gnu_release.d $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_file.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_string.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_reader.pmd_studio_rivermax_linux_amd64_gnu_release.d $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_file.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_string.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_writer.pmd_studio_rivermax_linux_amd64_gnu_release.d $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_common_definitions.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_model.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_read_file.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_read_string.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_reader.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_write_file.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_write_string.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_writer.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/src/modules/sadm/sadm/idrefs.pmd_studio_rivermax_linux_amd64_gnu_release.d $(TMP)dlb_pmd/src/modules/sadm/sadm/lookup3.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_file.pmd_studio_rivermax_linux_amd64_gnu_release.d $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_string.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/src/modules/xml/pmd_xml_reader.pmd_studio_rivermax_linux_amd64_gnu_release.d $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_file.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_string.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/src/modules/xml/pmd_xml_writer.pmd_studio_rivermax_linux_amd64_gnu_release.d $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/ember_connector.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/ember_controller.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/parameter_manager.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/utils/utils.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio.pmd_studio_rivermax_linux_amd64_gnu_release.d $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_beds.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_objects.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_outputs.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_presentations.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_console.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_console_emberplus.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_settings.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/frontend/pmd_studio/rivermax/pmd_studio_device.pmd_studio_rivermax_linux_amd64_gnu_release.d\
                        $(TMP)dlb_pmd/frontend/pmd_studio/rivermax/pmd_studio_device_settings.pmd_studio_rivermax_linux_amd64_gnu_release.d

cleandeps:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)

clean: cleanself
	$(MAKE) -C $(BASE)dlb_pmd/make/dlb_pmd_lib/linux_amd64_gnu cleanself
	$(MAKE) -C $(BASE)dlb_pmd/make/dlb_st2110_lib/linux_amd64_gnu cleanself
	$(MAKE) -C $(BASE)dlb_pmd/make/dlb_wave_lib/linux_amd64_gnu cleanself
	$(MAKE) -C $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_amd64_gnu cleanself
	$(MAKE) -C $(BASE)libui/alpha_4.1/libui/make/libui_alpha4_1/linux_amd64_gnu cleanself
	$(MAKE) -C $(BASE)libui/alpha_4.1/libui/make/libui_impl_alpha4_1/linux_amd64_gnu cleanself
	$(MAKE) -C $(BASE)zlib/1.2.11/make/zlib/linux_amd64_gnu cleanself

cleanself:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)
	$(RM) $(INPUTS_LINK_debug)
	$(RM) $(INPUTS_LINK_release)
	$(RM) pmd_studio_rivermax_debug
	$(RM) pmd_studio_rivermax_release

.PHONY: default FORCE clean cleandeps cleanself help

FORCE:

help:
	@echo "This makefile has the following targets:"
	@echo " pmd_studio_rivermax_debug"
	@echo " pmd_studio_rivermax_release"
	@echo " default"
	@echo " clean"
	@echo " cleandeps"
	@echo " help"

include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/ber.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/berio.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/berreader.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/bertag.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/bytebuffer.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/ember.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/emberasyncreader.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/emberframing.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/emberinternal.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/glow.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/glowrx.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/glowtx.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pcm.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool_klv.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/xml.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/os/linux/pmd_ctrl_c.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_api_read.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_api_write.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_equal.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_metadata_set.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/model/pmd_language.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/capture/pmd_capture.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/generate/pmd_random_generator.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/klv/pmd_crc32.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/klv/pmd_klv_reader.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/klv/pmd_klv_writer.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_bitstream.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_augmentor.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_extractor.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_smpte_337m.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_decoder.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_encoder.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/idmap.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_generator.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_ingester.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_file.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_string.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_reader.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_file.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_string.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_writer.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_common_definitions.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_model.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_read_file.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_read_string.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_reader.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_write_file.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_write_string.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_writer.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/idrefs.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/lookup3.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_file.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_string.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_reader.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_file.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_string.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_writer.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/ember_connector.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/ember_controller.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/parameter_manager.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/utils/utils.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_beds.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_objects.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_outputs.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_presentations.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_console.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_console_emberplus.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_settings.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/rivermax/pmd_studio_device.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/rivermax/pmd_studio_device_settings.pmd_studio_rivermax_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/ber.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/berio.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/berreader.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/bertag.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/bytebuffer.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/ember.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/emberasyncreader.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/emberframing.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/emberinternal.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/glow.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/glowrx.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)Lawo/ember-plus/libember_slim/Source/glowtx.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pcm.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/pmd_tool_klv.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_tool/xml.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/os/linux/pmd_ctrl_c.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_api_read.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_api_write.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_equal.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_metadata_set.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/model/pmd_language.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/capture/pmd_capture.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/generate/pmd_random_generator.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/klv/pmd_crc32.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/klv/pmd_klv_reader.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/klv/pmd_klv_writer.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_bitstream.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_augmentor.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_extractor.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_smpte_337m.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_decoder.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_encoder.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/idmap.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_generator.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_ingester.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_file.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_string.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_reader.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_file.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_string.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_writer.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_common_definitions.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_model.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_read_file.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_read_string.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_reader.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_write_file.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_write_string.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/dlb_sadm_writer.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/idrefs.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/sadm/lookup3.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_file.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_string.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_reader.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_file.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_string.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_writer.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/ember_connector.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/ember_controller.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/parameter_manager.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/ember_connector/utils/utils.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_beds.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_objects.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_outputs.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_audio_presentations.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_console.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_console_emberplus.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/pmd_studio_settings.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/rivermax/pmd_studio_device.pmd_studio_rivermax_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_pmd/frontend/pmd_studio/rivermax/pmd_studio_device_settings.pmd_studio_rivermax_linux_amd64_gnu_release.d)

$(BASE)dlb_pmd/make/dlb_pmd_lib/linux_amd64_gnu/dlb_pmd_lib_debug.a \
$(BASE)dlb_pmd/make/dlb_pmd_lib/linux_amd64_gnu/dlb_pmd_lib_release.a \
$(BASE)dlb_pmd/make/dlb_st2110_lib/linux_amd64_gnu/dlb_st2110_lib_debug.a \
$(BASE)dlb_pmd/make/dlb_st2110_lib/linux_amd64_gnu/dlb_st2110_lib_release.a \
$(BASE)dlb_pmd/make/dlb_wave_lib/linux_amd64_gnu/dlb_wave_lib_debug.a \
$(BASE)dlb_pmd/make/dlb_wave_lib/linux_amd64_gnu/dlb_wave_lib_release.a \
$(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_amd64_gnu/dlb_xml_debug.a \
$(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_amd64_gnu/dlb_xml_release.a \
$(BASE)libui/alpha_4.1/libui/make/libui_alpha4_1/linux_amd64_gnu/libui_alpha4_1_debug.a \
$(BASE)libui/alpha_4.1/libui/make/libui_alpha4_1/linux_amd64_gnu/libui_alpha4_1_release.a \
$(BASE)libui/alpha_4.1/libui/make/libui_impl_alpha4_1/linux_amd64_gnu/libui_impl_alpha4_1_debug.a \
$(BASE)libui/alpha_4.1/libui/make/libui_impl_alpha4_1/linux_amd64_gnu/libui_impl_alpha4_1_release.a \
$(BASE)zlib/1.2.11/make/zlib/linux_amd64_gnu/zlib_debug.a \
$(BASE)zlib/1.2.11/make/zlib/linux_amd64_gnu/zlib_release.a: %.a: FORCE
	$(MAKE) -C $(@D) $(@F)


