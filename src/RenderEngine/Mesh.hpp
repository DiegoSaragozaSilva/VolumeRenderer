#ifndef _MESH_H_
#define _MESH_H_

#include <glm/glm.hpp>
#include "../Vulkan/Buffer.hpp"

struct VertexInputDescription {
    std::vector<VkVertexInputBindingDescription> bindings;
	std::vector<VkVertexInputAttributeDescription> attributes;
};

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;

    static VertexInputDescription getVertexDescription() {
        VertexInputDescription description;

        // Vertex input binding description
        vk::VertexInputBindingDescription vertexInputBindingDescription (
            0,
            sizeof(Vertex),
            vk::VertexInputRate::eVertex
        );

        description.bindings.push_back(vertexInputBindingDescription);

        // Position binding (0)
        vk::VertexInputAttributeDescription positionAttribute (
            0,
            0,
            vk::Format::eR32G32B32Sfloat,
            offsetof(Vertex, position)
        );

        // Normal binding (1)
        vk::VertexInputAttributeDescription normalAttribute (
            0,
            1,
            vk::Format::eR32G32B32Sfloat,
            offsetof(Vertex, normal)
        );
        
        // Color binding (2)
        vk::VertexInputAttributeDescription colorAttribute (
            0,
            2,
            vk::Format::eR32G32B32Sfloat,
            offsetof(Vertex, color)
        );
 
        description.attributes.push_back(positionAttribute);
        description.attributes.push_back(normalAttribute);
        description.attributes.push_back(colorAttribute);
        return description;
    }
};

class Mesh {
public:
    Mesh();
    ~Mesh();

    void setVertices(std::vector<Vertex> vertices);
    void uploadMesh(Device* device);
private:
    std::vector<Vertex> vertices;
    Buffer* buffer;
};

#endif
