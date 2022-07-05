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
    RendererCreationInfo rendererInfo = {};
    rendererInfo.windowWidth = 800;
    rendererInfo.windowHeight = 600;
    rendererInfo.windowName = "Vulkan Renderer";
    rendererInfo.maxFramesInFlight = 2;
    rendererInfo.vertexShaderPath = "src/shaders/vert_raymarch.spv";
    rendererInfo.fragmentShaderPath = "src/shaders/frag_raymarch.spv";
    Renderer renderer = Renderer(&rendererInfo);

    const std::vector<Vertex> quad = {
        {{-1.f, -1.f}, {0.0f, 0.0f, 0.0f}},
        {{1.f, 1.f}, {0.0f, 0.0f, 0.0f}},
        {{1.f, -1.f}, {0.0f, 0.0f, 0.0f}},
        {{-1.f, -1.f}, {0.0f, 0.0f, 0.0f}},
        {{-1.f, 1.f}, {0.0f, 0.0f, 0.0f}},
        {{1.f, 1.f}, {0.0f, 0.0f, 0.0f}}
    };
    
    // renderer.addMeshToScene(quad);
   
    float time = 0.0f;
    UniformBufferObject ubo {};
    ubo.windowWidth = rendererInfo.windowWidth;
    ubo.windowHeight = rendererInfo.windowHeight;
    // while(!renderer.shouldWindowClose()) {
        // renderer.updateWindow();
        
        // ubo.time = time;
        // renderer.uploadUniform(&ubo, sizeof(ubo));
        // time += 0.01f;

        // renderer.render(); 
    // }

    return 1;
}
