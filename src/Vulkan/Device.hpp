#ifndef _DEVICE_H_
#define _DEVICE_H_

#include <spdlog/spdlog.h>
#include <vulkan/vulkan.hpp>

struct QueueConfig {
    uint32_t graphicsQueueIndex;
    uint32_t presentationQueueIndex;
    bool hasDifferentIndices;
};

class Device {
public:
    Device(vk::Instance* instance, vk::SurfaceKHR* windowSurface);
    ~Device();

    vk::PhysicalDevice* getPhysicalDevice();
    vk::Device* getLogicalDevice();
    uint32_t getGraphicsQueueIndex();
    uint32_t getPresentationQueueIndex();
    bool hasPresentationQueue();
    bool isAnisotropicFilteringSupported();
    bool isShaderMultiSamplingSupported();
    vk::SampleCountFlagBits getMultiSamplingLevel();
    vk::Format getDepthFormat();
    vk::Queue getGraphicsQueue();
    vk::Queue getPresentationQueue();
    uint32_t getMemoryTypeIndex(uint32_t filter, vk::MemoryPropertyFlags flags);
    std::vector<vk::Semaphore> createSemaphores(uint32_t count);
    std::vector<vk::Fence> createFences(uint32_t count);
    void destroySwapchain(vk::SwapchainKHR* swapchain);
    void destroyImage(vk::Image image);
    void destroyImageView(vk::ImageView* imageView);
    void destroyRenderPass(vk::RenderPass* renderPass);
    void destroyCommandPool(vk::CommandPool commandPool);
    void destroyFramebuffer(vk::Framebuffer framebuffer);
    void destroyShaderModule(vk::ShaderModule shaderModule);
    void freeDeviceMemory(vk::DeviceMemory deviceMemory);
private:
    vk::PhysicalDevice physicalDevice;
    vk::Device logicalDevice;
    QueueConfig queueConfig;
    vk::SampleCountFlagBits multiSamplingLevel;
    vk::Format depthFormat;
    vk::Queue graphicsQueue;
    vk::Queue presentationQueue;

    void pickPhysicalDevice(vk::Instance* instance);
    QueueConfig queryPhysicalDeviceQueues(vk::SurfaceKHR* windowSurface);
    vk::SampleCountFlagBits queryMultiSamplingLevel();
    vk::Format queryDepthFormat();
};

#endif
