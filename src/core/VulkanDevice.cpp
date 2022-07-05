#include "VulkanDevice.hpp"

VulkanDevice::VulkanDevice(VulkanDeviceCreateInfo* info) {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(info->instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        spdlog::error("No physical device found!");
        throw 0;
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(info->instance, &deviceCount, devices.data());

    for (const auto& device : devices) {
        if (isPhysicalDeviceSuitable(device)) {
            physicalDevice = device;
            break;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE) {
        spd::error("No suitable physical device found!");
        throw 0;
    }

    // CREATE LOGICAL DEVICE
}

VulkanDevice::~VulkanDevice() {}

bool VulkanDevice::isPhysicalDeviceSuitable(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, std::vector<const char*> requiredDeviceExtensions) {
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
     
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(requiredDeviceExtensions.begin(), requiredDeviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    bool extensionsSupported = requiredExtensions.empty();

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }
 
    return indices.graphicsFamily.has_value() && indices.graphicsFamily.has_value() && requiredExtensions.empty() && swapChainAdequate;   
}

SwapChainSupportDetails VulkanDevice::querySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {    
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}
