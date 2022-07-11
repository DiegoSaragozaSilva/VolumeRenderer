#include "VulkanDescriptorSetHandler.hpp"

VulkanDescriptorSetHandler::VulkanDescriptorSetHandler(VulkanDescriptorSetHandlerCreateInfo* info) {
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = info->shaderStageFlag;
    uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 1;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = info->shaderStageFlag;

    std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = bindings.size();
    layoutInfo.pBindings = bindings.data(); 

    if (vkCreateDescriptorSetLayout(info->device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        spdlog::error("Failed to create descriptor set layout!");
        throw 0;
    }

    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(info->maxFramesInFlight);
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(info->maxFramesInFlight);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = poolSizes.size();
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(info->maxFramesInFlight);

    if (vkCreateDescriptorPool(info->device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        spdlog::error("Failed to create descriptor pool");
        throw 0;
    }

    descriptorSets.resize(info->maxFramesInFlight);

    std::vector<VkDescriptorSetLayout> layouts(info->maxFramesInFlight, descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(info->maxFramesInFlight);
    allocInfo.pSetLayouts = layouts.data();

    if (vkAllocateDescriptorSets(info->device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
        spdlog::error("Failed to allocate memory for descriptor set!");
        throw 0;
    }
}

VulkanDescriptorSetHandler::~VulkanDescriptorSetHandler() {}

void VulkanDescriptorSetHandler::updateDescriptorSets(DescriptorSetCreateInfo* info) { 
    for (size_t i = 0; i < info->maxFramesInFlight; i++) {
        VkDescriptorBufferInfo bufferInfo{}; 
        bufferInfo.buffer = info->uniformBuffer;
        bufferInfo.offset = 0;
        bufferInfo.range = info->uniformBufferSize;

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = info->textureImageView;
        imageInfo.sampler = info->textureSampler;
        
        std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
        std::vector<uint8_t> validSets;
        if (info->uniformBuffer != nullptr && info->uniformBufferSize != 0) {
            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = descriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;
            validSets.push_back(0);
        }
        
        if (info->textureImageView != nullptr) {
            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = descriptorSets[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pImageInfo = &imageInfo;
            validSets.push_back(1);
        }
        
        std::vector<VkWriteDescriptorSet> validWrites;
        for (const auto& validSet : validSets) {
            validWrites.push_back(descriptorWrites[validSet]);
        }

        vkUpdateDescriptorSets(info->device, validWrites.size(), validWrites.data(), 0, nullptr);
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
