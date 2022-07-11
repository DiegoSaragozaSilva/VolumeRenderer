#pragma once

#define MAX_FRAMES_IN_FLIGHT 2

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
#include <glm/glm.hpp>
#include <array>
#include <spdlog/spdlog.h>
#include <fstream>
#include <iostream>

#include "Vertex.hpp"
#include "VulkanInstance.hpp"
#include "VulkanSurface.hpp"
#include "VulkanDevice.hpp"
#include "VulkanSwapChain.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanDescriptorSetHandler.hpp"
#include "VulkanGraphicsPipeline.hpp"
#include "VulkanCommandBufferHandler.hpp"
#include "VulkanSemaphore.hpp"
#include "VulkanFence.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanTexture.hpp"
#include "../engine/Engine.hpp"

// #include "Vulkan.hpp"

struct RendererCreateInfo {
    uint32_t windowWidth;
    uint32_t windowHeight;
    std::string windowName;
    bool enableValidationLayers;
};

struct TextureCreateInfo {
    VkImageType imageType;
    VkImageViewType imageViewType;
    VkFormat imageFormat;
    VkDeviceSize size;
    uint32_t width, height, depth; 
    void* data;   
};

struct VertexBuffer {
   VulkanBuffer* vulkanBuffer;
   Model* model;
};

class Renderer {
public:
    Renderer(RendererCreateInfo* info);
    ~Renderer();

    bool windowShouldClose();
    void pollEvents();
    void addModelToScene(Model* model);
    void attachUniformBufferToPipeline(VulkanBuffer* uniformBuffer);
    void attachTextureToPipeline(VulkanTexture* texture);
    VulkanBuffer* getUniformBuffer(VkDeviceSize bufferSize);
    VulkanTexture* getTexture(TextureCreateInfo* info);
    void updateUniformBufferData(VulkanBuffer* uniformBuffer, void* data);

    void render();
private:
    
    // Can be changed to be user defined
    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    // Can be changed to be user defined
    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    
    GLFWwindow* window;
    VulkanInstance* vulkanInstance;
    VulkanSurface* vulkanSurface;
    VulkanDevice* vulkanDevice;
    VulkanSwapChain* vulkanSwapChain;
    VulkanRenderPass* vulkanRenderPass;
    VulkanGraphicsPipeline* vulkanGraphicsPipeline;
    VulkanCommandBufferHandler* vulkanCommandBufferHandler;
    VulkanDescriptorSetHandler* vulkanDescriptorSetHandler;
    std::vector<VulkanSemaphore*> imageAvailableVulkanSemaphores;
    std::vector<VulkanSemaphore*> renderFinishedVulkanSemaphores;
    std::vector<VulkanFence*> inFlightVulkanFences;

    std::vector<VertexBuffer> scene;
    VulkanBuffer* currentUniformBuffer;
    VulkanTexture* currentTexture;
    
    uint32_t currentFrame;

    void initGLFW(uint32_t windowWidth, uint32_t windowHeight, std::string windowName);
    std::vector<char> readFile(const std::string& filename);
    void transitionImageLayouts(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t depth);
    void updateDescriptorSets();
};
