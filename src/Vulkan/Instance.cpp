#include "Instance.hpp"

Instance::Instance(std::string applicationName, std::string engineName, std::vector<const char*> requiredExtensions) {
    this->applicationName = applicationName;
    this->engineName = engineName;

    // Application information
    vk::ApplicationInfo applicationInfo(applicationName.c_str(), 1, engineName.c_str(), VK_API_VERSION_1_1);

    // Instance create information
    vk::InstanceCreateInfo instanceCreateInfo(vk::InstanceCreateFlags(),
                                              &applicationInfo,
                                              0, nullptr,
                                              requiredExtensions.size(),
                                              requiredExtensions.data());
    
    // Validation layers only in debug mode
    #ifndef NDEBUG
        // Khronos validation layer
        std::vector<const char*> validationLayers = {
            "VK_LAYER_KHRONOS_validation"
        };

        if (!checkValidationLayersSupport(validationLayers)) {
            spdlog::error("One or more requested validation layers are not available!");
            throw 0;
        }

        instanceCreateInfo.enabledLayerCount = validationLayers.size();
        instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
    #endif

    // Instance creation
    vkInstance = vk::createInstance(instanceCreateInfo);

    #ifndef NDEBUG
        std::string instanceInfo = "App name: " + applicationName + " | Engine name: " + engineName + " | Validation layers: ";
        for (auto layerName : validationLayers)
            instanceInfo += std::string(layerName) + " ";
        spdlog::info("Vulkan instance successfully created. " + instanceInfo);
    #endif
}

Instance::~Instance() {
    // Vulkan instance destruction
    vkInstance.destroy();

    #ifndef NDEBUG
        spdlog::info("Vulkan instance successfully destroyed.");
    #endif
}

vk::Instance* Instance::getInstance() {
    return &vkInstance;
}

bool Instance::checkValidationLayersSupport(std::vector<const char*> requestedLayers) {
    // Enumerate available vulkan instance validation layers
    std::vector<vk::LayerProperties> availableLayers = vk::enumerateInstanceLayerProperties();

    // Checking if the requested layers are present in the available layers
    for (const char* requestedLayer : requestedLayers) {
        bool layerFound = false;
        for (const auto& layerProperties : availableLayers) {
            if (strcmp(requestedLayer, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
            return false;
    }

    return true;
}
