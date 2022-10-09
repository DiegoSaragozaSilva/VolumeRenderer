#include "Pipeline.hpp"

Pipeline::Pipeline(Device* device, RenderPass* renderPass, std::vector<ShaderModule*> shaderModules, std::vector<vk::VertexInputBindingDescription> bindingDescription, std::vector<vk::VertexInputAttributeDescription> attributeDescription, vk::PrimitiveTopology topology, vk::PolygonMode polygonMode, vk::Viewport viewport, vk::Rect2D scissor) {
    // Querying all the necessary states information
    std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStageCreateInfos;
    for (size_t i = 0; i < shaderModules.size(); i++)
        shaderStageCreateInfos[i] = initShaderStage(shaderModules[i]);

    vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = initVertexInputState( bindingDescription, attributeDescription);
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = initInputAssembly(topology);
    vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = initRasterizationState(polygonMode);
    vk::PipelineMultisampleStateCreateInfo multiSampleStateCreateInfo = initMultiSamplingState(device->getMultiSamplingLevel());
    vk::PipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo = initDepthStencilState();
    vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = initColorBlendState();

    // Viewport state creation
    vk::PipelineViewportStateCreateInfo viewportStateCreateInfo (
        vk::PipelineViewportStateCreateFlags(),
        1,
        &viewport,
        1,
        &scissor
    );

    // Dynamic states
    std::array<vk::DynamicState, 2> dynamicStates = {
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor
    };

    vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo (
        vk::PipelineDynamicStateCreateFlags(),
        dynamicStates
    );

    // Pipeline layout create info
    // [TODO] Add support for push constants and descriptor set layouts
    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo (
        vk::PipelineLayoutCreateFlags(),
        0,
        nullptr,
        0,
        nullptr
    );

    // Pipeline layout creation
    pipelineLayout = device->getLogicalDevice()->createPipelineLayout(pipelineLayoutCreateInfo); 

    // Pipeline create info creation
    vk::GraphicsPipelineCreateInfo pipelineCreateInfo (
        vk::PipelineCreateFlags(),
        shaderStageCreateInfos,
        &vertexInputStateCreateInfo,
        &inputAssemblyStateCreateInfo,
        nullptr,
        &viewportStateCreateInfo,
        &rasterizationStateCreateInfo,
        &multiSampleStateCreateInfo,
        &depthStencilStateCreateInfo,
        &colorBlendStateCreateInfo,
        &dynamicStateCreateInfo,
        pipelineLayout,
        *(renderPass->getRenderPass())
    );

    // Pipeline creation
    vk::Result result;
    std::tie(result, pipeline) = device->getLogicalDevice()->createGraphicsPipeline(nullptr, pipelineCreateInfo);

    // Error checking
    switch (result) {
        case vk::Result::eSuccess:
            break;
        default:
            spdlog::error("Pipeline creation failed.");
            throw 0;
            break;
    }

    #ifndef NDEBUG
        spdlog::info("Vulkan pipeline successfully created.");
    #endif
}

Pipeline::~Pipeline() {
    #ifndef NDEBUG
        spdlog::info("Vulkan pipeline successfully destroyed.");
    #endif
}

vk::PipelineShaderStageCreateInfo Pipeline::initShaderStage(ShaderModule* shaderModule) {
    // Shader stage create info creation and return
    // All shader code must have a 'main' function to start with
    vk::PipelineShaderStageCreateInfo shaderStageCreateInfo (
        vk::PipelineShaderStageCreateFlags(),
        shaderModule->getShaderStage(),
        shaderModule->getShaderModule(),
        "main"
    );
    return shaderStageCreateInfo;
}

vk::PipelineVertexInputStateCreateInfo Pipeline::initVertexInputState(std::vector<vk::VertexInputBindingDescription> bindingDescription, std::vector<vk::VertexInputAttributeDescription> attributeDescription) {
    // Vertex input state create info creation and return
    vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo (
        vk::PipelineVertexInputStateCreateFlags(),
        bindingDescription.size(),
        bindingDescription.data(),
        attributeDescription.size(),
        attributeDescription.data()
    );
    return vertexInputStateCreateInfo;
}

vk::PipelineInputAssemblyStateCreateInfo Pipeline::initInputAssembly(vk::PrimitiveTopology topology) {
    // Input assembly state create info creation and return
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo (
        vk::PipelineInputAssemblyStateCreateFlags(),
        topology
    );
    return inputAssemblyStateCreateInfo;
}

vk::PipelineRasterizationStateCreateInfo Pipeline::initRasterizationState(vk::PolygonMode polygonMode) {
    // Rasterization state create info creation and return
    vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo (
        vk::PipelineRasterizationStateCreateFlags(),
        false,
        false,
        polygonMode,
        vk::CullModeFlagBits::eBack,
        vk::FrontFace::eClockwise,
        false,
        0.0f,
        0.0f,
        0.0f,
        1.0f
    );
    return rasterizationStateCreateInfo;
}
vk::PipelineMultisampleStateCreateInfo Pipeline::initMultiSamplingState(vk::SampleCountFlagBits multiSamplingLevel) {
    // Multisampling state create info creation and return
    vk::PipelineMultisampleStateCreateInfo multiSampleStateCreateInfo (
        vk::PipelineMultisampleStateCreateFlags(),
        multiSamplingLevel,
        false,
        0.0f,
        nullptr,
        false,
        false
    );
    return multiSampleStateCreateInfo;
}

vk::PipelineDepthStencilStateCreateInfo Pipeline::initDepthStencilState() {
    // Depth stencil state create info creation and return
    vk::PipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo (
        vk::PipelineDepthStencilStateCreateFlags(),
        true,
        true,
        vk::CompareOp::eLess,
        false,
        false,
        vk::StencilOpState(),
        vk::StencilOpState(),
        0.0f,
        0.0f
    );
    return depthStencilStateCreateInfo;
}

vk::PipelineColorBlendStateCreateInfo Pipeline::initColorBlendState() {
    // Color blend attachment state creation and return
    vk::ColorComponentFlags colorComponentFlags (
            vk::ColorComponentFlagBits::eR | 
            vk::ColorComponentFlagBits::eG | 
            vk::ColorComponentFlagBits::eB |
            vk::ColorComponentFlagBits::eA
    );

    vk::PipelineColorBlendAttachmentState colorBlendAttachmentState (
            false,                   
            vk::BlendFactor::eZero,  
            vk::BlendFactor::eZero,  
            vk::BlendOp::eAdd,       
            vk::BlendFactor::eZero,  
            vk::BlendFactor::eZero,  
            vk::BlendOp::eAdd,       
            colorComponentFlags
    );

    vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo (
            vk::PipelineColorBlendStateCreateFlags(),  
            false,                                     
            vk::LogicOp::eNoOp,                        
            colorBlendAttachmentState,         
            { { 1.0f, 1.0f, 1.0f, 1.0f } }             
    );

    return colorBlendStateCreateInfo;
}
