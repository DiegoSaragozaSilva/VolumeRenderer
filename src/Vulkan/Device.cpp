#include "Device.hpp"

Device::Device(vk::Instance* instance) {
    // Physical device picking
    pickPhysicalDevice();
}

Device::~Device() {}

void Device::pickPhysicalDevice() {
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
        if (extension.name == swapchainName) {
            swapchainSupported = true;
            break;
        }

    if (!swapchainSupported) {
        spdlog::error("Picked physical device has no swapchain support.");
        throw 0;
    }

    physicalDevice = selectedPhysicalDevice;

    #ifndef NDEBUG
        std::string physicalDeviceInfo = selectedProperties.deviceName + " | " + selectedProperties.deviceType + " | " + std::string(selectedProperties.driverVersion);
        spdlog::info("Physical device successfully picked. " +  physicalDeviceInfo);
    #endif
}
