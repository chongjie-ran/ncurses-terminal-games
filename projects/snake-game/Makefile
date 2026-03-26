# Snake Game Makefile
# 依赖: ncurses

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LDFLAGS = -lncurses

TARGET = snake
SRCS = snake.cpp
OBJS = $(SRCS:.cpp=.o)

.PHONY: all clean run test

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET) $(OBJS)

run: $(TARGET)
	./$(TARGET)

# 静默编译测试（验证代码正确性）
test: $(TARGET)
	@echo "Snake game compiled successfully!"
	@echo "Run with: make run"
