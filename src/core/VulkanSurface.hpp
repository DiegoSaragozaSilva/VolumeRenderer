#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>

struct VulkanSurfaceCreateInfo {
    VkInstance instance;
    GLFWwindow* window;
};

class VulkanSurface {
public:
    VulkanSurface(VulkanSurfaceCreateInfo* info);
    ~VulkanSurface();

    VkSurfaceKHR getSurface();
private:
    VkSurfaceKHR surface;
};
