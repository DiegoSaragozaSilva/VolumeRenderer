#ifndef _RENDERPASS_H_
#define _RENDERPASS_H_

#include "Device.hpp"
#include "Swapchain.hpp"

class RenderPass {
public:
    RenderPass(Device* device, Swapchain* swapchain);
    ~RenderPass();

    vk::RenderPass* getRenderPass();
private:
    vk::RenderPass renderPass;
};

#endif
