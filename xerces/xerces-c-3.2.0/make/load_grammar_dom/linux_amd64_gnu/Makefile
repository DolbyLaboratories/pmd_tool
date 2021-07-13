#-*-makefile-*-

BASE=../../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
CXXDEP = $(CXX)
CXXFLAGS_debug = -std=c++17 -g -ggdb3 -O0 -m64 -c -Wall
CXXFLAGS_release = -std=c++17 -O3 -ftree-vectorize -m64 -c -Wall
DEFINES_debug = -DELPP_NO_DEFAULT_LOG_FILE=1 -DHAVE_CONFIG_H=1 -DXERCES_BUILDING_LIBRARY=1
DEFINES_release = -DELPP_NO_DEFAULT_LOG_FILE=1 -DHAVE_CONFIG_H=1 -DNDEBUG=1 -DXERCES_BUILDING_LIBRARY=1
INCLUDES = -I$(BASE)xerces/xerces-c-3.2.0/frontend -I$(BASE)xerces/xerces-c-3.2.0/linux/x86 -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/FileManagers -I$(BASE)xerces/xerces-c-3.2.0/src
LD = $(CXX)
LDFLAGS_debug = -m64
LDFLAGS_release = -O2 -m64
LDLIBS = -lnsl -lpthread -lcurl -licui18n -licuuc -licudata


INPUTS_LINK_debug = $(TMP)xerces/xerces-c-3.2.0/frontend/load_grammar_dom.load_grammar_dom_linux_amd64_gnu_debug.o
INPUTS_LINK_release = $(TMP)xerces/xerces-c-3.2.0/frontend/load_grammar_dom.load_grammar_dom_linux_amd64_gnu_release.o

DEP_PROJ_LINK_debug = $(BASE)xerces/xerces-c-3.2.0/make/xerces_3_2_0_lib/linux_amd64_gnu/xerces_3_2_0_lib_debug.a
DEP_PROJ_LINK_release = $(BASE)xerces/xerces-c-3.2.0/make/xerces_3_2_0_lib/linux_amd64_gnu/xerces_3_2_0_lib_release.a

default: load_grammar_dom_debug load_grammar_dom_release

load_grammar_dom_debug: $(INPUTS_LINK_debug) $(DEP_PROJ_LINK_debug)
	$(LD) $(LDFLAGS_debug) -o $@ $+ $(LDLIBS)

load_grammar_dom_release: $(INPUTS_LINK_release) $(DEP_PROJ_LINK_release)
	$(LD) $(LDFLAGS_release) -o $@ $+ $(LDLIBS)

$(TMP)%.load_grammar_dom_linux_amd64_gnu_debug.o: $(BASE)%.cpp | $(TMP)%/..
	$(CXXDEP) -MM $(DEFINES_debug) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS_debug) $(DEFINES_debug) $(INCLUDES) -o $@ $<

$(TMP)%.load_grammar_dom_linux_amd64_gnu_release.o: $(BASE)%.cpp | $(TMP)%/..
	$(CXXDEP) -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMPILE_debug = $(TMP)xerces/xerces-c-3.2.0/frontend/load_grammar_dom.load_grammar_dom_linux_amd64_gnu_debug.d
CLEAN_COMPILE_release = $(TMP)xerces/xerces-c-3.2.0/frontend/load_grammar_dom.load_grammar_dom_linux_amd64_gnu_release.d

cleandeps:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)

clean: cleanself
	$(MAKE) -C $(BASE)xerces/xerces-c-3.2.0/make/xerces_3_2_0_lib/linux_amd64_gnu cleanself

cleanself:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)
	$(RM) $(INPUTS_LINK_debug)
	$(RM) $(INPUTS_LINK_release)
	$(RM) load_grammar_dom_debug
	$(RM) load_grammar_dom_release

.PHONY: default FORCE clean cleandeps cleanself help

FORCE:

help:
	@echo "This makefile has the following targets:"
	@echo " load_grammar_dom_debug"
	@echo " load_grammar_dom_release"
	@echo " default"
	@echo " clean"
	@echo " cleandeps"
	@echo " help"

include $(wildcard $(TMP)xerces/xerces-c-3.2.0/frontend/load_grammar_dom.load_grammar_dom_linux_amd64_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/frontend/load_grammar_dom.load_grammar_dom_linux_amd64_gnu_release.d)

$(BASE)xerces/xerces-c-3.2.0/make/xerces_3_2_0_lib/linux_amd64_gnu/xerces_3_2_0_lib_debug.a \
$(BASE)xerces/xerces-c-3.2.0/make/xerces_3_2_0_lib/linux_amd64_gnu/xerces_3_2_0_lib_release.a: %.a: FORCE
	$(MAKE) -C $(@D) $(@F)


