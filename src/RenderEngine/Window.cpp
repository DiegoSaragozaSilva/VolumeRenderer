#include "Window.hpp"

Window::Window(int width, int height, std::string name) {
    this->width = width;
    this->height = height;
    this->name = name;
    this->hasSurfaceBeenCreated = false;

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
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);

    #ifndef NDEBUG
        std::string windowInformation = "Window '" + name + "': " + "(" + std::to_string(width) + ", " + std::to_string(height) + ")";
        spdlog::info("Window successfully created. " + windowInformation);
    #endif
}

Window::~Window() {
    // GLFW termination
    glfwDestroyWindow(window);
    glfwTerminate();

    #ifndef NDEBUG
        spdlog::info("GLFW successfully terminated.");
    #endif
}

std::vector<const char*> Window::getGLFWExtensions() {
    // Enumerate all the glfw instance required extensions
    const char** glfwExtensions;
    uint32_t glfwExtensionCount = 0;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    return extensions;
}

vk::SurfaceKHR* Window::getSurface(vk::Instance* instance) {
    // If window surface has never been created, create and return it
    if (!hasSurfaceBeenCreated) {
        hasSurfaceBeenCreated = true;

        VkSurfaceKHR pSurface = VkSurfaceKHR(surface);        
        auto creationError = glfwCreateWindowSurface(*instance, window, NULL, &pSurface);
        if (creationError != 0) {
            spdlog::error("Failed to create GLFW window surface.");
            throw 0;
        }
        surface = vk::SurfaceKHR(pSurface);
    }
    return &surface;
}
