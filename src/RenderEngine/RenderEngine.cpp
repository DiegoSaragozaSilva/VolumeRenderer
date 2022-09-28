#include "RenderEngine.hpp"

RenderEngine::RenderEngine() {
    // Initializing all basic componentes and Vulkan
    initWindow();
    initVulkan();
}

RenderEngine::~RenderEngine() {
    // Clean up
    delete window;
    delete instance;
}

void RenderEngine::initWindow() {
    // Window and GLFW initialization
    window = new Window(800, 600, "Render Engine");
}

void RenderEngine::initVulkan() {
    // Vulkan instance initialization
    instance = new Instance("Render Engine", "Render Engine", window->getGLFWExtensions());    
}
