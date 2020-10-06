####################################################################################################
##
## CONFIGURATION
##
####################################################################################################

## Compilation parameters

CC = gcc
CXX = g++
CXXFLAGS := `sdl-config --cflags` -I./src -Wall -g
## Other CXXFLAGS: (also defined in src/Settings.h)
## -D__DISABLE_RATE_MONITOR__	<== don't monitor trasnfer: faster transfer but no statistics
## -D__COMPRESSED_MESSAGES__	<== update messages from server to clients are compressed with zlib
## -D__MESSAGE_BUFFER__		<== each thread stores messages in a private buffer and periodicaly
## 				transfers them to the main queue
## -D__SERVER_PLAYER_RATIO__	<== each player has a coefficient depending on its speed (works well
##				only if the server has one regular update thread)
## -D__USE_3DS_GRAPHICS__	<== use 3ds models instead of vrml for client GUI
LDFLAGS := `sdl-config --libs` -lSDL_net -lm
## -lz is also needed when compiling with -D__COMPRESSED_MESSAGES__
LDFLAGS_CLIENT = $(LDFLAGS) -lGL -lGLU
LDFLAGS_SERVER = $(LDFLAGS)
LDFLAGS_MASTER = $(LDFLAGS)

## Source and binary files

BUILD_DIR = build
SOURCE_DIR = src

SHARED_SOURCES   = $(wildcard $(SOURCE_DIR)/*.cpp) 	     $(wildcard $(SOURCE_DIR)/utils/*.cpp)
GAME_SOURCES_CPP = $(wildcard $(SOURCE_DIR)/game/*.cpp)
GAME_SOURCES_C   = $(wildcard $(SOURCE_DIR)/game/*.c)
COMM_SOURCES     = $(wildcard $(SOURCE_DIR)/comm/*.cpp)
GRAPHICS_SOURCES = $(wildcard $(SOURCE_DIR)/graphics/*.cpp) \
	$(wildcard $(SOURCE_DIR)/graphics/vrml/*.cpp) \
	$(wildcard $(SOURCE_DIR)/graphics/3ds/*.cpp) \
	$(wildcard $(SOURCE_DIR)/graphics/texture/*.cpp) \
	$(wildcard $(SOURCE_DIR)/graphics/font/*.cpp)

## SERVER
SERVER_DIR  = $(SOURCE_DIR)/server
SERVER_SRCS_CPP = $(SHARED_SOURCES) $(GAME_SOURCES_CPP) $(COMM_SOURCES) $(wildcard $(SERVER_DIR)/*.cpp)
SERVER_SRCS_C   = $(GAME_SOURCES_C)
SERVER_OBJS = $(patsubst $(SOURCE_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SERVER_SRCS_CPP)) \
              $(patsubst $(SOURCE_DIR)/%.c,$(BUILD_DIR)/%.o,$(SERVER_SRCS_C))

## MASTER
MASTER_DIR  = $(SOURCE_DIR)/master
MASTER_SRCS = $(wildcard $(MASTER_DIR)/*.cpp) $(SHARED_SOURCES) $(GAME_SOURCES) $(COMM_SOURCES)
MASTER_OBJS = $(patsubst $(SOURCE_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(MASTER_SRCS))

## CLIENT
CLIENT_DIR  = $(SOURCE_DIR)/client
CLIENT_SRCS_CPP = $(SHARED_SOURCES) $(GRAPHICS_SOURCES) $(GAME_SOURCES_CPP) $(COMM_SOURCES) \
				$(wildcard $(CLIENT_DIR)/*.cpp) $(wildcard $(CLIENT_DIR)/AStar/*.cpp) 	
CLIENT_SRCS_C   = $(GAME_SOURCES_C)
CLIENT_OBJS = $(patsubst $(SOURCE_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(CLIENT_SRCS_CPP))	\
              $(patsubst $(SOURCE_DIR)/%.c,$(BUILD_DIR)/%.o,$(CLIENT_SRCS_C))

SERVER_BINARY_NAME = server
CLIENT_BINARY_NAME = client
MASTER_BINARY_NAME = master

## Parameters for running

MASTER_CONFIG_FILE = config_default.ini
HOST_NAME := `uname -n`

####################################################################################################
##
## BUILD
##
####################################################################################################

all: client server 

## Create directories for object files
$(BUILD_DIR):
	@echo "Creating directories for the object files"
	@mkdir $(BUILD_DIR)
	@mkdir $(BUILD_DIR)/utils
	@mkdir $(BUILD_DIR)/server
	@mkdir $(BUILD_DIR)/client
	@mkdir $(BUILD_DIR)/client/AStar
	@mkdir $(BUILD_DIR)/comm
	@mkdir $(BUILD_DIR)/game
	@mkdir $(BUILD_DIR)/graphics
	@mkdir $(BUILD_DIR)/graphics/vrml
	@mkdir $(BUILD_DIR)/graphics/3ds
	@mkdir $(BUILD_DIR)/graphics/texture
	@mkdir $(BUILD_DIR)/graphics/font
	@mkdir $(BUILD_DIR)/master

## Build client and server

graphics: $(patsubst $(SOURCE_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(GRAPHICS_SOURCES))
game: $(patsubst $(SOURCE_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(GAME_SOURCES))
shared: $(patsubst $(SOURCE_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SHARED_SOURCES))
comm: $(patsubst $(SOURCE_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(COMM_SOURCES))
client_only: $(CLIENT_OBJS)
server_only: $(SERVER_OBJS)
master_only: $(MASTER_OBJS)

client: $(BUILD_DIR) $(CLIENT_OBJS)
	$(CXX) $(LDFLAGS_CLIENT) -o $(CLIENT_BINARY_NAME) $(CLIENT_OBJS)
server: $(BUILD_DIR) $(SERVER_OBJS)
	$(CXX) $(LDFLAGS_SERVER) -o $(SERVER_BINARY_NAME) $(SERVER_OBJS)
master: $(BUILD_DIR) $(MASTER_OBJS)
	$(CXX) $(LDFLAGS_MASTER) -o $(MASTER_BINARY_NAME) $(MASTER_OBJS)

## Target for generic cpp files
$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

## Target for generic c files
$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	$(CC) -c $(CXXFLAGS) -o $@ $<


## Call another makefile to build the game monitor
gamemonitor:
	make -C src/gamemonitor

## Clean
.PHONY: clean
clean:
	-rm -f -R $(BUILD_DIR)
	-rm -f $(SERVER_BINARY_NAME) $(CLIENT_BINARY_NAME) $(MASTER_BINARY_NAME)
	-rm -f core*
	-rm -f /tmp/master.log
clean_all: clean
	-make -C src/gamemonitor clean

####################################################################################################
##
## RUN
##
####################################################################################################

## run
run_master: master
	./$(MASTER_BINARY_NAME) $(MASTER_CONFIG_FILE) 7878 /tmp/master.log
run_server: run_server1
run_server1: server
	./$(SERVER_BINARY_NAME) $(HOST_NAME):7878 $(HOST_NAME):3000
run_server2: server
	./$(SERVER_BINARY_NAME) $(HOST_NAME):7878 $(HOST_NAME):3001
run_server3: server
	./$(SERVER_BINARY_NAME) $(HOST_NAME):7878 $(HOST_NAME):3002
run_server4: server
	./$(SERVER_BINARY_NAME) $(HOST_NAME):7878 $(HOST_NAME):3003

run_10:
	for (( i=1; i<=10; i++ )); do ./$(CLIENT_BINARY_NAME) $(HOST_NAME):3000 > /dev/null & echo -n $(i)" "; done
run_100:
	for (( i=1; i<=100; i++ )); do ./$(CLIENT_BINARY_NAME) $(HOST_NAME):3000 > /dev/null & echo -n $(i)" "; done
run_200:
	for (( i=1; i<=200; i++ )); do ./$(CLIENT_BINARY_NAME) $(HOST_NAME):3000 > /dev/null & echo -n $(i)" "; done
run_400:
	for (( i=1; i<=400; i++ )); do ./$(CLIENT_BINARY_NAME) $(HOST_NAME):3000 > /dev/null & echo -n $(i)" "; done
run_1000:
	for (( i=1; i<=1000; i++ )); do ./$(CLIENT_BINARY_NAME) $(HOST_NAME):3000 > /dev/null & echo -n $(i)" "; done

run_client: client
	./$(CLIENT_BINARY_NAME) $(HOST_NAME):3000
run_client_gui: client
	./$(CLIENT_BINARY_NAME) --gui $(HOST_NAME):3000

## client
## --gui --nogui --size=640x480 --bpp=32 --fullscreen --debug-AI --disconnect_timeout=5
## --console-fps --fps-average-param=0.99
