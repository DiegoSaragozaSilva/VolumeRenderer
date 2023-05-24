#ifndef _IMAGE_H_
#define _IMAGE_H_

#include "Device.hpp"
#include "CommandPool.hpp"

class Image {
public:
    Image(Device* device, CommandPool* commandPool, uint32_t width, uint32_t height, uint32_t depth, uint32_t mipmapLevels, vk::SampleCountFlagBits sampleCount, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usageFlags, vk::MemoryPropertyFlags memoryFlags, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
    ~Image();

    uint32_t getWidth();
    uint32_t getHeight();
    uint32_t getMipmapLevels();
    vk::Format getFormat();
    vk::ImageType getImageType();
    vk::Image getImage();
    vk::DeviceMemory getImageMemory();
    void generateMipmaps(Device* device, CommandPool* commandPool);
private:
    vk::Image image;
    vk::DeviceMemory imageMemory;
    uint32_t width, height, depth, mipmapLevels;
    vk::ImageType type;
    vk::Format format;

    vk::DeviceMemory allocateImageMemory(Device* device, vk::MemoryPropertyFlags memoryFlags);
    void transitionLayout(Device* device, CommandPool* commandPool, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
    void applyTransitionLayoutCommand(Device* device, CommandPool* commandPool, vk::PipelineStageFlags sourceStageFlags, vk::PipelineStageFlags destinationStageFlags, vk::ImageMemoryBarrier barrier);
};

#endif
