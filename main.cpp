#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>

#include "src/RenderEngine/RenderEngine.hpp"

int main() {
    RenderEngine* renderEngine = new RenderEngine();

    while (!renderEngine->windowShouldClose()) {
        renderEngine->renderFrame();
    }

    delete renderEngine;

    return 1;
}
