#ifndef _TEXTUREPOOL_H_
#define _TEXTUREPOOL_H_

#include <map>
#include "../Vulkan/Device.hpp"
#include "../Vulkan/CommandPool.hpp"
#include "Texture.hpp"
#include "Utils.hpp"

class TexturePool {
public:
    TexturePool();
    ~TexturePool();

    Texture* requireTexture(Device* device, CommandPool* commandPool, std::string textureName);
    std::map<std::string, Texture*> getPool();
private:
    std::map<std::string, Texture*> pool;
};

#endif
