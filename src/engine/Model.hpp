#pragma once

#include <glm/glm.hpp>

#include "../core/Vertex.hpp"
#include "Transform.hpp"

class Model {
public:
    Model();
    ~Model();

    Transform* transform;

    void setMesh(std::vector<Vertex> mesh, std::vector<uint32_t> indices);
    std::vector<Vertex> getMesh();
    std::vector<uint32_t> getMeshIndices();
    size_t getMeshSize();
    size_t getMeshIndiceSize();
private:
    std::vector<Vertex> mesh;
    std::vector<uint32_t> indices;
};
