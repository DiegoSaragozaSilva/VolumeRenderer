#include <iostream>
#include <vector>
#include <glm/gtc/random.hpp>

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
    rendererInfo.enableValidationLayers = false;
    Renderer* renderer = new Renderer(&rendererInfo);

    uint32_t numPoints = 50;
    std::vector<glm::vec3> volume(numPoints);
    for (uint32_t i = 0; i < numPoints; i++)
        volume.push_back(glm::ballRand(1.0f));
        //volume.push_back(glm::linearRand(glm::vec3({-1.f, -1.f, -1.f}), glm::vec3({1.f, 1.f, 1.f})));

    Octree* octree = new Octree();
    octree->setVolumeData(volume);
    octree->setMaxDepth(1, false);
    octree->generateOctree();

    std::vector<Vertex> octreeVertices = octree->_generateMesh();
    std::vector<uint32_t> octreeIndices = octree->_generateIndices(octreeVertices.size());

    Model* octreeModel = new Model();
    octreeModel->setMesh(octreeVertices, octreeIndices);

    renderer->addModelToScene(octreeModel);

    VulkanBuffer* uniformBuffer = renderer->getUniformBuffer(sizeof(UniformData));
    renderer->attachUniformBufferToPipeline(uniformBuffer);
    UniformData uniformData {};

    float time = 0.0f;
    int _time = 0;
    int maxDepth = 1;
    bool ascending = true;
    while (!renderer->windowShouldClose()) {
        renderer->pollEvents();

        if (_time % 350 == 0) {
            if (maxDepth < 5 && ascending) {
                maxDepth++;
                
                if (maxDepth == 5) ascending = false;
            }
            else if (maxDepth > 2 && !ascending) {
                maxDepth--;

                if (maxDepth == 2) ascending = true;
            }

            renderer->removeModelFromScene(octreeModel);

            octree->setMaxDepth(maxDepth, true);
            
            std::vector<Vertex> octreeVertices = octree->_generateMesh();
            std::vector<uint32_t> octreeIndices = octree->_generateIndices(octreeVertices.size());
            
            octreeModel->setMesh(octreeVertices, octreeIndices);

            renderer->addModelToScene(octreeModel);
        }

        uniformData.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        uniformData.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        uniformData.proj = glm::perspective(glm::radians(45.0f), 800 / (float) 600, 0.1f, 10.f);
        uniformData.proj[1][1] *= -1;
        renderer->updateUniformBufferData(uniformBuffer, &uniformData);
        time += 0.001f;
        _time += 1;

        renderer->render();
    }

    return 1;
}
