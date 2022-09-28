#ifndef _DEVICE_H_
#define _DEVICE_H_

#include <spdlog/spdlog.h>
#include <vulkan/vulkan.hpp>

class Device {
public:
    Device(vk::Instance* instance);
    ~Device();
private:
    vk::PhysicalDevice physicalDevice;
    vk::Device logicalDevice;
};

#endif
