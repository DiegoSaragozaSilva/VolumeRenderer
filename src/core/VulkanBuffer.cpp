#include "VulkanBuffer.hpp"

VulkanBuffer::VulkanBuffer(VulkanBufferCreateInfo* info) {
    bufferSize = info->bufferSize;

    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = info->bufferSize;
    bufferInfo.usage = info->usageFlags;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(info->device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        spdlog::error("Failed to create vulkan buffer!");
        throw 0;
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(info->device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findBufferMemoryType(info->physicalDevice, memRequirements.memoryTypeBits, info->propertyFlags);

    if (vkAllocateMemory(info->device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        spdlog::error("Failed to allocate memory for vulkan buffer!");
        throw 0;
    }

    vkBindBufferMemory(info->device, buffer, bufferMemory, 0);
}

uint32_t VulkanBuffer::findBufferMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags propertyFlags) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & propertyFlags) == propertyFlags) {
            return i;
        }
    }
    
    spdlog::error("Failed to find suitable buffer memory type!");
    throw 0;
}

void VulkanBuffer::fillBuffer(VkDevice device, void* data) {
    void* _data;
    vkMapMemory(device, bufferMemory, 0, bufferSize, 0, &_data);
    memcpy(_data, data, bufferSize);
    vkUnmapMemory(device, bufferMemory);
}

VkBuffer VulkanBuffer::getBuffer() {
    return buffer;
}

VkDeviceSize VulkanBuffer::getBufferSize() {
    return bufferSize;
}

void VulkanBuffer::freeBuffer(VkDevice device) {
    vkDestroyBuffer(device, buffer, nullptr);
    vkFreeMemory(device, bufferMemory, nullptr);  
}

