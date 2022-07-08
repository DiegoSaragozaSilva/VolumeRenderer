#include "VulkanDescriptorSetHandler.hpp"

VulkanDescriptorSetHandler::VulkanDescriptorSetHandler(VulkanDescriptorSetHandlerCreateInfo* info) {
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

    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = static_cast<uint32_t>(info->maxFramesInFlight);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = static_cast<uint32_t>(info->maxFramesInFlight);

    if (vkCreateDescriptorPool(info->device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        spdlog::error("Failed to create descriptor pool");
        throw 0;
    } 
}

VulkanDescriptorSetHandler::~VulkanDescriptorSetHandler() {}

void VulkanDescriptorSetHandler::updateDescriptorSets(DescriptorSetCreateInfo* info) {
    descriptorSets.resize(info->maxFramesInFlight);

    std::vector<VkDescriptorSetLayout> layouts(info->maxFramesInFlight, descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(info->maxFramesInFlight);
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(info->maxFramesInFlight);
    if (vkAllocateDescriptorSets(info->device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
        spdlog::error("Failed to allocate memory for descriptor set!");
        throw 0;
    }

    for (size_t i = 0; i < info->maxFramesInFlight; i++) {
        VkDescriptorBufferInfo bufferInfo{};
 
        bufferInfo.buffer = info->uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = info->uniformBufferSizes[i];
        
        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;
        descriptorWrite.pImageInfo = nullptr; // Optional
        descriptorWrite.pTexelBufferView = nullptr; // Optional
        
        vkUpdateDescriptorSets(info->device, 1, &descriptorWrite, 0, nullptr);
    }
}

VkDescriptorSetLayout VulkanDescriptorSetHandler::getDescriptorSetLayout() {
    return descriptorSetLayout;
}

VkDescriptorSet VulkanDescriptorSetHandler::getDescriptorSet(uint32_t setIndex) {
    if (descriptorSets.size() == 0)
        return nullptr;

    return descriptorSets[setIndex];
}
