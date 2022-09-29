#ifndef _DEVICE_H_
#define _DEVICE_H_

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
private:
    vk::PhysicalDevice physicalDevice;
    vk::Device logicalDevice;

    void pickPhysicalDevice(vk::Instance* instance);
    QueueConfig queryPhysicalDeviceQueues(vk::SurfaceKHR* windowSurface);
};

#endif
