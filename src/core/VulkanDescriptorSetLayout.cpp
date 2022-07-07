#include "VulkanDescriptorSetLayout.hpp"

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VulkanDescriptorSetLayoutCreateInfo* info) {
    VkDescriptorSetLayoutBinding layoutBinding{};
    layoutBinding.binding = info->binding;
    layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layoutBinding.descriptorCount = 1;
    layoutBinding.stageFlags = info->shaderStageFlag;
    layoutBinding.pImmutableSamplers = nullptr; // Optional
    
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &layoutBinding; 

    if (vkCreateDescriptorSetLayout(info->device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        spdlog::error("Failed to create descriptor set layout!");
        throw 0;
    }
}

VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout() {}

VkDescriptorSetLayout VulkanDescriptorSetLayout::getDescriptorSetLayout() {
    return descriptorSetLayout;
}
