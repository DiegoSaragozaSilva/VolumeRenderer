#pragma once

#include <vulkan/vulkan.h>
#include <spdlog/spdlog.h>

struct VulkanRenderPassCreateInfo {
    VkDevice device;
    VkFormat swapChainImageFormat; 
};

class VulkanRenderPass {
public:
    VulkanRenderPass(VulkanRenderPassCreateInfo* info);
    ~VulkanRenderPass();

    VkRenderPass getRenderPass();
private:
    VkRenderPass renderPass;
};
