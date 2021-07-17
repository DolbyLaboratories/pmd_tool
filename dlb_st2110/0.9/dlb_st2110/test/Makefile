SRC_DIR := ../src
OBJ_DIR := obj
BIN_DIR := bin

EXES := $(BIN_DIR)/dlb_aoip_discovery_main $(BIN_DIR)/dlb_st2110_player_main $(BIN_DIR)/dlb_st2110_mixer_main $(BIN_DIR)/dlb_st2110_recorder_main

SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

APP_SRC := $(wildcard *.cpp)
APP_OBJ := $(APP_SRC:%.cpp=$(OBJ_DIR)/%.o)

CPPFLAGS := -I../include -I/usr/include/mellanox/ -DELPP_NO_DEFAULT_LOG_FILE -std=c++17 -g
CFLAGS   := -Wall
LDFLAGS  := -g -L../src -Bstatic  
LDLIBS   := -lm -lsndfile -lrivermax -lstdc++ -lavahi-client -lavahi-common -lm -pthread

.PHONY: all clean

all: $(BIN_DIR) $(OBJ_DIR) $(EXES)

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

$(EXES): $(APP_OBJ) $(OBJ)
	$(CXX) $(LDFLAGS) $(OBJ) $(@:$(BIN_DIR)/%=$(OBJ_DIR)/%.o) $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR)

#-include $(OBJ:.o=.d)
