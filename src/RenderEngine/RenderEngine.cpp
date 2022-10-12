#include "RenderEngine.hpp"

#include <fstream>
#include <iterator>

RenderEngine::RenderEngine() {
    // Initializing all basic componentes and Vulkan
    initWindow();
    initVulkan();

    // Initialize the main camera
    camera = new Camera();
    camera->setPosition({0.0f, 0.0f, 0.0f});
    camera->setUpVector({0.0f, 1.0f, 0.0f});
    camera->setFocusPoint({0.0f, 0.0f, 0.0f});
    camera->setFOV(60.0f);
    camera->generateViewMatrix();
    camera->generateProjectionMatrix();

    // REMOVE LATER
    cubeMesh = Utils::loadOBJFile("assets/objs/cube.obj");
    cubeMesh->uploadMesh(vulkan.device);

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
        1.0f,
        0.0f,
        0.0f,
        1.0f
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
        0.0f,
        (float)(cos(window->getTime() / 5.0) * 5.0f),
    });
    camera->generateViewMatrix();

    // Calculate model view projection matrix
    glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    glm::mat4 mvp = camera->getProjectionMatrix() * camera->getViewMatrix() * modelMatrix;

    // Send MVP via push constants
    commandBuffer.pushConstants (
        render.defaultPipeline->getPipelineLayout(),
        vk::ShaderStageFlagBits::eVertex,
        0,
        sizeof(glm::mat4),
        &mvp
    );

    // Bind the cube vertex and index buffers
    vk::DeviceSize offsets[]{0};
    vk::Buffer cubeVertexBuffer = cubeMesh->getVertexBuffer()->getBuffer();
    commandBuffer.bindVertexBuffers(0, 1, &cubeVertexBuffer, offsets);
    commandBuffer.bindIndexBuffer(cubeMesh->getIndexBuffer()->getBuffer(), 0, vk::IndexType::eUint32);

    // Draw indexed
    commandBuffer.drawIndexed(cubeMesh->getNumIndices(), 1, 0, 0, 0);

    // End frame rendering
    bool endStatus = renderEnd();
    if (!endStatus)
        recreateRenderContext();
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
