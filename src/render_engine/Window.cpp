#include "Window.hpp"

Window::Window(int width, int height, std::string name) {
    this->width = width;
    this->height = height;
    this->name = name;

    // Initialize GLFW
    if (glfwInit() != GLFW_TRUE) {
        spdlog::error("Failed to initialize GLFW!");
        throw 0;
    }

    #ifndef NDEBUG
        std::string glfwVersion = glfwGetVersionString();
        spdlog::info("GLFW successfully initialized. GLFW version: " + glfwVersion);
    #endif

    // Window creation
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);

    #ifndef NDEBUG
        std::string windowInformation = "(" + std::to_string(width) + ", " + std::to_string(height) + ")";
        spdlog::info("Window successfully created. " + windowInformation);
    #endif
}

Window::~Window() {
    // GLFW termination
    glfwTerminate();

    #ifndef NDEBUG
        spdlog::info("GLFW successfully terminated.");
    #endif
}
