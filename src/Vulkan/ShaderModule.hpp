#ifndef _SHADERMODULE_H_
#define _SHADERMODULE_H_

#include "Device.hpp"

class ShaderModule {
public:
    ShaderModule(Device* device, std::vector<uint32_t> shaderCode);
    ~ShaderModule();

    vk::ShaderModule getShaderModule();
private:
    vk::ShaderModule shaderModule;
};

#endif
