#include "Pipeline.hpp"

Pipeline::Pipeline(Device* device, RenderPass* renderPass, std::vector<ShaderModule*> shaderModules, std::vector<vk::VertexInputBindingDescription> bindingDescription, std::vector<vk::VertexInputAttributeDescription> attributeDescription, vk::PrimitiveTopology topology, vk::PolygonMode polygonMode, vk::Viewport viewport, vk::Rect2D scissor, float lineWidth) {
    // Create the pipeline descriptor pool
    descriptorPool = createDescriptorPool(device);
    
    // Create the descriptor set layouts
    descriptorSetLayouts = createDescriptorSetLayouts(device, shaderModules);
    
    // Create the push constant ranges
    pushConstantRanges = createPushConstantRanges(shaderModules);

    // Vertex input state (No function for this one)
    vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo (
        vk::PipelineVertexInputStateCreateFlags(),
        bindingDescription.size(),
        bindingDescription.data(),
        attributeDescription.size(),
        attributeDescription.data()
    );

    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = initInputAssembly(topology);
    vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = initRasterizationState(polygonMode, lineWidth);
    vk::PipelineMultisampleStateCreateInfo multiSampleStateCreateInfo = initMultiSamplingState(device->getMultiSamplingLevel());
    vk::PipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo = initDepthStencilState();
 
    // Color blend attachment state (No function for this one)   
    vk::ColorComponentFlags colorComponentFlags (
            vk::ColorComponentFlagBits::eR | 
            vk::ColorComponentFlagBits::eG | 
            vk::ColorComponentFlagBits::eB |
            vk::ColorComponentFlagBits::eA
    );

    vk::PipelineColorBlendAttachmentState colorBlendAttachmentState (
            VK_TRUE,                   
            vk::BlendFactor::eSrcAlpha,  
            vk::BlendFactor::eOneMinusSrcAlpha,  
            vk::BlendOp::eAdd,       
            vk::BlendFactor::eSrcAlpha,  
            vk::BlendFactor::eOneMinusSrcAlpha,  
            vk::BlendOp::eAdd,       
            colorComponentFlags
    );

    vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo (
            vk::PipelineColorBlendStateCreateFlags(),  
            VK_FALSE,                                     
            vk::LogicOp::eCopy,
            1,
            &colorBlendAttachmentState,         
            { { 1.0f, 1.0f, 1.0f, 1.0f } }             
    );

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
    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo = initPipelineLayoutInfo();
    
    // Pipeline layout creation
    pipelineLayout = device->getLogicalDevice()->createPipelineLayout(pipelineLayoutCreateInfo); 
    
    // Querying all the necessary states information
    size_t numShaderStages = shaderModules.size();
    vk::Result result;

    if (numShaderStages == 2) { 
        std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStageCreateInfos;
        for (size_t i = 0; i < numShaderStages; i++)
            shaderStageCreateInfos[i] = initShaderStage(shaderModules[i]); 

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
        std::tie(result, pipeline) = device->getLogicalDevice()->createGraphicsPipeline(nullptr, pipelineCreateInfo);
    }
    else if (numShaderStages == 3) {
        std::array<vk::PipelineShaderStageCreateInfo, 3> shaderStageCreateInfos;
        for (size_t i = 0; i < numShaderStages; i++)
            shaderStageCreateInfos[i] = initShaderStage(shaderModules[i]); 
    
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
        std::tie(result, pipeline) = device->getLogicalDevice()->createGraphicsPipeline(nullptr, pipelineCreateInfo);
    }

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

vk::DescriptorPool Pipeline::createDescriptorPool(Device* device) {
    // Max descriptors
    const uint32_t maxDescriptors = 64;

    // Uniform buffers descriptors size
    vk::DescriptorPoolSize uniformBufferPoolSize (
        vk::DescriptorType::eUniformBuffer,
        maxDescriptors
    );

    // Image sampler descriptors size
    vk::DescriptorPoolSize imageSamplerPoolSize (
        vk::DescriptorType::eCombinedImageSampler,
        maxDescriptors
    );

    // [TODO] Maybe more descriptor types?

    // Group all pool sizes together
    std::array<vk::DescriptorPoolSize, 2> poolSizes = {
        uniformBufferPoolSize,
        imageSamplerPoolSize
    };

    // Desriptor pool create info
    vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo (
        vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
        maxDescriptors,
        poolSizes.size(),
        poolSizes.data()
    );

    // Return the descriptor pool
    return device->getLogicalDevice()->createDescriptorPool(descriptorPoolCreateInfo);
}

std::vector<vk::DescriptorSetLayout> Pipeline::createDescriptorSetLayouts(Device* device, std::vector<ShaderModule*> shaderModules) {
    // For each shader module that will be attached to the pipeline, query information about descriptor sets
    std::vector<std::tuple<uint32_t, uint32_t>> setIndexBindings;
    std::vector<vk::DescriptorSetLayoutBinding> bindings;
    
    for (ShaderModule* shaderModule : shaderModules) {
        // Query information about the shader module resourcers
        std::vector<uint32_t> shaderSPIRVCode = shaderModule->getSPIRVCode();
        spirv_cross::Compiler compiler(std::move(shaderSPIRVCode));
        spirv_cross::ShaderResources shaderResources = compiler.get_shader_resources();

        // Query uniform buffers information
        for (const spirv_cross::Resource& resource : shaderResources.uniform_buffers) {
            // Add (set, binding) to the set index bindings
            setIndexBindings.push_back (
                std::make_tuple(compiler.get_decoration(resource.id, spv::DecorationDescriptorSet), compiler.get_decoration(resource.id, spv::DecorationBinding))
            );

            vk::DescriptorSetLayoutBinding uniformBufferBinding (
                compiler.get_decoration(resource.id, spv::DecorationBinding),
                vk::DescriptorType::eUniformBuffer,
                1,
                shaderModule->getShaderStage(),
                nullptr
            );
            bindings.push_back(uniformBufferBinding);
        }

        // Query texture sampler information
        for (const spirv_cross::Resource& resource : shaderResources.sampled_images) {
            // Add (set, binding) to the set index bindings
            setIndexBindings.push_back (
                std::make_tuple(compiler.get_decoration(resource.id, spv::DecorationDescriptorSet), compiler.get_decoration(resource.id, spv::DecorationBinding))
            );

            vk::DescriptorSetLayoutBinding sampledImageBinding (
                compiler.get_decoration(resource.id, spv::DecorationBinding),
                vk::DescriptorType::eCombinedImageSampler,
                1,
                shaderModule->getShaderStage(),
                nullptr
            );
            bindings.push_back(sampledImageBinding);
        }
    }
    

    // Separate bindings per set
    std::vector<std::vector<vk::DescriptorSetLayoutBinding>> setBindings(setIndexBindings.size());
    for (const std::tuple<uint32_t, uint32_t>& setIndexBinding : setIndexBindings) {
        uint32_t set = (uint32_t)(std::get<0>(setIndexBinding));
        uint32_t binding = (uint32_t)(std::get<1>(setIndexBinding));
        setBindings[set].push_back(bindings[binding]);
    }
    
    // Construct descriptor set layouts with all the bindings
    std::vector<vk::DescriptorSetLayout> descriptorSetLayouts;
    for (const std::vector<vk::DescriptorSetLayoutBinding>& setBinding : setBindings) {
        vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo (
            vk::DescriptorSetLayoutCreateFlags(),
            setBinding.size(),
            setBinding.data()
        );

        descriptorSetLayouts.push_back(device->getLogicalDevice()->createDescriptorSetLayout(descriptorSetLayoutCreateInfo));
    }
    
    return descriptorSetLayouts;
}

std::vector<vk::PushConstantRange> Pipeline::createPushConstantRanges(std::vector<ShaderModule*> shaderModules) {
    // For each shader module that will be attached to the pipeline, query information about push constant ranges
    std::vector<vk::PushConstantRange> pushConstantRanges;
    for (ShaderModule* shaderModule : shaderModules) {
        uint32_t offset = shaderModule->getPushConstantOffset();
        uint32_t range = shaderModule->getPushConstantRange();
        if (range > 0) {
            vk::PushConstantRange pushConstantRange (
                shaderModule->getShaderStage(),
                offset,
                range
            );
            pushConstantRanges.push_back(pushConstantRange);
        }
    }
    return pushConstantRanges;
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
    return vk::PipelineVertexInputStateCreateInfo (
        vk::PipelineVertexInputStateCreateFlags(),
        bindingDescription.size(),
        bindingDescription.data(),
        attributeDescription.size(),
        attributeDescription.data()
    );
}

vk::PipelineInputAssemblyStateCreateInfo Pipeline::initInputAssembly(vk::PrimitiveTopology topology) {
    // Input assembly state create info creation and return
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo (
        vk::PipelineInputAssemblyStateCreateFlags(),
        topology
    );
    return inputAssemblyStateCreateInfo;
}

vk::PipelineRasterizationStateCreateInfo Pipeline::initRasterizationState(vk::PolygonMode polygonMode, float lineWidth) {
    // Rasterization state create info creation and return
    vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo (
        vk::PipelineRasterizationStateCreateFlags(),
        false,
        false,
        polygonMode,
        vk::CullModeFlagBits::eNone,
        vk::FrontFace::eClockwise,
        false,
        0.0f,
        0.0f,
        0.0f,
        lineWidth
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
            VK_TRUE,                   
            vk::BlendFactor::eSrcAlpha,  
            vk::BlendFactor::eOneMinusSrcAlpha,  
            vk::BlendOp::eAdd,       
            vk::BlendFactor::eOne,  
            vk::BlendFactor::eZero,  
            vk::BlendOp::eAdd,       
            colorComponentFlags
    );

    vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo (
            vk::PipelineColorBlendStateCreateFlags(),  
            VK_TRUE,                                     
            vk::LogicOp::eClear,
            1,
            &colorBlendAttachmentState,         
            { { 1.0f, 1.0f, 1.0f, 1.0f } }             
    );

    return colorBlendStateCreateInfo;
}

vk::PipelineLayoutCreateInfo Pipeline::initPipelineLayoutInfo() {
    // Creation and return of the pipeline layout create info
    return vk::PipelineLayoutCreateInfo (
        vk::PipelineLayoutCreateFlags(),
        descriptorSetLayouts.size(),
        descriptorSetLayouts.data(),
        pushConstantRanges.size(),
        pushConstantRanges.data()
    );
}

vk::Pipeline Pipeline::getPipeline() {
    return pipeline;
}

vk::PipelineLayout Pipeline::getPipelineLayout() {
    return pipelineLayout;
}

vk::DescriptorPool Pipeline::getDescriptorPool() {
    return descriptorPool;
}

std::vector<vk::DescriptorSetLayout> Pipeline::getDescriptorSetLayouts() {
    return descriptorSetLayouts;
}

vk::DescriptorSet Pipeline::getTextureSamplerDescriptorSet(Device* device, Texture* texture) {
    // Check for already existing descriptor set for this texture
    if (textureSamplerDescriptorSets.count(texture) == 0) {
        // If not found, create it
        textureSamplerDescriptorSets.insert(std::make_pair(
            texture,
            createTextureSamplerDescriptorSet(
                device,
                texture,
                descriptorSetLayouts[0] // [TODO] Differentiate descriptor sets for different sets
            )
        ));
    }

    // Return found descriptor set
    return textureSamplerDescriptorSets.at(texture);
}

vk::PushConstantRange Pipeline::getPushConstantRange(vk::ShaderStageFlagBits shaderStage) {
    for (vk::PushConstantRange range : pushConstantRanges) {
        if (range.stageFlags == shaderStage)
            return range;
    }

    spdlog::error("No push constant range found with provided shader stage flags");
    throw 0;
}

vk::DescriptorSet Pipeline::createTextureSamplerDescriptorSet(Device* device, Texture* texture, vk::DescriptorSetLayout descriptorSetLayout) {
    // Descriptor set allocate info
    vk::DescriptorSetAllocateInfo setAllocateInfo (
        descriptorPool,
        1,
        &descriptorSetLayout
    );

    // Descriptor set creation
    vk::DescriptorSet descriptorSet (
        std::move(device->getLogicalDevice()->allocateDescriptorSets(setAllocateInfo)[0])
    );

    // Descriptor image info
    vk::DescriptorImageInfo descriptorImageInfo (
        texture->getSampler(),
        *(texture->getImageView()->getImageView()),
        vk::ImageLayout::eShaderReadOnlyOptimal
    );

    // Write descriptor set
    vk::WriteDescriptorSet writeDescriptorSet (
        descriptorSet,
        0,
        0,
        1,
        vk::DescriptorType::eCombinedImageSampler,
        &descriptorImageInfo,
        nullptr,
        nullptr
    );

    // Update descriptor set and return
    device->getLogicalDevice()->updateDescriptorSets(1, &writeDescriptorSet, 0, nullptr);
    return descriptorSet;
}
