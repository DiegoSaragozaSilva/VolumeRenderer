#ifndef _RENDER_ENGINE_H_
#define _RENDER_ENGINE_H_

#include "../Vulkan/Instance.hpp"
#include "../Vulkan/Device.hpp"
#include "Window.hpp"

// Struct that holds all vulkan context variables
struct Vulkan {
    Instance* instance;
    Device* device;
};

class RenderEngine {
public:
    RenderEngine();
    ~RenderEngine();
private:
    Window* window;
    Vulkan vulkan;

    void initWindow();
    void initVulkan();
};

#endif
