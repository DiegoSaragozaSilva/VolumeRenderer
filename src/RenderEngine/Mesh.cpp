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

void Mesh::uploadMesh(Device* device) {
    // Vertices need to have data
    if (vertices.size() == 0) {
        spdlog::warn("Mesh data could not be uploaded to the GPU. No vertices found.");
        return;
    }

    // Allocate the mesh buffer with the vertices data
    buffer = new Buffer(
        device,
        vertices.data(),
        vertices.size() * sizeof(Vertex),
        vk::BufferUsageFlagBits::eVertexBuffer,
        vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible
    ); 
}
