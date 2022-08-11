#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <glm/gtc/random.hpp>

#include "src/core/Renderer.hpp"
#include "src/engine/Engine.hpp"

int main() { 
    RendererCreateInfo rendererInfo {};
    rendererInfo.windowWidth = 800;
    rendererInfo.windowHeight = 600;
    rendererInfo.windowName = "Vulkan renderer";
    rendererInfo.enableValidationLayers = true;
    Renderer* renderer = new Renderer(&rendererInfo);

    // Basic random volume
    srand(time(NULL));

    uint32_t maxPoints = 1000;
    std::vector<glm::vec3> volumeData(maxPoints);
    for (uint32_t i = 0; i < maxPoints; i++)
        volumeData[i] = glm::linearRand(glm::vec3(-1, -1, -1), glm::vec3(1, 1, 1));

    // Octree generation
    uint32_t octreeMaxDepth = 4;
    Octree* octree = new Octree(volumeData, octreeMaxDepth); 

    // Octree to texture data
    std::vector<uint32_t> textureData = octree->compressToTexture();
 
    std::cout << "Original size: " << volumeData.size() * sizeof(float) * 3 << " bytes\n";
    std::cout << "Compressed size: " << textureData.size() * sizeof(uint32_t) << " bytes\n";

    // Texture creation
    TextureCreateInfo octreeTextureInfo {};
    octreeTextureInfo.imageType = VK_IMAGE_TYPE_3D;
    octreeTextureInfo.imageViewType = VK_IMAGE_VIEW_TYPE_3D;
    octreeTextureInfo.imageFormat = VK_FORMAT_R8G8B8A8_UNORM;
    octreeTextureInfo.size = textureData.size() * sizeof(uint32_t);
    octreeTextureInfo.width = textureData.size() / 8;
    octreeTextureInfo.height = 2;
    octreeTextureInfo.depth = 2;
    octreeTextureInfo.data = textureData.data();
    VulkanTexture* octreeTexture = renderer->getTexture(&octreeTextureInfo);
    renderer->attachTextureToPipeline(octreeTexture);

    // Base quad for rendering
    std::vector<Vertex> quadMesh = {
        Vertex({-1, -1, 0}, {0, 0, 0}, {0, 0}),
        Vertex({1, -1, 0}, {0, 0, 0}, {1, 0}),
        Vertex({1, 1, 0}, {0, 0, 0}, {1, 1}),
        Vertex({-1, 1, 0}, {0, 0, 0}, {0, 1})
    };

    std::vector<uint32_t> quadIndices = {
        0, 2, 1,
        0, 3, 2
    };

    Model* quadModel = new Model;
    quadModel->setMesh(quadMesh, quadIndices);

    renderer->addModelToScene(quadModel);

    while (!renderer->windowShouldClose()) {
        renderer->pollEvents(); 

        renderer->render();
    }

    return 1;
}
