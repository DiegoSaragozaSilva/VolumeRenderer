#pragma once

#include <vulkan/vulkan.h>
#include <spdlog/spdlog.h>
#include <vector>
#include <iostream>

struct VulkanDescriptorSetHandlerCreateInfo {
    VkDevice device;
    uint32_t maxFramesInFlight;
};

// New
struct BindingCreateInfo {
    VkDevice device;
    uint32_t binding;
    VkDescriptorType descriptorType;
    VkShaderStageFlagBits stageFlags;
    uint32_t maxFramesInFlight;
};

struct DescriptorSetInfo {
    uint32_t binding;
    VkDescriptorType descriptorType;
    VkDescriptorBufferInfo bufferInfo;
    VkDescriptorImageInfo imageInfo;
};

struct DescriptorSetUpdateInfo {
    VkDevice device;
    uint32_t maxFramesInFlight;
    std::vector<DescriptorSetInfo> setInfos;
};

class VulkanDescriptorSetHandler {
public:
    VulkanDescriptorSetHandler(VulkanDescriptorSetHandlerCreateInfo* info);
    ~VulkanDescriptorSetHandler();
    
    // Old
    VkDescriptorSetLayout getDescriptorSetLayout();
    VkDescriptorSet getDescriptorSet(uint32_t setIndex);
    void updateDescriptorSets(DescriptorSetUpdateInfo* info);
    
    // New
    void addBinding(BindingCreateInfo* info);
    int getBinding(VkDescriptorType descriptorType);
private:

    // New
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
    std::vector<VkDescriptorSet> descriptorSets;
    bool descriptorSetMemoryAllocated;

    void recreateDescriptorSetLayout(VkDevice device);
    void recreateDescriptorPool(VkDevice device, uint32_t maxFramesInFlight);
    void allocateMemoryForDescriptorSets(VkDevice device, uint32_t maxFramesInFlight);
};
