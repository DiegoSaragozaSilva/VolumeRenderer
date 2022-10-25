#ifndef _INSTANCE_H_
#define _INSTANCE_H_

#include <spdlog/spdlog.h>
#include <vulkan/vulkan.hpp>

class Instance {
public:
    Instance(std::string applicationName, std::string engineName, std::vector<const char*> requiredExtensions);
    ~Instance();

    vk::Instance* getInstance();
    void destroySurface(vk::SurfaceKHR* surface);
private:
    std::string applicationName, engineName;
    vk::Instance instance;

    bool checkValidationLayersSupport(std::vector<const char*> validationLayers);
};

#endif
