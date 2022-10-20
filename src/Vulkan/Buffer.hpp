#ifndef _BUFFER_H_
#define _BUFFER_H_

#include "Device.hpp"

class Buffer {
public:
    Buffer(Device* device, void* data, size_t dataSize, vk::BufferUsageFlagBits bufferUsageFlag, vk::MemoryPropertyFlags memoryFlags);
    ~Buffer();

    vk::Buffer getBuffer();
    vk::DeviceMemory getDeviceMemory();
private:
    vk::Buffer buffer;
    vk::DeviceMemory deviceMemory;
};

#endif
