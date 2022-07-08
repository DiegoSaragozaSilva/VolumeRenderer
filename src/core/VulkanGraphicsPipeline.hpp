#pragma once

#include <vulkan/vulkan.h>
#include <spdlog/spdlog.h>
#include <vector>

struct VulkanGraphicsPipelineCreateInfo {
    VkDevice device;
    VkRenderPass renderPass;
    VkExtent2D swapChainExtent;
    VkVertexInputBindingDescription vertexBindingDescription;
    std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions;
    uint32_t descriptorSetLayoutCount;
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
    uint32_t numSubPasses;
    std::vector<char> vertexShaderCode;
    std::vector<char> fragmentShaderCode;
};

class VulkanGraphicsPipeline {
public:
    VulkanGraphicsPipeline(VulkanGraphicsPipelineCreateInfo* info);
    ~VulkanGraphicsPipeline();

    VkPipeline getGraphicsPipeline();
    VkPipelineLayout getPipelineLayout();
private:
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

    VkShaderModule createShaderModule(VkDevice device, std::vector<char> shaderCode);    
};
