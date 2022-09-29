#include "RenderEngine.hpp"

RenderEngine::RenderEngine() {
    // Initializing all basic componentes and Vulkan
    initWindow();
    initVulkan();
}

RenderEngine::~RenderEngine() {
    // Swapchain destruction
    vulkan.device->getLogicalDevice()->destroySwapchainKHR(*vulkan.swapchain->getSwapchain());
    delete vulkan.swapchain;

    // Window destruction
    vulkan.instance->getInstance()->destroySurfaceKHR(*window->getSurface(vulkan.instance->getInstance()));
    delete window;

    // Device destruction
    delete vulkan.device;

    // Instance destruction
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

    // Vulkan swapchain initialization
    vulkan.swapchain = new Swapchain(vulkan.device, window->getSurface(vulkan.instance->getInstance()), window->getWidth(), window->getHeight());
}
