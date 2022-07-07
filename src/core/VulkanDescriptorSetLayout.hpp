#pragma once

#include <vulkan/vulkan.h>
#include <spdlog/spdlog.h>

struct VulkanDescriptorSetLayoutCreateInfo {
    VkDevice device;
    uint32_t binding;
    VkDescriptorType descriptorType;
    VkShaderStageFlagBits shaderStageFlag;
};

class VulkanDescriptorSetLayout {
public:
    VulkanDescriptorSetLayout(VulkanDescriptorSetLayoutCreateInfo* info);
    ~VulkanDescriptorSetLayout();

    VkDescriptorSetLayout getDescriptorSetLayout();
private:
    VkDescriptorSetLayout descriptorSetLayout;
};
