CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LDFLAGS = -lncurses

SRCS = src/board.cpp src/solver.cpp src/main.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = huarong-dao

all: $(TARGET) test_console

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

test_console: src/board.cpp src/solver.cpp src/test.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(TARGET) test_console src/*.o

run: $(TARGET)
	./$(TARGET)

test: test_console
	./test_console

.PHONY: all clean run test
