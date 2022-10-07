#ifndef _RENDER_ENGINE_H_
#define _RENDER_ENGINE_H_

#include "../Vulkan/Instance.hpp"
#include "../Vulkan/Device.hpp"
#include "../Vulkan/Swapchain.hpp"
#include "../Vulkan/RenderPass.hpp"
#include "../Vulkan/CommandPool.hpp"
#include "../Vulkan/Image.hpp"
#include "../Vulkan/ImageView.hpp"
#include "Window.hpp"

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
};

// Struct that holds all vulkan render context variables
struct Render {
    Swapchain* swapchain;
    RenderPass* renderPass;
};

class RenderEngine {
public:
    RenderEngine();
    ~RenderEngine();
private:
    Window* window;
    Vulkan vulkan;
    Render render;

    void initWindow();
    void initVulkan();
    Image* createMultiSampleImage();
    Image* createDepthImage();
    ImageView* createImageView(Image* image, vk::ImageAspectFlags aspectFlags);
    std::vector<vk::Framebuffer> createFramebuffers();
    vk::Rect2D createScissor();
    vk::Viewport createViewport();
    std::vector<vk::ClearValue> createClearValues();
};

#endif
