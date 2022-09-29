#ifndef _SWAPCHAIN_H_
#define _SWAPCHAIN_H_

#include <spdlog/spdlog.h>
#include <vulkan/vulkan.hpp>
#include <vector>
#include <algorithm>

#include "Device.hpp"

struct SwapchainFormat {
    vk::ColorSpaceKHR colorSpace;
    vk::Format colorFormat;
};

class Swapchain {
public:
    Swapchain(Device* device, vk::SurfaceKHR* surface, uint32_t imageWidth, uint32_t imageHeight);
    ~Swapchain();

    vk::SwapchainKHR* getSwapchain();
private:
    vk::SwapchainKHR swapchain;
    std::vector<vk::Image> swapchainImages;
    SwapchainFormat format;
    vk::PresentModeKHR presentMode;
    vk::Extent2D extent;
    vk::SurfaceTransformFlagBitsKHR transformFlags;

    SwapchainFormat getFormat(vk::PhysicalDevice* physicalDevice, vk::SurfaceKHR surface);
    vk::PresentModeKHR getPresentMode(vk::PhysicalDevice* physicalDevice, vk::SurfaceKHR surface); 
};

#endif
