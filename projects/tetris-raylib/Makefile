CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
RAYLIB_HOME = /opt/homebrew
CXXINCLUDES = -I$(RAYLIB_HOME)/include

SRCS = tetris.cpp
TARGET = tetris

# macOS
RAYLIB_OPTS = $(RAYLIB_HOME)/lib/libraylib.a -framework CoreVideo -framework Cocoa -framework IOKit -lm

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(CXXINCLUDES) $(SRCS) $(RAYLIB_OPTS) -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all run clean
