#include "Swapchain.hpp"

Swapchain::Swapchain(Device* device, vk::SurfaceKHR* surface, uint32_t imageWidth, uint32_t imageHeight) {
    // Format and present mode
    format = getFormat(device->getPhysicalDevice(), *surface);
    presentMode = getPresentMode(device->getPhysicalDevice(), *surface);

    // Extent
    extent = vk::Extent2D {imageWidth, imageHeight};

    // Identity transform (No transform applied to the output image)
    transformFlags = vk::SurfaceTransformFlagBitsKHR::eIdentity;

    // List all the surface capabilities
    vk::SurfaceCapabilitiesKHR surfaceCapabilities = device->getPhysicalDevice()->getSurfaceCapabilitiesKHR(*surface);
    if (surfaceCapabilities.maxImageCount > 0 && surfaceCapabilities.minImageCount > surfaceCapabilities.maxImageCount)
        surfaceCapabilities.minImageCount = surfaceCapabilities.maxImageCount;

    // Swapchain creation
    vk::SwapchainCreateInfoKHR swapchainCreateInfo(vk::SwapchainCreateFlagsKHR(),
                                                   *surface,
                                                   surfaceCapabilities.minImageCount,
                                                   format.colorFormat,
                                                   format.colorSpace,
                                                   extent,
                                                   1,
                                                   vk::ImageUsageFlagBits::eColorAttachment,
                                                   vk::SharingMode::eExclusive,
                                                   0,
                                                   nullptr,
                                                   transformFlags,
                                                   vk::CompositeAlphaFlagBitsKHR::eOpaque,
                                                   presentMode,
                                                   VK_TRUE,
                                                   vk::SwapchainKHR());
    
    // Check for presentation queues presence
    if (device->hasPresentationQueue()) {
        std::array<uint32_t, 2> queueIndices = {
            device->getGraphicsQueueIndex(),
            device->getPresentationQueueIndex()
        };

        swapchainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        swapchainCreateInfo.queueFamilyIndexCount = 2;
        swapchainCreateInfo.pQueueFamilyIndices = queueIndices.data();
    }

    swapchain = device->getLogicalDevice()->createSwapchainKHR(swapchainCreateInfo);

    // Image views creation
    imageViews = createImageViews(device->getLogicalDevice());

    #ifndef NDEBUG
        std::string swapchainInfo = "Size: (" + std::to_string(imageWidth) + ", " + std::to_string(imageHeight) + ") | " + 
                                    "Image count: " + std::to_string(surfaceCapabilities.minImageCount) + " | " +
                                    "Present mode: " + std::to_string((int)presentMode) + " | " + 
                                    "Format: " + std::to_string((int)format.colorFormat);
        spdlog::info("Vulkan swapchain successfully created. " + swapchainInfo);
    #endif
}

Swapchain::~Swapchain() {
    // Image view destruction
    for (const auto& imageView : imageViews)
        delete imageView;

    #ifndef NDEBUG
        spdlog::info("Vulkan swapchain successfully destroyed.");
    #endif
}

SwapchainFormat Swapchain::getFormat(vk::PhysicalDevice* physicalDevice, vk::SurfaceKHR surface) {
    // List all available surface formats    
    std::vector<vk::SurfaceFormatKHR> availableSurfaceFormats = physicalDevice->getSurfaceFormatsKHR(surface);

    // At least one surface format must be available
    if (availableSurfaceFormats.size() == 0) {
        spdlog::error("No available surface formats.");
        throw 0;
    }

    // If only one surface format available with undefined format, choose manually
    vk::SurfaceFormatKHR defaultFormat = availableSurfaceFormats[0];
    if (availableSurfaceFormats.size() == 1 && defaultFormat.format == vk::Format::eUndefined) {
        #ifndef NDEBUG
            spdlog::warn("Available surface format is undefined. Selecting the default one.");
        #endif
        return SwapchainFormat {vk::ColorSpaceKHR::eSrgbNonlinear, vk::Format::eR8G8B8Unorm};
    }

    // Choose format with eR8G8B8Unorm format type
    for (const auto& availableFormat : availableSurfaceFormats)
        if (availableFormat.format == vk::Format::eR8G8B8Unorm)
            return SwapchainFormat {availableFormat.colorSpace, availableFormat.format};

    // Else, use the only available format
    #ifndef NDEBUG
        spdlog::warn("Surface format eR8B8G8Unorm not found in available formats. Selecting the first available one");
    #endif
    return SwapchainFormat {defaultFormat.colorSpace, defaultFormat.format};
}

vk::PresentModeKHR Swapchain::getPresentMode(vk::PhysicalDevice* physicalDevice, vk::SurfaceKHR surface) {
    // List all available present modes
    std::vector<vk::PresentModeKHR> availablePresentModes = physicalDevice->getSurfacePresentModesKHR(surface);

    // At least one present mode must be available
    if (availablePresentModes.size() == 0) {
        spdlog::error("No available surface present modes.");
        throw 0;
    }

    // Prefered presentation modes. Lower index equals higher preference
    std::vector<vk::PresentModeKHR> preferredModes = {
        vk::PresentModeKHR::eMailbox,
        vk::PresentModeKHR::eFifo,
        vk::PresentModeKHR::eFifoRelaxed,
        vk::PresentModeKHR::eImmediate
    };

    for (const auto& mode : preferredModes)
        if (std::find(availablePresentModes.begin(), availablePresentModes.end(), mode) != availablePresentModes.end())
            return mode;

    // At least one preferred mode must be present
    spdlog::error("No compatible presentation mode found in available modes.");
    throw 0;
}

vk::SwapchainKHR* Swapchain::getSwapchain() {
    return &swapchain;
}

uint32_t Swapchain::getImageCount() {
    return imageViews.size();
}

std::vector<ImageView*> Swapchain::createImageViews(vk::Device* logicalDevice) {
    std::vector<ImageView*> imageViews;

    // Create a new image view for each image in the swapchain
    std::vector<vk::Image> swapchainImages = logicalDevice->getSwapchainImagesKHR(swapchain);
    for (const auto& image : swapchainImages)
        imageViews.push_back(new ImageView(logicalDevice,
                                           image,
                                           vk::ImageViewType::e2D,
                                           format.colorFormat,
                                           vk::ImageAspectFlagBits::eColor,
                                           1));
    return imageViews;    
}

std::vector<vk::ImageView> Swapchain::getImageViews() {
    std::vector<vk::ImageView> pImageViews;
    for (ImageView* imageView : imageViews)
        pImageViews.push_back(*imageView->getImageView());
    return pImageViews;
}

vk::Format Swapchain::getColorFormat() {
    return format.colorFormat; 
}

vk::Extent2D Swapchain::getExtent() {
    return extent;
}
