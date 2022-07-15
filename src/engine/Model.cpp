#include "Model.hpp"

Model::Model() {
    transform = new Transform(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1.0f, 1.0f, 1.0f));    
}

Model::~Model() {}

void Model::setMesh(std::vector<Vertex> mesh, std::vector<uint32_t> indices) {
    this->mesh = mesh;
    this->indices = indices;
}

std::vector<Vertex> Model::getMesh() {
    return mesh;
}

std::vector<uint32_t> Model::getMeshIndices() {
    return indices;
}

size_t Model::getMeshSize() {
    return mesh.size();
}

size_t Model::getMeshIndiceSize() {
    return indices.size();
}
