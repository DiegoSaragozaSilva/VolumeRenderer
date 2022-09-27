#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <spdlog/spdlog.h>

#include "src/render_engine/Window.hpp"

int main() {
    Window* window = new Window(640, 320, "New window"); 

    return 1;
}
