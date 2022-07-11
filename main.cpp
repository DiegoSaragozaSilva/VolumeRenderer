#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

#include "src/core/Renderer.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

struct UniformData {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

int main() { 
    RendererCreateInfo rendererInfo = {};
    rendererInfo.windowWidth = 800;
    rendererInfo.windowHeight = 600;
    rendererInfo.windowName = "Vulkan Renderer";
    rendererInfo.enableValidationLayers = true;
    Renderer renderer = Renderer(&rendererInfo);

    // Model/Mesh rotine
    std::vector<Vertex> quad;
    quad.push_back(Vertex({-0.5f, -0.5f, 0.f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f}));
    quad.push_back(Vertex({0.5f, -0.5f, 0.f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}));
    quad.push_back(Vertex({0.5f, 0.5f, 0.f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f}));
    quad.push_back(Vertex({-0.5f, -0.5f, 0.f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f}));
    quad.push_back(Vertex({0.5f, 0.5f, 0.f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f}));
    quad.push_back(Vertex({-0.5f, 0.5f, 0.f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f}));
    
    Model quadModel = Model();
    quadModel.setMesh(quad);
    renderer.addModelToScene(&quadModel);
    
    // Uniform rotine
    VulkanBuffer* uniformBuffer = renderer.getUniformBuffer(sizeof(UniformData));   
    renderer.attachUniformBufferToPipeline(uniformBuffer);
    UniformData data {};

    // Texture rotine
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load("textures/texture.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }

    TextureCreateInfo textureInfo {};
    textureInfo.imageType = VK_IMAGE_TYPE_2D;
    textureInfo.imageViewType = VK_IMAGE_VIEW_TYPE_2D;
    textureInfo.imageFormat = VK_FORMAT_R8G8B8A8_SRGB;
    textureInfo.size = imageSize; 
    textureInfo.width = texWidth;
    textureInfo.height = texHeight;
    textureInfo.depth = 1;
    textureInfo.data = pixels;
    VulkanTexture* texture = renderer.getTexture(&textureInfo);
    renderer.attachTextureToPipeline(texture); 

    float time = 0.0f;
    while (!renderer.windowShouldClose()) {
        renderer.pollEvents();

        data.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        data.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        data.proj = glm::perspective(glm::radians(45.0f), 800 / (float) 600, 0.1f, 10.f);
        data.proj[1][1] *= -1;
        renderer.updateUniformBufferData(uniformBuffer, &data);    

        time += 0.001f;

        renderer.render();
    }

    // renderer.addMeshToScene(quad);
   
    // float time = 0.0f;
    // UniformBufferObject ubo {};
    // ubo.windowWidth = rendererInfo.windowWidth;
    // ubo.windowHeight = rendererInfo.windowHeight;
    // while(!renderer.shouldWindowClose()) {
        // renderer.updateWindow();
        
        // ubo.time = time;
        // renderer.uploadUniform(&ubo, sizeof(ubo));
        // time += 0.01f;

        // renderer.render(); 
    // }

    return 1;
}
