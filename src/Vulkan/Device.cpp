#include "Device.hpp"

Device::Device(vk::Instance* instance, vk::SurfaceKHR* windowSurface) {
    // Physical device picking
    pickPhysicalDevice(instance);

    // Querying the physical device queue families for graphics and presentation queues
    QueueConfig queueConfig = queryPhysicalDeviceQueues(windowSurface);

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

    #ifndef NDEBUG
        std::string devicesInfo = "Graphics queue index: " + std::to_string(queueConfig.graphicsQueueIndex) + " | Presentation queue index: " + std::to_string(queueConfig.presentationQueueIndex);
        spdlog::info("Vulkan physical and logical devices successfully created. " + devicesInfo);
    #endif
}

Device::~Device() {
    // Clean up
    logicalDevice.destroy(); 

    #ifndef NDEBUG
        spdlog::info("Vulkan logical device successfully destroyed.");
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
