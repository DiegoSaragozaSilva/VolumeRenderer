#pragma once

#include <vulkan/vulkan.h>
#include <spdlog/spdlog.h>

struct VulkanFenceCreateInfo {
    VkDevice device;
};

class VulkanFence {
public:
    VulkanFence(VulkanFenceCreateInfo* info);
    ~VulkanFence();

    VkFence getFence();
private:
    VkFence fence;
};
