#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
#include <glm/glm.hpp>
#include <array>
#include <spdlog/spdlog.h>

#include "VulkanInstance.hpp"
#include "VulkanSurface.hpp"
#include "VulkanDevice.hpp"
#include "VulkanSwapChain.hpp"
#include "VulkanRenderPass.hpp"

// #include "Vulkan.hpp"

struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription description;
        description.binding = 0;
        description.stride = sizeof(Vertex);
        description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        
        return description;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
        
        // pos description
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);
        
        // color description
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        return attributeDescriptions;
    }
};

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

    void initGLFW(uint32_t windowWidth, uint32_t windowHeight, std::string windowName);
};
