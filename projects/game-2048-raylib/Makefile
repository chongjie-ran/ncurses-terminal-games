CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -I$(RAYLIB_PATH)/include
LDFLAGS = -L$(RAYLIB_PATH)/lib -lraylib -framework CoreVideo -framework Cocoa -lm

RAYLIB_PATH ?= /opt/homebrew
TARGET = game2048_raylib
SRC = src/main.cpp src/game2048.cpp

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET) src/*.o

run: all
	./$(TARGET)

.PHONY: all clean run
