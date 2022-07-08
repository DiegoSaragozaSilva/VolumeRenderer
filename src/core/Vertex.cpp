#include "Vertex.hpp"

Vertex::Vertex(glm::vec3 pos, glm::vec3 color) {
    this->pos = pos;
    this->color = color;
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

    attributeDescriptions.push_back(posDescription);
    attributeDescriptions.push_back(colorDescription);
    return attributeDescriptions;
}

size_t Vertex::getVertexSize() {
    size_t posSize = sizeof(float) * 3;
    size_t colorSize = sizeof(float) * 3;
    
    return posSize + colorSize;
}
