#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
#include <glm/glm.hpp>
#include <array>
#include <spdlog/spdlog.h>
#include <fstream>

#include "Vertex.hpp"
#include "VulkanInstance.hpp"
#include "VulkanSurface.hpp"
#include "VulkanDevice.hpp"
#include "VulkanSwapChain.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanDescriptorSetLayout.hpp"
#include "VulkanGraphicsPipeline.hpp"

// #include "Vulkan.hpp"

struct RendererCreateInfo {
    uint32_t windowWidth;
    uint32_t windowHeight;
    std::string windowName;
    bool enableValidationLayers;
};

class Renderer {
public:
    Renderer(RendererCreateInfo* info);
    ~Renderer(); 

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

    void initGLFW(uint32_t windowWidth, uint32_t windowHeight, std::string windowName);
    std::vector<char> readFile(const std::string& filename);
};
