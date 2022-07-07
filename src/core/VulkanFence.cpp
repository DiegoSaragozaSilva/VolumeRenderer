#include "VulkanFence.hpp"

VulkanFence::VulkanFence(VulkanFenceCreateInfo* info) {
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateFence(info->device, &fenceInfo, nullptr, &fence) != VK_SUCCESS) {
        spdlog::info("Failed to create vulkan fence!");
        throw 0;
    }
}

VulkanFence::~VulkanFence() {}
