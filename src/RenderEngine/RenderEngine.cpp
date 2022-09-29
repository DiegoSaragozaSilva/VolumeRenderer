#include "RenderEngine.hpp"

RenderEngine::RenderEngine() {
    // Initializing all basic componentes and Vulkan
    initWindow();
    initVulkan();
}

RenderEngine::~RenderEngine() {
    // Clean up
    vulkan.instance->getInstance()->destroySurfaceKHR(*window->getSurface(vulkan.instance->getInstance()));
    delete window;
    delete vulkan.device;
    delete vulkan.instance;
}

void RenderEngine::initWindow() {
    // Window and GLFW initialization
    window = new Window(800, 600, "Render Engine");
}

void RenderEngine::initVulkan() {
    // Vulkan instance initialization
    vulkan.instance = new Instance("Render Engine", "Render Engine", window->getGLFWExtensions());    

    // Vulkan device initialization
    vulkan.device = new Device(vulkan.instance->getInstance(), window->getSurface(vulkan.instance->getInstance()));
}
