#pragma once

#include <vulkan/vulkan.h>
#include <spdlog/spdlog.h>
#include <optional>
#include <set>

struct VulkanDeviceCreateInfo {
   VkInstance instance;
   VkSurfaceKHR surface;
   std::vector<const char*> requiredDeviceExtensions;
   bool enableValidationLayers;
   std::vector<const char*> validationLayers;
};

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class VulkanDevice {
public:
    VulkanDevice(VulkanDeviceCreateInfo* info);
    ~VulkanDevice();

private:
    VkDevice device;
    VkPhysicalDevice physicalDevice;

    bool isPhysicalDeviceSuitable(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, std::vector<const char*> requiredDeviceExtensions);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface); 
};
