#pragma once

#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>

#include <iostream>
#include <vector>
#include <array>

#include "Utils.hpp"

class Voxelizer {
public:
    static std::vector<glm::vec3> voxelizeMesh(std::vector<std::vector<glm::vec3>> mesh, int lateralResolution);
private:
    static std::vector<glm::vec3> orthoProjectTriangle(std::vector<glm::vec3> triangle, int* projectedAxis);
    static float getPointTriangleZ(std::vector<glm::vec3> triangle, glm::vec2 point);
};
