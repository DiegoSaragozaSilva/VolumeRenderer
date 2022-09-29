#ifndef _DEVICE_H_
#define _DEVICE_H_

#include <iostream>
#include <spdlog/spdlog.h>
#include <vulkan/vulkan.hpp>

struct QueueConfig {
    uint32_t graphicsQueueIndex;
    uint32_t presentationQueueIndex;
    bool hasDifferentIndices;
};

class Device {
public:
    Device(vk::Instance* instance, vk::SurfaceKHR* windowSurface);
    ~Device();

    vk::PhysicalDevice* getPhysicalDevice();
    vk::Device* getLogicalDevice();
    uint32_t getGraphicsQueueIndex();
    uint32_t getPresentationQueueIndex();
    bool hasPresentationQueue();
    void destroySwapchain(vk::SwapchainKHR* swapchain);
    void destroyImageView(vk::ImageView* imageView);
private:
    vk::PhysicalDevice physicalDevice;
    vk::Device logicalDevice;
    QueueConfig queueConfig;

    void pickPhysicalDevice(vk::Instance* instance);
    QueueConfig queryPhysicalDeviceQueues(vk::SurfaceKHR* windowSurface);
};

#endif
