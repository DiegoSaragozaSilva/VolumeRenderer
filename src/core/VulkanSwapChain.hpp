#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <limits>
#include <algorithm>

// C++ crap
#include "VulkanDevice.hpp"

struct VulkanSwapChainCreateInfo {
    VkDevice device;
    VkSurfaceKHR surface;
    QueueFamilyIndices indices;
    SwapChainSupportDetails swapChainSupportDetails; 
    uint32_t windowWidth;
    uint32_t windowHeight;
};

struct VulkanFramebufferCreateInfo {
    VkDevice device;
    VkRenderPass renderPass;
};

class VulkanSwapChain {
public:
    VulkanSwapChain(VulkanSwapChainCreateInfo* info);
    ~VulkanSwapChain();

    void createSwapChainFramebuffers(VulkanFramebufferCreateInfo* info);
    VkFormat getImageFormat();
    VkExtent2D getExtent();
    VkSwapchainKHR getSwapChain();
    VkFramebuffer getFramebuffer(uint32_t framebufferIndex);
private:
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    std::vector<VkImageView> swapChainImageViews;
    VkExtent2D swapChainExtent;
    std::vector<VkFramebuffer> swapChainFramebuffers;
    
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(std::vector<VkSurfaceFormatKHR> formats);
    VkPresentModeKHR chooseSwapPresentMode(std::vector<VkPresentModeKHR> presentModes);
    VkExtent2D chooseSwapExtent(VkSurfaceCapabilitiesKHR capabilities, uint32_t windowWidth, uint32_t windowHeight);
};
