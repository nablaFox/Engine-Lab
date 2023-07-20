# tool
CXX = g++
CFLAGS = -std=c++17 -Wall -Wextra -Wpedantic
CFLAGS += -Ilib/glm -Ilib/glfw/include -Ilib/vkbootstrap -Ilib/vma/include
LDFLAGS = lib/glfw/src/libglfw3.a -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi
GLSLC = glslc

# paths
SRC_PATH = src
BIN_PATH = bin
SHADERS_PATH = shaders
DEMO_PATH = demos/general_demo

# files
SRC = $(wildcard $(SRC_PATH)/*.cpp) $(wildcard $(DEMO_PATH)/*.cpp) lib/vkbootstrap/VkBootstrap.cpp
OBJ = $(SRC:.cpp=.o)
BIN = $(BIN_PATH)/demo
SHADERS_VERT = $(wildcard $(SHADERS_PATH)/*.vert)
SHADERS_FRAG = $(wildcard $(SHADERS_PATH)/*.frag)
SHADERS_VERT_OBJ = $(patsubst %.vert, %.vert.spv, $(SHADERS_VERT))
SHADERS_FRAG_OBJ = $(patsubst %.frag, %.frag.spv, $(SHADERS_FRAG))
SHADERS = $(SHADERS_VERT_OBJ) $(SHADERS_FRAG_OBJ)

# rules
.PHONY: all clean

all: libs shaders demo

libs:
	cd lib/glm && cmake . && make
	cd lib/glfw && cmake . && make

demo: $(OBJ)
	mkdir -p $(BIN_PATH)
	$(CXX) -o $(BIN) $^ $(LDFLAGS)

shaders: $(SHADERS)

%.spv: %
	$(GLSLC) $< -o $@

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ) $(SHADERS) $(BIN)