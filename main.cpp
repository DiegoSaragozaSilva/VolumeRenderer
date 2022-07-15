#include <iostream>
#include <vector>

#include "src/core/Renderer.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

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

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str())) {
        throw std::runtime_error(warn + err);
    }

    VulkanBuffer* uniformBuffer = renderer->getUniformBuffer(sizeof(UniformData));
    renderer->attachUniformBufferToPipeline(uniformBuffer);

    UniformData uniformData {};
    float time = 0.0f;
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
