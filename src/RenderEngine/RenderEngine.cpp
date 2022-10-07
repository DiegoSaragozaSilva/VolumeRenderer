#include "RenderEngine.hpp"

RenderEngine::RenderEngine() {
    // Initializing all basic componentes and Vulkan
    initWindow();
    initVulkan();

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
    skyColor.color = vk::ClearColorValue({
        0.0f,
        0.0f,
        0.0f,
        1.0f
    });

    // Depth clear color
    vk::ClearValue depthColor;
    depthColor.depthStencil = vk::ClearDepthStencilValue(1.0f, 0.0f);

    return std::vector<vk::ClearValue> {skyColor, depthColor};
}
