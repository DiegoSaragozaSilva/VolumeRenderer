#include "TexturePool.hpp"

TexturePool::TexturePool() {
    #ifndef NDEBUG
        spdlog::info("Texture pool successfully created.");
    #endif
}

TexturePool::~TexturePool() {}

Texture* TexturePool::requireTexture(Device* device, CommandPool* commandPool, std::string textureName) {
    // Check if texture already in pool, if not load it and return.
    // Else, just return
    if (pool.find(textureName) == pool.end()) {
        ImageData newImageData = Utils::loadImageFile(textureName);

        // If incoming image failed to load, return the default texture
        if (!newImageData.loaded)
            return pool["assets/textures/default.png"];

        Texture* newTexture = new Texture(device, commandPool, newImageData); 
        pool.insert(std::pair<std::string, Texture*>(textureName, newTexture));
    }
    return pool[textureName];
}

Texture* TexturePool::getTexture(Device* device, CommandPool* commandPool, std::string textureName) {
    if (pool.find(textureName) == pool.end()) return nullptr;
    return pool[textureName];
}

void TexturePool::addTextureToPool(std::string textureName, Texture* texture) {
    pool.insert(std::pair<std::string, Texture*>(textureName, texture));
}

void TexturePool::removeTextureFromPool(std::string textureName) {
    Texture* toRemove = pool[textureName];
    pool.erase(textureName);
    delete toRemove;
}

std::map<std::string, Texture*> TexturePool::getPool() {
    return pool;
}
