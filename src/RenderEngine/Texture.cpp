#include "Texture.hpp"

Texture::Texture(Device* device, CommandPool* commandPool, ImageData imageData) {
    this->name = imageData.name;

    // Image mipmap levels
    uint32_t mipmapLevels = std::floor(std::log2(std::max(imageData.width, imageData.height))) + 1;

    // Create the image GPU staging buffer
    Buffer* imageStagingBuffer = new Buffer(
        device,
        imageData.data.data(),
        imageData.data.size() * sizeof(uint8_t),
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
    );

    // Create the vulkan image
    image = new Image(
        device,
        commandPool,
        imageData.width,
        imageData.height,
        imageData.depth,
        1,
        vk::SampleCountFlagBits::e1,
        vk::Format::eR8G8B8A8Unorm,
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eSampled,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eTransferDstOptimal
    );

    // Create the image subresources
    vk::ImageSubresourceLayers imageSubresourceLayers (
        vk::ImageAspectFlagBits::eColor, 
        0,                               
        0,                               
        1
    );

    // Create the image extent
    vk::Extent3D imageExtent (
        imageData.width,
        imageData.height,
        imageData.depth
    );

    // Create the vulkan buffer image copy
    vk::BufferImageCopy bufferImageCopy (
        0,
        0,
        0,
        imageSubresourceLayers,
        vk::Offset3D(),
        imageExtent
    );

    // Create a new command buffer and copy the buffer to the image
    vk::CommandBuffer commandBuffer = commandPool->beginCommandBuffer(*(device->getLogicalDevice()));
    commandBuffer.copyBufferToImage(imageStagingBuffer->getBuffer(), image->getImage(), vk::ImageLayout::eTransferDstOptimal, 1, &bufferImageCopy);
    commandPool->endCommandBuffer(commandBuffer, device);

    // Create the image view
    imageView = new ImageView (  
        device->getLogicalDevice(),
        image->getImage(),
        image->getImageType() == vk::ImageType::e1D ? vk::ImageViewType::e1D : (image->getImageType() == vk::ImageType::e2D ? vk::ImageViewType::e2D : vk::ImageViewType::e3D),
        image->getFormat(),
        vk::ImageAspectFlagBits::eColor,
        image->getMipmapLevels()
    );

    // Create the sampler create info
    vk::SamplerCreateInfo samplerCreateInfo (
        vk::SamplerCreateFlags(),
        vk::Filter::eLinear,
        vk::Filter::eLinear,
        vk::SamplerMipmapMode::eLinear,   
        vk::SamplerAddressMode::eRepeat,  
        vk::SamplerAddressMode::eRepeat,  
        vk::SamplerAddressMode::eRepeat,  
        0.0f,                             
        false,                            
        1.0f,                             
        VK_FALSE,                         
        vk::CompareOp::eNever,            
        0.0f,                             
        image->getMipmapLevels(),         
        vk::BorderColor::eIntOpaqueBlack, 
        VK_FALSE                          
    );

    // Create the sampler
    sampler = device->getLogicalDevice()->createSampler(samplerCreateInfo);

    // Generate image mipmaps
    image->generateMipmaps(device, commandPool);

    #ifndef NDEBUG
        spdlog::info("Vulkan texture successfully created.");
    #endif
}

Texture::~Texture() {

}

Image* Texture::getImage() {
    return image;
}

ImageView* Texture::getImageView() {
    return imageView;
}

vk::Sampler Texture::getSampler() {
    return sampler;
}
