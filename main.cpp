#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

#include "src/core/Renderer.hpp"

struct UniformBufferObject {
    int windowWidth;
    int windowHeight;
    float time;
};

int main() { 
    RendererCreateInfo rendererInfo = {};
    rendererInfo.windowWidth = 800;
    rendererInfo.windowHeight = 600;
    rendererInfo.windowName = "Vulkan Renderer";
    rendererInfo.enableValidationLayers = true;
    Renderer renderer = Renderer(&rendererInfo);

    std::vector<Vertex> quad;
    quad.push_back(Vertex({-1.f, -1.f, 0.f}, {0.0f, 0.0f, 0.0f}));
    quad.push_back(Vertex({1.f, 1.f, 0.f}, {0.0f, 0.0f, 0.0f}));
    quad.push_back(Vertex({1.f, -1.f, 0.f}, {0.0f, 0.0f, 0.0f}));
    quad.push_back(Vertex({-1.f, -1.f, 0.f}, {0.0f, 0.0f, 0.0f}));
    quad.push_back(Vertex({-1.f, 1.f, 0.f}, {0.0f, 0.0f, 0.0f}));
    quad.push_back(Vertex({1.f, 1.f, 0.f}, {0.0f, 0.0f, 0.0f}));
    
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
