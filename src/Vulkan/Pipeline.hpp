#ifndef _PIPELINE_H_
#define _PIPELINE_H_

#include "Device.hpp"
#include "RenderPass.hpp"
#include "ShaderModule.hpp"

class Pipeline {
public:
    Pipeline(Device* device, RenderPass* renderPass, std::vector<ShaderModule*> shaderModules, vk::PrimitiveTopology topology, vk::PolygonMode polygonMode, vk::Viewport viewport, vk::Rect2D scissor);
    ~Pipeline();
private:
    vk::Pipeline pipeline;
    vk::PipelineLayout pipelineLayout;

    vk::PipelineShaderStageCreateInfo initShaderStage(ShaderModule* shaderModule);
    vk::PipelineVertexInputStateCreateInfo initVertexInputState();
    vk::PipelineInputAssemblyStateCreateInfo initInputAssembly(vk::PrimitiveTopology topology);
    vk::PipelineRasterizationStateCreateInfo initRasterizationState(vk::PolygonMode polygonMode); 
    vk::PipelineMultisampleStateCreateInfo initMultiSamplingState(vk::SampleCountFlagBits multiSamplingLevel);
    vk::PipelineDepthStencilStateCreateInfo initDepthStencilState();
    vk::PipelineColorBlendStateCreateInfo initColorBlendState();
};

#endif
