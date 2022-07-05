#include "VulkanSurface.hpp"

VulkanSurface::VulkanSurface(VulkanSurfaceCreateInfo* info) {
    if (glfwCreateWindowSurface(info->instance, info->window, nullptr, &surface) != VK_SUCCESS) {
        spdlog::error("Failed to create vulkan surface from window!");
        throw 0;
    }
}

VulkanSurface::~VulkanSurface() {}

VkSurfaceKHR VulkanSurface::getSurface() {
    return surface;
}
