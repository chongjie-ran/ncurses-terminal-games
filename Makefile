# Minesweeper with Raylib - Makefile
# Requires: raylib (brew install raylib on macOS)

CC = gcc
CFLAGS = -Wall -Wextra -O2 -I$(RAYLIB_PATH)/include
LDFLAGS = -L$(RAYLIB_PATH)/lib -lraylib -lm
RAYLIB_PATH ?= /opt/homebrew

TARGET = minesweeper_raylib
SRC = src/main.c src/game.c src/draw.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(SRC) -o $@ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
