#include "ShaderModule.hpp"

ShaderModule::ShaderModule(Device* device, std::vector<uint32_t> shaderCode) {
    // Shader module create info
    vk::ShaderModuleCreateInfo shaderModuleCreateInfo (
        vk::ShaderModuleCreateFlags(),
        shaderCode
    );

    // Shader module creation
    shaderModule = device->getLogicalDevice()->createShaderModule(shaderModuleCreateInfo);

    #ifndef NDEBUG
        spdlog::info("Shader module successfully created.");
    #endif   
}

ShaderModule::~ShaderModule(){
    #ifndef NDEBUG
        spdlog::info("Shader module successfully destroyed.");
    #endif
}

vk::ShaderModule ShaderModule::getShaderModule() {
    return shaderModule;
}
