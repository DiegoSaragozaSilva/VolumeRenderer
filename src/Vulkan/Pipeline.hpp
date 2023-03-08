#ifndef _PIPELINE_H_
#define _PIPELINE_H_

#include <unordered_map>
#include <tuple>
#include <SPIRV-Cross/spirv_cross.hpp>
#include "../RenderEngine/Texture.hpp"
#include "Device.hpp"
#include "RenderPass.hpp"
#include "ShaderModule.hpp"

class Pipeline {
public:
    Pipeline(Device* device, RenderPass* renderPass, std::vector<ShaderModule*> shaderModules, std::vector<vk::VertexInputBindingDescription> bindingDescription, std::vector<vk::VertexInputAttributeDescription> attributeDescription, vk::PrimitiveTopology topology, vk::PolygonMode polygonMode, vk::Viewport viewport, vk::Rect2D scissor, float lineWidth);
    ~Pipeline();

    vk::Pipeline getPipeline();
    vk::PipelineLayout getPipelineLayout();
    vk::DescriptorPool getDescriptorPool();
    std::vector<vk::DescriptorSetLayout> getDescriptorSetLayouts();
    vk::DescriptorSet getTextureSamplerDescriptorSet(Device* device, Texture* texture);
    vk::PushConstantRange getPushConstantRange(vk::ShaderStageFlagBits shaderStage);
private:
    vk::Pipeline pipeline;
    vk::PipelineLayout pipelineLayout;
    vk::DescriptorPool descriptorPool;
    std::vector<vk::DescriptorSetLayout> descriptorSetLayouts;
    std::vector<vk::PushConstantRange> pushConstantRanges;
    std::unordered_map<Texture*, vk::DescriptorSet> textureSamplerDescriptorSets;

    vk::DescriptorPool createDescriptorPool(Device* device);
    std::vector<vk::DescriptorSetLayout> createDescriptorSetLayouts(Device* device, std::vector<ShaderModule*> shaderModules);
    std::vector<vk::PushConstantRange> createPushConstantRanges(std::vector<ShaderModule*> shaderModules);
    vk::PipelineShaderStageCreateInfo initShaderStage(ShaderModule* shaderModule);
    vk::PipelineVertexInputStateCreateInfo initVertexInputState(std::vector<vk::VertexInputBindingDescription> bindingDescription, std::vector<vk::VertexInputAttributeDescription> attributeDescription);
    vk::PipelineInputAssemblyStateCreateInfo initInputAssembly(vk::PrimitiveTopology topology);
    vk::PipelineRasterizationStateCreateInfo initRasterizationState(vk::PolygonMode polygonMode, float lineWidth); 
    vk::PipelineMultisampleStateCreateInfo initMultiSamplingState(vk::SampleCountFlagBits multiSamplingLevel);
    vk::PipelineDepthStencilStateCreateInfo initDepthStencilState();
    vk::PipelineColorBlendStateCreateInfo initColorBlendState();
    vk::PipelineLayoutCreateInfo initPipelineLayoutInfo();
    vk::DescriptorSet createTextureSamplerDescriptorSet(Device* device, Texture* texture, vk::DescriptorSetLayout descriptorSetLayout);
};

#endif
