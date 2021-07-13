#-*-makefile-*-

BASE=../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
ARFLAGS = -rc
CXXDEP = $(CXX)
CXXFLAGS_debug = -std=c++17 -msse2 -g -ggdb3 -O0 -m32 -c -Wall
CXXFLAGS_release = -std=c++17 -msse2 -O3 -ftree-vectorize -m32 -c -Wall
DEFINES_debug = -DELPP_NO_DEFAULT_LOG_FILE=1
DEFINES_release = -DELPP_NO_DEFAULT_LOG_FILE=1 -DNDEBUG=1
INCLUDES = -I$(BASE)../../../../../../../usr/include/mellanox -I$(BASE)dlb_st2110/0.9/dlb_st2110/src -I$(BASE)dlb_st2110/0.9/dlb_st2110/include


INPUTS_ARCHIVE_debug = $(TMP)dlb_st2110/0.9/dlb_st2110/src/audio_buffer.dlb_st2110_lib_linux_x86_gnu_debug.o $(TMP)dlb_st2110/0.9/dlb_st2110/src/dlb_aoip_services.dlb_st2110_lib_linux_x86_gnu_debug.o\
                       $(TMP)dlb_st2110/0.9/dlb_st2110/src/dlb_st2110_hardware.dlb_st2110_lib_linux_x86_gnu_debug.o $(TMP)dlb_st2110/0.9/dlb_st2110/src/dlb_st2110_logging.dlb_st2110_lib_linux_x86_gnu_debug.o\
                       $(TMP)dlb_st2110/0.9/dlb_st2110/src/dlb_st2110_receiver.dlb_st2110_lib_linux_x86_gnu_debug.o $(TMP)dlb_st2110/0.9/dlb_st2110/src/dlb_st2110_sdp.dlb_st2110_lib_linux_x86_gnu_debug.o\
                       $(TMP)dlb_st2110/0.9/dlb_st2110/src/dlb_st2110_transmitter.dlb_st2110_lib_linux_x86_gnu_debug.o $(TMP)dlb_st2110/0.9/dlb_st2110/src/easylogging++.dlb_st2110_lib_linux_x86_gnu_debug.o\
                       $(TMP)dlb_st2110/0.9/dlb_st2110/src/mclock.dlb_st2110_lib_linux_x86_gnu_debug.o $(TMP)dlb_st2110/0.9/dlb_st2110/src/rav_discovery.dlb_st2110_lib_linux_x86_gnu_debug.o\
                       $(TMP)dlb_st2110/0.9/dlb_st2110/src/sap_discovery.dlb_st2110_lib_linux_x86_gnu_debug.o
INPUTS_ARCHIVE_release = $(TMP)dlb_st2110/0.9/dlb_st2110/src/audio_buffer.dlb_st2110_lib_linux_x86_gnu_release.o $(TMP)dlb_st2110/0.9/dlb_st2110/src/dlb_aoip_services.dlb_st2110_lib_linux_x86_gnu_release.o\
                         $(TMP)dlb_st2110/0.9/dlb_st2110/src/dlb_st2110_hardware.dlb_st2110_lib_linux_x86_gnu_release.o\
                         $(TMP)dlb_st2110/0.9/dlb_st2110/src/dlb_st2110_logging.dlb_st2110_lib_linux_x86_gnu_release.o $(TMP)dlb_st2110/0.9/dlb_st2110/src/dlb_st2110_receiver.dlb_st2110_lib_linux_x86_gnu_release.o\
                         $(TMP)dlb_st2110/0.9/dlb_st2110/src/dlb_st2110_sdp.dlb_st2110_lib_linux_x86_gnu_release.o $(TMP)dlb_st2110/0.9/dlb_st2110/src/dlb_st2110_transmitter.dlb_st2110_lib_linux_x86_gnu_release.o\
                         $(TMP)dlb_st2110/0.9/dlb_st2110/src/easylogging++.dlb_st2110_lib_linux_x86_gnu_release.o $(TMP)dlb_st2110/0.9/dlb_st2110/src/mclock.dlb_st2110_lib_linux_x86_gnu_release.o\
                         $(TMP)dlb_st2110/0.9/dlb_st2110/src/rav_discovery.dlb_st2110_lib_linux_x86_gnu_release.o $(TMP)dlb_st2110/0.9/dlb_st2110/src/sap_discovery.dlb_st2110_lib_linux_x86_gnu_release.o


default: dlb_st2110_lib_debug.a dlb_st2110_lib_release.a

dlb_st2110_lib_debug.a: $(INPUTS_ARCHIVE_debug)
	$(AR) $(ARFLAGS)  $@ $^

dlb_st2110_lib_release.a: $(INPUTS_ARCHIVE_release)
	$(AR) $(ARFLAGS)  $@ $^

$(TMP)%.dlb_st2110_lib_linux_x86_gnu_debug.o: $(BASE)%.cpp | $(TMP)%/..
	$(CXXDEP) -msse2 -MM $(DEFINES_debug) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS_debug) $(DEFINES_debug) $(INCLUDES) -o $@ $<

$(TMP)%.dlb_st2110_lib_linux_x86_gnu_release.o: $(BASE)%.cpp | $(TMP)%/..
	$(CXXDEP) -msse2 -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CXXFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMPILE_debug = $(TMP)dlb_st2110/0.9/dlb_st2110/src/audio_buffer.dlb_st2110_lib_linux_x86_gnu_debug.d $(TMP)dlb_st2110/0.9/dlb_st2110/src/dlb_aoip_services.dlb_st2110_lib_linux_x86_gnu_debug.d\
                      $(TMP)dlb_st2110/0.9/dlb_st2110/src/dlb_st2110_hardware.dlb_st2110_lib_linux_x86_gnu_debug.d $(TMP)dlb_st2110/0.9/dlb_st2110/src/dlb_st2110_logging.dlb_st2110_lib_linux_x86_gnu_debug.d\
                      $(TMP)dlb_st2110/0.9/dlb_st2110/src/dlb_st2110_receiver.dlb_st2110_lib_linux_x86_gnu_debug.d $(TMP)dlb_st2110/0.9/dlb_st2110/src/dlb_st2110_sdp.dlb_st2110_lib_linux_x86_gnu_debug.d\
                      $(TMP)dlb_st2110/0.9/dlb_st2110/src/dlb_st2110_transmitter.dlb_st2110_lib_linux_x86_gnu_debug.d $(TMP)dlb_st2110/0.9/dlb_st2110/src/easylogging++.dlb_st2110_lib_linux_x86_gnu_debug.d\
                      $(TMP)dlb_st2110/0.9/dlb_st2110/src/mclock.dlb_st2110_lib_linux_x86_gnu_debug.d $(TMP)dlb_st2110/0.9/dlb_st2110/src/rav_discovery.dlb_st2110_lib_linux_x86_gnu_debug.d\
                      $(TMP)dlb_st2110/0.9/dlb_st2110/src/sap_discovery.dlb_st2110_lib_linux_x86_gnu_debug.d
CLEAN_COMPILE_release = $(TMP)dlb_st2110/0.9/dlb_st2110/src/audio_buffer.dlb_st2110_lib_linux_x86_gnu_release.d $(TMP)dlb_st2110/0.9/dlb_st2110/src/dlb_aoip_services.dlb_st2110_lib_linux_x86_gnu_release.d\
                        $(TMP)dlb_st2110/0.9/dlb_st2110/src/dlb_st2110_hardware.dlb_st2110_lib_linux_x86_gnu_release.d $(TMP)dlb_st2110/0.9/dlb_st2110/src/dlb_st2110_logging.dlb_st2110_lib_linux_x86_gnu_release.d\
                        $(TMP)dlb_st2110/0.9/dlb_st2110/src/dlb_st2110_receiver.dlb_st2110_lib_linux_x86_gnu_release.d $(TMP)dlb_st2110/0.9/dlb_st2110/src/dlb_st2110_sdp.dlb_st2110_lib_linux_x86_gnu_release.d\
                        $(TMP)dlb_st2110/0.9/dlb_st2110/src/dlb_st2110_transmitter.dlb_st2110_lib_linux_x86_gnu_release.d\
                        $(TMP)dlb_st2110/0.9/dlb_st2110/src/easylogging++.dlb_st2110_lib_linux_x86_gnu_release.d $(TMP)dlb_st2110/0.9/dlb_st2110/src/mclock.dlb_st2110_lib_linux_x86_gnu_release.d\
                        $(TMP)dlb_st2110/0.9/dlb_st2110/src/rav_discovery.dlb_st2110_lib_linux_x86_gnu_release.d $(TMP)dlb_st2110/0.9/dlb_st2110/src/sap_discovery.dlb_st2110_lib_linux_x86_gnu_release.d

cleandeps:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)

clean: cleanself

cleanself:
	$(RM) $(CLEAN_COMPILE_debug)
	$(RM) $(CLEAN_COMPILE_release)
	$(RM) $(INPUTS_ARCHIVE_debug)
	$(RM) $(INPUTS_ARCHIVE_release)
	$(RM) dlb_st2110_lib_debug.a
	$(RM) dlb_st2110_lib_release.a

.PHONY: default FORCE clean cleandeps cleanself help

FORCE:

help:
	@echo "This makefile has the following targets:"
	@echo " dlb_st2110_lib_debug.a"
	@echo " dlb_st2110_lib_release.a"
	@echo " default"
	@echo " clean"
	@echo " cleandeps"
	@echo " help"

include $(wildcard $(TMP)dlb_st2110/0.9/dlb_st2110/src/audio_buffer.dlb_st2110_lib_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_st2110/0.9/dlb_st2110/src/dlb_aoip_services.dlb_st2110_lib_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_st2110/0.9/dlb_st2110/src/dlb_st2110_hardware.dlb_st2110_lib_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_st2110/0.9/dlb_st2110/src/dlb_st2110_logging.dlb_st2110_lib_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_st2110/0.9/dlb_st2110/src/dlb_st2110_receiver.dlb_st2110_lib_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_st2110/0.9/dlb_st2110/src/dlb_st2110_sdp.dlb_st2110_lib_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_st2110/0.9/dlb_st2110/src/dlb_st2110_transmitter.dlb_st2110_lib_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_st2110/0.9/dlb_st2110/src/easylogging++.dlb_st2110_lib_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_st2110/0.9/dlb_st2110/src/mclock.dlb_st2110_lib_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_st2110/0.9/dlb_st2110/src/rav_discovery.dlb_st2110_lib_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_st2110/0.9/dlb_st2110/src/sap_discovery.dlb_st2110_lib_linux_x86_gnu_debug.d)
include $(wildcard $(TMP)dlb_st2110/0.9/dlb_st2110/src/audio_buffer.dlb_st2110_lib_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_st2110/0.9/dlb_st2110/src/dlb_aoip_services.dlb_st2110_lib_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_st2110/0.9/dlb_st2110/src/dlb_st2110_hardware.dlb_st2110_lib_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_st2110/0.9/dlb_st2110/src/dlb_st2110_logging.dlb_st2110_lib_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_st2110/0.9/dlb_st2110/src/dlb_st2110_receiver.dlb_st2110_lib_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_st2110/0.9/dlb_st2110/src/dlb_st2110_sdp.dlb_st2110_lib_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_st2110/0.9/dlb_st2110/src/dlb_st2110_transmitter.dlb_st2110_lib_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_st2110/0.9/dlb_st2110/src/easylogging++.dlb_st2110_lib_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_st2110/0.9/dlb_st2110/src/mclock.dlb_st2110_lib_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_st2110/0.9/dlb_st2110/src/rav_discovery.dlb_st2110_lib_linux_x86_gnu_release.d)
include $(wildcard $(TMP)dlb_st2110/0.9/dlb_st2110/src/sap_discovery.dlb_st2110_lib_linux_x86_gnu_release.d)


