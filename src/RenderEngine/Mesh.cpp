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

std::vector<Vertex> Mesh::getVertices() {
    return vertices;
}

std::vector<uint32_t> Mesh::getIndices() {
    return indices;
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

AABB Mesh::getBoundingBox() {
    AABB boundingBox = {
        .min = glm::vec3(std::numeric_limits<float>::max()),
        .max = glm::vec3(std::numeric_limits<float>::min()),
        .center = glm::vec3(0.0f)
    };

    for (uint32_t i = 0; i < vertices.size(); i++) {
        glm::vec3 vertexPosition = vertices[i].position;
        
        boundingBox.min.x = vertexPosition.x < boundingBox.min.x ? vertexPosition.x : boundingBox.min.x;
        boundingBox.min.y = vertexPosition.y < boundingBox.min.y ? vertexPosition.y : boundingBox.min.y;
        boundingBox.min.z = vertexPosition.z < boundingBox.min.z ? vertexPosition.z : boundingBox.min.z;

        boundingBox.max.x = vertexPosition.x > boundingBox.max.x ? vertexPosition.x : boundingBox.max.x;
        boundingBox.max.y = vertexPosition.y > boundingBox.max.y ? vertexPosition.y : boundingBox.max.y;
        boundingBox.max.z = vertexPosition.z > boundingBox.max.z ? vertexPosition.z : boundingBox.max.z;
    }

    boundingBox.center = (boundingBox.max + boundingBox.min) / 2.0f;

    return boundingBox;
}

void Mesh::generateNormals() {
    // Reset all the normals
    for (uint32_t i = 0; i < vertices.size(); i++)
        vertices[i].normal = {0.0f, 0.0f, 0.0f};

    // For each mesh face, generate a weighted normal
    for (uint32_t i = 0; i < indices.size(); i += 3) {
        glm::vec3 p1 = vertices[indices[i + 0]].position;
        glm::vec3 p2 = vertices[indices[i + 1]].position;
        glm::vec3 p3 = vertices[indices[i + 2]].position;

        glm::vec3 U = p2 - p1;
        glm::vec3 V = p3 - p2;

        glm::vec3 n = glm::cross(U, V);

        float a1 = glm::acos(glm::dot(glm::normalize(p2 - p1), glm::normalize(p3 - p1)));
        float a2 = glm::acos(glm::dot(glm::normalize(p3 - p2), glm::normalize(p1 - p2)));
        float a3 = glm::acos(glm::dot(glm::normalize(p1 - p3), glm::normalize(p2 - p3)));

        vertices[indices[i + 0]].normal += n * a1;
        vertices[indices[i + 1]].normal += n * a2;
        vertices[indices[i + 2]].normal += n * a3;
    }

    // For each vertex, normalize the normal
    for (uint32_t i = 0; i < vertices.size(); i++)
        vertices[i].normal = glm::normalize(vertices[i].normal);
}

void Mesh::translateByMatrix(glm::mat4 translationMatrix) {
    for (uint32_t i = 0; i < vertices.size(); i++)
        vertices[i].position = glm::vec3(translationMatrix * glm::vec4(vertices[i].position, 1.0f));
}
