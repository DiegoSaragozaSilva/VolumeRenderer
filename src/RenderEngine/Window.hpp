#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <spdlog/spdlog.h>
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

class Window {
public:
    Window(uint32_t width, uint32_t height, std::string name);
    ~Window();

    std::vector<const char*> getGLFWExtensions();
    vk::SurfaceKHR* getSurface(vk::Instance* instance);
    uint32_t getWidth();
    uint32_t getHeight();
    bool shouldClose();
private:
    bool hasSurfaceBeenCreated;
    uint32_t width, height;
    std::string name;
    GLFWwindow* window;
    vk::SurfaceKHR surface;
};

#endif
