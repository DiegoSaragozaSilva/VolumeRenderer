#ifndef _VOXELIZER_H_
#define _VOXELIZER_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/hash.hpp>
#include <vector>
#include <unordered_set>
#include <cmath>
#include <algorithm>
#include <iostream>

#include "Mesh.hpp"

struct Volume {
    int scale;
    std::vector<glm::vec3> voxels; // Change vector data type to a Voxel struct that contains per voxel data
};

class Voxelizer {
public:
    static int scale;
    static int density;

    static Volume voxelizeMesh(Mesh* mesh);
    static void normalizeMesh(Mesh* mesh);
    static Mesh* triangulateVolume(Volume volume);
private:
    Voxelizer();

    static std::vector<glm::vec3> getMeshSurfacePoints(Mesh* mesh);
    static void removeDuplicatedVoxels(std::vector<glm::vec3>& voxels);
};

#endif