#include "Image.hpp"

Image::Image(Device* device, CommandPool* commandPool, uint32_t width, uint32_t height, uint32_t mipmapLevels, vk::SampleCountFlagBits sampleCount, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usageFlags, vk::MemoryPropertyFlags memoryFlags, vk::ImageLayout oldLayout, vk::ImageLayout newLayout) {
    this->width = width;
    this->height = height;
    this->mipmapLevels = mipmapLevels;
    this->format = format;

    // Image extent [TODO]: Enable 3D images
    vk::Extent3D extent (
        width,
        height,
        1
    );

    // Image creation
    vk::ImageCreateInfo imageCreateInfo (
        vk::ImageCreateFlags(),
        vk::ImageType::e2D,
        format,
        extent,
        mipmapLevels,
        1,
        sampleCount,
        tiling,
        usageFlags,
        vk::SharingMode::eExclusive,
        0,
        nullptr,
        vk::ImageLayout::eUndefined
    );

    image = device->getLogicalDevice()->createImage(imageCreateInfo);

    // Allocate image memory and bind it
    imageMemory = allocateImageMemory(device, memoryFlags);

    // Transition between the old and new layout via command buffer
    transitionLayout(device, commandPool, oldLayout, newLayout);

    #ifndef NDEBUG
        std::string imageInfo = "Extent: (" + std::to_string(width) + ", " + std::to_string(height) + ") | " +
                                "Mip mapping level: " + std::to_string(mipmapLevels) + " | " +
                                "Sample count: " + std::to_string((int)sampleCount);
        spdlog::info("Vulkan image successfully created. " + imageInfo);
    #endif
}

Image::~Image() {
    #ifndef NDEBUG
        spdlog::info("Vulkan image successfully destroyed.");
    #endif
}

vk::DeviceMemory Image::allocateImageMemory(Device* device, vk::MemoryPropertyFlags memoryFlags) {
    // Find the memory requirements for this image
    vk::MemoryRequirements memoryRequirements = device->getLogicalDevice()->getImageMemoryRequirements(image);

    // Query the physical device memory type index for this image
    uint32_t memoryTypeIndex = device->getMemoryTypeIndex(memoryRequirements.memoryTypeBits, memoryFlags);

    // Memory allocate info
    vk::MemoryAllocateInfo memoryAllocateInfo (
        memoryRequirements.size,
        memoryTypeIndex
    );

    // Request the device memory and bind this allocated memory to the image
    vk::DeviceMemory deviceMemory = device->getLogicalDevice()->allocateMemory(memoryAllocateInfo);
    device->getLogicalDevice()->bindImageMemory(image, deviceMemory, 0);

    std::stringstream addressStream;
    addressStream << &deviceMemory;
    spdlog::info("Image device memory (" + addressStream.str() + ") allocated.");

    return deviceMemory;
}

void Image::transitionLayout(Device* device, CommandPool* commandPool, vk::ImageLayout oldLayout, vk::ImageLayout newLayout) {
    // Create image barrier based on old and new layout
    vk::ImageMemoryBarrier barrier;
    barrier.image = image;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = mipmapLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    // Stage: Undefined -> Stage: Color attachment optimal
    if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eColorAttachmentOptimal) {
        barrier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
        return applyTransitionLayoutCommand(device, commandPool, vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eColorAttachmentOutput, barrier);        
    }

    // Stage: Undefined -> Stage: Depth stencil attachment optimal
    else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
        barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
        barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;        
        return applyTransitionLayoutCommand(device, commandPool, vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eEarlyFragmentTests, barrier);        
    }

    // Unknow transition from old to new layout
    spdlog::error("Unsupported image transition from old to new layout");
    throw 0;
}

void Image::applyTransitionLayoutCommand(Device* device, CommandPool* commandPool, vk::PipelineStageFlags sourceStageFlags, vk::PipelineStageFlags destinationStageFlags, vk::ImageMemoryBarrier barrier) {
    // Get a new command buffer from command pool
    vk::CommandBuffer commandBuffer = commandPool->beginCommandBuffer(*device->getLogicalDevice());

    // Start a pipeline barrier command based on the image barrier and the destination and source flags
    commandBuffer.pipelineBarrier(sourceStageFlags, destinationStageFlags, vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &barrier);

    // End the command buffer
    commandPool->endCommandBuffer(commandBuffer, device); 
}

uint32_t Image::getWidth() {
    return width;
}

uint32_t Image::getHeight() {
    return height;
}

uint32_t Image::getMipmapLevels() {
    return mipmapLevels;
}

vk::Format Image::getFormat() {
    return format;
}

vk::Image Image::getImage() {
    return image;
}

vk::DeviceMemory Image::getImageMemory() {
    return imageMemory;
}

