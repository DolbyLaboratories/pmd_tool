SRC_DIR := ../src
OBJ_DIR := obj
BIN_DIR := bin

EXES := $(BIN_DIR)/dlb_aoip_discovery_main $(BIN_DIR)/dlb_st2110_player_main $(BIN_DIR)/dlb_st2110_mixer_main $(BIN_DIR)/dlb_st2110_recorder_main

SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

APP_SRC := $(wildcard *.cpp)
APP_OBJ := $(APP_SRC:%.cpp=$(OBJ_DIR)/%.o)

CPPFLAGS := -I../include -I../../../../dlb_nmos_node/1.0/dlb_nmos_node/include -I/usr/include/mellanox/ -DELPP_NO_DEFAULT_LOG_FILE -std=c++17 -g
CFLAGS   := -Wall
LDFLAGS  := -m64
LDLIBS   := ../../../../dlb_pmd/make/dlb_st2110_lib/linux_amd64_gnu/dlb_st2110_lib_debug.a ../../../../dlb_nmos_node/1.0/dlb_nmos_node/lib/linux64/libdlb_nmos_node_lib.debug.a ../../../../zlib/1.2.11/make/zlib/linux_amd64_gnu/zlib_debug.a -lpthread -lavahi-client -lavahi-common -lrivermax -lpthread -ldl -lrt -lresolv -lstdc++fs -ldns_sd -lpangocairo-1.0 -lpango-1.0 -latk-1.0 -lcairo-gobject -lcairo -lgdk_pixbuf-2.0 -lgio-2.0 -lgobject-2.0 -lglib-2.0 -lsndfile -lm -ldl
#LDLIBS   := -lm -lsndfile -lstdc++ -lpthread -lavahi-client -lavahi-common -lrivermax -lpthread -ldl -lrt -lresolv -lstdc++fs -ldns_sd -lpangocairo-1.0 -lpango-1.0 -latk-1.0 -lcairo-gobject -lcairo -lgio-2.0 -lgobject-2.0 -lglib-2.0 -ldlb_nmos_node_lib.debug 

.PHONY: all clean

all: $(BIN_DIR) $(OBJ_DIR) $(EXES)

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

$(EXES): $(APP_OBJ) $(OBJ)
	$(CXX) $(LDFLAGS) -o $@ $(OBJ) $(@:$(BIN_DIR)/%=$(OBJ_DIR)/%.o) $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR)

#-include $(OBJ:.o=.d)
