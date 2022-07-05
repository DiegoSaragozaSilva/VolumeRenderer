INC_DIR = include
CFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi -I$(INC_DIR)
SRC = $(wildcard src/*/*.cpp)

VulkanTest: main.cpp
	g++ $(CFLAGS) -o VulkanTest main.cpp $(SRC) $(LDFLAGS)

.PHONY: test clean

test: VulkanTest
	./VulkanTest

clean:
	rm -f VulkanTest
