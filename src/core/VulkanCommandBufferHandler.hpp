#pragma once

#include <vulkan/vulkan.h>
#include <spdlog/spdlog.h>
#include <iostream>

// C++ crap
#include "VulkanDevice.hpp"

struct VulkanCommandBufferHandlerCreateInfo {
    VkDevice device;
    QueueFamilyIndices queueFamilyIndices;
    uint32_t numCommandBuffers;
};

struct CommandBufferRecordInfo {
    uint32_t bufferIndex;
    std::vector<VkBuffer> vertexBuffers;
    VkRenderPass renderPass;
    VkFramebuffer framebuffer;
    VkExtent2D swapChainExtent;
    VkPipeline graphicsPipeline;
    VkPipelineLayout pipelineLayout;
    VkDescriptorSet descriptorSet;
    uint32_t numPrimitives;
};

class VulkanCommandBufferHandler {
public:
    VulkanCommandBufferHandler(VulkanCommandBufferHandlerCreateInfo* info);
    ~VulkanCommandBufferHandler();

    VkCommandBuffer getCommandBuffer(uint32_t bufferIndex);
    void recordCommandBuffer(CommandBufferRecordInfo* info);
private:
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;
};
