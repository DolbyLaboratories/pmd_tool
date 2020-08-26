#-*-makefile-*-

BASE=../../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
CXXDEP = $(CXX)
CXXFLAGS_debug = -g -O0 -arch x86_64 -fPIC -c -Wall -std=c++11
CXXFLAGS_release = -O3 -arch x86_64 -fPIC -c -Wall -std=c++11
DEFINES_debug = -DHAVE_CONFIG_H=1 -DXERCES_BUILDING_LIBRARY=1
DEFINES_release = -DHAVE_CONFIG_H=1 -DNDEBUG=1 -DXERCES_BUILDING_LIBRARY=1
INCLUDES = -I$(BASE)xerces/xerces-c-3.2.0/frontend -I$(BASE)xerces/xerces-c-3.2.0/osx -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/FileManagers -I$(BASE)xerces/xerces-c-3.2.0/src
LD = $(CXX)
LDFLAGS_debug = -arch x86_64 -dead_strip
LDFLAGS_release = -arch x86_64 -dead_strip -O2
LDLIBS = -lpthread -lcurl -Wl,-framework -Wl,CoreServices


INPUTS_LINK_debug = $(TMP)xerces/xerces-c-3.2.0/frontend/load_grammar_sax.load_grammar_sax_osx_amd64_clang_debug.o
INPUTS_LINK_release = $(TMP)xerces/xerces-c-3.2.0/frontend/load_grammar_sax.load_grammar_sax_osx_amd64_clang_release.o

DEP_PROJ_LINK_debug = $(BASE)xerces/xerces-c-3.2.0/make/xerces_3_2_0_lib/osx_amd64_clang/xerces_3_2_0_lib_debug.a
DEP_PROJ_LINK_release = $(BASE)xerces/xerces-c-3.2.0/make/xerces_3_2_0_lib/osx_amd64_clang/xerces_3_2_0_lib_release.a

default: load_grammar_sax_debug load_grammar_sax_release

load_grammar_sax_debug: $(INPUTS_LINK_debug) $(DEP_PROJ_LINK_debug)
	$(LD) $(LDFLAGS_debug) -o $@ $+ $(LDLIBS)

load_grammar_sax_release: $(INPUTS_LINK_release) $(DEP_PROJ_LINK_release)
	$(LD) $(LDFLAGS_release) -o $@ $+ $(LDLIBS)

$(TMP)%.load_grammar_sax_osx_amd64_clang_debug.o: $(BASE)%.cpp | $(TMP)%/..
	$(CXXDEP) -arch x86_64 -MM -std=c++11 $(DEFINES_debug) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS_debug) $(DEFINES_debug) $(INCLUDES) -o $@ $<

$(TMP)%.load_grammar_sax_osx_amd64_clang_release.o: $(BASE)%.cpp | $(TMP)%/..
	$(CXXDEP) -arch x86_64 -MM -std=c++11 $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMPILE_debug = $(TMP)xerces/xerces-c-3.2.0/frontend/load_grammar_sax.load_grammar_sax_osx_amd64_clang_debug.d
CLEAN_COMPILE_release = $(TMP)xerces/xerces-c-3.2.0/frontend/load_grammar_sax.load_grammar_sax_osx_amd64_clang_release.d

cleandeps:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)

clean: cleanself
	$(MAKE) -C $(BASE)xerces/xerces-c-3.2.0/make/xerces_3_2_0_lib/osx_amd64_clang cleanself

cleanself:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)
	$(RM) $(INPUTS_LINK_debug)
	$(RM) $(INPUTS_LINK_release)
	$(RM) load_grammar_sax_debug
	$(RM) load_grammar_sax_release

.PHONY: default FORCE clean cleandeps cleanself help

FORCE:

help:
	@echo "This makefile has the following targets:"
	@echo " load_grammar_sax_debug"
	@echo " load_grammar_sax_release"
	@echo " default"
	@echo " clean"
	@echo " cleandeps"
	@echo " help"

include $(wildcard $(TMP)xerces/xerces-c-3.2.0/frontend/load_grammar_sax.load_grammar_sax_osx_amd64_clang_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/frontend/load_grammar_sax.load_grammar_sax_osx_amd64_clang_release.d)

$(BASE)xerces/xerces-c-3.2.0/make/xerces_3_2_0_lib/osx_amd64_clang/xerces_3_2_0_lib_debug.a \
$(BASE)xerces/xerces-c-3.2.0/make/xerces_3_2_0_lib/osx_amd64_clang/xerces_3_2_0_lib_release.a: %.a: FORCE
	$(MAKE) -C $(@D) $(@F)


