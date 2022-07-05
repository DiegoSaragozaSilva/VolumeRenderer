#pragma once

#include <vulkan/vulkan.h>
#include <spdlog/spdlog.h>
#include <vector>

struct VulkanInstanceCreateInfo {
    bool enableValidationLayers;
    std::vector<const char*> validationLayers;
    uint32_t extensionCount;
    const char** extensions;
    std::string applicationName;
    std::string engineName;

};

class VulkanInstance {
public:
    VulkanInstance(VulkanInstanceCreateInfo* info);
    ~VulkanInstance();

    VkInstance getInstance();
private:
    VkInstance instance;

    void checkValidationLayerSupport(std::vector<const char*> validationLayers); 
};
