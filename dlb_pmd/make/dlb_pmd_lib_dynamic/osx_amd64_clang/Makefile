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
CXXFLAGS_debug = -std=c++11 -std=c++17 -g -O0 -arch x86_64 -fPIC -c -Wall
CXXFLAGS_release = -std=c++11 -std=c++17 -O3 -arch x86_64 -fPIC -c -Wall
DEFINES_debug = -DELPP_NO_DEFAULT_LOG_FILE=1 -DHAVE_HIDDEN=1 -D_LARGEFILE64_SOURCE=1
DEFINES_release = -DELPP_NO_DEFAULT_LOG_FILE=1 -DHAVE_HIDDEN=1 -DNDEBUG=1 -D_LARGEFILE64_SOURCE=1
INCLUDES = -I$(BASE)boost/boost_1_73_0 -I$(BASE)dlb_adm/include -I$(BASE)dlb_buffer/1.3 -I$(BASE)dlb_octfile/1.1 -I$(BASE)dlb_pmd/include -I$(BASE)dlb_pmd -I$(BASE)dlb_pmd/os -I$(BASE)dlb_pmd/src/model -I$(BASE)dlb_pmd/src/modules/klv -I$(BASE)dlb_pmd/src/modules/pcm -I$(BASE)dlb_pmd/src/modules/sadm -I$(BASE)dlb_pmd/src/modules/xml -I$(BASE)dlb_xmllib/1.0.4 -I$(BASE). -I$(BASE). -I$(BASE)dlb_xmllib/1.0.4 -I$(BASE). -I$(BASE)zlib/1.2.11/zlib-1.2.11


INPUTS_ARCHIVE_debug = $(TMP)dlb_adm/src/dlb_adm_api.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)dlb_pmd/os/osx/pmd_ctrl_c.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_pmd/src/api/dlb_pmd.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/api/dlb_pmd_api_read.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_pmd/src/api/dlb_pmd_api_write.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/api/dlb_pmd_equal.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_pmd/src/api/dlb_pmd_metadata_set.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/model/pmd_language.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_pmd/src/model/pmd_model.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/model/pmd_model_combo.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_pmd/src/modules/capture/pmd_capture.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/modules/generate/pmd_random_generator.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_pmd/src/modules/klv/pmd_crc32.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/modules/klv/pmd_klv_reader.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_pmd/src/modules/klv/pmd_klv_writer.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/modules/pcm/pmd_bitstream.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_augmentor.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_extractor.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_pmd/src/modules/pcm/pmd_smpte_337m.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_decoder.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_encoder.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_pmd/src/modules/sadm/pmd_core_model_generator.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_pmd/src/modules/sadm/pmd_core_model_ingester.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_file.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_file.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_file.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_string.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_pmd/src/modules/xml/pmd_xml_reader.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_file.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_string.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)dlb_pmd/src/modules/xml/pmd_xml_writer.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_xmllib/1.0.4/dlb_xml/src/dlb_xml.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)zlib/1.2.11/zlib-1.2.11/adler32.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)zlib/1.2.11/zlib-1.2.11/compress.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)zlib/1.2.11/zlib-1.2.11/crc32.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)zlib/1.2.11/zlib-1.2.11/deflate.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)zlib/1.2.11/zlib-1.2.11/gzclose.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)zlib/1.2.11/zlib-1.2.11/gzlib.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)zlib/1.2.11/zlib-1.2.11/gzread.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)zlib/1.2.11/zlib-1.2.11/gzwrite.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)zlib/1.2.11/zlib-1.2.11/infback.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)zlib/1.2.11/zlib-1.2.11/inffast.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)zlib/1.2.11/zlib-1.2.11/inflate.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)zlib/1.2.11/zlib-1.2.11/inftrees.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)zlib/1.2.11/zlib-1.2.11/trees.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)zlib/1.2.11/zlib-1.2.11/uncompr.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)zlib/1.2.11/zlib-1.2.11/zutil.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_adm/src/adm_identity/AdmIdSequenceMap.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)dlb_adm/src/adm_identity/AdmIdTranslator.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_adm/src/adm_transformer/XMLGenerator.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)dlb_adm/src/adm_transformer/XMLIngester.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_adm/src/adm_xml/AttributeDescriptor.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)dlb_adm/src/adm_xml/AttributeValue.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_adm/src/adm_xml/ContentContainer.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)dlb_adm/src/adm_xml/EntityContainer.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_adm/src/adm_xml/EntityDB.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)dlb_adm/src/adm_xml/EntityDescriptor.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_adm/src/adm_xml/EntityRecord.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)dlb_adm/src/adm_xml/RelationshipDB.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_adm/src/adm_xml/RelationshipDescriptor.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)dlb_adm/src/adm_xml/RelationshipRecord.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_adm/src/adm_xml/XMLBuffer.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)dlb_adm/src/adm_xml/XMLContainer.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_adm/src/adm_xml/XMLReader.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)dlb_adm/src/adm_xml/XMLReaderStack.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_adm/src/adm_xml/XMLWriter.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)dlb_adm/src/adm_xml/dlb_adm_xml_container.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_adm/src/core_model/AudioElement.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)dlb_adm/src/core_model/AudioTrack.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_adm/src/core_model/BlockUpdate.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)dlb_adm/src/core_model/ContentGroup.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_adm/src/core_model/CoreModel.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)dlb_adm/src/core_model/ElementGroup.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_adm/src/core_model/ElementRecord.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)dlb_adm/src/core_model/ElementTable.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_adm/src/core_model/EntityName.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)dlb_adm/src/core_model/FrameFormat.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_adm/src/core_model/Gain.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)dlb_adm/src/core_model/ModelEntity.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_adm/src/core_model/ModelEntityContainer.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)dlb_adm/src/core_model/Position.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_adm/src/core_model/Presentation.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)dlb_adm/src/core_model/PresentationRecord.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_adm/src/core_model/PresentationTable.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)dlb_adm/src/core_model/Source.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_adm/src/core_model/SourceGroup.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)dlb_adm/src/core_model/SourceRecord.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_adm/src/core_model/Target.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)dlb_adm/src/core_model/TargetGroup.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_adm/src/core_model/UpdateRecord.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)dlb_adm/src/core_model/UpdateTable.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_adm/src/core_model/dlb_adm_core_model.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o $(TMP)dlb_adm/src/dlb_adm_api_pp.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o\
                       $(TMP)dlb_pmd/src/model/DlbPmdModelWrapper.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o
INPUTS_ARCHIVE_release = $(TMP)dlb_adm/src/dlb_adm_api.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)dlb_pmd/os/osx/pmd_ctrl_c.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_pmd/src/api/dlb_pmd.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/api/dlb_pmd_api_read.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_pmd/src/api/dlb_pmd_api_write.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/api/dlb_pmd_equal.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_pmd/src/api/dlb_pmd_metadata_set.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/model/pmd_language.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_pmd/src/model/pmd_model.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/model/pmd_model_combo.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_pmd/src/modules/capture/pmd_capture.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/modules/generate/pmd_random_generator.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_pmd/src/modules/klv/pmd_crc32.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/modules/klv/pmd_klv_reader.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_pmd/src/modules/klv/pmd_klv_writer.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/modules/pcm/pmd_bitstream.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_augmentor.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_extractor.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_pmd/src/modules/pcm/pmd_smpte_337m.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_decoder.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_encoder.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_pmd/src/modules/sadm/pmd_core_model_generator.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_pmd/src/modules/sadm/pmd_core_model_ingester.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_file.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_file.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_file.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_string.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_pmd/src/modules/xml/pmd_xml_reader.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_file.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_string.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_pmd/src/modules/xml/pmd_xml_writer.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)dlb_xmllib/1.0.4/dlb_xml/src/dlb_xml.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)zlib/1.2.11/zlib-1.2.11/adler32.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)zlib/1.2.11/zlib-1.2.11/compress.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)zlib/1.2.11/zlib-1.2.11/crc32.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)zlib/1.2.11/zlib-1.2.11/deflate.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)zlib/1.2.11/zlib-1.2.11/gzclose.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)zlib/1.2.11/zlib-1.2.11/gzlib.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)zlib/1.2.11/zlib-1.2.11/gzread.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)zlib/1.2.11/zlib-1.2.11/gzwrite.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)zlib/1.2.11/zlib-1.2.11/infback.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)zlib/1.2.11/zlib-1.2.11/inffast.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)zlib/1.2.11/zlib-1.2.11/inflate.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)zlib/1.2.11/zlib-1.2.11/inftrees.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)zlib/1.2.11/zlib-1.2.11/trees.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)zlib/1.2.11/zlib-1.2.11/uncompr.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)zlib/1.2.11/zlib-1.2.11/zutil.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)dlb_adm/src/adm_identity/AdmIdSequenceMap.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_adm/src/adm_identity/AdmIdTranslator.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)dlb_adm/src/adm_transformer/XMLGenerator.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_adm/src/adm_transformer/XMLIngester.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)dlb_adm/src/adm_xml/AttributeDescriptor.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_adm/src/adm_xml/AttributeValue.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)dlb_adm/src/adm_xml/ContentContainer.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_adm/src/adm_xml/EntityContainer.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)dlb_adm/src/adm_xml/EntityDB.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_adm/src/adm_xml/EntityDescriptor.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)dlb_adm/src/adm_xml/EntityRecord.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_adm/src/adm_xml/RelationshipDB.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)dlb_adm/src/adm_xml/RelationshipDescriptor.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_adm/src/adm_xml/RelationshipRecord.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)dlb_adm/src/adm_xml/XMLBuffer.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_adm/src/adm_xml/XMLContainer.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)dlb_adm/src/adm_xml/XMLReader.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_adm/src/adm_xml/XMLReaderStack.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)dlb_adm/src/adm_xml/XMLWriter.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_adm/src/adm_xml/dlb_adm_xml_container.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)dlb_adm/src/core_model/AudioElement.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_adm/src/core_model/AudioTrack.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)dlb_adm/src/core_model/BlockUpdate.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_adm/src/core_model/ContentGroup.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)dlb_adm/src/core_model/CoreModel.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_adm/src/core_model/ElementGroup.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)dlb_adm/src/core_model/ElementRecord.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_adm/src/core_model/ElementTable.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)dlb_adm/src/core_model/EntityName.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_adm/src/core_model/FrameFormat.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)dlb_adm/src/core_model/Gain.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_adm/src/core_model/ModelEntity.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)dlb_adm/src/core_model/ModelEntityContainer.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_adm/src/core_model/Position.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)dlb_adm/src/core_model/Presentation.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_adm/src/core_model/PresentationRecord.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)dlb_adm/src/core_model/PresentationTable.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_adm/src/core_model/Source.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)dlb_adm/src/core_model/SourceGroup.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_adm/src/core_model/SourceRecord.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)dlb_adm/src/core_model/Target.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_adm/src/core_model/TargetGroup.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)dlb_adm/src/core_model/UpdateRecord.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_adm/src/core_model/UpdateTable.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)dlb_adm/src/core_model/dlb_adm_core_model.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o\
                         $(TMP)dlb_adm/src/dlb_adm_api_pp.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o $(TMP)dlb_pmd/src/model/DlbPmdModelWrapper.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o


default: dlb_pmd_lib_dynamic_debug.dylib dlb_pmd_lib_dynamic_release.dylib

dlb_pmd_lib_dynamic_debug.dylib: $(INPUTS_ARCHIVE_debug)
	$(AR) $(ARFLAGS)   $@ $^

dlb_pmd_lib_dynamic_release.dylib: $(INPUTS_ARCHIVE_release)
	$(AR) $(ARFLAGS)   $@ $^

$(TMP)%.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -arch x86_64 -MM $(DEFINES_debug) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_debug) $(DEFINES_debug) $(INCLUDES) -o $@ $<

$(TMP)%.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.o: $(BASE)%.cpp | $(TMP)%/..
	$(CXXDEP) -std=c++11 -arch x86_64 -MM $(DEFINES_debug) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS_debug) $(DEFINES_debug) $(INCLUDES) -o $@ $<

$(TMP)%.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -arch x86_64 -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

$(TMP)%.dlb_pmd_lib_dynamic_osx_amd64_clang_release.o: $(BASE)%.cpp | $(TMP)%/..
	$(CXXDEP) -std=c++11 -arch x86_64 -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMPILE_debug = $(TMP)dlb_adm/src/dlb_adm_api.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)dlb_pmd/os/osx/pmd_ctrl_c.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/api/dlb_pmd.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/api/dlb_pmd_api_read.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/api/dlb_pmd_api_write.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/api/dlb_pmd_equal.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/api/dlb_pmd_metadata_set.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/model/pmd_language.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/model/pmd_model.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/model/pmd_model_combo.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/modules/capture/pmd_capture.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/modules/generate/pmd_random_generator.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/modules/klv/pmd_crc32.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/modules/klv/pmd_klv_reader.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/modules/klv/pmd_klv_writer.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/modules/pcm/pmd_bitstream.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_augmentor.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_extractor.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/modules/pcm/pmd_smpte_337m.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_decoder.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_encoder.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/modules/sadm/pmd_core_model_generator.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/modules/sadm/pmd_core_model_ingester.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_file.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_file.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_file.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_string.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/modules/xml/pmd_xml_reader.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_file.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_string.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)dlb_pmd/src/modules/xml/pmd_xml_writer.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)dlb_xmllib/1.0.4/dlb_xml/src/dlb_xml.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)zlib/1.2.11/zlib-1.2.11/adler32.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)zlib/1.2.11/zlib-1.2.11/compress.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)zlib/1.2.11/zlib-1.2.11/crc32.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)zlib/1.2.11/zlib-1.2.11/deflate.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)zlib/1.2.11/zlib-1.2.11/gzclose.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)zlib/1.2.11/zlib-1.2.11/gzlib.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)zlib/1.2.11/zlib-1.2.11/gzread.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)zlib/1.2.11/zlib-1.2.11/gzwrite.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)zlib/1.2.11/zlib-1.2.11/infback.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)zlib/1.2.11/zlib-1.2.11/inffast.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)zlib/1.2.11/zlib-1.2.11/inflate.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)zlib/1.2.11/zlib-1.2.11/inftrees.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)zlib/1.2.11/zlib-1.2.11/trees.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)zlib/1.2.11/zlib-1.2.11/uncompr.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)zlib/1.2.11/zlib-1.2.11/zutil.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)dlb_adm/src/adm_identity/AdmIdSequenceMap.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)dlb_adm/src/adm_identity/AdmIdTranslator.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)dlb_adm/src/adm_transformer/XMLGenerator.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)dlb_adm/src/adm_transformer/XMLIngester.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)dlb_adm/src/adm_xml/AttributeDescriptor.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)dlb_adm/src/adm_xml/AttributeValue.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)dlb_adm/src/adm_xml/ContentContainer.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)dlb_adm/src/adm_xml/EntityContainer.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)dlb_adm/src/adm_xml/EntityDB.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)dlb_adm/src/adm_xml/EntityDescriptor.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)dlb_adm/src/adm_xml/EntityRecord.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)dlb_adm/src/adm_xml/RelationshipDB.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)dlb_adm/src/adm_xml/RelationshipDescriptor.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)dlb_adm/src/adm_xml/RelationshipRecord.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)dlb_adm/src/adm_xml/XMLBuffer.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)dlb_adm/src/adm_xml/XMLContainer.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)dlb_adm/src/adm_xml/XMLReader.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)dlb_adm/src/adm_xml/XMLReaderStack.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)dlb_adm/src/adm_xml/XMLWriter.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)dlb_adm/src/adm_xml/dlb_adm_xml_container.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)dlb_adm/src/core_model/AudioElement.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)dlb_adm/src/core_model/AudioTrack.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)dlb_adm/src/core_model/BlockUpdate.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)dlb_adm/src/core_model/ContentGroup.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)dlb_adm/src/core_model/CoreModel.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)dlb_adm/src/core_model/ElementGroup.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)dlb_adm/src/core_model/ElementRecord.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)dlb_adm/src/core_model/ElementTable.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)dlb_adm/src/core_model/EntityName.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)dlb_adm/src/core_model/FrameFormat.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)dlb_adm/src/core_model/Gain.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)dlb_adm/src/core_model/ModelEntity.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)dlb_adm/src/core_model/ModelEntityContainer.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)dlb_adm/src/core_model/Position.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)dlb_adm/src/core_model/Presentation.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)dlb_adm/src/core_model/PresentationRecord.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)dlb_adm/src/core_model/PresentationTable.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)dlb_adm/src/core_model/Source.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)dlb_adm/src/core_model/SourceGroup.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)dlb_adm/src/core_model/SourceRecord.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)dlb_adm/src/core_model/Target.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)dlb_adm/src/core_model/TargetGroup.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)dlb_adm/src/core_model/UpdateRecord.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)dlb_adm/src/core_model/UpdateTable.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)dlb_adm/src/core_model/dlb_adm_core_model.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d $(TMP)dlb_adm/src/dlb_adm_api_pp.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d\
                      $(TMP)dlb_pmd/src/model/DlbPmdModelWrapper.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d
CLEAN_COMPILE_release = $(TMP)dlb_adm/src/dlb_adm_api.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)dlb_pmd/os/osx/pmd_ctrl_c.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/api/dlb_pmd.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/api/dlb_pmd_api_read.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/api/dlb_pmd_api_write.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/api/dlb_pmd_equal.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/api/dlb_pmd_metadata_set.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/model/pmd_language.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/model/pmd_model.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/model/pmd_model_combo.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/capture/pmd_capture.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/modules/generate/pmd_random_generator.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/klv/pmd_crc32.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/modules/klv/pmd_klv_reader.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/klv/pmd_klv_writer.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/modules/pcm/pmd_bitstream.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_augmentor.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_extractor.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/pcm/pmd_smpte_337m.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_decoder.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_encoder.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/sadm/pmd_core_model_generator.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/sadm/pmd_core_model_ingester.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_file.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_file.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_file.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_string.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/xml/pmd_xml_reader.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_file.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_string.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_pmd/src/modules/xml/pmd_xml_writer.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)dlb_xmllib/1.0.4/dlb_xml/src/dlb_xml.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)zlib/1.2.11/zlib-1.2.11/adler32.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)zlib/1.2.11/zlib-1.2.11/compress.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)zlib/1.2.11/zlib-1.2.11/crc32.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)zlib/1.2.11/zlib-1.2.11/deflate.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)zlib/1.2.11/zlib-1.2.11/gzclose.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)zlib/1.2.11/zlib-1.2.11/gzlib.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)zlib/1.2.11/zlib-1.2.11/gzread.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)zlib/1.2.11/zlib-1.2.11/gzwrite.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)zlib/1.2.11/zlib-1.2.11/infback.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)zlib/1.2.11/zlib-1.2.11/inffast.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)zlib/1.2.11/zlib-1.2.11/inflate.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)zlib/1.2.11/zlib-1.2.11/inftrees.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)zlib/1.2.11/zlib-1.2.11/trees.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)zlib/1.2.11/zlib-1.2.11/uncompr.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)zlib/1.2.11/zlib-1.2.11/zutil.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)dlb_adm/src/adm_identity/AdmIdSequenceMap.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_adm/src/adm_identity/AdmIdTranslator.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)dlb_adm/src/adm_transformer/XMLGenerator.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_adm/src/adm_transformer/XMLIngester.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)dlb_adm/src/adm_xml/AttributeDescriptor.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_adm/src/adm_xml/AttributeValue.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)dlb_adm/src/adm_xml/ContentContainer.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_adm/src/adm_xml/EntityContainer.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)dlb_adm/src/adm_xml/EntityDB.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_adm/src/adm_xml/EntityDescriptor.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)dlb_adm/src/adm_xml/EntityRecord.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_adm/src/adm_xml/RelationshipDB.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)dlb_adm/src/adm_xml/RelationshipDescriptor.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_adm/src/adm_xml/RelationshipRecord.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)dlb_adm/src/adm_xml/XMLBuffer.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_adm/src/adm_xml/XMLContainer.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)dlb_adm/src/adm_xml/XMLReader.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_adm/src/adm_xml/XMLReaderStack.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)dlb_adm/src/adm_xml/XMLWriter.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_adm/src/adm_xml/dlb_adm_xml_container.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)dlb_adm/src/core_model/AudioElement.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_adm/src/core_model/AudioTrack.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)dlb_adm/src/core_model/BlockUpdate.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_adm/src/core_model/ContentGroup.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)dlb_adm/src/core_model/CoreModel.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_adm/src/core_model/ElementGroup.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)dlb_adm/src/core_model/ElementRecord.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_adm/src/core_model/ElementTable.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)dlb_adm/src/core_model/EntityName.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_adm/src/core_model/FrameFormat.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)dlb_adm/src/core_model/Gain.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_adm/src/core_model/ModelEntity.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)dlb_adm/src/core_model/ModelEntityContainer.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_adm/src/core_model/Position.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)dlb_adm/src/core_model/Presentation.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_adm/src/core_model/PresentationRecord.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)dlb_adm/src/core_model/PresentationTable.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_adm/src/core_model/Source.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)dlb_adm/src/core_model/SourceGroup.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_adm/src/core_model/SourceRecord.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)dlb_adm/src/core_model/Target.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_adm/src/core_model/TargetGroup.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)dlb_adm/src/core_model/UpdateRecord.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_adm/src/core_model/UpdateTable.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)dlb_adm/src/core_model/dlb_adm_core_model.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d\
                        $(TMP)dlb_adm/src/dlb_adm_api_pp.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d $(TMP)dlb_pmd/src/model/DlbPmdModelWrapper.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d

cleandeps:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)

clean: cleanself

cleanself:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)
	$(RM) $(INPUTS_ARCHIVE_debug)
	$(RM) $(INPUTS_ARCHIVE_release)
	$(RM) dlb_pmd_lib_dynamic_debug.dylib
	$(RM) dlb_pmd_lib_dynamic_release.dylib

.PHONY: default FORCE clean cleandeps cleanself help

FORCE:

help:
	@echo "This makefile has the following targets:"
	@echo " dlb_pmd_lib_dynamic_debug.dylib"
	@echo " dlb_pmd_lib_dynamic_release.dylib"
	@echo " default"
	@echo " clean"
	@echo " cleandeps"
	@echo " help"

include $(wildcard $(TMP)dlb_adm/src/dlb_adm_api.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/os/osx/pmd_ctrl_c.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_api_read.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_api_write.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_equal.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_metadata_set.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/model/pmd_language.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/model/pmd_model.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/model/pmd_model_combo.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/capture/pmd_capture.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/generate/pmd_random_generator.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/klv/pmd_crc32.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/klv/pmd_klv_reader.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/klv/pmd_klv_writer.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_bitstream.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_augmentor.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_extractor.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_smpte_337m.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_decoder.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_encoder.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_core_model_generator.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_core_model_ingester.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_file.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_file.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_file.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_string.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_reader.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_file.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_string.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_writer.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_xmllib/1.0.4/dlb_xml/src/dlb_xml.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/adler32.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/compress.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/crc32.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/deflate.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/gzclose.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/gzlib.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/gzread.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/gzwrite.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/infback.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/inffast.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/inflate.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/inftrees.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/trees.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/uncompr.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/zutil.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/adm_identity/AdmIdSequenceMap.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/adm_identity/AdmIdTranslator.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/adm_transformer/XMLGenerator.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/adm_transformer/XMLIngester.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/AttributeDescriptor.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/AttributeValue.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/ContentContainer.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/EntityContainer.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/EntityDB.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/EntityDescriptor.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/EntityRecord.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/RelationshipDB.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/RelationshipDescriptor.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/RelationshipRecord.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/XMLBuffer.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/XMLContainer.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/XMLReader.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/XMLReaderStack.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/XMLWriter.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/dlb_adm_xml_container.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/AudioElement.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/AudioTrack.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/BlockUpdate.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/ContentGroup.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/CoreModel.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/ElementGroup.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/ElementRecord.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/ElementTable.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/EntityName.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/FrameFormat.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/Gain.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/ModelEntity.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/ModelEntityContainer.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/Position.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/Presentation.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/PresentationRecord.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/PresentationTable.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/Source.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/SourceGroup.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/SourceRecord.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/Target.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/TargetGroup.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/UpdateRecord.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/UpdateTable.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/dlb_adm_core_model.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/dlb_adm_api_pp.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_pmd/src/model/DlbPmdModelWrapper.dlb_pmd_lib_dynamic_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)dlb_adm/src/dlb_adm_api.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/os/osx/pmd_ctrl_c.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_api_read.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_api_write.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_equal.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/api/dlb_pmd_metadata_set.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/model/pmd_language.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/model/pmd_model.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/model/pmd_model_combo.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/capture/pmd_capture.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/generate/pmd_random_generator.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/klv/pmd_crc32.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/klv/pmd_klv_reader.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/klv/pmd_klv_writer.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_bitstream.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_augmentor.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_pcm_extractor.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/pmd_smpte_337m.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_decoder.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/pcm/sadm_bitstream_encoder.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_core_model_generator.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_core_model_ingester.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_read_file.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/sadm/pmd_sadm_write_file.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_file.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_read_string.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_reader.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_file.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_write_string.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/modules/xml/pmd_xml_writer.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_xmllib/1.0.4/dlb_xml/src/dlb_xml.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/adler32.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/compress.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/crc32.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/deflate.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/gzclose.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/gzlib.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/gzread.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/gzwrite.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/infback.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/inffast.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/inflate.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/inftrees.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/trees.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/uncompr.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)zlib/1.2.11/zlib-1.2.11/zutil.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/adm_identity/AdmIdSequenceMap.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/adm_identity/AdmIdTranslator.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/adm_transformer/XMLGenerator.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/adm_transformer/XMLIngester.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/AttributeDescriptor.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/AttributeValue.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/ContentContainer.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/EntityContainer.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/EntityDB.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/EntityDescriptor.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/EntityRecord.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/RelationshipDB.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/RelationshipDescriptor.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/RelationshipRecord.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/XMLBuffer.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/XMLContainer.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/XMLReader.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/XMLReaderStack.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/XMLWriter.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/dlb_adm_xml_container.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/AudioElement.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/AudioTrack.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/BlockUpdate.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/ContentGroup.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/CoreModel.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/ElementGroup.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/ElementRecord.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/ElementTable.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/EntityName.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/FrameFormat.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/Gain.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/ModelEntity.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/ModelEntityContainer.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/Position.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/Presentation.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/PresentationRecord.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/PresentationTable.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/Source.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/SourceGroup.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/SourceRecord.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/Target.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/TargetGroup.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/UpdateRecord.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/UpdateTable.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/dlb_adm_core_model.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_adm/src/dlb_adm_api_pp.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)
include $(wildcard $(TMP)dlb_pmd/src/model/DlbPmdModelWrapper.dlb_pmd_lib_dynamic_osx_amd64_clang_release.d)


