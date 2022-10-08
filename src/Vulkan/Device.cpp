#include "Device.hpp"

Device::Device(vk::Instance* instance, vk::SurfaceKHR* windowSurface) {
    // Physical device picking
    pickPhysicalDevice(instance);

    // Querying the physical device queue families for graphics and presentation queues
    queueConfig = queryPhysicalDeviceQueues(windowSurface);

    // Querying multisampling level
    multiSamplingLevel = queryMultiSamplingLevel();

    // Querying depth format
    depthFormat = queryDepthFormat();

    // Logical device creation
    // Queue priority (not relevant)
    const float deviceQueuePriority = 1.0f;

    // Create only one queue create info for both queues using the graphics queue index
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos(1);
    queueCreateInfos[0] = vk::DeviceQueueCreateInfo {
        vk::DeviceQueueCreateFlags(),
        queueConfig.graphicsQueueIndex,
        1,
        &deviceQueuePriority
    };

    // If presentation queue is different from graphics queue, create a config for it
    if (queueConfig.hasDifferentIndices) {
        queueCreateInfos.push_back(vk::DeviceQueueCreateInfo {
            vk::DeviceQueueCreateFlags(),
            queueConfig.presentationQueueIndex,
            1,
            &deviceQueuePriority
        });
    }

    // Swapchain must be supported as an extension
    std::vector<const char*> extensionNames = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    // Device create info with queues and extensions data
    vk::DeviceCreateInfo deviceCreateInfo (
        vk::DeviceCreateFlags(),
        queueCreateInfos.size(),
        queueCreateInfos.data(),
        0,
        nullptr,
        extensionNames.size(),
        extensionNames.data(),
        nullptr
    );

    // Logical device creation
    logicalDevice = physicalDevice.createDevice(deviceCreateInfo);

    // Graphics queue storage
    graphicsQueue = logicalDevice.getQueue(0, queueConfig.graphicsQueueIndex);

    // Presentation queue storage
    presentationQueue = logicalDevice.getQueue(0, queueConfig.presentationQueueIndex);

    #ifndef NDEBUG
        std::string devicesInfo = "Graphics queue index: " + std::to_string(queueConfig.graphicsQueueIndex) + " | Presentation queue index: " + std::to_string(queueConfig.presentationQueueIndex);
        spdlog::info("Vulkan physical and logical devices successfully created. " + devicesInfo);
    #endif
}

Device::~Device() {
    // Clean up
    logicalDevice.destroy(); 

    #ifndef NDEBUG
        spdlog::info("Vulkan device successfully destroyed.");
    #endif
}

void Device::pickPhysicalDevice(vk::Instance* instance) {
    // Enumerate all available physical devices
    std::vector<vk::PhysicalDevice> physicalDevices = instance->enumeratePhysicalDevices();
    if (physicalDevices.size() == 0) {
        spdlog::error("No available physical devices.");
        throw 0;
    }

    // Get first physical device properties
    vk::PhysicalDevice selectedPhysicalDevice = physicalDevices[0];
    vk::PhysicalDeviceProperties selectedProperties = selectedPhysicalDevice.getProperties();

    // If selected physical device is not a discrete GPU, next device is selected.
    // If no discrete GPU is found, the next available one is picked.
    if (selectedProperties.deviceType != vk::PhysicalDeviceType::eDiscreteGpu) {
        for (size_t i = 1; i < physicalDevices.size(); i++) {
            vk::PhysicalDevice nextPhysicalDevice = physicalDevices[i];
            vk::PhysicalDeviceProperties nextProperties = nextPhysicalDevice.getProperties();

            if (nextProperties.deviceType != vk::PhysicalDeviceType::eDiscreteGpu) {
                selectedPhysicalDevice = nextPhysicalDevice;
                selectedProperties = nextProperties;
                break;
            }
        }
    }

    // Check support for swap chain
    bool swapchainSupported = false;
    std::string swapchainName = VK_KHR_SWAPCHAIN_EXTENSION_NAME;

    for (const auto& extension : selectedPhysicalDevice.enumerateDeviceExtensionProperties())
        if (strcmp(extension.extensionName, swapchainName.c_str())) {
            swapchainSupported = true;
            break;
        }

    if (!swapchainSupported) {
        spdlog::error("Picked physical device has no swapchain support.");
        throw 0;
    }

    physicalDevice = selectedPhysicalDevice;

    #ifndef NDEBUG
        std::string physicalDeviceInfo = std::string(selectedProperties.deviceName) + " | " + std::to_string(int(selectedProperties.deviceType)) + " | " + std::to_string(selectedProperties.driverVersion);
        spdlog::info("Physical device successfully picked. " +  physicalDeviceInfo);
    #endif
}

QueueConfig Device::queryPhysicalDeviceQueues(vk::SurfaceKHR* windowSurface) {
    // An unset queue is defined as the numeric limit of uint32
    const uint32_t unsetQueue = std::numeric_limits<uint32_t>::max();

    // Queue config, both queue family indices
    QueueConfig queueConfig;
    queueConfig.graphicsQueueIndex = unsetQueue;
    queueConfig.presentationQueueIndex = unsetQueue;

    // Fetch all queues supported by the physical device
    std::vector<vk::QueueFamilyProperties> queueFamilies = physicalDevice.getQueueFamilyProperties();

    // For each supported queue, if queue supports both graphics and presentation set both to the same index. Else, find only graphics queue index
    for (size_t i = 0; i < queueFamilies.size(); i++) {
        vk::QueueFamilyProperties queueProperties = queueFamilies[i];
        if (queueProperties.queueCount > 0 && queueProperties.queueFlags & vk::QueueFlagBits::eGraphics) {
            if (queueConfig.graphicsQueueIndex == unsetQueue)
                queueConfig.graphicsQueueIndex = i;

            if (physicalDevice.getSurfaceSupportKHR(i, *windowSurface)) {
                queueConfig.graphicsQueueIndex = i;
                queueConfig.presentationQueueIndex = i;
                break;
            }
        }
    }

    // The graphics queue must be supported
    if (queueConfig.graphicsQueueIndex == unsetQueue) {
        spdlog::error("No graphics queue supported by the physical device");
        throw 0;
    }

    // Again, try to find a queue family that supports only presentation
    if (queueConfig.presentationQueueIndex == unsetQueue)
        for (size_t i = 0; i < queueFamilies.size(); i++)
            if (physicalDevice.getSurfaceSupportKHR(i, *windowSurface)) {
                queueConfig.presentationQueueIndex = i;
                break;
            }

    // The presentation queue must be supported
    if (queueConfig.presentationQueueIndex == unsetQueue) {
        spdlog::error("No presentation queue supported by the physical device");
        throw 0;
    }

    // Check for different queue indices
    queueConfig.hasDifferentIndices = queueConfig.graphicsQueueIndex != queueConfig.presentationQueueIndex;

    return queueConfig;
}

vk::SampleCountFlagBits Device::queryMultiSamplingLevel() {
    // List all the sampling levels supported
    vk::PhysicalDeviceProperties properties = physicalDevice.getProperties();
    vk::SampleCountFlags supportedSampleCountFlags = properties.limits.framebufferColorSampleCounts;

    // Preferred multisampling count. Lower index means higher preferrence
    std::vector<vk::SampleCountFlagBits> preferredSampleCounts = {
        vk::SampleCountFlagBits::e8,
        vk::SampleCountFlagBits::e4,
        vk::SampleCountFlagBits::e2,
        vk::SampleCountFlagBits::e1,
    };

    // Find the multisampling count that the physical device supports first
    for (vk::SampleCountFlagBits sampleCount : preferredSampleCounts)
        if (supportedSampleCountFlags & sampleCount)
            return sampleCount;

    // At least one multisampling count must be supported
    spdlog::error("No multisampling supported.");
    throw 0;
}

vk::Format Device::queryDepthFormat() {
    // List all format properties available
    vk::FormatProperties formatProperties = physicalDevice.getFormatProperties(vk::Format::eD32Sfloat);

    // Check support for for 32 bit stencil format
    if (formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
        return vk::Format::eD32Sfloat;

    // Throw if not supported
    spdlog::error("32 bit depth stencil not supported.");
    throw 0;
}

vk::Queue Device::getGraphicsQueue() {
    return graphicsQueue;
}

vk::Queue Device::getPresentationQueue() {
    return presentationQueue;
}

uint32_t Device::getMemoryTypeIndex(uint32_t filter, vk::MemoryPropertyFlags flags) {
    // Fetch all the memory types from the physical device
    vk::PhysicalDeviceMemoryProperties memoryProperties = physicalDevice.getMemoryProperties();

    // Loop through all properties trying to find a suitable one based on the arguments provided
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
        if ((filter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & flags) == flags)
            return i;
    }

    // At least one memory type index must be found
    spdlog::error("Failed to find a suitable memory type");
    throw 0;
}

std::vector<vk::Semaphore> Device::createSemaphores(uint32_t count) {
    // Semaphores creation and return
    std::vector<vk::Semaphore> semaphores;
    vk::SemaphoreCreateInfo semaphoreCreateInfo;
    for (int i = 0; i < count; i++)
        semaphores.push_back(logicalDevice.createSemaphore(semaphoreCreateInfo));

    return semaphores;
}

std::vector<vk::Fence> Device::createFences(uint32_t count) {
    // Fences creation and return
    std::vector<vk::Fence> fences;
    vk::FenceCreateInfo fenceCreateInfo (
        vk::FenceCreateFlagBits::eSignaled        
    );

    for (int i = 0; i < count; i++)
        fences.push_back(logicalDevice.createFence(fenceCreateInfo));

    return fences;
}

vk::PhysicalDevice* Device::getPhysicalDevice() {
    return &physicalDevice;
}

vk::Device* Device::getLogicalDevice() {
    return &logicalDevice;
}

uint32_t Device::getGraphicsQueueIndex() {
    return queueConfig.graphicsQueueIndex;
}

uint32_t Device::getPresentationQueueIndex() {
    return queueConfig.presentationQueueIndex;
}

vk::SampleCountFlagBits Device::getMultiSamplingLevel() {
    return multiSamplingLevel;
}

vk::Format Device::getDepthFormat() {
    return depthFormat;
}

bool Device::hasPresentationQueue() {
    return queueConfig.hasDifferentIndices;
}

void Device::destroySwapchain(vk::SwapchainKHR* swapchain) {
    logicalDevice.destroySwapchainKHR(*swapchain);
    swapchain = nullptr;
}

void Device::destroyImage(vk::Image image) {
    logicalDevice.destroyImage(image);
}

void Device::destroyImageView(vk::ImageView* imageView) {
    logicalDevice.destroyImageView(*imageView);
    imageView = nullptr;
}

void Device::destroyRenderPass(vk::RenderPass* renderPass) {
    logicalDevice.destroyRenderPass(*renderPass);
    renderPass = nullptr;
}

void Device::destroyCommandPool(vk::CommandPool commandPool) {
    logicalDevice.destroyCommandPool(commandPool);
}

void Device::destroyFramebuffer(vk::Framebuffer framebuffer) {
    logicalDevice.destroyFramebuffer(framebuffer);
}

void Device::destroyShaderModule(vk::ShaderModule shaderModule) {
    logicalDevice.destroyShaderModule(shaderModule);
}

void Device::freeDeviceMemory(vk::DeviceMemory deviceMemory) {
    logicalDevice.freeMemory(deviceMemory);

    std::stringstream addressStream;
    addressStream << &deviceMemory;
    spdlog::info("Device memory (" + addressStream.str() + ") freed.");
}
