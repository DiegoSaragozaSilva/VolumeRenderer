#include "Image.hpp"

Image::Image(Device* device, CommandPool* commandPool, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipmapLevels, vk::SampleCountFlagBits sampleCount, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usageFlags, vk::MemoryPropertyFlags memoryFlags, vk::ImageLayout oldLayout, vk::ImageLayout newLayout) {
    this->width = width;
    this->height = height;
    this->depth = depth;
    this->type = depth == 0 ? vk::ImageType::e1D : (depth == 1 ? vk::ImageType::e2D : vk::ImageType::e3D);
    this->mipmapLevels = mipmapLevels;
    this->format = format;

    // Image extent
    vk::Extent3D extent (
        width,
        height,
        depth
    );

    // Image creation
    vk::ImageCreateInfo imageCreateInfo (
        vk::ImageCreateFlags(),
        type,
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
        std::string imageInfo = "Extent: (" + std::to_string(width) + ", " + std::to_string(height) + ", " + std::to_string(depth) + ") | " +
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

    // Stage: Undefined -> Stage: Transfer destination optimal
    else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal) {
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
        return applyTransitionLayoutCommand(device, commandPool, vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer, barrier);        
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

void Image::generateMipmaps(Device* device, CommandPool* commandPool) {
    // Image subresource range
    vk::ImageSubresourceRange barrierSubresourceRange (
        vk::ImageAspectFlagBits::eColor, 
        0,                               
        1,                               
        0,                               
        1
    );

    // Image memory barries
    vk::ImageMemoryBarrier barrier (
        vk::AccessFlags(),           
        vk::AccessFlags(),           
        vk::ImageLayout::eUndefined, 
        vk::ImageLayout::eUndefined, 
        VK_QUEUE_FAMILY_IGNORED,     
        VK_QUEUE_FAMILY_IGNORED,     
        image,            
        barrierSubresourceRange
    );

    // Begin a new command buffer to transition to shader readining
    vk::CommandBuffer commandBuffer = commandPool->beginCommandBuffer(*(device->getLogicalDevice()));

    // Generate the mipmap level images
    int mipWidth = width;
    int mipHeight = height;
    int mipLevels = mipmapLevels;
    for (uint32_t mipLevel = 1; mipLevel < mipLevels; mipLevel++)
    {
        barrier.subresourceRange.baseMipLevel = mipLevel - 1;
        barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
        barrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

        commandBuffer.pipelineBarrier(
            vk::PipelineStageFlagBits::eTransfer,
            vk::PipelineStageFlagBits::eTransfer,
            vk::DependencyFlags(),
            0,
            nullptr,
            0,
            nullptr,
            1,
            &barrier
        );

        vk::ImageSubresourceLayers sourceSubresource(
            vk::ImageAspectFlagBits::eColor, 
            mipLevel - 1,                    
            0,                               
            1
        );

        std::array<vk::Offset3D, 2> sourceOffsets = {
            vk::Offset3D(0, 0, 0),
            vk::Offset3D(mipWidth, mipHeight, 1)
        };

        vk::ImageSubresourceLayers destinationSubresource(
            vk::ImageAspectFlagBits::eColor, 
            mipLevel,                        
            0,                               
            1
        );

        std::array<vk::Offset3D, 2> destinationOffsets = {
            vk::Offset3D(0, 0, 0),
            vk::Offset3D(mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1)
        };

        vk::ImageBlit blit(
            sourceSubresource,      
            sourceOffsets,          
            destinationSubresource, 
            destinationOffsets
        );

        commandBuffer.blitImage(
            image, vk::ImageLayout::eTransferSrcOptimal,
            image, vk::ImageLayout::eTransferDstOptimal,
            1, &blit,
            vk::Filter::eLinear
        );

        barrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
        barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        commandBuffer.pipelineBarrier(
            vk::PipelineStageFlagBits::eTransfer,
            vk::PipelineStageFlagBits::eFragmentShader,
            vk::DependencyFlags(),
            0,
            nullptr,
            0,
            nullptr,
            1,
            &barrier
        );

        if (mipWidth > 1)
            mipWidth /= 2;
        if (mipHeight > 1)
            mipHeight /= 2;
    }

    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
    barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
    barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

    commandBuffer.pipelineBarrier(
        vk::PipelineStageFlagBits::eTransfer,
        vk::PipelineStageFlagBits::eFragmentShader,
        vk::DependencyFlags(),
        0,
        nullptr,
        0,
        nullptr,
        1,
        &barrier
    );

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

vk::ImageType Image::getImageType() {
    return type;
}
