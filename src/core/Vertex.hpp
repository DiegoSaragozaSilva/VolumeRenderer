#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <vector>

class Vertex {
public:
    Vertex(glm::vec3 pos, glm::vec3 color);
    ~Vertex();

    glm::vec3 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription getBindingDescription();
    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
    static size_t getVertexSize();
};
