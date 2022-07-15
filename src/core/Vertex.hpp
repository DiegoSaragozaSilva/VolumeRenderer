#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <vector>

class Vertex {
public:
    Vertex() {};
    Vertex(glm::vec3 pos, glm::vec3 color, glm::vec2 uv);
    ~Vertex();

    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 uv;

    static VkVertexInputBindingDescription getBindingDescription();
    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
    static size_t getVertexSize();
};
