#include <iostream>
#include <vector>

#include "src/core/Renderer.hpp"
#include "src/engine/Engine.hpp"

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

    std::vector<glm::vec3> volume = {
        {0.f, 0.f, 0.f},
        {1.f, 0.f, 0.f},
        {1.f, 1.f, 0.f},
        {0.f, 1.f, 0.f},
        {0.f, 0.f, 1.f},
        {1.f, 0.f, 1.f},
        {1.f, 1.f, 1.f},
        {0.f, 1.f, 1.f}
    };

    Octree* octree = new Octree();
    octree->setVolumeData(volume);
    octree->generateOctree();

    std::vector<Vertex> octreeVertices = octree->_generateMesh();
    std::vector<uint32_t> octreeIndices = octree->_generateIndices(octreeVertices.size());

    std::cout << "Vert count: " << octreeVertices.size() << "\n";
    std::cout << "Ind count: " << octreeIndices.size() << "\n";

    Model* octreeModel = new Model();
    octreeModel->setMesh(octreeVertices, octreeIndices);

    renderer->addModelToScene(octreeModel);

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
