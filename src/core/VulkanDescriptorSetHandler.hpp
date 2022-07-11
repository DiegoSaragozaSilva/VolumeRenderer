#pragma once

#include <vulkan/vulkan.h>
#include <spdlog/spdlog.h>
#include <vector>
#include <iostream>

struct VulkanDescriptorSetHandlerCreateInfo {
    VkDevice device;
    VkDescriptorType descriptorType;
    VkShaderStageFlagBits shaderStageFlag;
    uint32_t maxFramesInFlight;
};

struct DescriptorSetCreateInfo {
    VkDevice device;
    uint32_t maxFramesInFlight;
    VkBuffer uniformBuffer;
    VkDeviceSize uniformBufferSize;
    VkImageView textureImageView;
    VkSampler textureSampler;
};

class VulkanDescriptorSetHandler {
public:
    VulkanDescriptorSetHandler(VulkanDescriptorSetHandlerCreateInfo* info);
    ~VulkanDescriptorSetHandler();

    VkDescriptorSetLayout getDescriptorSetLayout();
    VkDescriptorSet getDescriptorSet(uint32_t setIndex);
    void updateDescriptorSets(DescriptorSetCreateInfo* info);
private:
    VkDescriptorPool descriptorPool;
    VkDescriptorSetLayout descriptorSetLayout;
    std::vector<VkDescriptorSet> descriptorSets;
};
