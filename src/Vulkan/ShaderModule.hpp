#ifndef _SHADERMODULE_H_
#define _SHADERMODULE_H_

#include <SPIRV-Cross/spirv_cross.hpp>
#include "Device.hpp"

class ShaderModule {
public:
    ShaderModule(Device* device, std::vector<uint32_t> shaderCode, vk::ShaderStageFlagBits shaderStage);
    ~ShaderModule();

    vk::ShaderModule getShaderModule();
    vk::ShaderStageFlagBits getShaderStage();
    std::vector<uint32_t> getSPIRVCode();
private:
    vk::ShaderModule shaderModule;
    vk::ShaderStageFlagBits shaderStage;
    std::vector<uint32_t> shaderCode;
};

#endif
