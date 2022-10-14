#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "../Vulkan/Device.hpp"
#include "../Vulkan/CommandPool.hpp"
#include "../Vulkan/Image.hpp"
#include "../Vulkan/ImageView.hpp"
#include "Utils.hpp"

class Texture {
public:
    Texture(Device* device, CommandPool* commandPool, ImageData imageData);
    ~Texture();

    std::string name;

    ImageView* getImageView();
    vk::Sampler getSampler();
private:
    Image* image;
    ImageView* imageView;
    vk::Sampler sampler;
};

#endif
