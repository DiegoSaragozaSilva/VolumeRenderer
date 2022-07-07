#include "VulkanSemaphore.hpp"

VulkanSemaphore::VulkanSemaphore(VulkanSemaphoreCreateInfo* info) {
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (vkCreateSemaphore(info->device, &semaphoreInfo, nullptr, &semaphore) != VK_SUCCESS) {
        spdlog::error("Failed to create vulkan semaphore!");
        throw 0;
    }
}

VulkanSemaphore::~VulkanSemaphore() {}
