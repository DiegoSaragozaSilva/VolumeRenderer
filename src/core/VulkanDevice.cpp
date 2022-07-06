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
        if (isPhysicalDeviceSuitable(device, info->surface, info->requiredDeviceExtensions)) {
            physicalDevice = device;
            break;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE) {
        spdlog::error("No suitable physical device found!");
        throw 0;
    }

    QueueFamilyIndices indices = findQueueFamilies(physicalDevice, info->surface);
 
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(info->requiredDeviceExtensions.size());
    createInfo.ppEnabledExtensionNames = info->requiredDeviceExtensions.data();
   
    if (info->enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(info->validationLayers.size());
        createInfo.ppEnabledLayerNames = info->validationLayers.data();
    }
    else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
        spdlog::error("Failed to create logical device!");
        throw 0;
    }

    vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
    vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
}

VulkanDevice::~VulkanDevice() {}

bool VulkanDevice::isPhysicalDeviceSuitable(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, std::vector<const char*> requiredDeviceExtensions) {
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);
     
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
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice, surface);
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

    swapChainSupportDetails = details;

    return details;
}

QueueFamilyIndices VulkanDevice::findQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);
        
        if (presentSupport) {
            indices.presentFamily = i;
        }

        i++;
    }

    swapChainQueueFamilies = indices;

    return indices;
}

VkDevice VulkanDevice::getDevice() {
    return device;
}

QueueFamilyIndices VulkanDevice::getQueueFamilies() {
    return swapChainQueueFamilies;
}

SwapChainSupportDetails VulkanDevice::getSwapChainSupportDetails() {
    return swapChainSupportDetails;
}
