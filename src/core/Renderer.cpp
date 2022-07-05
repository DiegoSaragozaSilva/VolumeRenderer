#include "Renderer.hpp"

Renderer::Renderer(RendererCreateInfo* info) {
    // GLFW initialization
    initGLFW(info->windowWidth, info->windowHeight, info->windowName);

    // Vulkan instance creation
    spdlog::info("Creating vulkan instance...");
    VulkanInstanceCreateInfo vulkanInstanceInfo {};
    vulkanInstanceInfo.enableValidationLayers = info->enableValidationLayers;
    vulkanInstanceInfo.validationLayers = validationLayers;
    vulkanInstanceInfo.extensions = glfwGetRequiredInstanceExtensions(&vulkanInstanceInfo.extensionCount);
    vulkanInstanceInfo.applicationName = "Volume Renderer";
    vulkanInstanceInfo.engineName = "Volume engine";
    vulkanInstance = new VulkanInstance(&vulkanInstanceInfo);
    spdlog::info("Vulkan instance successfully created!");

    // Vulkan surface creation
    spdlog::info("Creating vulkan surface...");
    VulkanSurfaceCreateInfo vulkanSurfaceInfo {};
    vulkanSurfaceInfo.instance = vulkanInstance->getInstance();
    vulkanSurfaceInfo.window = window;
    vulkanSurface = new VulkanSurface(&vulkanSurfaceInfo);
    spdlog::info("Vulkan surface successfully created!");

    // Vulkan physical device picking and logical device creation
    spdlog::info("Picking physical device and logical device creation...");
    VulkanDeviceCreateInfo vulkanDeviceInfo {};
    vulkanDeviceInfo.instance = vulkanInstance->getInstance();
    vulkanDeviceInfo.surface = vulkanSurface->getSurface();
    vulkanDeviceInfo.requiredDeviceExtensions = deviceExtensions;
    vulkanDeviceInfo.enableValidationLayers = info->enableValidationLayers;
    vulkanDeviceInfo.validationLayers = validationLayers;
    vulkanDevice = new VulkanDevice(&vulkanDeviceInfo);
    spdlog::info("Physical and logical device successfully created!");
}

Renderer::~Renderer() {
    
}

void Renderer::initGLFW(uint32_t windowWidth, uint32_t windowHeight, std::string windowName) {
    if (!glfwInit()) {
        spdlog::error("GLFW could not be initialized!");
        throw 0;
    }

    // Disable window resize
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // Disable default OpenGL context creation
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(windowWidth, windowHeight, windowName.c_str(), nullptr, nullptr);
    if (!window) {
        spdlog::error("Failed to create GLFW window!");
        throw 0;
    }
}

