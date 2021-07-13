#-*-makefile-*-

BASE=../../../../../../
TMP=$(BASE)
.PRECIOUS: %/..
%/..:; @test $(dir $(basename $(@D))) | mkdir -p $(dir $(basename $(@D)))
CCDEP = $(CC)
CFLAGS_debug = -g -ggdb3 -O0 -m64 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
CFLAGS_release = -O3 -ftree-vectorize -m64 -c -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -pedantic -std=gnu99 -Wdeclaration-after-statement -Wvla
DEFINES_debug = -DELPP_NO_DEFAULT_LOG_FILE=1 -DWIN32=1 -DWIN64=1 -D_CONSOLE=1
DEFINES_release = -DELPP_NO_DEFAULT_LOG_FILE=1 -DNDEBUG=1 -DWIN32=1 -DWIN64=1 -D_CONSOLE=1
INCLUDES = -I$(BASE)dlb_buffer/1.3 -I$(BASE)dlb_octfile/1.1 -I$(BASE)dlb_socket/0.1/dlb_socket/include/win32 -I$(BASE)dlb_socket/0.1/dlb_socket/include -I$(BASE)dlb_socket/0.1 -I$(BASE).
LD = $(CC)
LDFLAGS_debug = -m64
LDFLAGS_release = -O2 -m64

COMMON_FILES = $(TMP)dlb_socket/0.1/dlb_socket/frontend/client

INPUTS_COMMON_LINK_debug = $(addsuffix .dlb_http_client_windows_amd64_gnu_debug.o,$(COMMON_FILES))
INPUTS_COMMON_LINK_release = $(addsuffix .dlb_http_client_windows_amd64_gnu_release.o,$(COMMON_FILES))

DEP_PROJ_LINK_debug = $(BASE)dlb_socket/0.1/dlb_socket/make/dlb_socket/windows_amd64_gnu/dlb_socket_debug.lib
DEP_PROJ_LINK_release = $(BASE)dlb_socket/0.1/dlb_socket/make/dlb_socket/windows_amd64_gnu/dlb_socket_release.lib

default: dlb_http_client_debug.exe dlb_http_client_release.exe

dlb_http_client_debug.exe: $(INPUTS_COMMON_LINK_debug) $(DEP_PROJ_LINK_debug)
	$(LD) $(LDFLAGS_debug) -o $@ $+ 

dlb_http_client_release.exe: $(INPUTS_COMMON_LINK_release) $(DEP_PROJ_LINK_release)
	$(LD) $(LDFLAGS_release) -o $@ $+ 

$(TMP)%.dlb_http_client_windows_amd64_gnu_debug.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -MM $(DEFINES_debug) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_debug) $(DEFINES_debug) $(INCLUDES) -o $@ $<

$(TMP)%.dlb_http_client_windows_amd64_gnu_release.o: $(BASE)%.c | $(TMP)%/..
	$(CCDEP) -MM $(DEFINES_release) $(INCLUDES) -MT $@ -o $(patsubst %.o,%.d,$@) $<
	$(CC) $(CFLAGS_release) $(DEFINES_release) $(INCLUDES) -o $@ $<

CLEAN_COMMON_COMPILE_debug = $(addsuffix .dlb_http_client_windows_amd64_gnu_debug.d,$(COMMON_FILES))
CLEAN_COMMON_COMPILE_release = $(addsuffix .dlb_http_client_windows_amd64_gnu_release.d,$(COMMON_FILES))

cleandeps:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)

clean: cleanself
	$(MAKE) -C $(BASE)dlb_socket/0.1/dlb_socket/make/dlb_socket/windows_amd64_gnu cleanself

cleanself:
	$(RM) $(CLEAN_COMMON_COMPILE_debug)
	$(RM) $(CLEAN_COMMON_COMPILE_release)
	$(RM) $(INPUTS_COMMON_LINK_debug)
	$(RM) $(INPUTS_COMMON_LINK_release)
	$(RM) dlb_http_client_debug.exe
	$(RM) dlb_http_client_release.exe

.PHONY: default FORCE clean cleandeps cleanself help

FORCE:

help:
	@echo "This makefile has the following targets:"
	@echo " dlb_http_client_debug.exe"
	@echo " dlb_http_client_release.exe"
	@echo " default"
	@echo " clean"
	@echo " cleandeps"
	@echo " help"

include $(wildcard $(TMP)dlb_socket/0.1/dlb_socket/frontend/client.dlb_http_client_windows_amd64_gnu_debug.d)
include $(wildcard $(TMP)dlb_socket/0.1/dlb_socket/frontend/client.dlb_http_client_windows_amd64_gnu_release.d)

$(BASE)dlb_socket/0.1/dlb_socket/make/dlb_socket/windows_amd64_gnu/dlb_socket_debug.lib \
$(BASE)dlb_socket/0.1/dlb_socket/make/dlb_socket/windows_amd64_gnu/dlb_socket_release.lib: %.lib: FORCE
	$(MAKE) -C $(@D) $(@F)


