#include "VulkanInstance.hpp"

VulkanInstance::VulkanInstance(VulkanInstanceCreateInfo* info) {
    if (info->enableValidationLayers) {
        checkValidationLayerSupport(info->validationLayers);
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = info->applicationName.c_str();
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = info->engineName.c_str();
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = info->extensionCount;
    createInfo.ppEnabledExtensionNames = info->extensions;
    createInfo.enabledLayerCount = 0;

    if (info->enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(info->validationLayers.size());
        createInfo.ppEnabledLayerNames = info->validationLayers.data();
    }
    else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        spdlog::error("Failed to create Vulkan instance!");
        throw 0;
    }
}

VulkanInstance::~VulkanInstance() {}

void VulkanInstance::checkValidationLayerSupport(std::vector<const char*> validationLayers) {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const auto& layer : validationLayers) {
        bool layerFound = false;
        for (const auto& availableLayer : availableLayers) {
            if (strcmp(layer, availableLayer.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            spdlog::error("Some or all requested validation layers were not found!");
            throw 0;
        }
    }
}

VkInstance VulkanInstance::getInstance() {
    return instance;
}
