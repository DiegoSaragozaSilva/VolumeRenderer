#pragma once

#include <vulkan/vulkan.h>
#include <spdlog/spdlog.h>

struct VulkanSemaphoreCreateInfo {
    VkDevice device;
};

class VulkanSemaphore {
public:
    VulkanSemaphore(VulkanSemaphoreCreateInfo* info);
    ~VulkanSemaphore();

    VkSemaphore getSemaphore();
private:
    VkSemaphore semaphore;
};
