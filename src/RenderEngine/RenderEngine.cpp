#include "RenderEngine.hpp"

#include <fstream>
#include <iterator>

RenderEngine::RenderEngine() {
    // Initializing all basic componentes and Vulkan
    initWindow();
    initVulkan();
    initImgui();

    // Initialize the main camera
    camera = new Camera();
    camera->setPosition({0.0f, 0.0f, 0.0f});
    camera->setUpVector({0.0f, 1.0f, 0.0f});
    camera->setFrontVector({0.0f, 0.0f, -1.0f});
    camera->setFOV(60.0f);
    camera->setNearPlane(0.1f);
    camera->setFarPlane(1000.0f);
    camera->generateViewMatrix();
    camera->generateProjectionMatrix();

    // Initialize the texture pool and required the default texture
    texturePool = new TexturePool();
    texturePool->requireTexture(vulkan.device, vulkan.commandPool, "assets/textures/default.png");

    // Initialize the UIStates
    uiStates.showCameraProperties = false;
    uiStates.showDebugStructures = false;
    uiStates.octreeTargetDepth = 5;

    // Initialize time data
    deltaTime = 0.0;
    lastTime = 0.0;

    // Initialize octree
    targetOctree = nullptr;

    #ifndef NDEBUG
        spdlog::info("Render engine successfully initialized");
    #endif
}

RenderEngine::~RenderEngine() {
    // Destroy the scene
    clearScene();

    // Default pipeline destruction
    deletePipeline(render.defaultPipeline);

    // Terminate ImGui
    ImGui::DestroyContext();

    // Render pass destruction
    vulkan.device->destroyRenderPass(render.renderPass->getRenderPass());
    delete render.renderPass;

    // Swapchain destruction
    std::vector<vk::ImageView> swapchainImageViews = render.swapchain->getImageViews();
    for (vk::ImageView imageView : swapchainImageViews) {
        vulkan.device->destroyImageView(&imageView);
    }
    vulkan.device->destroySwapchain(render.swapchain->getSwapchain());
    delete render.swapchain;

    // Command pool destruction
    vulkan.device->destroyCommandPool(vulkan.commandPool->getCommandPool());
    delete vulkan.commandPool;

    // Window destruction
    vulkan.instance->destroySurface(window->getSurface(vulkan.instance->getInstance()));
    delete window;

    // Multisample image, multisample image view, depth image and depth image view destruction
    vulkan.device->destroyImageView(vulkan.multiSampleImageView->getImageView());
    vulkan.device->freeDeviceMemory(vulkan.multiSampleImage->getImageMemory());
    vulkan.device->destroyImage(vulkan.multiSampleImage->getImage());
    delete vulkan.multiSampleImageView;
    delete vulkan.multiSampleImage;

    vulkan.device->destroyImageView(vulkan.depthImageView->getImageView());
    vulkan.device->freeDeviceMemory(vulkan.depthImage->getImageMemory());
    vulkan.device->destroyImage(vulkan.depthImage->getImage());
    delete vulkan.depthImageView;
    delete vulkan.depthImage;

    // Framebuffers destruction
    for (vk::Framebuffer framebuffer : vulkan.framebuffers)
        vulkan.device->destroyFramebuffer(framebuffer);
    vulkan.framebuffers.clear();
    #ifndef NDEBUG
        spdlog::info("Vulkan framebuffers successfully destroyed.");
    #endif
   
    // Sync objects destruction
    for (vk::Semaphore semaphore : vulkan.graphicsSemaphores)
        vulkan.device->destroySemaphore(semaphore);
    for (vk::Semaphore semaphore : vulkan.presentationSemaphores)
        vulkan.device->destroySemaphore(semaphore);
    for (vk::Fence fence : vulkan.graphicsFences)
        vulkan.device->destroyFence(fence);

    // Texture pool cleanup
    std::map<std::string, Texture*> pool = texturePool->getPool();
    for (const auto& poolItem : pool)
        deleteTexture(poolItem.second);

    // Device destruction 
    delete vulkan.device;

    // Instance destruction
    delete vulkan.instance;

    #ifndef NDEBUG
        spdlog::info("Render engine successfully destroyed");
    #endif
}

void RenderEngine::initWindow() {
    // Window and GLFW initialization
    window = new Window(800, 600, "Render Engine");
}

void RenderEngine::initImgui() {
    // Creating a descriptor pool for the library
    vk::DescriptorPoolSize poolSizes[] = {
        vk::DescriptorPoolSize(vk::DescriptorType::eSampler, 1000),
		vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, 1000),
		vk::DescriptorPoolSize(vk::DescriptorType::eSampledImage, 1000),
		vk::DescriptorPoolSize(vk::DescriptorType::eStorageImage, 1000),
		vk::DescriptorPoolSize(vk::DescriptorType::eUniformTexelBuffer, 1000),
		vk::DescriptorPoolSize(vk::DescriptorType::eStorageTexelBuffer, 1000),
		vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, 1000),
		vk::DescriptorPoolSize(vk::DescriptorType::eStorageBuffer, 1000),
		vk::DescriptorPoolSize(vk::DescriptorType::eUniformBufferDynamic, 1000),
		vk::DescriptorPoolSize(vk::DescriptorType::eStorageBufferDynamic, 1000),
		vk::DescriptorPoolSize(vk::DescriptorType::eInputAttachment, 1000)
	};

    vk::DescriptorPoolCreateInfo poolInfo (
        vk::DescriptorPoolCreateFlags(),
        1000,
        std::size(poolSizes),
        poolSizes
    );

    vk::DescriptorPool imguiPool = vulkan.device->getLogicalDevice()->createDescriptorPool(poolInfo);

	// Initialize the library
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForVulkan(window->getWindow(), true);

	ImGui_ImplVulkan_InitInfo initInfo = {};
	initInfo.Instance = *(vulkan.instance->getInstance());
	initInfo.PhysicalDevice = *(vulkan.device->getPhysicalDevice());
	initInfo.Device = *(vulkan.device->getLogicalDevice());
	initInfo.Queue = vulkan.device->getGraphicsQueue();
	initInfo.DescriptorPool = imguiPool;
	initInfo.MinImageCount = 3;
	initInfo.ImageCount = 3;
	initInfo.MSAASamples = (VkSampleCountFlagBits)vulkan.device->getMultiSamplingLevel();
	ImGui_ImplVulkan_Init(&initInfo, *(render.renderPass->getRenderPass()));

    // Upload Imgui fonts
    vk::CommandBuffer commandBuffer = vulkan.commandPool->beginCommandBuffer(*(vulkan.device->getLogicalDevice()));
    ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
    vulkan.commandPool->endCommandBuffer(commandBuffer, vulkan.device);
    ImGui_ImplVulkan_DestroyFontUploadObjects();
}

void RenderEngine::initVulkan() {
    // Vulkan instance initialization
    vulkan.instance = new Instance("Render Engine", "Render Engine", window->getGLFWExtensions());    

    // Vulkan device initialization
    vulkan.device = new Device(vulkan.instance->getInstance(), window->getSurface(vulkan.instance->getInstance()));

    // Vulkan command pool initialization
    vulkan.commandPool = new CommandPool(vulkan.device);

    // Vulkan swapchain initialization
    render.swapchain = new Swapchain(vulkan.device, window->getSurface(vulkan.instance->getInstance()), window->getWidth(), window->getHeight());

    // Vulkan multisampling image initialization
    vulkan.multiSampleImage = createMultiSampleImage();

    // Vulkan multisampling image view initialization
    vulkan.multiSampleImageView = createImageView(vulkan.multiSampleImage, vk::ImageAspectFlagBits::eColor);

    // Vulkan depth image initialization
    vulkan.depthImage = createDepthImage();

    // Vulkan depth image view initialization
    vulkan.depthImageView = createImageView(vulkan.depthImage, vk::ImageAspectFlagBits::eDepth);

    // Vulkan render pass initialization
    render.renderPass = new RenderPass(vulkan.device, render.swapchain);

    // Vulkan framebuffers initialization
    vulkan.framebuffers = createFramebuffers();

    // Vulkan render command buffers initialization
    vulkan.commandBuffers = vulkan.commandPool->createCommandBuffers(vulkan.device, render.swapchain->getImageCount());

    // Max render frames
    vulkan.maxRenderFrames = 2;

    // Current frame index and swapchain image index
    vulkan.currentFrameIndex = 0;
    vulkan.currentSwapchainImageIndex = 0;

    // Graphics and presentation semaphores initialization
    vulkan.graphicsSemaphores = vulkan.device->createSemaphores(vulkan.maxRenderFrames);
    vulkan.presentationSemaphores = vulkan.device->createSemaphores(vulkan.maxRenderFrames);

    // Graphics fences initialization
    vulkan.graphicsFences = vulkan.device->createFences(vulkan.maxRenderFrames);

    // Swapchain scissor initialization
    vulkan.scissor = createScissor();

    // Swapchain viewport initialization
    vulkan.viewport = createViewport();

    // Render clear colors creation
    vulkan.clearValues = createClearValues();

    // Default shaders initialization
    render.defaultShaders.push_back(new ShaderModule(vulkan.device, Utils::loadShaderCode("assets/shaders/default.vert.spv"), vk::ShaderStageFlagBits::eVertex, 0, 64));
    render.defaultShaders.push_back(new ShaderModule(vulkan.device, Utils::loadShaderCode("assets/shaders/default.frag.spv"), vk::ShaderStageFlagBits::eFragment, 64, 32));

    // Voxel shaders initialization
    render.voxelShaders.push_back(new ShaderModule(vulkan.device, Utils::loadShaderCode("assets/shaders/voxel.geom.spv"), vk::ShaderStageFlagBits::eGeometry, 0, 80));
    render.voxelShaders.push_back(new ShaderModule(vulkan.device, Utils::loadShaderCode("assets/shaders/voxel.vert.spv"), vk::ShaderStageFlagBits::eVertex, 0, 0));
    render.voxelShaders.push_back(new ShaderModule(vulkan.device, Utils::loadShaderCode("assets/shaders/voxel.frag.spv"), vk::ShaderStageFlagBits::eFragment, 80, 32));

    // Debug shaders initialization
    render.debugShaders.push_back(new ShaderModule(vulkan.device, Utils::loadShaderCode("assets/shaders/debug.vert.spv"), vk::ShaderStageFlagBits::eVertex, 0, 64));
    render.debugShaders.push_back(new ShaderModule(vulkan.device, Utils::loadShaderCode("assets/shaders/debug.frag.spv"), vk::ShaderStageFlagBits::eFragment, 0, 0));

    // Default pipeline initialization
    VertexInputDescription vertexDescription = Vertex::getVertexDescription();
    render.defaultPipeline = new Pipeline(
        vulkan.device,
        render.renderPass,
        render.defaultShaders,
        vertexDescription.bindings,
        vertexDescription.attributes,
        vk::PrimitiveTopology::eTriangleList,
        vk::PolygonMode::eFill,
        vulkan.viewport,
        vulkan.scissor,
        1.0f
    );

    // Voxel pipeline initialization
    render.voxelPipeline = new Pipeline(
        vulkan.device,
        render.renderPass,
        render.voxelShaders,
        vertexDescription.bindings,
        vertexDescription.attributes,
        vk::PrimitiveTopology::ePointList,
        vk::PolygonMode::eFill,
        vulkan.viewport,
        vulkan.scissor,
        1.0f
    );

    // Debug pipeline initialization
    render.debugPipeline = new Pipeline(
        vulkan.device,
        render.renderPass,
        render.debugShaders,
        vertexDescription.bindings,
        vertexDescription.attributes,
        vk::PrimitiveTopology::eLineList,
        vk::PolygonMode::eFill,
        vulkan.viewport,
        vulkan.scissor,
        1.0f
    );

    // Default material initialization
    render.defaultMaterial.diffuseTextureMap = "assets/textures/default.png";
}

Image* RenderEngine::createMultiSampleImage() {
    // Get extent from the swapchain
    vk::Extent2D extent = render.swapchain->getExtent();

    // Construct and return the multisample image
    return new Image(
        vulkan.device,
        vulkan.commandPool,
        extent.width,
        extent.height,
        1,
        vulkan.device->getMultiSamplingLevel(),
        render.swapchain->getColorFormat(),
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eColorAttachmentOptimal
    );
}

Image* RenderEngine::createDepthImage() {
    // Get extent from the swapchain
    vk::Extent2D extent = render.swapchain->getExtent();

    // Construct and return the depth image
    return new Image(
        vulkan.device,
        vulkan.commandPool,
        extent.width,
        extent.height,
        1,
        vulkan.device->getMultiSamplingLevel(),
        vulkan.device->getDepthFormat(),
        vk::ImageTiling::eOptimal,
        vk::ImageUsageFlagBits::eDepthStencilAttachment,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        vk::ImageLayout::eUndefined,
        vk::ImageLayout::eDepthStencilAttachmentOptimal
    );
}

ImageView* RenderEngine::createImageView(Image* image, vk::ImageAspectFlags aspectFlags) {
    // Create and return an image view for an image
    return new ImageView(
        vulkan.device->getLogicalDevice(),
        image->getImage(),
        vk::ImageViewType::e2D,
        image->getFormat(),
        aspectFlags,
        image->getMipmapLevels()
    );
}

std::vector<vk::Framebuffer> RenderEngine::createFramebuffers() {
    std::vector<vk::Framebuffer> framebuffers;

    // Get swapchain extent
    vk::Extent2D extent = render.swapchain->getExtent();

    // For each swapchain image view, create an image view that encapsulates all the images from the vulkan context
    std::vector<vk::ImageView> swapchainImageViews = render.swapchain->getImageViews();
    for (vk::ImageView swapchainImageView : swapchainImageViews) {
        std::vector<vk::ImageView> attachments = {
            *(vulkan.multiSampleImageView->getImageView()),
            *(vulkan.depthImageView->getImageView()),
            swapchainImageView
        };

        vk::FramebufferCreateInfo framebufferCreateInfo (
            vk::FramebufferCreateFlags(),
            *(render.renderPass->getRenderPass()),
            attachments.size(),
            attachments.data(),
            extent.width,
            extent.height,
            1
        );

        framebuffers.push_back(vulkan.device->getLogicalDevice()->createFramebuffer(framebufferCreateInfo));
    }

    #ifndef NDEBUG
        spdlog::info("Vulkan framebuffers successfully created.");
    #endif

    return framebuffers;
}

vk::Rect2D RenderEngine::createScissor() {
    // Swapchain scissor creation and return
    vk::Offset2D offset = {0, 0};
    return vk::Rect2D (offset, render.swapchain->getExtent());
}

vk::Viewport RenderEngine::createViewport() {
    // Swapchain viewport creation and return
    vk::Extent2D extent = render.swapchain->getExtent();
    return vk::Viewport (0.0f, 0.0f, extent.width, extent.height, 0.0f, 1.0f);
}

std::vector<vk::ClearValue> RenderEngine::createClearValues() {
    // Sky clear color
    vk::ClearValue skyColor;
    skyColor.color = vk::ClearColorValue(std::array<float, 4> {
        135.f / 255.f, 206.f / 255.f, 235.f / 255.f, 1.0f
    });

    // Depth clear color
    vk::ClearValue depthColor;
    depthColor.depthStencil = vk::ClearDepthStencilValue(1.0f, 0.0f);

    return std::vector<vk::ClearValue> {skyColor, depthColor};
}

uint32_t RenderEngine::getNextImageIndex(vk::Fence fence, vk::Semaphore semaphore) {
    // Wait for the render fence for some timeout time
    uint64_t timeout = std::numeric_limits<uint64_t>::max();
    vulkan.device->getLogicalDevice()->waitForFences(1, &fence, VK_TRUE, timeout);

    // After waiting, reset the fence for future use
    vulkan.device->getLogicalDevice()->resetFences(1, &fence);

    // Get next swapchain image index and return it
    vk::ResultValue nextImageIndex = vulkan.device->getLogicalDevice()->acquireNextImageKHR(
        *(render.swapchain->getSwapchain()),
        timeout,
        semaphore,
        nullptr
    );

    return nextImageIndex.value;
}

void RenderEngine::recreateRenderContext() {
    // Wait for the device to be idle
    vulkan.device->getLogicalDevice()->waitIdle();

    // Force window poll events
    window->pollEvents();

    // Destroy the old swapchain and render pass
    vulkan.device->destroyRenderPass(render.renderPass->getRenderPass());
    delete render.renderPass;

    std::vector<vk::ImageView> swapchainImageViews = render.swapchain->getImageViews();
    for (vk::ImageView imageView : swapchainImageViews) {
        vulkan.device->destroyImageView(&imageView);
    }
    vulkan.device->destroySwapchain(render.swapchain->getSwapchain());
    delete render.swapchain;

    // Destroy old framebuffers and image views
    vulkan.device->destroyImageView(vulkan.multiSampleImageView->getImageView());
    vulkan.device->freeDeviceMemory(vulkan.multiSampleImage->getImageMemory());
    vulkan.device->destroyImage(vulkan.multiSampleImage->getImage());
    delete vulkan.multiSampleImageView;
    delete vulkan.multiSampleImage;

    vulkan.device->destroyImageView(vulkan.depthImageView->getImageView());
    vulkan.device->freeDeviceMemory(vulkan.depthImage->getImageMemory());
    vulkan.device->destroyImage(vulkan.depthImage->getImage());
    delete vulkan.depthImageView;
    delete vulkan.depthImage;

    for (vk::Framebuffer framebuffer : vulkan.framebuffers)
        vulkan.device->destroyFramebuffer(framebuffer);
    vulkan.framebuffers.clear();
    #ifndef NDEBUG
        spdlog::info("Vulkan framebuffers successfully destroyed.");
    #endif

    // Recreate the render context swapchain and render pass
    render.swapchain = new Swapchain(vulkan.device, window->getSurface(vulkan.instance->getInstance()), window->getWidth(), window->getHeight(), nullptr);
    render.renderPass = new RenderPass(vulkan.device, render.swapchain);

    // Recreate the image views
    vulkan.multiSampleImage = createMultiSampleImage();
    vulkan.multiSampleImageView = createImageView(vulkan.multiSampleImage, vk::ImageAspectFlagBits::eColor);
    vulkan.depthImage = createDepthImage();
    vulkan.depthImageView = createImageView(vulkan.depthImage, vk::ImageAspectFlagBits::eDepth);

    // Recreate the framebuffers
    vulkan.framebuffers = createFramebuffers();

    // Recreate the viewport and scissor and signal the pipeline dynamic state
    vulkan.scissor = createScissor();
    vulkan.viewport = createViewport();

    // Update camera aspect ratio and projection matrix
    camera->setAspectRatio((float)window->getWidth() / (float)window->getHeight());
    camera->generateProjectionMatrix();
}

void RenderEngine::renderFrame() {
    // Window poll events
    window->pollEvents();

    // Update delta time
    double currentTime = window->getTime();
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    // Calculate model view projection matrix
    glm::mat4 modelMatrix = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)), glm::vec3(1.0f, 1.0f, 1.0f));
    glm::mat4 mvp = camera->getProjectionMatrix() * camera->getViewMatrix() * modelMatrix;

    // Get the camera position and direction
    glm::vec3 _camPosition = camera->getPosition();
    glm::vec3 _camDirection = camera->getFrontVector();
    glm::vec4 camPosition = glm::vec4(_camPosition.x, _camPosition.y, _camPosition.z, 1.0f);
    glm::vec4 camDirection = glm::vec4(_camDirection.x, _camDirection.y, _camDirection.z, 1.0f);

    // Get Octree information
    glm::vec4 octreeData = glm::vec4(0.0f);
    if (targetOctree != nullptr) {
        float octreeDepth = (float)uiStates.octreeTargetDepth;
        glm::vec4 octreeAABBMin = glm::vec4(targetOctree->getRoot()->getVoxel().aabb.min, 1.0f);
        glm::vec4 octreeAABBMax = glm::vec4(targetOctree->getRoot()->getVoxel().aabb.max, 1.0f);
        octreeData = octreeAABBMax - octreeAABBMin;
        octreeData.w = octreeDepth;
    }

    // Start Imgui frame
    ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();

    // Render Imgui UI
    renderUI();

    // Imgui render
    ImGui::Render();

    // Begin frame rendering
    bool beginStatus = renderBegin();
    if (!beginStatus)
        recreateRenderContext();

    // Bind the default pipeline
    vk::CommandBuffer commandBuffer = vulkan.commandBuffers[vulkan.currentSwapchainImageIndex];
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, render.defaultPipeline->getPipeline());

    // Fill the push constants struct and send it
    PushConstants pushConstants = {
        mvp,
        octreeData,
        camPosition,
        camDirection
    };

    vk::PushConstantRange range = render.defaultPipeline->getPushConstantRange(vk::ShaderStageFlagBits::eVertex);
    commandBuffer.pushConstants (
        render.defaultPipeline->getPipelineLayout(),
        vk::ShaderStageFlagBits::eVertex,
        0,
        64,
        &mvp
    );

    FragmentConstants fragmentConstants = {
        .viewPosition = camPosition,
        .viewDirection = camDirection
    };
    range = render.defaultPipeline->getPushConstantRange(vk::ShaderStageFlagBits::eFragment);
    commandBuffer.pushConstants (
        render.defaultPipeline->getPipelineLayout(),
        vk::ShaderStageFlagBits::eFragment,
        64,
        32,
        &fragmentConstants
    );

    // For each mesh in scene bind it and send descriptors
    for (Mesh* mesh : scene) {
        // Bind the mesh vertex and index buffers
        vk::DeviceSize offsets[]{0};
        vk::Buffer meshVertexBuffer = mesh->getVertexBuffer()->getBuffer();
        commandBuffer.bindVertexBuffers(0, 1, &meshVertexBuffer, offsets);
        commandBuffer.bindIndexBuffer(mesh->getIndexBuffer()->getBuffer(), 0, vk::IndexType::eUint32);

        // Draw indexed per material based configuration
        uint32_t indexStart = 0;
        std::vector<Material> meshMaterials = mesh->getMaterials();
        if (meshMaterials.size() > 0) {
            for (Material material : meshMaterials) {
                // Get texture descriptor set
                vk::DescriptorSet materialTextureSamplerDescriptorSet = render.defaultPipeline->getTextureSamplerDescriptorSet(
                        vulkan.device,
                        texturePool->requireTexture(vulkan.device, vulkan.commandPool, material.diffuseTextureMap)
                );

                // Bind texture descriptor set
                commandBuffer.bindDescriptorSets(
                    vk::PipelineBindPoint::eGraphics,
                    render.defaultPipeline->getPipelineLayout(),
                    0,
                    1,
                    &materialTextureSamplerDescriptorSet,
                    0,
                    nullptr
                );

                commandBuffer.drawIndexed(material.indexCount, 1, 0, indexStart, 0);
                indexStart += material.indexCount;
            }
        }
        else {
            // Get texture descriptor set
            vk::DescriptorSet materialTextureSamplerDescriptorSet = render.defaultPipeline->getTextureSamplerDescriptorSet(
                    vulkan.device,
                    texturePool->requireTexture(vulkan.device, vulkan.commandPool, render.defaultMaterial.diffuseTextureMap)
            );

            // Bind texture descriptor set
            commandBuffer.bindDescriptorSets(
                vk::PipelineBindPoint::eGraphics,
                render.defaultPipeline->getPipelineLayout(),
                0,
                1,
                &materialTextureSamplerDescriptorSet,
                0,
                nullptr
            );

            commandBuffer.drawIndexed(mesh->getNumIndices(), 1, 0, 0, 0);
        }
    }

    // Bind volume pipeline
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, render.voxelPipeline->getPipeline());

    // Push constants
    GeometryConstants geometryConstants = {
        .mvp = mvp,
        .octreeData = octreeData
    };
    range = render.voxelPipeline->getPushConstantRange(vk::ShaderStageFlagBits::eGeometry);
    commandBuffer.pushConstants (
        render.voxelPipeline->getPipelineLayout(),
        vk::ShaderStageFlagBits::eGeometry,
        0,
        sizeof(geometryConstants),
        &geometryConstants
    );

    range = render.voxelPipeline->getPushConstantRange(vk::ShaderStageFlagBits::eFragment);
    commandBuffer.pushConstants (
        render.voxelPipeline->getPipelineLayout(),
        vk::ShaderStageFlagBits::eFragment,
        80,
        32,
        &fragmentConstants
    );

    for (Mesh* mesh : voxelScene) {
        vk::DeviceSize offsets[]{0};
        vk::Buffer volumeVertexBuffer = mesh->getVertexBuffer()->getBuffer();
        commandBuffer.bindVertexBuffers(0, 1, &volumeVertexBuffer, offsets);
        commandBuffer.bindIndexBuffer(mesh->getIndexBuffer()->getBuffer(), 0, vk::IndexType::eUint32);

        // Draw indexed
        commandBuffer.drawIndexed(mesh->getNumIndices(), 1, 0, 0, 0);
    }

    if (uiStates.showDebugStructures) {
        // Bind debug pipeline
        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, render.debugPipeline->getPipeline());

        // Push constants
        commandBuffer.pushConstants (
            render.debugPipeline->getPipelineLayout(),
            vk::ShaderStageFlagBits::eVertex,
            0,
            64,
            &mvp
        );

        // Bind the mesh vertex and index buffers
        for (Mesh* mesh : debugScene) {
            vk::DeviceSize offsets[]{0};
            vk::Buffer debugVertexBuffer = mesh->getVertexBuffer()->getBuffer();
            commandBuffer.bindVertexBuffers(0, 1, &debugVertexBuffer, offsets);
            commandBuffer.bindIndexBuffer(mesh->getIndexBuffer()->getBuffer(), 0, vk::IndexType::eUint32);

            // Draw indexed
            commandBuffer.drawIndexed(mesh->getNumIndices(), 1, 0, 0, 0);
        }
    }

    // Imgui end render
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);

    // End frame rendering
    bool endStatus = renderEnd();
    if (!endStatus)
        recreateRenderContext();
}

void RenderEngine::renderUI() {
    // Start frame and UI window
    ImGui::NewFrame();

    // Main menu bar
    if (ImGui::BeginMainMenuBar()) {
        ImGui::Text("Render Engine (RE)");
        ImGui::Separator();
        if (ImGui::BeginMenu("Scene")) {
            if (ImGui::BeginMenu("Load OBJ")) {
                std::vector<std::string> objFiles = Utils::listFolderFiles("assets/objs");
                for (const auto& file : objFiles)
                    if (ImGui::MenuItem(file.c_str()))
                        addOBJToScene(file);
                ImGui::EndMenu();
            }

            if (ImGui::MenuItem("Reload shaders")) {
                spdlog::warn("Not implemented yet!");
            }

            if (ImGui::MenuItem("Clear scene"))
                clearScene();

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Voxelizer")) {
            if (ImGui::BeginMenu("Voxelize OBJ")) {
                std::vector<std::string> objFiles = Utils::listFolderFiles("assets/objs");
                for (const auto& file : objFiles)
                    if (ImGui::MenuItem(file.c_str()))
                        addVoxelizedOBJToScene(file);
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Options")) {
            ImGui::Checkbox("Show camera properties", &uiStates.showCameraProperties);
            ImGui::Checkbox("Show debug structures", &uiStates.showDebugStructures);
            ImGui::InputInt("Voxel scale", &Voxelizer::scale);
            ImGui::InputInt("Voxel density", &Voxelizer::density);
            ImGui::SliderInt("Octree rendering depth", &uiStates.octreeTargetDepth, 1, 10); 
            ImGui::EndMenu();
        }
        
        // FPS Counter
        std::string fpsStr = std::to_string(1000.0f / ImGui::GetIO().Framerate) + " ms/frame | " + std::to_string(ImGui::GetIO().Framerate) + " FPS";
        float fpsStrSize = ImGui::CalcTextSize(fpsStr.c_str()).x;
        ImGui::SameLine(ImGui::GetWindowWidth() - fpsStrSize - 20.0f);
        ImGui::Text(fpsStr.c_str());

        ImGui::EndMainMenuBar();
    }

    // Camera properties window
    if (uiStates.showCameraProperties) {
        glm::vec3 position = camera->getPosition();
        glm::vec3 upVector = camera->getUpVector();
        glm::vec3 frontVector = camera->getFrontVector();
        float yaw = camera->getYaw();
        float pitch = camera->getPitch();

        std::string positionStr = "Position: " + std::to_string(position.x) + " " + std::to_string(position.y) + " " + std::to_string(position.z);
        std::string frontVectorStr = "Front: " + std::to_string(frontVector.x) + " " + std::to_string(frontVector.y) + " " + std::to_string(frontVector.z);
        std::string upVectorStr = "Up: " + std::to_string(upVector.x) + " " + std::to_string(upVector.y) + " " + std::to_string(upVector.z);
        std::string rotationStr = "Rotation: " + std::to_string(yaw) + " " + std::to_string(pitch) + " " + std::to_string(0.0f);

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoCollapse;
        ImGui::Begin("Camera properties", &uiStates.showCameraProperties, windowFlags);
        ImGui::Text(positionStr.c_str());
        ImGui::Text(rotationStr.c_str());
        ImGui::Text(frontVectorStr.c_str());
        ImGui::Text(upVectorStr.c_str());
        ImGui::End();
    }

    ImGui::EndFrame();
}

bool RenderEngine::renderBegin() {
    // Get the right graphics fence and semaphore
    vk::Fence graphicsFence = vulkan.graphicsFences[vulkan.currentFrameIndex];
    vk::Semaphore graphicsSemaphore = vulkan.graphicsSemaphores[vulkan.currentFrameIndex];

    // Acquire the next swapchain image index
    // If the swapchain is outdated, it needs to be recreated
    try {
        vulkan.currentSwapchainImageIndex = getNextImageIndex(graphicsFence, graphicsSemaphore);
    }
    catch (vk::OutOfDateKHRError outOfDateError) {
        return false;
    }

    // Prepare the image command buffer for drawing
    vk::CommandBuffer commandBuffer = vulkan.commandBuffers[vulkan.currentSwapchainImageIndex];
    commandBuffer.reset(vk::CommandBufferResetFlagBits::eReleaseResources);

    // Begin the command buffer
    vk::CommandBufferBeginInfo commandBufferBeginInfo (
        vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
        nullptr
    );
    commandBuffer.begin(&commandBufferBeginInfo);

    // Scissor config
    commandBuffer.setScissor (
        0,
        1,
        &vulkan.scissor
    );

    // Viewport config
    commandBuffer.setViewport (
        0,
        1,
        &vulkan.viewport
    );

    // Begin the command buffer record with the render pass
    vk::RenderPassBeginInfo renderPassBeginInfo (
        *(render.renderPass->getRenderPass()),
        vulkan.framebuffers[vulkan.currentSwapchainImageIndex],
        vulkan.scissor,
        2,
        vulkan.clearValues.data()
    );

    // Record the command buffer command
    commandBuffer.beginRenderPass(
        &renderPassBeginInfo,
        vk::SubpassContents::eInline
    );

    return true;
}

bool RenderEngine::renderEnd() {
    // Get the current command buffer
    vk::CommandBuffer commandBuffer = vulkan.commandBuffers[vulkan.currentSwapchainImageIndex];

    // End the command buffer recording
    commandBuffer.endRenderPass();
    commandBuffer.end();

    // Prepare and submit the command buffer to the graphics queue
    vk::Fence graphicsFence = vulkan.graphicsFences[vulkan.currentFrameIndex];
    vk::Semaphore graphicsSemaphore = vulkan.graphicsSemaphores[vulkan.currentFrameIndex];
    vk::Semaphore presentationSemaphore = vulkan.presentationSemaphores[vulkan.currentFrameIndex];
    vk::PipelineStageFlags pipelineStageFlags = vk::PipelineStageFlagBits::eColorAttachmentOutput;

    // Create a submit info for the graphics queue
    std::vector<vk::CommandBuffer> commandBuffers = {commandBuffer};
    vk::SubmitInfo submitInfo (
        1,
        &graphicsSemaphore,
        &pipelineStageFlags,
        1,
        commandBuffers.data(),
        1,
        &presentationSemaphore
    );

    // Submit the command buffer
    vulkan.device->getGraphicsQueue().submit(1, &submitInfo, graphicsFence);

    // Create a present info for the presentation queue
    vk::PresentInfoKHR presentationInfo (
        1,
        &presentationSemaphore,
        1,
        render.swapchain->getSwapchain(),
        &vulkan.currentSwapchainImageIndex,
        nullptr
    );

    // Submit the presentation info to the presentation queue
    // If swapchain is out of date or suboptimal, return false
    try {
        if (vulkan.device->getPresentationQueue().presentKHR(presentationInfo) == vk::Result::eSuboptimalKHR)
            return false;
    }
    catch (vk::OutOfDateKHRError outOfDateError) {
        return false;
    }
    
    // Wait for the presentation queue to be idle
    vulkan.device->getPresentationQueue().waitIdle();

    // Increment the current frame index. Hash to max frames
    vulkan.currentFrameIndex = (vulkan.currentFrameIndex + 1) % vulkan.maxRenderFrames;

    return true;
}

void RenderEngine::addOBJToScene(std::string objPath) {
    // Load model from obj path and add it to scene
    Mesh* newMesh = Utils::loadOBJFile(objPath, "assets/materials");
    addMeshToScene(newMesh);
}

void RenderEngine::addVoxelizedOBJToScene(std::string objPath) {
    // Clear octree if it exists
    if (targetOctree != nullptr)
        delete targetOctree;

    // Load model from obj path, voxelize it and add to the scene
    Mesh* newMesh = Utils::loadOBJFile(objPath, "assets/materials");
    Volume meshVolume = Voxelizer::voxelizeMesh(newMesh);

    targetOctree = new Octree();
    targetOctree->build(meshVolume.voxels, uiStates.octreeTargetDepth);
    addVolumeMeshToScene(targetOctree->compressToMesh(uiStates.octreeTargetDepth));

    // std::vector<Mesh*> debugOctreeMeshes = targetOctree->getDebugMeshes();
    // for (Mesh* debugMesh : debugOctreeMeshes)
    //     addDebugMeshToScene(debugMesh); 
}

void RenderEngine::addMeshToScene(Mesh* mesh) {
    mesh->uploadMesh(vulkan.device);
    scene.push_back(mesh);
}

void RenderEngine::addVolumeMeshToScene(Mesh* mesh) {
    mesh->uploadMesh(vulkan.device);
    voxelScene.push_back(mesh);
}

void RenderEngine::addDebugMeshToScene(Mesh* mesh) {
    mesh->uploadMesh(vulkan.device);
    debugScene.push_back(mesh);
}

void RenderEngine::clearScene() {
    // Clear scene and free resources
    for (Mesh* mesh : scene) {
        vulkan.device->freeDeviceMemory(mesh->getVertexBuffer()->getDeviceMemory());
        vulkan.device->freeDeviceMemory(mesh->getIndexBuffer()->getDeviceMemory());
            delete mesh;
    }
    scene.clear();

    for (Mesh* mesh : voxelScene) {
        vulkan.device->freeDeviceMemory(mesh->getVertexBuffer()->getDeviceMemory());
        vulkan.device->freeDeviceMemory(mesh->getIndexBuffer()->getDeviceMemory());
        delete mesh;
    }
    voxelScene.clear();

    for (Mesh* mesh : debugScene) {
        vulkan.device->freeDeviceMemory(mesh->getVertexBuffer()->getDeviceMemory());
        vulkan.device->freeDeviceMemory(mesh->getIndexBuffer()->getDeviceMemory());
        delete mesh;
    }
    debugScene.clear();
}

double RenderEngine::getDeltaTime() {
    return deltaTime;
}

void RenderEngine::deletePipeline(Pipeline* pipeline) {
    // Destroy the pipeline components
    vulkan.device->destroyDescriptorPool(pipeline->getDescriptorPool());
    
    std::vector<vk::DescriptorSetLayout> descriptorSetLayouts = pipeline->getDescriptorSetLayouts();
    for (vk::DescriptorSetLayout layout : descriptorSetLayouts)
        vulkan.device->destroyDescriptorSetLayout(layout);

    vulkan.device->destroyPipelineLayout(pipeline->getPipelineLayout());
    vulkan.device->destroyPipeline(pipeline->getPipeline());

    delete pipeline;
}

void RenderEngine::deleteTexture(Texture* texture) {
    // Destroy the texture components
    vulkan.device->destroySampler(texture->getSampler());
    vulkan.device->destroyImageView(texture->getImageView()->getImageView());
    vulkan.device->freeDeviceMemory(texture->getImage()->getImageMemory());
    vulkan.device->destroyImage(texture->getImage()->getImage());
    delete texture;
}
