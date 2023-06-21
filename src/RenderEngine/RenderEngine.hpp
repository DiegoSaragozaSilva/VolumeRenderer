#ifndef _RENDER_ENGINE_H_
#define _RENDER_ENGINE_H_

#include <cmath>
#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_glfw.h"
#include "../imgui/imgui_impl_vulkan.h"
#include "../Vulkan/Instance.hpp"
#include "../Vulkan/Device.hpp"
#include "../Vulkan/Swapchain.hpp"
#include "../Vulkan/RenderPass.hpp"
#include "../Vulkan/CommandPool.hpp"
#include "../Vulkan/Image.hpp"
#include "../Vulkan/ImageView.hpp"
#include "../Vulkan/ShaderModule.hpp"
#include "../Vulkan/Pipeline.hpp"
#include "Utils.hpp"
#include "Mesh.hpp"
#include "Camera.hpp"
#include "Texture.hpp"
#include "TexturePool.hpp"
#include "Window.hpp"
#include "Voxelizer.hpp"
#include "Octree.hpp"
#include "TransferFunction.hpp"

// Struct that holds all vulkan context variables
struct Vulkan {
    Instance* instance;
    Device* device;
    CommandPool* commandPool;
    Image* multiSampleImage;
    ImageView* multiSampleImageView;
    Image* depthImage;
    ImageView* depthImageView;
    std::vector<vk::Framebuffer> framebuffers;
    std::vector<vk::CommandBuffer> commandBuffers;
    std::vector<vk::Semaphore> graphicsSemaphores;
    std::vector<vk::Semaphore> presentationSemaphores;
    std::vector<vk::Fence> graphicsFences;
    std::vector<vk::ClearValue> clearValues;
    vk::Rect2D scissor;
    vk::Viewport viewport;
    uint32_t maxRenderFrames;
    uint32_t currentFrameIndex;
    uint32_t currentSwapchainImageIndex;
};

// Struct that holds all vulkan render context variables
struct Render {
    Swapchain* swapchain;
    RenderPass* renderPass;
    std::vector<ShaderModule*> defaultShaders;
    std::vector<ShaderModule*> voxelShaders;
    std::vector<ShaderModule*> debugShaders;
    Pipeline* defaultPipeline;
    Pipeline* voxelPipeline;
    Pipeline* debugPipeline;
    Material defaultMaterial;
};

// Struct for basic push constants
struct PushConstants {
    glm::mat4 mvp;
    glm::vec4 viewPosition;
    glm::vec4 viewDirection;
};

struct VoxelConstants {
    glm::mat4 cameraView;
    glm::mat4 cameraProjection;
    glm::vec4 transferAlpha;
    glm::vec4 planeCutoff;
    glm::vec2 windowDimensions;
    glm::vec2 transferRed;
    glm::vec2 transferGreen;
    glm::vec2 transferBlue;
    glm::vec3 cameraPos;
    float octreeDepth;
    glm::vec3 octreeMin;
    float opacityCutoffMin;
    glm::vec3 octreeMax;
    float opacityCutoffMax;
};

// Struct that holds all the UI states
struct UIStates {
    bool showCameraProperties;
    bool invertPlaneCutoff;
    int octreeTargetDepth;
    float opacityCutoffMin;
    float opacityCutoffMax;
    glm::vec3 planeCutoff;
    glm::vec2 transferRed;
    glm::vec2 transferGreen;
    glm::vec2 transferBlue;
    glm::vec4 transferAlpha;
};

class RenderEngine {
public:
    RenderEngine();
    ~RenderEngine();

    Window* window;
    Camera* camera;

    void init();
    void renderFrame();
    double getDeltaTime();
    void addMeshToScene(Mesh* mesh);
    void addVolumeMeshToScene(Mesh* mesh);
    void addDebugMeshToScene(Mesh* mesh);
private:
    Vulkan vulkan;
    Render render;
    TexturePool* texturePool;
    std::vector<Mesh*> scene;
    std::vector<Mesh*> voxelScene;
    std::vector<Mesh*> debugScene;
    Octree* targetOctree;
    TransferFunction* rgbTransferFunction;
    UIStates uiStates;
    double deltaTime, lastTime;

    void initWindow();
    void initImgui();
    void initVulkan();
    Image* createMultiSampleImage();
    Image* createDepthImage();
    ImageView* createImageView(Image* image, vk::ImageAspectFlags aspectFlags);
    std::vector<vk::Framebuffer> createFramebuffers();
    vk::Rect2D createScissor();
    vk::Viewport createViewport();
    std::vector<vk::ClearValue> createClearValues();
    uint32_t getNextImageIndex(vk::Fence fence, vk::Semaphore semaphore);
    void recreateRenderContext();
    bool renderBegin();
    bool renderEnd();
    void renderUI();
    void addOBJToScene(std::string objPath);
    void addVoxelizedOBJToScene(std::string objPath);
    void addVoxelizedVolumeToScene(std::string folderPath);
    void addRayTraceQuadToScene();
    void updateTransferFunctionTexture();
    void clearScene();
    void deletePipeline(Pipeline* pipeline);
    void deleteTexture(Texture* texture);
};

#endif
