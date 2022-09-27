#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <spdlog/spdlog.h>
#include <GLFW/glfw3.h>

class Window {
public:
    Window(int width, int height, std::string name);
    ~Window();
private:
    int width, height;
    std::string name;
    GLFWwindow* window;
};

#endif
