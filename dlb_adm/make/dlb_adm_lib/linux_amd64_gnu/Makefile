#-*-makefile-*-

BASE=../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
ARFLAGS = -rc
CCDEP = $(CC)
CFLAGS_debug = -g -ggdb3 -O0 -m64 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
CFLAGS_release = -O3 -ftree-vectorize -m64 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
CXXDEP = $(CXX)
CXXFLAGS_debug = -std=c++11 -std=c++17 -g -ggdb3 -O0 -m64 -c -Wall
CXXFLAGS_release = -std=c++11 -std=c++17 -O3 -ftree-vectorize -m64 -c -Wall
DEFINES_debug = -DELPP_NO_DEFAULT_LOG_FILE=1
DEFINES_release = -DELPP_NO_DEFAULT_LOG_FILE=1 -DNDEBUG=1
INCLUDES = -I$(BASE)boost/boost_1_73_0 -I$(BASE)dlb_adm/include -I$(BASE). -I$(BASE)dlb_xmllib/1.0.4


INPUTS_ARCHIVE_debug = $(TMP)dlb_adm/src/dlb_adm_api.dlb_adm_lib_linux_amd64_gnu_debug.o $(TMP)dlb_adm/src/adm_identity/AdmIdSequenceMap.dlb_adm_lib_linux_amd64_gnu_debug.o\
                       $(TMP)dlb_adm/src/adm_identity/AdmIdTranslator.dlb_adm_lib_linux_amd64_gnu_debug.o $(TMP)dlb_adm/src/adm_transformer/XMLGenerator.dlb_adm_lib_linux_amd64_gnu_debug.o\
                       $(TMP)dlb_adm/src/adm_transformer/XMLIngester.dlb_adm_lib_linux_amd64_gnu_debug.o $(TMP)dlb_adm/src/adm_xml/AttributeDescriptor.dlb_adm_lib_linux_amd64_gnu_debug.o\
                       $(TMP)dlb_adm/src/adm_xml/AttributeValue.dlb_adm_lib_linux_amd64_gnu_debug.o $(TMP)dlb_adm/src/adm_xml/ContentContainer.dlb_adm_lib_linux_amd64_gnu_debug.o\
                       $(TMP)dlb_adm/src/adm_xml/EntityContainer.dlb_adm_lib_linux_amd64_gnu_debug.o $(TMP)dlb_adm/src/adm_xml/EntityDB.dlb_adm_lib_linux_amd64_gnu_debug.o\
                       $(TMP)dlb_adm/src/adm_xml/EntityDescriptor.dlb_adm_lib_linux_amd64_gnu_debug.o $(TMP)dlb_adm/src/adm_xml/EntityRecord.dlb_adm_lib_linux_amd64_gnu_debug.o\
                       $(TMP)dlb_adm/src/adm_xml/RelationshipDB.dlb_adm_lib_linux_amd64_gnu_debug.o $(TMP)dlb_adm/src/adm_xml/RelationshipDescriptor.dlb_adm_lib_linux_amd64_gnu_debug.o\
                       $(TMP)dlb_adm/src/adm_xml/RelationshipRecord.dlb_adm_lib_linux_amd64_gnu_debug.o $(TMP)dlb_adm/src/adm_xml/XMLBuffer.dlb_adm_lib_linux_amd64_gnu_debug.o\
                       $(TMP)dlb_adm/src/adm_xml/XMLContainer.dlb_adm_lib_linux_amd64_gnu_debug.o $(TMP)dlb_adm/src/adm_xml/XMLReader.dlb_adm_lib_linux_amd64_gnu_debug.o\
                       $(TMP)dlb_adm/src/adm_xml/XMLReaderStack.dlb_adm_lib_linux_amd64_gnu_debug.o $(TMP)dlb_adm/src/adm_xml/XMLWriter.dlb_adm_lib_linux_amd64_gnu_debug.o\
                       $(TMP)dlb_adm/src/adm_xml/dlb_adm_xml_container.dlb_adm_lib_linux_amd64_gnu_debug.o $(TMP)dlb_adm/src/core_model/AudioElement.dlb_adm_lib_linux_amd64_gnu_debug.o\
                       $(TMP)dlb_adm/src/core_model/AudioTrack.dlb_adm_lib_linux_amd64_gnu_debug.o $(TMP)dlb_adm/src/core_model/BlockUpdate.dlb_adm_lib_linux_amd64_gnu_debug.o\
                       $(TMP)dlb_adm/src/core_model/ContentGroup.dlb_adm_lib_linux_amd64_gnu_debug.o $(TMP)dlb_adm/src/core_model/CoreModel.dlb_adm_lib_linux_amd64_gnu_debug.o\
                       $(TMP)dlb_adm/src/core_model/ElementGroup.dlb_adm_lib_linux_amd64_gnu_debug.o $(TMP)dlb_adm/src/core_model/ElementRecord.dlb_adm_lib_linux_amd64_gnu_debug.o\
                       $(TMP)dlb_adm/src/core_model/ElementTable.dlb_adm_lib_linux_amd64_gnu_debug.o $(TMP)dlb_adm/src/core_model/EntityName.dlb_adm_lib_linux_amd64_gnu_debug.o\
                       $(TMP)dlb_adm/src/core_model/FrameFormat.dlb_adm_lib_linux_amd64_gnu_debug.o $(TMP)dlb_adm/src/core_model/Gain.dlb_adm_lib_linux_amd64_gnu_debug.o\
                       $(TMP)dlb_adm/src/core_model/ModelEntity.dlb_adm_lib_linux_amd64_gnu_debug.o $(TMP)dlb_adm/src/core_model/ModelEntityContainer.dlb_adm_lib_linux_amd64_gnu_debug.o\
                       $(TMP)dlb_adm/src/core_model/Position.dlb_adm_lib_linux_amd64_gnu_debug.o $(TMP)dlb_adm/src/core_model/Presentation.dlb_adm_lib_linux_amd64_gnu_debug.o\
                       $(TMP)dlb_adm/src/core_model/PresentationRecord.dlb_adm_lib_linux_amd64_gnu_debug.o $(TMP)dlb_adm/src/core_model/PresentationTable.dlb_adm_lib_linux_amd64_gnu_debug.o\
                       $(TMP)dlb_adm/src/core_model/Source.dlb_adm_lib_linux_amd64_gnu_debug.o $(TMP)dlb_adm/src/core_model/SourceGroup.dlb_adm_lib_linux_amd64_gnu_debug.o\
                       $(TMP)dlb_adm/src/core_model/SourceRecord.dlb_adm_lib_linux_amd64_gnu_debug.o $(TMP)dlb_adm/src/core_model/Target.dlb_adm_lib_linux_amd64_gnu_debug.o\
                       $(TMP)dlb_adm/src/core_model/TargetGroup.dlb_adm_lib_linux_amd64_gnu_debug.o $(TMP)dlb_adm/src/core_model/UpdateRecord.dlb_adm_lib_linux_amd64_gnu_debug.o\
                       $(TMP)dlb_adm/src/core_model/UpdateTable.dlb_adm_lib_linux_amd64_gnu_debug.o $(TMP)dlb_adm/src/core_model/dlb_adm_core_model.dlb_adm_lib_linux_amd64_gnu_debug.o\
                       $(TMP)dlb_adm/src/dlb_adm_api_pp.dlb_adm_lib_linux_amd64_gnu_debug.o
INPUTS_ARCHIVE_release = $(TMP)dlb_adm/src/dlb_adm_api.dlb_adm_lib_linux_amd64_gnu_release.o $(TMP)dlb_adm/src/adm_identity/AdmIdSequenceMap.dlb_adm_lib_linux_amd64_gnu_release.o\
                         $(TMP)dlb_adm/src/adm_identity/AdmIdTranslator.dlb_adm_lib_linux_amd64_gnu_release.o $(TMP)dlb_adm/src/adm_transformer/XMLGenerator.dlb_adm_lib_linux_amd64_gnu_release.o\
                         $(TMP)dlb_adm/src/adm_transformer/XMLIngester.dlb_adm_lib_linux_amd64_gnu_release.o $(TMP)dlb_adm/src/adm_xml/AttributeDescriptor.dlb_adm_lib_linux_amd64_gnu_release.o\
                         $(TMP)dlb_adm/src/adm_xml/AttributeValue.dlb_adm_lib_linux_amd64_gnu_release.o $(TMP)dlb_adm/src/adm_xml/ContentContainer.dlb_adm_lib_linux_amd64_gnu_release.o\
                         $(TMP)dlb_adm/src/adm_xml/EntityContainer.dlb_adm_lib_linux_amd64_gnu_release.o $(TMP)dlb_adm/src/adm_xml/EntityDB.dlb_adm_lib_linux_amd64_gnu_release.o\
                         $(TMP)dlb_adm/src/adm_xml/EntityDescriptor.dlb_adm_lib_linux_amd64_gnu_release.o $(TMP)dlb_adm/src/adm_xml/EntityRecord.dlb_adm_lib_linux_amd64_gnu_release.o\
                         $(TMP)dlb_adm/src/adm_xml/RelationshipDB.dlb_adm_lib_linux_amd64_gnu_release.o $(TMP)dlb_adm/src/adm_xml/RelationshipDescriptor.dlb_adm_lib_linux_amd64_gnu_release.o\
                         $(TMP)dlb_adm/src/adm_xml/RelationshipRecord.dlb_adm_lib_linux_amd64_gnu_release.o $(TMP)dlb_adm/src/adm_xml/XMLBuffer.dlb_adm_lib_linux_amd64_gnu_release.o\
                         $(TMP)dlb_adm/src/adm_xml/XMLContainer.dlb_adm_lib_linux_amd64_gnu_release.o $(TMP)dlb_adm/src/adm_xml/XMLReader.dlb_adm_lib_linux_amd64_gnu_release.o\
                         $(TMP)dlb_adm/src/adm_xml/XMLReaderStack.dlb_adm_lib_linux_amd64_gnu_release.o $(TMP)dlb_adm/src/adm_xml/XMLWriter.dlb_adm_lib_linux_amd64_gnu_release.o\
                         $(TMP)dlb_adm/src/adm_xml/dlb_adm_xml_container.dlb_adm_lib_linux_amd64_gnu_release.o $(TMP)dlb_adm/src/core_model/AudioElement.dlb_adm_lib_linux_amd64_gnu_release.o\
                         $(TMP)dlb_adm/src/core_model/AudioTrack.dlb_adm_lib_linux_amd64_gnu_release.o $(TMP)dlb_adm/src/core_model/BlockUpdate.dlb_adm_lib_linux_amd64_gnu_release.o\
                         $(TMP)dlb_adm/src/core_model/ContentGroup.dlb_adm_lib_linux_amd64_gnu_release.o $(TMP)dlb_adm/src/core_model/CoreModel.dlb_adm_lib_linux_amd64_gnu_release.o\
                         $(TMP)dlb_adm/src/core_model/ElementGroup.dlb_adm_lib_linux_amd64_gnu_release.o $(TMP)dlb_adm/src/core_model/ElementRecord.dlb_adm_lib_linux_amd64_gnu_release.o\
                         $(TMP)dlb_adm/src/core_model/ElementTable.dlb_adm_lib_linux_amd64_gnu_release.o $(TMP)dlb_adm/src/core_model/EntityName.dlb_adm_lib_linux_amd64_gnu_release.o\
                         $(TMP)dlb_adm/src/core_model/FrameFormat.dlb_adm_lib_linux_amd64_gnu_release.o $(TMP)dlb_adm/src/core_model/Gain.dlb_adm_lib_linux_amd64_gnu_release.o\
                         $(TMP)dlb_adm/src/core_model/ModelEntity.dlb_adm_lib_linux_amd64_gnu_release.o $(TMP)dlb_adm/src/core_model/ModelEntityContainer.dlb_adm_lib_linux_amd64_gnu_release.o\
                         $(TMP)dlb_adm/src/core_model/Position.dlb_adm_lib_linux_amd64_gnu_release.o $(TMP)dlb_adm/src/core_model/Presentation.dlb_adm_lib_linux_amd64_gnu_release.o\
                         $(TMP)dlb_adm/src/core_model/PresentationRecord.dlb_adm_lib_linux_amd64_gnu_release.o $(TMP)dlb_adm/src/core_model/PresentationTable.dlb_adm_lib_linux_amd64_gnu_release.o\
                         $(TMP)dlb_adm/src/core_model/Source.dlb_adm_lib_linux_amd64_gnu_release.o $(TMP)dlb_adm/src/core_model/SourceGroup.dlb_adm_lib_linux_amd64_gnu_release.o\
                         $(TMP)dlb_adm/src/core_model/SourceRecord.dlb_adm_lib_linux_amd64_gnu_release.o $(TMP)dlb_adm/src/core_model/Target.dlb_adm_lib_linux_amd64_gnu_release.o\
                         $(TMP)dlb_adm/src/core_model/TargetGroup.dlb_adm_lib_linux_amd64_gnu_release.o $(TMP)dlb_adm/src/core_model/UpdateRecord.dlb_adm_lib_linux_amd64_gnu_release.o\
                         $(TMP)dlb_adm/src/core_model/UpdateTable.dlb_adm_lib_linux_amd64_gnu_release.o $(TMP)dlb_adm/src/core_model/dlb_adm_core_model.dlb_adm_lib_linux_amd64_gnu_release.o\
                         $(TMP)dlb_adm/src/dlb_adm_api_pp.dlb_adm_lib_linux_amd64_gnu_release.o

DEP_PROJ_ARCHIVE_debug = $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_amd64_gnu/dlb_xml_debug.a
DEP_PROJ_ARCHIVE_release = $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_amd64_gnu/dlb_xml_release.a

default: dlb_adm_lib_debug.a dlb_adm_lib_release.a

dlb_adm_lib_debug.a: $(INPUTS_ARCHIVE_debug)
	$(AR) $(ARFLAGS)  $@ $^

dlb_adm_lib_release.a: $(INPUTS_ARCHIVE_release)
	$(AR) $(ARFLAGS)  $@ $^

$(TMP)%.dlb_adm_lib_linux_amd64_gnu_debug.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -MM $(DEFINES_debug) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_debug) $(DEFINES_debug) $(INCLUDES) -o $@ $<

$(TMP)%.dlb_adm_lib_linux_amd64_gnu_debug.o: $(BASE)%.cpp | $(TMP)%/..
	$(CXXDEP) -std=c++11 -MM $(DEFINES_debug) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS_debug) $(DEFINES_debug) $(INCLUDES) -o $@ $<

$(TMP)%.dlb_adm_lib_linux_amd64_gnu_release.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

$(TMP)%.dlb_adm_lib_linux_amd64_gnu_release.o: $(BASE)%.cpp | $(TMP)%/..
	$(CXXDEP) -std=c++11 -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMPILE_debug = $(TMP)dlb_adm/src/dlb_adm_api.dlb_adm_lib_linux_amd64_gnu_debug.d $(TMP)dlb_adm/src/adm_identity/AdmIdSequenceMap.dlb_adm_lib_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_adm/src/adm_identity/AdmIdTranslator.dlb_adm_lib_linux_amd64_gnu_debug.d $(TMP)dlb_adm/src/adm_transformer/XMLGenerator.dlb_adm_lib_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_adm/src/adm_transformer/XMLIngester.dlb_adm_lib_linux_amd64_gnu_debug.d $(TMP)dlb_adm/src/adm_xml/AttributeDescriptor.dlb_adm_lib_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_adm/src/adm_xml/AttributeValue.dlb_adm_lib_linux_amd64_gnu_debug.d $(TMP)dlb_adm/src/adm_xml/ContentContainer.dlb_adm_lib_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_adm/src/adm_xml/EntityContainer.dlb_adm_lib_linux_amd64_gnu_debug.d $(TMP)dlb_adm/src/adm_xml/EntityDB.dlb_adm_lib_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_adm/src/adm_xml/EntityDescriptor.dlb_adm_lib_linux_amd64_gnu_debug.d $(TMP)dlb_adm/src/adm_xml/EntityRecord.dlb_adm_lib_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_adm/src/adm_xml/RelationshipDB.dlb_adm_lib_linux_amd64_gnu_debug.d $(TMP)dlb_adm/src/adm_xml/RelationshipDescriptor.dlb_adm_lib_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_adm/src/adm_xml/RelationshipRecord.dlb_adm_lib_linux_amd64_gnu_debug.d $(TMP)dlb_adm/src/adm_xml/XMLBuffer.dlb_adm_lib_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_adm/src/adm_xml/XMLContainer.dlb_adm_lib_linux_amd64_gnu_debug.d $(TMP)dlb_adm/src/adm_xml/XMLReader.dlb_adm_lib_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_adm/src/adm_xml/XMLReaderStack.dlb_adm_lib_linux_amd64_gnu_debug.d $(TMP)dlb_adm/src/adm_xml/XMLWriter.dlb_adm_lib_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_adm/src/adm_xml/dlb_adm_xml_container.dlb_adm_lib_linux_amd64_gnu_debug.d $(TMP)dlb_adm/src/core_model/AudioElement.dlb_adm_lib_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_adm/src/core_model/AudioTrack.dlb_adm_lib_linux_amd64_gnu_debug.d $(TMP)dlb_adm/src/core_model/BlockUpdate.dlb_adm_lib_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_adm/src/core_model/ContentGroup.dlb_adm_lib_linux_amd64_gnu_debug.d $(TMP)dlb_adm/src/core_model/CoreModel.dlb_adm_lib_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_adm/src/core_model/ElementGroup.dlb_adm_lib_linux_amd64_gnu_debug.d $(TMP)dlb_adm/src/core_model/ElementRecord.dlb_adm_lib_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_adm/src/core_model/ElementTable.dlb_adm_lib_linux_amd64_gnu_debug.d $(TMP)dlb_adm/src/core_model/EntityName.dlb_adm_lib_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_adm/src/core_model/FrameFormat.dlb_adm_lib_linux_amd64_gnu_debug.d $(TMP)dlb_adm/src/core_model/Gain.dlb_adm_lib_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_adm/src/core_model/ModelEntity.dlb_adm_lib_linux_amd64_gnu_debug.d $(TMP)dlb_adm/src/core_model/ModelEntityContainer.dlb_adm_lib_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_adm/src/core_model/Position.dlb_adm_lib_linux_amd64_gnu_debug.d $(TMP)dlb_adm/src/core_model/Presentation.dlb_adm_lib_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_adm/src/core_model/PresentationRecord.dlb_adm_lib_linux_amd64_gnu_debug.d $(TMP)dlb_adm/src/core_model/PresentationTable.dlb_adm_lib_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_adm/src/core_model/Source.dlb_adm_lib_linux_amd64_gnu_debug.d $(TMP)dlb_adm/src/core_model/SourceGroup.dlb_adm_lib_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_adm/src/core_model/SourceRecord.dlb_adm_lib_linux_amd64_gnu_debug.d $(TMP)dlb_adm/src/core_model/Target.dlb_adm_lib_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_adm/src/core_model/TargetGroup.dlb_adm_lib_linux_amd64_gnu_debug.d $(TMP)dlb_adm/src/core_model/UpdateRecord.dlb_adm_lib_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_adm/src/core_model/UpdateTable.dlb_adm_lib_linux_amd64_gnu_debug.d $(TMP)dlb_adm/src/core_model/dlb_adm_core_model.dlb_adm_lib_linux_amd64_gnu_debug.d\
                      $(TMP)dlb_adm/src/dlb_adm_api_pp.dlb_adm_lib_linux_amd64_gnu_debug.d
CLEAN_COMPILE_release = $(TMP)dlb_adm/src/dlb_adm_api.dlb_adm_lib_linux_amd64_gnu_release.d $(TMP)dlb_adm/src/adm_identity/AdmIdSequenceMap.dlb_adm_lib_linux_amd64_gnu_release.d\
                        $(TMP)dlb_adm/src/adm_identity/AdmIdTranslator.dlb_adm_lib_linux_amd64_gnu_release.d $(TMP)dlb_adm/src/adm_transformer/XMLGenerator.dlb_adm_lib_linux_amd64_gnu_release.d\
                        $(TMP)dlb_adm/src/adm_transformer/XMLIngester.dlb_adm_lib_linux_amd64_gnu_release.d $(TMP)dlb_adm/src/adm_xml/AttributeDescriptor.dlb_adm_lib_linux_amd64_gnu_release.d\
                        $(TMP)dlb_adm/src/adm_xml/AttributeValue.dlb_adm_lib_linux_amd64_gnu_release.d $(TMP)dlb_adm/src/adm_xml/ContentContainer.dlb_adm_lib_linux_amd64_gnu_release.d\
                        $(TMP)dlb_adm/src/adm_xml/EntityContainer.dlb_adm_lib_linux_amd64_gnu_release.d $(TMP)dlb_adm/src/adm_xml/EntityDB.dlb_adm_lib_linux_amd64_gnu_release.d\
                        $(TMP)dlb_adm/src/adm_xml/EntityDescriptor.dlb_adm_lib_linux_amd64_gnu_release.d $(TMP)dlb_adm/src/adm_xml/EntityRecord.dlb_adm_lib_linux_amd64_gnu_release.d\
                        $(TMP)dlb_adm/src/adm_xml/RelationshipDB.dlb_adm_lib_linux_amd64_gnu_release.d $(TMP)dlb_adm/src/adm_xml/RelationshipDescriptor.dlb_adm_lib_linux_amd64_gnu_release.d\
                        $(TMP)dlb_adm/src/adm_xml/RelationshipRecord.dlb_adm_lib_linux_amd64_gnu_release.d $(TMP)dlb_adm/src/adm_xml/XMLBuffer.dlb_adm_lib_linux_amd64_gnu_release.d\
                        $(TMP)dlb_adm/src/adm_xml/XMLContainer.dlb_adm_lib_linux_amd64_gnu_release.d $(TMP)dlb_adm/src/adm_xml/XMLReader.dlb_adm_lib_linux_amd64_gnu_release.d\
                        $(TMP)dlb_adm/src/adm_xml/XMLReaderStack.dlb_adm_lib_linux_amd64_gnu_release.d $(TMP)dlb_adm/src/adm_xml/XMLWriter.dlb_adm_lib_linux_amd64_gnu_release.d\
                        $(TMP)dlb_adm/src/adm_xml/dlb_adm_xml_container.dlb_adm_lib_linux_amd64_gnu_release.d $(TMP)dlb_adm/src/core_model/AudioElement.dlb_adm_lib_linux_amd64_gnu_release.d\
                        $(TMP)dlb_adm/src/core_model/AudioTrack.dlb_adm_lib_linux_amd64_gnu_release.d $(TMP)dlb_adm/src/core_model/BlockUpdate.dlb_adm_lib_linux_amd64_gnu_release.d\
                        $(TMP)dlb_adm/src/core_model/ContentGroup.dlb_adm_lib_linux_amd64_gnu_release.d $(TMP)dlb_adm/src/core_model/CoreModel.dlb_adm_lib_linux_amd64_gnu_release.d\
                        $(TMP)dlb_adm/src/core_model/ElementGroup.dlb_adm_lib_linux_amd64_gnu_release.d $(TMP)dlb_adm/src/core_model/ElementRecord.dlb_adm_lib_linux_amd64_gnu_release.d\
                        $(TMP)dlb_adm/src/core_model/ElementTable.dlb_adm_lib_linux_amd64_gnu_release.d $(TMP)dlb_adm/src/core_model/EntityName.dlb_adm_lib_linux_amd64_gnu_release.d\
                        $(TMP)dlb_adm/src/core_model/FrameFormat.dlb_adm_lib_linux_amd64_gnu_release.d $(TMP)dlb_adm/src/core_model/Gain.dlb_adm_lib_linux_amd64_gnu_release.d\
                        $(TMP)dlb_adm/src/core_model/ModelEntity.dlb_adm_lib_linux_amd64_gnu_release.d $(TMP)dlb_adm/src/core_model/ModelEntityContainer.dlb_adm_lib_linux_amd64_gnu_release.d\
                        $(TMP)dlb_adm/src/core_model/Position.dlb_adm_lib_linux_amd64_gnu_release.d $(TMP)dlb_adm/src/core_model/Presentation.dlb_adm_lib_linux_amd64_gnu_release.d\
                        $(TMP)dlb_adm/src/core_model/PresentationRecord.dlb_adm_lib_linux_amd64_gnu_release.d $(TMP)dlb_adm/src/core_model/PresentationTable.dlb_adm_lib_linux_amd64_gnu_release.d\
                        $(TMP)dlb_adm/src/core_model/Source.dlb_adm_lib_linux_amd64_gnu_release.d $(TMP)dlb_adm/src/core_model/SourceGroup.dlb_adm_lib_linux_amd64_gnu_release.d\
                        $(TMP)dlb_adm/src/core_model/SourceRecord.dlb_adm_lib_linux_amd64_gnu_release.d $(TMP)dlb_adm/src/core_model/Target.dlb_adm_lib_linux_amd64_gnu_release.d\
                        $(TMP)dlb_adm/src/core_model/TargetGroup.dlb_adm_lib_linux_amd64_gnu_release.d $(TMP)dlb_adm/src/core_model/UpdateRecord.dlb_adm_lib_linux_amd64_gnu_release.d\
                        $(TMP)dlb_adm/src/core_model/UpdateTable.dlb_adm_lib_linux_amd64_gnu_release.d $(TMP)dlb_adm/src/core_model/dlb_adm_core_model.dlb_adm_lib_linux_amd64_gnu_release.d\
                        $(TMP)dlb_adm/src/dlb_adm_api_pp.dlb_adm_lib_linux_amd64_gnu_release.d

cleandeps:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)

clean: cleanself
	$(MAKE) -C $(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_amd64_gnu cleanself

cleanself:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)
	$(RM) $(INPUTS_ARCHIVE_debug)
	$(RM) $(INPUTS_ARCHIVE_release)
	$(RM) dlb_adm_lib_debug.a
	$(RM) dlb_adm_lib_release.a

.PHONY: default FORCE clean cleandeps cleanself help

FORCE:

help:
	@echo "This makefile has the following targets:"
	@echo " dlb_adm_lib_debug.a"
	@echo " dlb_adm_lib_release.a"
	@echo " default"
	@echo " clean"
	@echo " cleandeps"
	@echo " help"

include $(wildcard $(TMP)dlb_adm/src/dlb_adm_api.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/adm_identity/AdmIdSequenceMap.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/adm_identity/AdmIdTranslator.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/adm_transformer/XMLGenerator.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/adm_transformer/XMLIngester.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/AttributeDescriptor.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/AttributeValue.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/ContentContainer.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/EntityContainer.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/EntityDB.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/EntityDescriptor.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/EntityRecord.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/RelationshipDB.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/RelationshipDescriptor.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/RelationshipRecord.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/XMLBuffer.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/XMLContainer.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/XMLReader.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/XMLReaderStack.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/XMLWriter.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/dlb_adm_xml_container.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/AudioElement.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/AudioTrack.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/BlockUpdate.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/ContentGroup.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/CoreModel.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/ElementGroup.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/ElementRecord.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/ElementTable.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/EntityName.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/FrameFormat.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/Gain.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/ModelEntity.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/ModelEntityContainer.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/Position.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/Presentation.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/PresentationRecord.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/PresentationTable.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/Source.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/SourceGroup.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/SourceRecord.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/Target.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/TargetGroup.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/UpdateRecord.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/UpdateTable.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/dlb_adm_core_model.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/dlb_adm_api_pp.dlb_adm_lib_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_adm/src/dlb_adm_api.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/adm_identity/AdmIdSequenceMap.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/adm_identity/AdmIdTranslator.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/adm_transformer/XMLGenerator.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/adm_transformer/XMLIngester.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/AttributeDescriptor.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/AttributeValue.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/ContentContainer.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/EntityContainer.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/EntityDB.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/EntityDescriptor.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/EntityRecord.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/RelationshipDB.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/RelationshipDescriptor.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/RelationshipRecord.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/XMLBuffer.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/XMLContainer.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/XMLReader.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/XMLReaderStack.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/XMLWriter.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/adm_xml/dlb_adm_xml_container.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/AudioElement.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/AudioTrack.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/BlockUpdate.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/ContentGroup.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/CoreModel.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/ElementGroup.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/ElementRecord.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/ElementTable.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/EntityName.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/FrameFormat.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/Gain.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/ModelEntity.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/ModelEntityContainer.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/Position.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/Presentation.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/PresentationRecord.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/PresentationTable.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/Source.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/SourceGroup.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/SourceRecord.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/Target.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/TargetGroup.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/UpdateRecord.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/UpdateTable.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/core_model/dlb_adm_core_model.dlb_adm_lib_linux_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_adm/src/dlb_adm_api_pp.dlb_adm_lib_linux_amd64_gnu_release.d)

$(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_amd64_gnu/dlb_xml_debug.a \
$(BASE)dlb_xmllib/1.0.4/dlb_xml/make/dlb_xml/linux_amd64_gnu/dlb_xml_release.a: %.a: FORCE
	$(MAKE) -C $(@D) $(@F)


