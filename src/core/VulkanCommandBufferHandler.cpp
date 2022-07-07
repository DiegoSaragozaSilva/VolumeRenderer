#include "VulkanCommandBufferHandler.hpp"

VulkanCommandBufferHandler::VulkanCommandBufferHandler(VulkanCommandBufferHandlerCreateInfo* info) { 
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = info->queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(info->device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
        spdlog::info("Failed to create command pool!");
        throw 0;
    }

    commandBuffers.resize(info->numCommandBuffers);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = info->numCommandBuffers;

    if (vkAllocateCommandBuffers(info->device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        spdlog::info("Failed to create command buffers!");
        throw 0;
    }
}

VulkanCommandBufferHandler::~VulkanCommandBufferHandler() {}
