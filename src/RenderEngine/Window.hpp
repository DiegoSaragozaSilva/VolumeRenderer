#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <spdlog/spdlog.h>
#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

class Window {
public:
    Window(int width, int height, std::string name);
    ~Window();

    std::vector<const char*> getGLFWExtensions();
    vk::SurfaceKHR* getWindowSurface(vk::Instance* instance);
private:
    bool hasSurfaceBeenCreated;
    int width, height;
    std::string name;
    GLFWwindow* window;
    vk::SurfaceKHR surface;
};

#endif
