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
    VkBuffer indexBuffer;
    uint32_t numIndices;
    VkRenderPass renderPass;
    VkFramebuffer framebuffer;
    VkExtent2D swapChainExtent;
    VkPipeline graphicsPipeline;
    VkPipelineLayout pipelineLayout;
    VkDescriptorSet descriptorSet;
};

class VulkanCommandBufferHandler {
public:
    VulkanCommandBufferHandler(VulkanCommandBufferHandlerCreateInfo* info);
    ~VulkanCommandBufferHandler();

    VkCommandBuffer getCommandBuffer(uint32_t bufferIndex);
    void recordCommandBuffer(CommandBufferRecordInfo* info);
    VkCommandBuffer beginSingleTimeCommands(VkDevice device);
    void endSingleTimeCommands(VkDevice device, VkQueue graphicsQueue, VkCommandBuffer commandBuffer);
private:
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;
};
