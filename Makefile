# tool
CXX = g++
CFLAGS = -std=c++17 -Wall -Wextra -Wpedantic
CFLAGS += -Ilib/glm -Ilib/glfw/include
LDFLAGS = lib/glfw/src/libglfw3.a -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

# paths
SRC_PATH = src
BIN_PATH = bin

# files
SRC = $(wildcard $(SRC_PATH)/*.cpp)
OBJ = $(SRC:.cpp=.o)
BIN = $(BIN_PATH)/test

# rules
.PHONY: all clean

all: libs test

libs:
	cd lib/glm && cmake . && make
	cd lib/glfw && cmake . && make

test: $(OBJ)
	mkdir -p $(BIN_PATH)
	$(CXX) -o $(BIN) $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ)