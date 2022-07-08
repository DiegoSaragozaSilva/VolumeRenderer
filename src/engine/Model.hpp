#pragma once

#include <glm/glm.hpp>

#include "../core/Vertex.hpp"
#include "Transform.hpp"

class Model {
public:
    Model();
    ~Model();

    Transform* transform;

    void setMesh(std::vector<Vertex> mesh);
    std::vector<Vertex> getMesh();
    size_t getMeshSize();
private:
    std::vector<Vertex> mesh;
};
