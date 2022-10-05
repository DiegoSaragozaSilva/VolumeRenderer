#include "CommandPool.hpp"

CommandPool::CommandPool(Device* device) {
    // Command pool creation
    vk::CommandPoolCreateInfo commandPoolCreateInfo (
        vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        device->getGraphicsQueueIndex()
    );

    commandPool = device->getLogicalDevice()->createCommandPool(commandPoolCreateInfo);
    
    #ifndef NDEBUG
        spdlog::info("Vulkan command pool successfully created.");
    #endif
}

CommandPool::~CommandPool() {
    #ifndef NDEBUG
        spdlog::info("Vulkan command pool successfully destroyed.");
    #endif
}

vk::CommandPool CommandPool::getCommandPool() {
    return commandPool;
}

vk::CommandBuffer CommandPool::beginCommandBuffer(vk::Device device) {
    // Single command buffer allocation info
    vk::CommandBufferAllocateInfo commandBufferAllocateInfo (
        commandPool,
        vk::CommandBufferLevel::ePrimary,
        1
    );

    // Single command buffer creation and move to the device
    vk::CommandBuffer commandBuffer (
        std::move(device.allocateCommandBuffers(commandBufferAllocateInfo)[0])
    );

    // Define command buffer begining. One time submit
    vk::CommandBufferBeginInfo commandBufferBeginInfo (
        vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
        nullptr
    );

    // Request the command buffer to begin and return
    commandBuffer.begin(commandBufferBeginInfo);
    return commandBuffer;   
}

void CommandPool::endCommandBuffer(vk::CommandBuffer commandBuffer, Device* device) {
    // Request the command buffer to end
    commandBuffer.end();

    // Define the submission of the command buffer to the graphics queue
    vk::SubmitInfo submitInfo (
        0,
        nullptr,
        nullptr,
        1,
        &commandBuffer,
        0,
        nullptr
    );

    // Submit the command buffer to the graphics queue and wait for it to be completed
    vk::Result submitStatus = device->getGraphicsQueue().submit(1, &submitInfo, vk::Fence());
    if (submitStatus != vk::Result::eSuccess) {
        spdlog::error("Graphics queue failed to submit command buffer.");
        throw 0;
    }
    device->getGraphicsQueue().waitIdle();
}
