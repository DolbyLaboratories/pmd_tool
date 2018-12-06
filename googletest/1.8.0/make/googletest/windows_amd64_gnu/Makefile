#-*-makefile-*-

BASE=../../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
ARFLAGS = -rc
CXXDEP = $(CXX)
CXXFLAGS_debug = -g -ggdb3 -O0 -m64 -c -Wall
CXXFLAGS_release = -O3 -ftree-vectorize -m64 -c -Wall
DEFINES_debug = -DWIN32=1 -DWIN64=1 -D_LIB=1
DEFINES_release = -DNDEBUG=1 -DWIN32=1 -DWIN64=1 -D_LIB=1
INCLUDES = -I$(BASE)googletest/1.8.0/include -I$(BASE)googletest/1.8.0/include -I$(BASE)googletest/1.8.0


INPUTS_ARCHIVE_debug = $(TMP)googletest/1.8.0/src/gtest-all.googletest_windows_amd64_gnu_debug.o $(TMP)googletest/1.8.0/src/gtest-death-test.googletest_windows_amd64_gnu_debug.o\
                       $(TMP)googletest/1.8.0/src/gtest-filepath.googletest_windows_amd64_gnu_debug.o $(TMP)googletest/1.8.0/src/gtest-port.googletest_windows_amd64_gnu_debug.o\
                       $(TMP)googletest/1.8.0/src/gtest-printers.googletest_windows_amd64_gnu_debug.o $(TMP)googletest/1.8.0/src/gtest-test-part.googletest_windows_amd64_gnu_debug.o\
                       $(TMP)googletest/1.8.0/src/gtest-typed-test.googletest_windows_amd64_gnu_debug.o $(TMP)googletest/1.8.0/src/gtest.googletest_windows_amd64_gnu_debug.o\
                       $(TMP)googletest/1.8.0/src/gtest_main.googletest_windows_amd64_gnu_debug.o
INPUTS_ARCHIVE_release = $(TMP)googletest/1.8.0/src/gtest-all.googletest_windows_amd64_gnu_release.o $(TMP)googletest/1.8.0/src/gtest-death-test.googletest_windows_amd64_gnu_release.o\
                         $(TMP)googletest/1.8.0/src/gtest-filepath.googletest_windows_amd64_gnu_release.o $(TMP)googletest/1.8.0/src/gtest-port.googletest_windows_amd64_gnu_release.o\
                         $(TMP)googletest/1.8.0/src/gtest-printers.googletest_windows_amd64_gnu_release.o $(TMP)googletest/1.8.0/src/gtest-test-part.googletest_windows_amd64_gnu_release.o\
                         $(TMP)googletest/1.8.0/src/gtest-typed-test.googletest_windows_amd64_gnu_release.o $(TMP)googletest/1.8.0/src/gtest.googletest_windows_amd64_gnu_release.o\
                         $(TMP)googletest/1.8.0/src/gtest_main.googletest_windows_amd64_gnu_release.o


default: googletest_debug.lib googletest_release.lib

googletest_debug.lib: $(INPUTS_ARCHIVE_debug)
	$(AR) $(ARFLAGS)  $@ $^

googletest_release.lib: $(INPUTS_ARCHIVE_release)
	$(AR) $(ARFLAGS)  $@ $^

$(TMP)%.googletest_windows_amd64_gnu_debug.o: $(BASE)%.cc | $(TMP)%/..
	$(CXXDEP) -MM $(DEFINES_debug) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS_debug) $(DEFINES_debug) $(INCLUDES) -o $@ $<

$(TMP)%.googletest_windows_amd64_gnu_release.o: $(BASE)%.cc | $(TMP)%/..
	$(CXXDEP) -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMPILE_debug = $(TMP)googletest/1.8.0/src/gtest-all.googletest_windows_amd64_gnu_debug.d $(TMP)googletest/1.8.0/src/gtest-death-test.googletest_windows_amd64_gnu_debug.d\
                      $(TMP)googletest/1.8.0/src/gtest-filepath.googletest_windows_amd64_gnu_debug.d $(TMP)googletest/1.8.0/src/gtest-port.googletest_windows_amd64_gnu_debug.d\
                      $(TMP)googletest/1.8.0/src/gtest-printers.googletest_windows_amd64_gnu_debug.d $(TMP)googletest/1.8.0/src/gtest-test-part.googletest_windows_amd64_gnu_debug.d\
                      $(TMP)googletest/1.8.0/src/gtest-typed-test.googletest_windows_amd64_gnu_debug.d $(TMP)googletest/1.8.0/src/gtest.googletest_windows_amd64_gnu_debug.d\
                      $(TMP)googletest/1.8.0/src/gtest_main.googletest_windows_amd64_gnu_debug.d
CLEAN_COMPILE_release = $(TMP)googletest/1.8.0/src/gtest-all.googletest_windows_amd64_gnu_release.d $(TMP)googletest/1.8.0/src/gtest-death-test.googletest_windows_amd64_gnu_release.d\
                        $(TMP)googletest/1.8.0/src/gtest-filepath.googletest_windows_amd64_gnu_release.d $(TMP)googletest/1.8.0/src/gtest-port.googletest_windows_amd64_gnu_release.d\
                        $(TMP)googletest/1.8.0/src/gtest-printers.googletest_windows_amd64_gnu_release.d $(TMP)googletest/1.8.0/src/gtest-test-part.googletest_windows_amd64_gnu_release.d\
                        $(TMP)googletest/1.8.0/src/gtest-typed-test.googletest_windows_amd64_gnu_release.d $(TMP)googletest/1.8.0/src/gtest.googletest_windows_amd64_gnu_release.d\
                        $(TMP)googletest/1.8.0/src/gtest_main.googletest_windows_amd64_gnu_release.d

cleandeps:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)

clean: cleanself

cleanself:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)
	$(RM) $(INPUTS_ARCHIVE_debug)
	$(RM) $(INPUTS_ARCHIVE_release)
	$(RM) googletest_debug.lib
	$(RM) googletest_release.lib

.PHONY: default FORCE clean cleandeps cleanself help

FORCE:

help:
	@echo "This makefile has the following targets:"
	@echo " googletest_debug.lib"
	@echo " googletest_release.lib"
	@echo " default"
	@echo " clean"
	@echo " cleandeps"
	@echo " help"

include $(wildcard $(TMP)googletest/1.8.0/src/gtest-all.googletest_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)googletest/1.8.0/src/gtest-death-test.googletest_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)googletest/1.8.0/src/gtest-filepath.googletest_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)googletest/1.8.0/src/gtest-port.googletest_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)googletest/1.8.0/src/gtest-printers.googletest_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)googletest/1.8.0/src/gtest-test-part.googletest_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)googletest/1.8.0/src/gtest-typed-test.googletest_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)googletest/1.8.0/src/gtest.googletest_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)googletest/1.8.0/src/gtest_main.googletest_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)googletest/1.8.0/src/gtest-all.googletest_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)googletest/1.8.0/src/gtest-death-test.googletest_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)googletest/1.8.0/src/gtest-filepath.googletest_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)googletest/1.8.0/src/gtest-port.googletest_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)googletest/1.8.0/src/gtest-printers.googletest_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)googletest/1.8.0/src/gtest-test-part.googletest_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)googletest/1.8.0/src/gtest-typed-test.googletest_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)googletest/1.8.0/src/gtest.googletest_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)googletest/1.8.0/src/gtest_main.googletest_windows_amd64_gnu_release.d)


