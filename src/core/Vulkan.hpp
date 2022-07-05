#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <cstring>
#include <optional>
#include <set>
#include <limits>
#include <algorithm>
#include <fstream>
#include <array>
#include <queue>

struct VulkanHandlerCreationInfo {
    uint32_t glfwExtensionCount;
    const char** glfwExtensions;
    GLFWwindow* window;
    uint32_t maxFramesInFlight;
    std::string vertexShaderPath, fragmentShaderPath;
    VkVertexInputBindingDescription vertexBindingDescription;
    std::array<VkVertexInputAttributeDescription, 2> vertexAttributeDescriptions;
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

struct VertexBuffer {
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    size_t size;
};

struct UniformBuffer {
    void* data;
    size_t size;
};

class VulkanHandler {
public:
    VulkanHandler(VulkanHandlerCreationInfo* info);
    ~VulkanHandler();

    void drawFrame(std::vector<VertexBuffer> vertexBuffers);
    void recreateSwapChain();
    void destroyVertexBuffer(VertexBuffer buffer);
    void fillVertexBuffer(VertexBuffer vertexBuffer, float* data);
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void addUniformBufferToQueue(UniformBuffer uniformBuffer);
    void createUniformBuffers(size_t totalBufferSize);
    void createDescriptorPool(size_t totalBufferSize);

private:
    bool enableValidationLayers = true;
    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    GLFWwindow* rendererWindow;
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkSurfaceKHR surface;
    VkQueue presentQueue;
    VkQueue graphicsQueue;
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    std::vector<VkFramebuffer> swapChainFramebuffers;
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    uint32_t maxFramesInFlight;
    uint32_t currentFrame;
    VkVertexInputBindingDescription vertexBindingDescription;
    std::array<VkVertexInputAttributeDescription, 2> vertexAttributeDescriptions;
    VkDescriptorSetLayout descriptorSetLayout;
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::queue<UniformBuffer> uniformBufferQueue;
    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;
    std::string vertexShaderPath, fragmentShaderPath;

    void checkForExtensionSupport();
    void checkValidationLayerSupport();
    void createInstance(uint32_t glfwExtensionCount, const char** glfwExtensions);
    bool isDeviceSuitable(VkPhysicalDevice physicalDevice);
    void pickPhysicalDevice();
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice);
    void createLogicalDevice();
    void createSurface();
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice physicalDevice);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    void createSwapChain();
    void createImageViews();
    void createGraphicsPipeline();
    std::vector<char> readFile(const std::string& filename);
    VkShaderModule createShaderModule(const std::vector<char>& code);
    void createRenderPass();
    void createFramebuffers();
    void createCommandPool();
    void createCommandBuffers();
    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, std::vector<VertexBuffer> vertexBuffers);
    void createSyncObjects();
    void cleanupSwapChain();
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void createDescriptorSetLayout(); 
    void updateUniformBuffer(UniformBuffer uniformBuffer); 
    void createDescriptorSets(size_t totalBufferSize);
};
