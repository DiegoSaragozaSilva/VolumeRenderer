#ifndef _COMMANDPOOL_H_
#define _COMMANDPOOL_H_

#include <spdlog/spdlog.h>
#include <vector>
#include "Device.hpp"

class CommandPool {
public:
    CommandPool(Device* device);
    ~CommandPool();

    vk::CommandPool getCommandPool();
    vk::CommandBuffer beginCommandBuffer(vk::Device device);
    void endCommandBuffer(vk::CommandBuffer commandBuffer, Device* device);
    std::vector<vk::CommandBuffer> createCommandBuffers(Device* device, uint32_t count);
private:
    vk::CommandPool commandPool;
};

#endif
