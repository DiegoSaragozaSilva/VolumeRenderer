#ifndef _IMAGEVIEW_H_
#define _IMAGEVIEW_H_

#include <spdlog/spdlog.h>
#include <vulkan/vulkan.hpp>

class ImageView {
public:
    ImageView(vk::Device* logicalDevice, vk::Image image, vk::ImageViewType type, vk::Format format, vk::ImageAspectFlags aspectFlags, uint32_t mipmapLevel);
    ~ImageView();

    vk::ImageView* getImageView();
private:
    vk::ImageView imageView;
    vk::Image image;
    vk::ImageViewType type;
    vk::Format format;
    uint32_t mipmapLevel;
};

#endif
