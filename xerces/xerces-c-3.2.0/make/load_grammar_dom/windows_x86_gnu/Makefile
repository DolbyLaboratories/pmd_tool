#-*-makefile-*-

BASE=../../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
CXXDEP = $(CXX)
CXXFLAGS_debug = -msse2 -g -ggdb3 -O0 -m32 -c -Wall
CXXFLAGS_release = -msse2 -O3 -ftree-vectorize -m32 -c -Wall
DEFINES_debug = -DHAVE_CONFIG_H=1 -DWIN32=1 -DXERCES_BUILDING_LIBRARY=1 -D_CONSOLE=1
DEFINES_release = -DHAVE_CONFIG_H=1 -DNDEBUG=1 -DWIN32=1 -DXERCES_BUILDING_LIBRARY=1 -D_CONSOLE=1
INCLUDES = -I$(BASE)xerces/xerces-c-3.2.0/frontend -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/FileManagers -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/MsgLoaders/Win32 -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/MutexManagers -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors/WinSock -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/Transcoders/Win32 -I$(BASE)xerces/xerces-c-3.2.0/windows -I$(BASE)xerces/xerces-c-3.2.0/src -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/dom/impl -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/dom -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/framework -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/framework/psvi -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/internal -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/parsers -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/sax -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/sax2 -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/MsgLoaders/InMemory -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/NetAccessors -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/util/regx -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/validators/DTD -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/validators/common -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/validators/datatype -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/validators/schema/identity -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/validators/schema -I$(BASE)xerces/xerces-c-3.2.0/src/xercesc/xinclude
LD = $(CXX)
LDFLAGS_debug = -m32
LDFLAGS_release = -O2 -m32


INPUTS_LINK_debug = $(TMP)xerces/xerces-c-3.2.0/frontend/load_grammar_dom.load_grammar_dom_windows_x86_gnu_debug.o
INPUTS_LINK_release = $(TMP)xerces/xerces-c-3.2.0/frontend/load_grammar_dom.load_grammar_dom_windows_x86_gnu_release.o

DEP_PROJ_LINK_debug = $(BASE)xerces/xerces-c-3.2.0/make/xerces_3_2_0_lib/windows_x86_gnu/xerces_3_2_0_lib_debug.lib
DEP_PROJ_LINK_release = $(BASE)xerces/xerces-c-3.2.0/make/xerces_3_2_0_lib/windows_x86_gnu/xerces_3_2_0_lib_release.lib

default: load_grammar_dom_debug.exe load_grammar_dom_release.exe

load_grammar_dom_debug.exe: $(INPUTS_LINK_debug) $(DEP_PROJ_LINK_debug)
	$(LD) $(LDFLAGS_debug) -o $@ $+ 

load_grammar_dom_release.exe: $(INPUTS_LINK_release) $(DEP_PROJ_LINK_release)
	$(LD) $(LDFLAGS_release) -o $@ $+ 

$(TMP)%.load_grammar_dom_windows_x86_gnu_debug.o: $(BASE)%.cpp | $(TMP)%/..
	$(CXXDEP) -msse2 -MM $(DEFINES_debug) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS_debug) $(DEFINES_debug) $(INCLUDES) -o $@ $<

$(TMP)%.load_grammar_dom_windows_x86_gnu_release.o: $(BASE)%.cpp | $(TMP)%/..
	$(CXXDEP) -msse2 -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMPILE_debug = $(TMP)xerces/xerces-c-3.2.0/frontend/load_grammar_dom.load_grammar_dom_windows_x86_gnu_debug.d
CLEAN_COMPILE_release = $(TMP)xerces/xerces-c-3.2.0/frontend/load_grammar_dom.load_grammar_dom_windows_x86_gnu_release.d

cleandeps:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)

clean: cleanself
	$(MAKE) -C $(BASE)xerces/xerces-c-3.2.0/make/xerces_3_2_0_lib/windows_x86_gnu cleanself

cleanself:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)
	$(RM) $(INPUTS_LINK_debug)
	$(RM) $(INPUTS_LINK_release)
	$(RM) load_grammar_dom_debug.exe
	$(RM) load_grammar_dom_release.exe

.PHONY: default FORCE clean cleandeps cleanself help

FORCE:

help:
	@echo "This makefile has the following targets:"
	@echo " load_grammar_dom_debug.exe"
	@echo " load_grammar_dom_release.exe"
	@echo " default"
	@echo " clean"
	@echo " cleandeps"
	@echo " help"

include $(wildcard $(TMP)xerces/xerces-c-3.2.0/frontend/load_grammar_dom.load_grammar_dom_windows_x86_gnu_debug.d)
include $(wildcard $(TMP)xerces/xerces-c-3.2.0/frontend/load_grammar_dom.load_grammar_dom_windows_x86_gnu_release.d)

$(BASE)xerces/xerces-c-3.2.0/make/xerces_3_2_0_lib/windows_x86_gnu/xerces_3_2_0_lib_debug.lib \
$(BASE)xerces/xerces-c-3.2.0/make/xerces_3_2_0_lib/windows_x86_gnu/xerces_3_2_0_lib_release.lib: %.lib: FORCE
	$(MAKE) -C $(@D) $(@F)


