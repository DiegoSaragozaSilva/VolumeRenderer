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

struct RendererCreationInfo {
    uint32_t windowWidth, windowHeight;
    std::string windowName;
    uint32_t maxFramesInFlight;
    std::string vertexShaderPath, fragmentShaderPath;
};

class Renderer {
public:
    Renderer(RendererCreationInfo* info);
    ~Renderer(); 

    bool shouldWindowClose();
    void updateWindow();
    void render();
    void setWindowSize(int width, int height);
    void updateRenderer();
    void addMeshToScene(std::vector<Vertex> data);
    void uploadUniform(void* data, size_t size);
private:
    
    // Can be changed to be user defined
    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    // Can be changed to be user defined
    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
    
    // Can be changed to be user defined
    bool enableValidationLayers;

    GLFWwindow* window;
    VulkanInstance* vulkanInstance;
    VulkanSurface* vulkanSurface;
    VulkanDevice* vulkanDevice;

    void initGLFW(uint32_t windowWidth, uint32_t windowHeight, std::string windowName);
};
