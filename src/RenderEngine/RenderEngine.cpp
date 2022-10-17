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
    camera->setFocusPoint({0.0f, 0.0f, 0.0f});
    camera->setFOV(60.0f);
    camera->setNearPlane(0.1f);
    camera->setFarPlane(1000.0f);
    camera->generateViewMatrix();
    camera->generateProjectionMatrix();

    // Initialize the texture pool and required the default texture
    texturePool = new TexturePool();
    texturePool->requireTexture(vulkan.device, vulkan.commandPool, "assets/textures/default.png");

    #ifndef NDEBUG
        spdlog::info("Render engine successfully initialized");
    #endif
}

RenderEngine::~RenderEngine() {
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
    render.defaultShaders.push_back(new ShaderModule(vulkan.device, Utils::loadShaderCode("assets/shaders/default.vert.spv"), vk::ShaderStageFlagBits::eVertex));
    render.defaultShaders.push_back(new ShaderModule(vulkan.device, Utils::loadShaderCode("assets/shaders/default.frag.spv"), vk::ShaderStageFlagBits::eFragment));

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
        vulkan.scissor
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

    // Rotate the camera around the scene
    camera->setPosition({
        (float)(sin(window->getTime() / 5.0) * 5.0f),  
        10.0f,
        (float)(cos(window->getTime() / 5.0) * 5.0f),
    });
    camera->setFocusPoint({
        0.0f,
        10.0f,
        0.0f
    });
    camera->generateViewMatrix();

    // Calculate model view projection matrix
    glm::mat4 modelMatrix = glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, 0.0f)), glm::vec3(0.05f, 0.05f, 0.05f));
    glm::mat4 mvp = camera->getProjectionMatrix() * camera->getViewMatrix() * modelMatrix;

    // Send MVP via push constants
    commandBuffer.pushConstants (
        render.defaultPipeline->getPipelineLayout(),
        vk::ShaderStageFlagBits::eVertex,
        0,
        sizeof(glm::mat4),
        &mvp
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
        // FPS Counter
        std::string fpsStr = std::to_string(1000.0f / ImGui::GetIO().Framerate) + " ms/frame | " + std::to_string(ImGui::GetIO().Framerate) + " FPS";
        float fpsStrSize = ImGui::CalcTextSize(fpsStr.c_str()).x;
        ImGui::SameLine(ImGui::GetWindowWidth() - fpsStrSize - 20.0f);
        ImGui::Text(fpsStr.c_str());

        ImGui::EndMainMenuBar();
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
    vk::SubmitInfo submitInfo (
        1,
        &graphicsSemaphore,
        &pipelineStageFlags,
        1,
        &commandBuffer,
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

bool RenderEngine::windowShouldClose() {
    // Return the window should close state
    return window->shouldClose();
}

void RenderEngine::addOBJToScene(std::string objPath) {
    // Load model from obj path and add it to scene
    Mesh* newMesh = Utils::loadOBJFile(objPath, "assets/materials");
    newMesh->uploadMesh(vulkan.device);
    scene.push_back(newMesh);
}

void RenderEngine::clearScene() {
    // Clear scene
    scene.clear();
}
