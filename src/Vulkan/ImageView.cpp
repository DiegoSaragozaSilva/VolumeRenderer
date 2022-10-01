#include "ImageView.hpp"

ImageView::ImageView(vk::Device* logicalDevice, vk::Image image, vk::ImageViewType type, vk::Format format, vk::ImageAspectFlags aspectFlags, uint32_t mipmapLevel) {
    this->image = image;
    this->type = type;
    this->format = format;
    this->mipmapLevel = mipmapLevel;

    // Subresource info
    vk::ImageSubresourceRange subresourceRangeInfo(aspectFlags,
                                                   0,
                                                   mipmapLevel,
                                                   0,
                                                   1);

    // Image view creation
    vk::ImageViewCreateInfo imageViewCreateInfo(vk::ImageViewCreateFlags(),
                                                image,
                                                type,
                                                format,
                                                vk::ComponentMapping(),
                                                subresourceRangeInfo);

    imageView = logicalDevice->createImageView(imageViewCreateInfo);

    #ifndef NDEBUG
        std::string imageViewInfo = "Type: " + std::to_string((int)type) + " | " +
                                    "Format: " + std::to_string((int)format) + " | " +
                                    "Mipmap level: " + std::to_string(mipmapLevel);
        spdlog::info("Vulkan Image view successfully created. " + imageViewInfo);
    #endif
}

ImageView::~ImageView() {
    #ifndef NDEBUG
        spdlog::info("Vulkan image view successfully destroyed.");
    #endif
}

vk::ImageView* ImageView::getImageView() {
    return &imageView;
}
