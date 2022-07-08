#pragma once

#include <vulkan/vulkan.h>
#include <spdlog/spdlog.h>
#include <cstring>

struct VulkanBufferCreateInfo {
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    VkDeviceSize bufferSize;
    VkBufferUsageFlags usageFlags;
    VkMemoryPropertyFlags propertyFlags;
};

class VulkanBuffer {
public:
    VulkanBuffer(VulkanBufferCreateInfo* info);
    ~VulkanBuffer();

    VkBuffer getBuffer();
    VkDeviceSize getBufferSize();
    void fillBuffer(VkDevice device, void* data);
private:
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    VkDeviceSize bufferSize;

    uint32_t findBufferMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags propertyFlags);
};
