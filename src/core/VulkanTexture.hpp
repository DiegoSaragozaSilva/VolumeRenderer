#pragma once

#include <vulkan/vulkan.h>

#include "VulkanBuffer.hpp"

struct VulkanTextureCreateInfo {
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    VkImageType imageType;
    VkImageViewType imageViewType;
    uint32_t width, height, depth;
    void* data;
    VkDeviceSize imageSize;
    VkFormat imageFormat;
    VkImageUsageFlags usage;
};

class VulkanTexture {
public:
    VulkanTexture(VulkanTextureCreateInfo* info);
    ~VulkanTexture();

    VkImageView getImageView();
    VkSampler getSampler();
    VkImage getTextureImage();
    VkBuffer getBuffer();
private:
    VulkanBuffer* vulkanBuffer;
    VkImage textureImage;
    VkSampler textureSampler;
    VkImageView textureImageView;
    VkDeviceMemory textureImageMemory;
};
