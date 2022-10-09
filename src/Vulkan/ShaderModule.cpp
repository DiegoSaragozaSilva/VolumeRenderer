#include "ShaderModule.hpp"

ShaderModule::ShaderModule(Device* device, std::vector<uint32_t> shaderCode, vk::ShaderStageFlagBits shaderStage) {
    this->shaderStage = shaderStage;

    // Shader module create info
    vk::ShaderModuleCreateInfo shaderModuleCreateInfo (
        vk::ShaderModuleCreateFlags(),
        shaderCode
    );

    // Shader module creation
    shaderModule = device->getLogicalDevice()->createShaderModule(shaderModuleCreateInfo);

    #ifndef NDEBUG
        std::string shaderModuleInfo = "Shader stage: " + std::to_string((int)shaderStage);
        spdlog::info("Shader module successfully created. " + shaderModuleInfo);
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

vk::ShaderStageFlagBits ShaderModule::getShaderStage() {
    return shaderStage;
}
