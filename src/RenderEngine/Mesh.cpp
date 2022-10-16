#include "Mesh.hpp"

Mesh::Mesh() {}

Mesh::~Mesh() {
    #ifndef NDEBUG
        spdlog::info("Mesh successfully destroyed.");
    #endif
}

void Mesh::setVertices(std::vector<Vertex> vertices) {
    this->vertices = vertices;
}

void Mesh::setIndices(std::vector<uint32_t> indices) {
    this->indices = indices;
}

void Mesh::setMaterials(std::vector<Material> materials) {
    this->materials = materials;
}

void Mesh::uploadMesh(Device* device) {
    // Vertices need to have data
    if (vertices.size() == 0) {
        spdlog::warn("Mesh data could not be uploaded to the GPU. No vertices found.");
        return;
    }

    // Indices need to have data if vertices size is greater than or equal to three
    if (indices.size() == 0) {
        spdlog::warn("Mesh data could not be uploaded to the GPU. No indices found.");
        return;
       
    }

    // Allocate the mesh buffer with the vertices data
    vertexBuffer = new Buffer (
        device,
        vertices.data(),
        vertices.size() * sizeof(Vertex),
        vk::BufferUsageFlagBits::eVertexBuffer,
        vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible
    );

    // Allocate the mesh buffer with the indices data
    indexBuffer = new Buffer (
        device,
        indices.data(),
        indices.size() * sizeof(uint32_t),
        vk::BufferUsageFlagBits::eIndexBuffer,
        vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible
    );
}

Buffer* Mesh::getVertexBuffer() {
    return vertexBuffer;
}

Buffer* Mesh::getIndexBuffer() {
    return indexBuffer;
}

std::vector<Material> Mesh::getMaterials() {
    return materials;
}

uint32_t Mesh::getNumIndices() {
    return indices.size();
}

void Mesh::generateNormals() {
    // For each mesh face, generate a normal
    for (uint32_t i = 0; i < vertices.size(); i += 3) {
        glm::vec3 p1 = vertices[i + 0].position;
        glm::vec3 p2 = vertices[i + 1].position;
        glm::vec3 p3 = vertices[i + 2].position;

        glm::vec3 U = p2 - p1;
        glm::vec3 V = p3 - p2;

        glm::vec3 n = {
            U.y * V.z - U.z * V.y,
            U.z * V.x - U.x * V.z,
            U.x * V.y - U.y * V.x
        };

        vertices[i + 0].normal = n;
        vertices[i + 1].normal = n;
        vertices[i + 2].normal = n;
    }
}
