#include "Renderer.hpp"

Renderer::Renderer(RendererCreateInfo* info) {
    currentFrame = 0;

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

    // [NOTE] Use RendererCreateInfo to manipulate descriptors
    // Basic descriptor set for uniform buffers
    spdlog::info("Creating descriptor handler...");
    VulkanDescriptorSetHandlerCreateInfo descriptorHandlerInfo {};
    descriptorHandlerInfo.device = vulkanDevice->getDevice();
    descriptorHandlerInfo.binding = 0;
    descriptorHandlerInfo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorHandlerInfo.shaderStageFlag = VK_SHADER_STAGE_ALL_GRAPHICS;
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
    vulkanGraphicsPipelineInfo.vertexShaderCode = readFile("shaders/vert_raymarch.spv");
    vulkanGraphicsPipelineInfo.fragmentShaderCode = readFile("shaders/frag_raymarch.spv");
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
    std::vector<float> meshData(sizeof(float) * model->getMeshSize() * Vertex::getVertexSize());
    uint32_t i = 0;
    for (const auto& vertex : modelMesh) {
        meshData[i + 0] = vertex.pos.x;
        meshData[i + 1] = vertex.pos.y;
        meshData[i + 2] = vertex.pos.z;
        meshData[i + 3] = vertex.color.x;
        meshData[i + 4] = vertex.color.y;
        meshData[i + 5] = vertex.color.z;
        i += 6;
    }

    vulkanBuffer->fillBuffer(vulkanDevice->getDevice(), meshData.data());

    VertexBuffer vertexBuffer {};
    vertexBuffer.vulkanBuffer = vulkanBuffer;
    vertexBuffer.model = model;
    scene.push_back(vertexBuffer);
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
    
    uint32_t numPrimitives = 0;
    std::vector<VkBuffer> vulkanVertexBuffers;
    for (const auto& vertexBuffer : scene) {
        numPrimitives += vertexBuffer.model->getMeshSize() / 3;
        vulkanVertexBuffers.push_back(vertexBuffer.vulkanBuffer->getBuffer()); 
    }

    CommandBufferRecordInfo recordInfo {};
    recordInfo.bufferIndex = currentFrame;
    recordInfo.vertexBuffers = vulkanVertexBuffers;
    recordInfo.renderPass = vulkanRenderPass->getRenderPass();
    recordInfo.framebuffer = vulkanSwapChain->getFramebuffer(imageIndex);
    recordInfo.swapChainExtent = vulkanSwapChain->getExtent();
    recordInfo.graphicsPipeline = vulkanGraphicsPipeline->getGraphicsPipeline();
    recordInfo.pipelineLayout = vulkanGraphicsPipeline->getPipelineLayout();
    recordInfo.descriptorSet = vulkanDescriptorSetHandler->getDescriptorSet(currentFrame);
    recordInfo.numPrimitives = numPrimitives;
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

void Renderer::attachUniformBufferToPipeline(VulkanBuffer* uniformBuffer) {
    // Add uniform buffer to vector
    uniformBuffers.push_back(uniformBuffer);

    // Descriptor set recreation
    DescriptorSetCreateInfo setInfo {};
    setInfo.device = vulkanDevice->getDevice();
    setInfo.maxFramesInFlight = MAX_FRAMES_IN_FLIGHT;

    std::vector<VkBuffer> uBuffers;
    std::vector<VkDeviceSize> uBufferSizes;
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VulkanBuffer* uniformBuffer = uniformBuffers.back();
        uBuffers.push_back(uniformBuffer->getBuffer());
        uBufferSizes.push_back(uniformBuffer->getBufferSize());
    }

    setInfo.uniformBuffers = uBuffers;
    setInfo.uniformBufferSizes = uBufferSizes;     
    vulkanDescriptorSetHandler->updateDescriptorSets(&setInfo);       

    // Pipeline recreation
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
    vulkanGraphicsPipelineInfo.vertexShaderCode = readFile("shaders/vert_raymarch.spv");
    vulkanGraphicsPipelineInfo.fragmentShaderCode = readFile("shaders/frag_raymarch.spv");
    vulkanGraphicsPipeline = new VulkanGraphicsPipeline(&vulkanGraphicsPipelineInfo);
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
