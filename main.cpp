#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>

#include "src/render_engine/RenderEngine.hpp"

int main() {
    RenderEngine* renderEngine = new RenderEngine();
    delete renderEngine;

    return 1;
}
