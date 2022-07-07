#pragma once

#include <vulkan/vulkan.h>
#include <spdlog/spdlog.h>

// C++ crap
#include "VulkanDevice.hpp"

struct VulkanCommandBufferHandlerCreateInfo {
    VkDevice device;
    QueueFamilyIndices queueFamilyIndices;
    uint32_t numCommandBuffers;
};

class VulkanCommandBufferHandler {
public:
    VulkanCommandBufferHandler(VulkanCommandBufferHandlerCreateInfo* info);
    ~VulkanCommandBufferHandler();
private:
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;
};
