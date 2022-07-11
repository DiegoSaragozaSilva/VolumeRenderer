#include "Vertex.hpp"

Vertex::Vertex(glm::vec3 pos, glm::vec3 color, glm::vec2 uv) {
    this->pos = pos;
    this->color = color;
    this->uv = uv;
}

Vertex::~Vertex() {}

VkVertexInputBindingDescription Vertex::getBindingDescription() {
    VkVertexInputBindingDescription description;
    description.binding = 0;
    description.stride = Vertex::getVertexSize();
    description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        
    return description;
}

std::vector<VkVertexInputAttributeDescription> Vertex::getAttributeDescriptions() {
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
        
    // Pos description
    VkVertexInputAttributeDescription posDescription {};
    posDescription.binding = 0;
    posDescription.location = 0;
    posDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
    posDescription.offset = offsetof(Vertex, pos);
        
    // Color description
    VkVertexInputAttributeDescription colorDescription {};
    colorDescription.binding = 0;
    colorDescription.location = 1;
    colorDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
    colorDescription.offset = offsetof(Vertex, color);

    // UV description
    VkVertexInputAttributeDescription uvDescription {};
    uvDescription.binding = 0;
    uvDescription.location = 2;
    uvDescription.format = VK_FORMAT_R32G32_SFLOAT;
    uvDescription.offset = offsetof(Vertex, uv);

    attributeDescriptions.push_back(posDescription);
    attributeDescriptions.push_back(colorDescription);
    attributeDescriptions.push_back(uvDescription);
    return attributeDescriptions;
}

size_t Vertex::getVertexSize() {
    size_t posSize = sizeof(float) * 3;
    size_t colorSize = sizeof(float) * 3;
    size_t uvSize = sizeof(float) * 2;

    return posSize + colorSize + uvSize;
}
