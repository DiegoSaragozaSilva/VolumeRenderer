#ifndef _MESH_H_
#define _MESH_H_

#include <glm/glm.hpp>
#include <limits>
#include "Geometry.hpp"
#include "../Vulkan/Buffer.hpp"

struct VertexInputDescription {
    std::vector<vk::VertexInputBindingDescription> bindings;
	std::vector<vk::VertexInputAttributeDescription> attributes;
};

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec4 color;
    glm::vec2 uv;

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
            1,
            0,
            vk::Format::eR32G32B32Sfloat,
            offsetof(Vertex, normal)
        );
        
        // Color binding (2)
        vk::VertexInputAttributeDescription colorAttribute (
            2,
            0,
            vk::Format::eR32G32B32Sfloat,
            offsetof(Vertex, color)
        );

        // UV binding (3)
        vk::VertexInputAttributeDescription uvAttribute (
            3,
            0,
            vk::Format::eR32G32Sfloat,
            offsetof(Vertex, uv)
        );
 
        description.attributes.push_back(positionAttribute);
        description.attributes.push_back(normalAttribute);
        description.attributes.push_back(colorAttribute);
        description.attributes.push_back(uvAttribute);
        return description;
    }
};

struct Material {
    glm::vec3 ambientColor;
    glm::vec3 diffuseColor;
    glm::vec3 specularColor;
    glm::vec3 transmittanceColor;
    glm::vec3 emissionColor;
    float specularExponent;
    float transparency;
    float indexOfRefraction;
    int illuminationModel;
    std::string ambientTextureMap;
    std::string diffuseTextureMap;
    std::string specularColorMap;
    std::string specularHighlightTextureMap;
    std::string alphaTextureMap;
    std::string bumpTextureMap;
    std::string displacementTextureMap;
    std::string reflectionTextureMap;
    uint32_t indexCount;
};

class Mesh {
public:
    Mesh();
    ~Mesh();

    bool hasNormals;

    void setVertices(std::vector<Vertex> vertices);
    void setIndices(std::vector<uint32_t> indices);
    void setMaterials(std::vector<Material> materials);
    std::vector<Vertex> getVertices();
    std::vector<uint32_t> getIndices();
    Buffer* getVertexBuffer();
    Buffer* getIndexBuffer();
    std::vector<Material> getMaterials();
    uint32_t getNumIndices();
    AABB getBoundingBox();
    void uploadMesh(Device* device);
    void generateNormals();
    void translateByMatrix(glm::mat4 translationMatrix);
private:
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<Material> materials;

    Buffer* vertexBuffer;
    Buffer* indexBuffer;
};

#endif
