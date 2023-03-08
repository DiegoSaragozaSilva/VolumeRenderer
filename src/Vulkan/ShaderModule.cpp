#include "ShaderModule.hpp"

ShaderModule::ShaderModule(Device* device, std::vector<uint32_t> shaderCode, vk::ShaderStageFlagBits shaderStage, uint32_t pushConstantOffset, uint32_t pushConstantRange) {
    this->shaderStage = shaderStage;
    this->shaderCode = shaderCode;
    this->pushConstantOffset = pushConstantOffset;
    this->pushConstantRange = pushConstantRange;

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

std::vector<uint32_t> ShaderModule::getSPIRVCode() {
    return shaderCode;
}

uint32_t ShaderModule::getPushConstantOffset() {
    return pushConstantOffset;
}

uint32_t ShaderModule::getPushConstantRange() {
    return pushConstantRange;
}
