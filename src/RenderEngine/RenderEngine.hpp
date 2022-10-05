#ifndef _RENDER_ENGINE_H_
#define _RENDER_ENGINE_H_

#include "../Vulkan/Instance.hpp"
#include "../Vulkan/Device.hpp"
#include "../Vulkan/Swapchain.hpp"
#include "../Vulkan/RenderPass.hpp"
#include "../Vulkan/CommandPool.hpp"
#include "Window.hpp"

// Struct that holds all vulkan context variables
struct Vulkan {
    Instance* instance;
    Device* device;
    CommandPool* commandPool;
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
};

#endif
