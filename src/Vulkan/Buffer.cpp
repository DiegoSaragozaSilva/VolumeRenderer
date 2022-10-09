#include "Buffer.hpp"

Buffer::Buffer(Device* device, void* data, size_t dataSize, vk::BufferUsageFlagBits bufferUsageFlag, vk::MemoryPropertyFlags memoryFlags) {
    // Buffer create info
    vk::BufferCreateInfo bufferCreateInfo (
        vk::BufferCreateFlags(),
        dataSize,
        bufferUsageFlag
    );

    // Create buffer
    buffer = device->getLogicalDevice()->createBuffer(bufferCreateInfo);

    // Buffer memory allocation info
    vk::MemoryRequirements memoryRequirements = device->getLogicalDevice()->getBufferMemoryRequirements(buffer);
    uint32_t memoryTypeIndex = device->getMemoryTypeIndex(memoryRequirements.memoryTypeBits, memoryFlags);
    vk::MemoryAllocateInfo memoryAllocateInfo (
        memoryRequirements.size,
        memoryTypeIndex
    );

    // Buffer memory allocation
    deviceMemory = device->getLogicalDevice()->allocateMemory(memoryAllocateInfo);

    // Copying data to the device memory
    void* pData = (void*)(device->getLogicalDevice()->mapMemory(deviceMemory, 0, memoryRequirements.size));
    memcpy(pData, data, dataSize);
    device->getLogicalDevice()->unmapMemory(deviceMemory);

    // Bind the device memory to the buffer
    device->getLogicalDevice()->bindBufferMemory(buffer, deviceMemory, 0);

    #ifndef NDEBUG
        std::string bufferInfo = "Buffer size: " + std::to_string(dataSize);
        spdlog::info("Vulkan buffer successfully created. " + bufferInfo);
    #endif
}

Buffer::~Buffer() {
    #ifndef NDEBUG
        spdlog::info("Vulkan buffer successfully destroyed.");
    #endif
}

vk::Buffer Buffer::getBuffer() {
    return buffer;
}
