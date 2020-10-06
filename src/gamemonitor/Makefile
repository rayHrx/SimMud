CXX = g++
CFLAGS := `wx-config --cxxflags` `sdl-config --cflags` -I../../src -g -Wall
CXXFLAGS = $(CFLAGS)
LDFLAGS = -L ~/lib/lib
LIBS := `sdl-config --libs` -lSDL_net `wx-config --libs` 
EXE = gamemonitor

COMM_SOURCES = $(wildcard ../comm/*.cpp)
UTILS_SOURCES = $(wildcard ../utils/*.cpp)
#SRCS = RegionWindow.cpp main.cpp ParsePacket.cpp $(COMM_SOURCES) $(UTILS_SOURCES)
SRCS = main.cpp $(COMM_SOURCES) $(UTILS_SOURCES)
OBJS := $(patsubst %.cpp,%.o,$(SRCS))

all: $(EXE)

$(EXE): $(OBJS)
	$(CXX) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

run:
	./$(EXE) master.log

clean:
	rm -f $(OBJS) $(EXE)
	rm -f core*
