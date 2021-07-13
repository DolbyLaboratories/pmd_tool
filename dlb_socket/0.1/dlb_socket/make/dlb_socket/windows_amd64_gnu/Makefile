#-*-makefile-*-

BASE=../../../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
ARFLAGS = -rc
CCDEP = $(CC)
CFLAGS_debug = -g -ggdb3 -O0 -m64 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
CFLAGS_release = -O3 -ftree-vectorize -m64 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
DEFINES_debug = -DELPP_NO_DEFAULT_LOG_FILE=1 -DWIN32=1 -DWIN64=1 -D_LIB=1
DEFINES_release = -DELPP_NO_DEFAULT_LOG_FILE=1 -DNDEBUG=1 -DWIN32=1 -DWIN64=1 -D_LIB=1
INCLUDES = -I$(BASE)dlb_buffer/1.3 -I$(BASE)dlb_octfile/1.1 -I$(BASE)dlb_socket/0.1/dlb_socket/include/win32 -I$(BASE)dlb_socket/0.1/dlb_socket/include -I$(BASE)dlb_socket/0.1/dlb_socket/src/win32 -I$(BASE)dlb_socket/0.1 -I$(BASE).

COMMON_FILES = $(TMP)dlb_socket/0.1/dlb_socket/src/dlb_http_client $(TMP)dlb_socket/0.1/dlb_socket/src/dlb_http_server $(TMP)dlb_socket/0.1/dlb_socket/src/dlb_socket

INPUTS_COMMON_ARCHIVE_debug = $(addsuffix .dlb_socket_windows_amd64_gnu_debug.o,$(COMMON_FILES))
INPUTS_COMMON_ARCHIVE_release = $(addsuffix .dlb_socket_windows_amd64_gnu_release.o,$(COMMON_FILES))


default: dlb_socket_debug.lib dlb_socket_release.lib

dlb_socket_debug.lib: $(INPUTS_COMMON_ARCHIVE_debug)
	$(AR) $(ARFLAGS)  $@ $^

dlb_socket_release.lib: $(INPUTS_COMMON_ARCHIVE_release)
	$(AR) $(ARFLAGS)  $@ $^

$(TMP)%.dlb_socket_windows_amd64_gnu_debug.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -MM $(DEFINES_debug) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_debug) $(DEFINES_debug) $(INCLUDES) -o $@ $<

$(TMP)%.dlb_socket_windows_amd64_gnu_release.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMMON_COMPILE_debug = $(addsuffix .dlb_socket_windows_amd64_gnu_debug.d,$(COMMON_FILES))
CLEAN_COMMON_COMPILE_release = $(addsuffix .dlb_socket_windows_amd64_gnu_release.d,$(COMMON_FILES))

cleandeps:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)

clean: cleanself

cleanself:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)
	$(RM) $(INPUTS_COMMON_ARCHIVE_debug)
	$(RM) $(INPUTS_COMMON_ARCHIVE_release)
	$(RM) dlb_socket_debug.lib
	$(RM) dlb_socket_release.lib

.PHONY: default FORCE clean cleandeps cleanself help

FORCE:

help:
	@echo "This makefile has the following targets:"
	@echo " dlb_socket_debug.lib"
	@echo " dlb_socket_release.lib"
	@echo " default"
	@echo " clean"
	@echo " cleandeps"
	@echo " help"

include $(wildcard $(TMP)dlb_socket/0.1/dlb_socket/src/dlb_http_client.dlb_socket_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_socket/0.1/dlb_socket/src/dlb_http_server.dlb_socket_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_socket/0.1/dlb_socket/src/dlb_socket.dlb_socket_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_socket/0.1/dlb_socket/src/dlb_http_client.dlb_socket_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_socket/0.1/dlb_socket/src/dlb_http_server.dlb_socket_windows_amd64_gnu_release.d)
include $(wildcard $(TMP)dlb_socket/0.1/dlb_socket/src/dlb_socket.dlb_socket_windows_amd64_gnu_release.d)


