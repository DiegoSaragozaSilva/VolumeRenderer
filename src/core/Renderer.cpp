#include "Renderer.hpp"

Renderer::Renderer(RendererCreateInfo* info) {
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
    // Basic descriptor set layout for uniform buffers
    spdlog::info("Creating basic uniform descriptor set layout...");
    VulkanDescriptorSetLayoutCreateInfo uniformDescriptorInfo {};
    uniformDescriptorInfo.device = vulkanDevice->getDevice();
    uniformDescriptorInfo.binding = 0;
    uniformDescriptorInfo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uniformDescriptorInfo.shaderStageFlag = VK_SHADER_STAGE_ALL_GRAPHICS;
    vulkanDescriptorSetLayouts.push_back(new VulkanDescriptorSetLayout(&uniformDescriptorInfo));
    spdlog::info("Basic uniform descriptor set layout successfully created!");

    // Basic graphics pipeline creation
    spdlog::info("Creating basic vulkan graphics pipeline...");
    VulkanGraphicsPipelineCreateInfo vulkanGraphicsPipelineInfo {};
    vulkanGraphicsPipelineInfo.device = vulkanDevice->getDevice();
    vulkanGraphicsPipelineInfo.renderPass = vulkanRenderPass->getRenderPass(); 
    vulkanGraphicsPipelineInfo.swapChainExtent = vulkanSwapChain->getExtent();
    vulkanGraphicsPipelineInfo.vertexBindingDescription = Vertex::getBindingDescription();
    vulkanGraphicsPipelineInfo.vertexAttributeDescriptions = Vertex::getAttributeDescriptions();
    vulkanGraphicsPipelineInfo.descriptorSetLayoutCount = vulkanDescriptorSetLayouts.size();
    
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
    for (const auto& vulkanDescriptorSetLayout : vulkanDescriptorSetLayouts) {
        descriptorSetLayouts.push_back(vulkanDescriptorSetLayout->getDescriptorSetLayout());
    }

    vulkanGraphicsPipelineInfo.descriptorSetLayouts = descriptorSetLayouts;
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
