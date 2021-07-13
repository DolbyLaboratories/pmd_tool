#-*-makefile-*-

BASE=../../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
ARFLAGS = -rc
CXXDEP = $(CXX)
CXXFLAGS_debug = -std=c++17 -msse2 -g -ggdb3 -O0 -m32 -c -Wall
CXXFLAGS_release = -std=c++17 -msse2 -O3 -ftree-vectorize -m32 -c -Wall
DEFINES_debug = -DELPP_NO_DEFAULT_LOG_FILE=1
DEFINES_release = -DELPP_NO_DEFAULT_LOG_FILE=1 -DNDEBUG=1
INCLUDES = -I$(BASE)googletest/1.8.0/include -I$(BASE)googletest/1.8.0/include -I$(BASE)googletest/1.8.0


INPUTS_ARCHIVE_debug = $(TMP)googletest/1.8.0/src/gtest-death-test.googletest_linux_x86_gnu_debug.o $(TMP)googletest/1.8.0/src/gtest-filepath.googletest_linux_x86_gnu_debug.o\
                       $(TMP)googletest/1.8.0/src/gtest-port.googletest_linux_x86_gnu_debug.o $(TMP)googletest/1.8.0/src/gtest-printers.googletest_linux_x86_gnu_debug.o\
                       $(TMP)googletest/1.8.0/src/gtest-test-part.googletest_linux_x86_gnu_debug.o $(TMP)googletest/1.8.0/src/gtest-typed-test.googletest_linux_x86_gnu_debug.o\
                       $(TMP)googletest/1.8.0/src/gtest.googletest_linux_x86_gnu_debug.o $(TMP)googletest/1.8.0/src/gtest_main.googletest_linux_x86_gnu_debug.o
INPUTS_ARCHIVE_release = $(TMP)googletest/1.8.0/src/gtest-death-test.googletest_linux_x86_gnu_release.o $(TMP)googletest/1.8.0/src/gtest-filepath.googletest_linux_x86_gnu_release.o\
                         $(TMP)googletest/1.8.0/src/gtest-port.googletest_linux_x86_gnu_release.o $(TMP)googletest/1.8.0/src/gtest-printers.googletest_linux_x86_gnu_release.o\
                         $(TMP)googletest/1.8.0/src/gtest-test-part.googletest_linux_x86_gnu_release.o $(TMP)googletest/1.8.0/src/gtest-typed-test.googletest_linux_x86_gnu_release.o\
                         $(TMP)googletest/1.8.0/src/gtest.googletest_linux_x86_gnu_release.o $(TMP)googletest/1.8.0/src/gtest_main.googletest_linux_x86_gnu_release.o


default: googletest_debug.a googletest_release.a

googletest_debug.a: $(INPUTS_ARCHIVE_debug)
	$(AR) $(ARFLAGS)  $@ $^

googletest_release.a: $(INPUTS_ARCHIVE_release)
	$(AR) $(ARFLAGS)  $@ $^

$(TMP)%.googletest_linux_x86_gnu_debug.o: $(BASE)%.cc | $(TMP)%/..
	$(CXXDEP) -msse2 -MM $(DEFINES_debug) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS_debug) $(DEFINES_debug) $(INCLUDES) -o $@ $<

$(TMP)%.googletest_linux_x86_gnu_release.o: $(BASE)%.cc | $(TMP)%/..
	$(CXXDEP) -msse2 -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMPILE_debug = $(TMP)googletest/1.8.0/src/gtest-death-test.googletest_linux_x86_gnu_debug.d $(TMP)googletest/1.8.0/src/gtest-filepath.googletest_linux_x86_gnu_debug.d\
                      $(TMP)googletest/1.8.0/src/gtest-port.googletest_linux_x86_gnu_debug.d $(TMP)googletest/1.8.0/src/gtest-printers.googletest_linux_x86_gnu_debug.d\
                      $(TMP)googletest/1.8.0/src/gtest-test-part.googletest_linux_x86_gnu_debug.d $(TMP)googletest/1.8.0/src/gtest-typed-test.googletest_linux_x86_gnu_debug.d\
                      $(TMP)googletest/1.8.0/src/gtest.googletest_linux_x86_gnu_debug.d $(TMP)googletest/1.8.0/src/gtest_main.googletest_linux_x86_gnu_debug.d
CLEAN_COMPILE_release = $(TMP)googletest/1.8.0/src/gtest-death-test.googletest_linux_x86_gnu_release.d $(TMP)googletest/1.8.0/src/gtest-filepath.googletest_linux_x86_gnu_release.d\
                        $(TMP)googletest/1.8.0/src/gtest-port.googletest_linux_x86_gnu_release.d $(TMP)googletest/1.8.0/src/gtest-printers.googletest_linux_x86_gnu_release.d\
                        $(TMP)googletest/1.8.0/src/gtest-test-part.googletest_linux_x86_gnu_release.d $(TMP)googletest/1.8.0/src/gtest-typed-test.googletest_linux_x86_gnu_release.d\
                        $(TMP)googletest/1.8.0/src/gtest.googletest_linux_x86_gnu_release.d $(TMP)googletest/1.8.0/src/gtest_main.googletest_linux_x86_gnu_release.d

cleandeps:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)

clean: cleanself

cleanself:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)
	$(RM) $(INPUTS_ARCHIVE_debug)
	$(RM) $(INPUTS_ARCHIVE_release)
	$(RM) googletest_debug.a
	$(RM) googletest_release.a

.PHONY: default FORCE clean cleandeps cleanself help

FORCE:

help:
	@echo "This makefile has the following targets:"
	@echo " googletest_debug.a"
	@echo " googletest_release.a"
	@echo " default"
	@echo " clean"
	@echo " cleandeps"
	@echo " help"

include $(wildcard $(TMP)googletest/1.8.0/src/gtest-death-test.googletest_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)googletest/1.8.0/src/gtest-filepath.googletest_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)googletest/1.8.0/src/gtest-port.googletest_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)googletest/1.8.0/src/gtest-printers.googletest_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)googletest/1.8.0/src/gtest-test-part.googletest_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)googletest/1.8.0/src/gtest-typed-test.googletest_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)googletest/1.8.0/src/gtest.googletest_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)googletest/1.8.0/src/gtest_main.googletest_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)googletest/1.8.0/src/gtest-death-test.googletest_linux_x86_gnu_release.d)
include $(wildcard $(TMP)googletest/1.8.0/src/gtest-filepath.googletest_linux_x86_gnu_release.d)
include $(wildcard $(TMP)googletest/1.8.0/src/gtest-port.googletest_linux_x86_gnu_release.d)
include $(wildcard $(TMP)googletest/1.8.0/src/gtest-printers.googletest_linux_x86_gnu_release.d)
include $(wildcard $(TMP)googletest/1.8.0/src/gtest-test-part.googletest_linux_x86_gnu_release.d)
include $(wildcard $(TMP)googletest/1.8.0/src/gtest-typed-test.googletest_linux_x86_gnu_release.d)
include $(wildcard $(TMP)googletest/1.8.0/src/gtest.googletest_linux_x86_gnu_release.d)
include $(wildcard $(TMP)googletest/1.8.0/src/gtest_main.googletest_linux_x86_gnu_release.d)


