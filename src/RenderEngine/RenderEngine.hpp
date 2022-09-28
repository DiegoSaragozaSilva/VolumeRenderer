#ifndef _RENDER_ENGINE_H_
#define _RENDER_ENGINE_H_

#include "../Vulkan/Instance.hpp"
#include "Window.hpp"

class RenderEngine {
public:
    RenderEngine();
    ~RenderEngine();
private:
    Window* window;

    Instance* instance;

    void initWindow();
    void initVulkan();
};

#endif
