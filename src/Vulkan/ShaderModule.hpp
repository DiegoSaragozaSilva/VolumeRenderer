#ifndef _SHADERMODULE_H_
#define _SHADERMODULE_H_

#include "Device.hpp"

class ShaderModule {
public:
    ShaderModule(Device* device, std::vector<uint32_t> shaderCode, vk::ShaderStageFlagBits shaderStage);
    ~ShaderModule();

    vk::ShaderModule getShaderModule();
    vk::ShaderStageFlagBits getShaderStage();
private:
    vk::ShaderModule shaderModule;
    vk::ShaderStageFlagBits shaderStage;
};

#endif
