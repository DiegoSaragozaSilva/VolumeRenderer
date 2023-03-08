#ifndef _SHADERMODULE_H_
#define _SHADERMODULE_H_

#include <SPIRV-Cross/spirv_cross.hpp>
#include "Device.hpp"

class ShaderModule {
public:
    ShaderModule(Device* device, std::vector<uint32_t> shaderCode, vk::ShaderStageFlagBits shaderStage, uint32_t pushConstantOffset, uint32_t pushConstantRange);
    ~ShaderModule();

    vk::ShaderModule getShaderModule();
    vk::ShaderStageFlagBits getShaderStage();
    std::vector<uint32_t> getSPIRVCode();
    uint32_t getPushConstantOffset();
    uint32_t getPushConstantRange();
private:
    uint32_t pushConstantOffset;
    uint32_t pushConstantRange;
    vk::ShaderModule shaderModule;
    vk::ShaderStageFlagBits shaderStage;
    std::vector<uint32_t> shaderCode;
};

#endif
