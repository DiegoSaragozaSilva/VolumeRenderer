#include "Model.hpp"

Model::Model() {
    transform = new Transform(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(1.0f, 1.0f, 1.0f));    
}

Model::~Model() {}

void Model::setMesh(std::vector<Vertex> mesh) {
    this->mesh = mesh;
}

std::vector<Vertex> Model::getMesh() {
    return mesh;
}

size_t Model::getMeshSize() {
    return mesh.size();
}
