#include "VulkanTexture.hpp"

VulkanTexture::VulkanTexture(VulkanTextureCreateInfo* info) {
    VulkanBufferCreateInfo bufferInfo {};
    bufferInfo.device = info->device;
    bufferInfo.physicalDevice = info->physicalDevice;
    bufferInfo.bufferSize = info->imageSize;
    bufferInfo.usageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferInfo.propertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT; 
    vulkanBuffer = new VulkanBuffer(&bufferInfo);

    vulkanBuffer->fillBuffer(info->device, info->data); 

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = info->imageType;
    imageInfo.extent.width = info->width;
    imageInfo.extent.height = info->height;
    imageInfo.extent.depth = info->depth;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = info->imageFormat;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = info->usage;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.flags = 0; // Optional
    
    if (vkCreateImage(info->device, &imageInfo, nullptr, &textureImage) != VK_SUCCESS) {
        spdlog::error("Failed to create texture image!");
        throw 0;
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(info->device, textureImage, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = VulkanBuffer::findBufferMemoryType(info->physicalDevice, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (vkAllocateMemory(info->device, &allocInfo, nullptr, &textureImageMemory) != VK_SUCCESS) {
        spdlog::error("Failed to allocate memory for texture image!");
        throw 0;
    }

    vkBindImageMemory(info->device, textureImage, textureImageMemory, 0);

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = textureImage;
    viewInfo.viewType = info->imageViewType;
    viewInfo.format = info->imageFormat;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(info->device, &viewInfo, nullptr, &textureImageView) != VK_SUCCESS) {
        spdlog::error("Failed to create texture image view!");
        throw 0;
    }

    // [NOTE] Include sampler config in texture create info structure
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_NEAREST;
    samplerInfo.minFilter = VK_FILTER_NEAREST;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_FALSE; 
    samplerInfo.maxAnisotropy = 1.0f;
    
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    if (vkCreateSampler(info->device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
        spdlog::error("Failed to create texture sampler!");
        throw 0;
    }
}

VulkanTexture::~VulkanTexture() {}

VkImage VulkanTexture::getTextureImage() {
    return textureImage;
}

VkBuffer VulkanTexture::getBuffer() {
    return vulkanBuffer->getBuffer();
}

VkImageView VulkanTexture::getImageView() {
    return textureImageView;
}

VkSampler VulkanTexture::getSampler() {
    return textureSampler;
}
