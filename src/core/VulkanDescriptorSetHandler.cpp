#include "VulkanDescriptorSetHandler.hpp"

VulkanDescriptorSetHandler::VulkanDescriptorSetHandler(VulkanDescriptorSetHandlerCreateInfo* info) { 
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 0;
    layoutInfo.pBindings = nullptr; 

    if (vkCreateDescriptorSetLayout(info->device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        spdlog::error("Failed to create descriptor set layout!");
        throw 0;
    }

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 0;
    poolInfo.pPoolSizes = nullptr;
    poolInfo.maxSets = info->maxFramesInFlight;

    if (vkCreateDescriptorPool(info->device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        spdlog::error("Failed to create descriptor pool");
        throw 0;
    }
}

VulkanDescriptorSetHandler::~VulkanDescriptorSetHandler() {}

VkDescriptorSetLayout VulkanDescriptorSetHandler::getDescriptorSetLayout() {
    return descriptorSetLayout;
}

VkDescriptorSet VulkanDescriptorSetHandler::getDescriptorSet(uint32_t setIndex) {
    if (descriptorSets.size() == 0)
        return nullptr;

    return descriptorSets[setIndex];
}

void VulkanDescriptorSetHandler::addBinding(BindingCreateInfo* info) {
    VkDescriptorSetLayoutBinding layoutBinding{};
    layoutBinding.binding = info->binding;
    layoutBinding.stageFlags = info->stageFlags;
    layoutBinding.descriptorType = info->descriptorType;
    layoutBinding.descriptorCount = 1;

    layoutBindings.push_back(layoutBinding);
    
    recreateDescriptorSetLayout(info->device);
    recreateDescriptorPool(info->device, info->maxFramesInFlight);
    allocateMemoryForDescriptorSets(info->device, info->maxFramesInFlight);
}

void VulkanDescriptorSetHandler::recreateDescriptorSetLayout(VkDevice device) {
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = layoutBindings.size();
    layoutInfo.pBindings = layoutBindings.data(); 

    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        spdlog::error("Failed to recreate descriptor set layout!");
        throw 0;
    }
}

void VulkanDescriptorSetHandler::recreateDescriptorPool(VkDevice device, uint32_t maxFramesInFlight) {
    std::vector<VkDescriptorPoolSize> poolSizes {};

    for (int i = 0; i < layoutBindings.size(); i++) {
        VkDescriptorPoolSize poolSize {};
        poolSize.type = layoutBindings[i].descriptorType;
        poolSize.descriptorCount = static_cast<uint32_t>(maxFramesInFlight);
        poolSizes.push_back(poolSize);
    }

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = poolSizes.size();
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(maxFramesInFlight);
    
    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        spdlog::error("Failed to recreate descriptor pool");
        throw 0;
    }
}

void VulkanDescriptorSetHandler::allocateMemoryForDescriptorSets(VkDevice device, uint32_t maxFramesInFlight) {
    descriptorSets.resize(maxFramesInFlight);

    std::vector<VkDescriptorSetLayout> layouts(maxFramesInFlight, descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(maxFramesInFlight);
    allocInfo.pSetLayouts = layouts.data();

    if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
        spdlog::error("Failed to reallocate memory for descriptor set!");
        throw 0;
    }
}

void VulkanDescriptorSetHandler::updateDescriptorSets(DescriptorSetUpdateInfo* info) {     
    for (size_t i = 0; i < info->maxFramesInFlight; i++) {
        
        int j = 0;
        std::vector<VkWriteDescriptorSet> descriptorWrites(info->setInfos.size());
        for (const auto& setInfo : info->setInfos) {
            descriptorWrites[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[j].dstSet = descriptorSets[i];
            descriptorWrites[j].dstBinding = setInfo.binding;
            descriptorWrites[j].dstArrayElement = 0;
            descriptorWrites[j].descriptorType = setInfo.descriptorType;
            descriptorWrites[j].descriptorCount = 1;    

            if (setInfo.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
                descriptorWrites[j].pBufferInfo = &setInfo.bufferInfo;
            }

            else if (setInfo.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
                descriptorWrites[j].pImageInfo = &setInfo.imageInfo;
            }

            j++;
        }

        vkUpdateDescriptorSets(info->device, descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
    }
}

int VulkanDescriptorSetHandler::getBinding(VkDescriptorType descriptorType) {
    for (const auto& layoutBinding : layoutBindings) {
        if (layoutBinding.descriptorType == descriptorType)
            return layoutBinding.binding;
    }

    return -1;
}

