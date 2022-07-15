#include <iostream>
#include <vector>

#include "src/core/Renderer.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

struct UniformData {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

int main() { 
    RendererCreateInfo rendererInfo {};
    rendererInfo.windowWidth = 800;
    rendererInfo.windowHeight = 600;
    rendererInfo.windowName = "Vulkan renderer";
    rendererInfo.enableValidationLayers = true;
    Renderer* renderer = new Renderer(&rendererInfo);

    // Viking room model
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, "volumes/viking_room.obj")) {
        throw std::runtime_error(warn + err);
    }

    std::vector<Vertex> roomVertices;
    std::vector<uint32_t> roomIndices;

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            Vertex vertex;
            vertex.pos.x = attrib.vertices[3 * index.vertex_index + 0];
            vertex.pos.y = attrib.vertices[3 * index.vertex_index + 1];
            vertex.pos.z = attrib.vertices[3 * index.vertex_index + 2];
            vertex.uv.x = attrib.texcoords[2 * index.texcoord_index + 0];
            vertex.uv.y = 1.0f - attrib.texcoords[2 * index.texcoord_index + 1];
            vertex.color.x = 1.0f;
            vertex.color.y = 0.0f;
            vertex.color.z = 1.0f;

            roomVertices.push_back(vertex);     
            roomIndices.push_back(roomIndices.size());
        }
    }

    Model* roomModel = new Model();
    roomModel->setMesh(roomVertices, roomIndices);

    renderer->addModelToScene(roomModel);
    
    // Uniform
    VulkanBuffer* uniformBuffer = renderer->getUniformBuffer(sizeof(UniformData));
    renderer->attachUniformBufferToPipeline(uniformBuffer);
    
    UniformData uniformData {};
    float time = 0.0f;

    // Texture
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load("textures/viking_room.png", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }

    TextureCreateInfo roomTextureInfo {};
    roomTextureInfo.imageType = VK_IMAGE_TYPE_2D;
    roomTextureInfo.imageViewType = VK_IMAGE_VIEW_TYPE_2D;
    roomTextureInfo.imageFormat = VK_FORMAT_R8G8B8A8_SRGB;
    roomTextureInfo.size = imageSize;
    roomTextureInfo.width = texWidth;
    roomTextureInfo.height = texHeight;
    roomTextureInfo.depth = 1;
    roomTextureInfo.data = pixels;
    VulkanTexture* roomTexture = renderer->getTexture(&roomTextureInfo);
    renderer->attachTextureToPipeline(roomTexture);

    while (!renderer->windowShouldClose()) {
        renderer->pollEvents();
        
        uniformData.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        uniformData.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        uniformData.proj = glm::perspective(glm::radians(45.0f), 800 / (float) 600, 0.1f, 10.f);
        uniformData.proj[1][1] *= -1;
        renderer->updateUniformBufferData(uniformBuffer, &uniformData);

        time += 0.001f;

        renderer->render();
    }

    return 1;
}
