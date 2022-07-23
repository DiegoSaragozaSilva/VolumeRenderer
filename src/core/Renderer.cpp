#include "Renderer.hpp"

Renderer::Renderer(RendererCreateInfo* info) {
    currentFrame = 0;
    bindingCount = 0;

    shouldRecreatePipeline = false;

    currentUniformBuffer = nullptr;
    currentTexture = nullptr;

    // GLFW initialization
    initGLFW(info->windowWidth, info->windowHeight, info->windowName);

    // Vulkan instance creation
    spdlog::info("Creating vulkan instance...");
    VulkanInstanceCreateInfo vulkanInstanceInfo {};
    vulkanInstanceInfo.enableValidationLayers = info->enableValidationLayers;
    vulkanInstanceInfo.validationLayers = validationLayers;
    vulkanInstanceInfo.extensions = glfwGetRequiredInstanceExtensions(&vulkanInstanceInfo.extensionCount);
    vulkanInstanceInfo.applicationName = "Volume Renderer";
    vulkanInstanceInfo.engineName = "Volume engine";
    vulkanInstance = new VulkanInstance(&vulkanInstanceInfo);
    spdlog::info("Vulkan instance successfully created!");

    // Vulkan surface creation
    spdlog::info("Creating vulkan surface...");
    VulkanSurfaceCreateInfo vulkanSurfaceInfo {};
    vulkanSurfaceInfo.instance = vulkanInstance->getInstance();
    vulkanSurfaceInfo.window = window;
    vulkanSurface = new VulkanSurface(&vulkanSurfaceInfo);
    spdlog::info("Vulkan surface successfully created!");

    // Vulkan physical device picking and logical device creation
    spdlog::info("Picking physical device and logical device creation...");
    VulkanDeviceCreateInfo vulkanDeviceInfo {};
    vulkanDeviceInfo.instance = vulkanInstance->getInstance();
    vulkanDeviceInfo.surface = vulkanSurface->getSurface();
    vulkanDeviceInfo.requiredDeviceExtensions = deviceExtensions;
    vulkanDeviceInfo.enableValidationLayers = info->enableValidationLayers;
    vulkanDeviceInfo.validationLayers = validationLayers;
    vulkanDevice = new VulkanDevice(&vulkanDeviceInfo);
    spdlog::info("Physical and logical device successfully created!");

    // Vulkan swap chain creation
    spdlog::info("Creating vulkan swap chain...");
    VulkanSwapChainCreateInfo vulkanSwapChainInfo {};
    vulkanSwapChainInfo.device = vulkanDevice->getDevice();
    vulkanSwapChainInfo.surface = vulkanSurface->getSurface();
    vulkanSwapChainInfo.windowWidth = info->windowWidth;
    vulkanSwapChainInfo.windowHeight = info->windowHeight;
    vulkanSwapChainInfo.swapChainSupportDetails = vulkanDevice->getSwapChainSupportDetails();
    vulkanSwapChainInfo.indices = vulkanDevice->getQueueFamilies();
    vulkanSwapChain = new VulkanSwapChain(&vulkanSwapChainInfo);
    spdlog::info("Vulkan swap chain successfully created!");

    // Vulkan render pass creation
    spdlog::info("Creating basic vulkan render pass...");
    VulkanRenderPassCreateInfo vulkanRenderPassInfo {};
    vulkanRenderPassInfo.device = vulkanDevice->getDevice();
    vulkanRenderPassInfo.swapChainImageFormat = vulkanSwapChain->getImageFormat();
    vulkanRenderPass = new VulkanRenderPass(&vulkanRenderPassInfo);
    spdlog::info("Basic vulkan render pass successfully created!");

    // Basic descriptor set for uniform buffers
    spdlog::info("Creating descriptor handler...");
    VulkanDescriptorSetHandlerCreateInfo descriptorHandlerInfo {};
    descriptorHandlerInfo.device = vulkanDevice->getDevice();
    descriptorHandlerInfo.maxFramesInFlight = MAX_FRAMES_IN_FLIGHT;
    vulkanDescriptorSetHandler = new VulkanDescriptorSetHandler(&descriptorHandlerInfo);
    spdlog::info("Descriptor handler successfully created!");

    // Basic graphics pipeline creation
    spdlog::info("Creating basic vulkan graphics pipeline...");
    VulkanGraphicsPipelineCreateInfo vulkanGraphicsPipelineInfo {};
    vulkanGraphicsPipelineInfo.device = vulkanDevice->getDevice();
    vulkanGraphicsPipelineInfo.renderPass = vulkanRenderPass->getRenderPass(); 
    vulkanGraphicsPipelineInfo.swapChainExtent = vulkanSwapChain->getExtent();
    vulkanGraphicsPipelineInfo.vertexBindingDescription = Vertex::getBindingDescription();
    vulkanGraphicsPipelineInfo.vertexAttributeDescriptions = Vertex::getAttributeDescriptions();
    vulkanGraphicsPipelineInfo.descriptorSetLayoutCount = 1; 

    std::vector<VkDescriptorSetLayout> layouts = {vulkanDescriptorSetHandler->getDescriptorSetLayout()};
    vulkanGraphicsPipelineInfo.descriptorSetLayouts = layouts;
    vulkanGraphicsPipelineInfo.numSubPasses = 0;
    // [NOTE] User should be able to bind own vertex and fragment shaders
    vulkanGraphicsPipelineInfo.vertexShaderCode = readFile("shaders/vert.spv");
    vulkanGraphicsPipelineInfo.fragmentShaderCode = readFile("shaders/frag.spv");
    vulkanGraphicsPipeline = new VulkanGraphicsPipeline(&vulkanGraphicsPipelineInfo);
    spdlog::info("Basic vulkan graphics pipeline successfully created!");

    // Framebuffers creation
    spdlog::info("Creating swap chain framebuffers...");
    VulkanFramebufferCreateInfo framebufferInfo {};
    framebufferInfo.device = vulkanDevice->getDevice();
    framebufferInfo.renderPass = vulkanRenderPass->getRenderPass();
    vulkanSwapChain->createSwapChainFramebuffers(&framebufferInfo);
    spdlog::info("Swap chain framebuffers successfully created!");

    // Command buffer handler creation
    spdlog::info("Creating command buffer handler...");
    VulkanCommandBufferHandlerCreateInfo commandBufferHandlerInfo {};
    commandBufferHandlerInfo.device = vulkanDevice->getDevice();
    commandBufferHandlerInfo.queueFamilyIndices = vulkanDevice->getQueueFamilies();
    commandBufferHandlerInfo.numCommandBuffers = MAX_FRAMES_IN_FLIGHT;
    vulkanCommandBufferHandler = new VulkanCommandBufferHandler(&commandBufferHandlerInfo);
    spdlog::info("Command buffer handler successfully created!");

    // Sync objects creation
    spdlog::info("Creating basic sync objects...");
    VulkanSemaphoreCreateInfo vulkanSemaphoreInfo {};
    vulkanSemaphoreInfo.device = vulkanDevice->getDevice();

    VulkanFenceCreateInfo vulkanFenceInfo {};
    vulkanFenceInfo.device = vulkanDevice->getDevice();
    
    imageAvailableVulkanSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderFinishedVulkanSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    inFlightVulkanFences.resize(MAX_FRAMES_IN_FLIGHT);

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        imageAvailableVulkanSemaphores[i] = new VulkanSemaphore(&vulkanSemaphoreInfo);
        renderFinishedVulkanSemaphores[i] = new VulkanSemaphore(&vulkanSemaphoreInfo);
        inFlightVulkanFences[i] = new VulkanFence(&vulkanFenceInfo);
    }
    spdlog::info("Basic sync objects created!");
}

Renderer::~Renderer() {}

void Renderer::initGLFW(uint32_t windowWidth, uint32_t windowHeight, std::string windowName) {
    if (!glfwInit()) {
        spdlog::error("GLFW could not be initialized!");
        throw 0;
    }

    // Disable window resize
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // Disable default OpenGL context creation
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(windowWidth, windowHeight, windowName.c_str(), nullptr, nullptr);
    if (!window) {
        spdlog::error("Failed to create GLFW window!");
        throw 0;
    }
}

std::vector<char> Renderer::readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        spdlog::error("Failed to load file: {}", filename.c_str());
        throw 0;
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;

}

bool Renderer::windowShouldClose() {
    return glfwWindowShouldClose(window);
}

void Renderer::pollEvents() {
    glfwPollEvents();

    if (shouldRecreatePipeline) {
        shouldRecreatePipeline = false;

        recreateGraphicsPipeline();
    }
}

void Renderer::addModelToScene(Model* model) {
    VulkanBufferCreateInfo vulkanBufferInfo {};
    vulkanBufferInfo.device = vulkanDevice->getDevice();
    vulkanBufferInfo.physicalDevice = vulkanDevice->getPhysicalDevice();
    vulkanBufferInfo.bufferSize = model->getMeshSize() * Vertex::getVertexSize();
    vulkanBufferInfo.usageFlags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    vulkanBufferInfo.propertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    VulkanBuffer* vulkanBuffer = new VulkanBuffer(&vulkanBufferInfo);

    std::vector<Vertex> modelMesh = model->getMesh();
    std::vector<float> meshData(8 * modelMesh.size());
    uint32_t i = 0;
    for (const auto& vertex : modelMesh) {
        meshData[i + 0] = vertex.pos.x;
        meshData[i + 1] = vertex.pos.y;
        meshData[i + 2] = vertex.pos.z;
        meshData[i + 3] = vertex.color.x;
        meshData[i + 4] = vertex.color.y;
        meshData[i + 5] = vertex.color.z;
        meshData[i + 6] = vertex.uv.x;
        meshData[i + 7] = vertex.uv.y;
        i += 8;
    }

    vulkanBuffer->fillBuffer(vulkanDevice->getDevice(), meshData.data());

    VertexBuffer vertexBuffer {};
    vertexBuffer.vulkanBuffer = vulkanBuffer;
    vertexBuffer.model = model;
    
    VulkanBufferCreateInfo indexBufferInfo {};
    indexBufferInfo.device = vulkanDevice->getDevice();
    indexBufferInfo.physicalDevice = vulkanDevice->getPhysicalDevice();
    indexBufferInfo.bufferSize = model->getMeshIndiceSize() * sizeof(uint32_t);
    indexBufferInfo.usageFlags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    indexBufferInfo.propertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    VulkanBuffer* indexVulkanBuffer = new VulkanBuffer(&indexBufferInfo);

    std::vector<uint32_t> indices = model->getMeshIndices();
    indexVulkanBuffer->fillBuffer(vulkanDevice->getDevice(), indices.data());

    IndexBuffer indexBuffer {};
    indexBuffer.vulkanBuffer = indexVulkanBuffer;
    indexBuffer.model = model;

    ModelHandler modelHandler {};
    modelHandler.vertexBuffer = vertexBuffer;
    modelHandler.indexBuffer = indexBuffer;

    scene.push_back(modelHandler);
}

void Renderer::render() {
    VkFence inFlightFence = inFlightVulkanFences[currentFrame]->getFence();
    VkSemaphore imageAvailableSemaphore = imageAvailableVulkanSemaphores[currentFrame]->getSemaphore(); 
    VkSemaphore renderFinishedSemaphore = renderFinishedVulkanSemaphores[currentFrame]->getSemaphore();
    VkCommandBuffer currentCommandBuffer = vulkanCommandBufferHandler->getCommandBuffer(currentFrame);
    
    vkWaitForFences(vulkanDevice->getDevice(), 1, &inFlightFence, VK_TRUE, UINT64_MAX);
 
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(vulkanDevice->getDevice(), vulkanSwapChain->getSwapChain(), UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        // [NOTE] Add this function
        // recreateSwapChain();
        return;
    } 
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        spdlog::error("Failed to acquire swap chain image!");
        throw 0;
    }
    
    vkResetFences(vulkanDevice->getDevice(), 1, &inFlightFence);
    vkResetCommandBuffer(currentCommandBuffer, 0);
    
    std::vector<VkBuffer> vulkanVertexBuffers = {scene[0].vertexBuffer.vulkanBuffer->getBuffer()};
    VkBuffer vulkanIndexBuffer = scene[0].indexBuffer.vulkanBuffer->getBuffer();
    uint32_t numIndices = scene[0].indexBuffer.vulkanBuffer->getBufferSize() / sizeof(uint32_t);

    CommandBufferRecordInfo recordInfo {};
    recordInfo.bufferIndex = currentFrame;
    recordInfo.vertexBuffers = vulkanVertexBuffers;
    recordInfo.indexBuffer = vulkanIndexBuffer; // Crap
    recordInfo.numIndices = numIndices; 
    recordInfo.renderPass = vulkanRenderPass->getRenderPass();
    recordInfo.framebuffer = vulkanSwapChain->getFramebuffer(imageIndex);
    recordInfo.swapChainExtent = vulkanSwapChain->getExtent();
    recordInfo.graphicsPipeline = vulkanGraphicsPipeline->getGraphicsPipeline();
    recordInfo.pipelineLayout = vulkanGraphicsPipeline->getPipelineLayout();
    recordInfo.descriptorSet = vulkanDescriptorSetHandler->getDescriptorSet(currentFrame);
    vulkanCommandBufferHandler->recordCommandBuffer(&recordInfo); 
   
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &currentCommandBuffer;
    
    VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(vulkanDevice->getGraphicsQueue(), 1, &submitInfo, inFlightFence) != VK_SUCCESS) {
        spdlog::error("Failed to submit command buffer to graphics queue!");
        throw 0;
    }
    
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {vulkanSwapChain->getSwapChain()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = nullptr; // Optional

    result = vkQueuePresentKHR(vulkanDevice->getPresentQueue(), &presentInfo);
        
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        // recreateSwapChain();
    } 
    else if (result != VK_SUCCESS) {
        spdlog::error("Failed to present image to present queue!");
        throw 0;
    }
 
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::attachTextureToPipeline(VulkanTexture* texture) {
    currentTexture = texture;

    if (vulkanDescriptorSetHandler->getBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) < 0) {
        BindingCreateInfo textureBindingInfo {};
        textureBindingInfo.device = vulkanDevice->getDevice();
        textureBindingInfo.binding = bindingCount;
        textureBindingInfo.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        textureBindingInfo.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
        textureBindingInfo.maxFramesInFlight = MAX_FRAMES_IN_FLIGHT;
        vulkanDescriptorSetHandler->addBinding(&textureBindingInfo);
        bindingCount++;
    }

    updateDescriptorSets();
    shouldRecreatePipeline = true;
}

void Renderer::attachUniformBufferToPipeline(VulkanBuffer* uniformBuffer) {
    currentUniformBuffer = uniformBuffer;
   
    if (vulkanDescriptorSetHandler->getBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) < 0) {
        BindingCreateInfo uniformBindingInfo {};
        uniformBindingInfo.device = vulkanDevice->getDevice();
        uniformBindingInfo.binding = bindingCount;
        uniformBindingInfo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uniformBindingInfo.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
        uniformBindingInfo.maxFramesInFlight = MAX_FRAMES_IN_FLIGHT;
        vulkanDescriptorSetHandler->addBinding(&uniformBindingInfo);
        bindingCount++;
    }

    updateDescriptorSets();
    shouldRecreatePipeline = true;
}

void Renderer::updateDescriptorSets() {
    std::vector<DescriptorSetInfo> setInfos;

    // Uniform buffer
    if (currentUniformBuffer != nullptr) {
        VkDescriptorBufferInfo bufferInfo {};
        bufferInfo.buffer = currentUniformBuffer->getBuffer();
        bufferInfo.range = currentUniformBuffer->getBufferSize();
        bufferInfo.offset = 0;

        DescriptorSetInfo uniformSetInfo {};
        uniformSetInfo.binding = vulkanDescriptorSetHandler->getBinding(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
        uniformSetInfo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uniformSetInfo.bufferInfo = bufferInfo;

        setInfos.push_back(uniformSetInfo);
    }

    // Textures
    if (currentTexture != nullptr) {
        VkDescriptorImageInfo textureInfo {};
        textureInfo.sampler = currentTexture->getSampler();
        textureInfo.imageView = currentTexture->getImageView();
        textureInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        DescriptorSetInfo textureSetInfo {};
        textureSetInfo.binding = vulkanDescriptorSetHandler->getBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
        textureSetInfo.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        textureSetInfo.imageInfo = textureInfo;

        setInfos.push_back(textureSetInfo);       
    }

    DescriptorSetUpdateInfo setInfo {};
    setInfo.device = vulkanDevice->getDevice();
    setInfo.maxFramesInFlight = MAX_FRAMES_IN_FLIGHT;
    setInfo.setInfos = setInfos;

    vulkanDescriptorSetHandler->updateDescriptorSets(&setInfo);
}

VulkanBuffer* Renderer::getUniformBuffer(VkDeviceSize bufferSize) {
    VulkanBufferCreateInfo bufferInfo {};
    bufferInfo.device = vulkanDevice->getDevice();
    bufferInfo.physicalDevice = vulkanDevice->getPhysicalDevice();
    bufferInfo.bufferSize = bufferSize;
    bufferInfo.usageFlags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    bufferInfo.propertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    VulkanBuffer* uniformBuffer = new VulkanBuffer(&bufferInfo);

    return uniformBuffer;
}

void Renderer::updateUniformBufferData(VulkanBuffer* uniformBuffer, void* data) {
    uniformBuffer->fillBuffer(vulkanDevice->getDevice(), data);
}

void Renderer::transitionImageLayouts(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {
    VkCommandBuffer commandBuffer = vulkanCommandBufferHandler->beginSingleTimeCommands(vulkanDevice->getDevice());

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;
    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } 
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } 
    else {
        spdlog::error("Unsupported layout transition. Transition failed!");
        throw 0;
    }


    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    vulkanCommandBufferHandler->endSingleTimeCommands(vulkanDevice->getDevice(), vulkanDevice->getGraphicsQueue(), commandBuffer);
}

void Renderer::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t depth) {
    VkCommandBuffer commandBuffer = vulkanCommandBufferHandler->beginSingleTimeCommands(vulkanDevice->getDevice());
    
    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = {0, 0, 0};
    region.imageExtent = {width, height, depth};
    
    vkCmdCopyBufferToImage(
        commandBuffer,
        buffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &region
    );

    vulkanCommandBufferHandler->endSingleTimeCommands(vulkanDevice->getDevice(), vulkanDevice->getGraphicsQueue(), commandBuffer);
}

VulkanTexture* Renderer::getTexture(TextureCreateInfo* info) {
    VulkanTextureCreateInfo textureInfo {};
    textureInfo.device = vulkanDevice->getDevice();
    textureInfo.physicalDevice = vulkanDevice->getPhysicalDevice();
    textureInfo.imageType = info->imageType;
    textureInfo.imageViewType = info->imageViewType;
    textureInfo.width = info->width;
    textureInfo.height = info->height;
    textureInfo.depth = info->depth;
    textureInfo.data = info->data;
    textureInfo.imageSize = info->size;
    textureInfo.imageFormat = info->imageFormat;
    textureInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    VulkanTexture* texture = new VulkanTexture(&textureInfo);

    transitionImageLayouts(texture->getTextureImage(), info->imageFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufferToImage(texture->getBuffer(), texture->getTextureImage(), info->width, info->height, info->depth);
    transitionImageLayouts(texture->getTextureImage(), info->imageFormat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    return texture;
}

void Renderer::recreateGraphicsPipeline() {
    delete vulkanGraphicsPipeline;

    VulkanGraphicsPipelineCreateInfo vulkanGraphicsPipelineInfo {};
    vulkanGraphicsPipelineInfo.device = vulkanDevice->getDevice();
    vulkanGraphicsPipelineInfo.renderPass = vulkanRenderPass->getRenderPass(); 
    vulkanGraphicsPipelineInfo.swapChainExtent = vulkanSwapChain->getExtent();
    vulkanGraphicsPipelineInfo.vertexBindingDescription = Vertex::getBindingDescription();
    vulkanGraphicsPipelineInfo.vertexAttributeDescriptions = Vertex::getAttributeDescriptions();
    vulkanGraphicsPipelineInfo.descriptorSetLayoutCount = 1; 
    std::vector<VkDescriptorSetLayout> layouts = {vulkanDescriptorSetHandler->getDescriptorSetLayout()};
    vulkanGraphicsPipelineInfo.descriptorSetLayouts = layouts;
    vulkanGraphicsPipelineInfo.numSubPasses = 0;
    vulkanGraphicsPipelineInfo.vertexShaderCode = readFile("shaders/vert.spv");
    vulkanGraphicsPipelineInfo.fragmentShaderCode = readFile("shaders/frag.spv");
    vulkanGraphicsPipeline = new VulkanGraphicsPipeline(&vulkanGraphicsPipelineInfo);
}
