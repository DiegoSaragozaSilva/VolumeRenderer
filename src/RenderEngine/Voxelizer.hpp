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
#include "Geometry.hpp"
#include "Utils.hpp"

struct Volume {
    glm::vec3 scale;
    std::vector<Voxel> voxels;
};

class Voxelizer {
public:
    static int scale;
    static int density;

    static Volume voxelizeMesh(Mesh* mesh);
    static Volume voxelizeVolumetricData(VolumetricData data);
private:
    Voxelizer();

    static std::vector<Voxel> getMeshSurfacePoints(Mesh* mesh);
    static void normalizeMesh(Mesh* mesh);
    static void removeDuplicatedVoxels(std::vector<Voxel>& voxels);
};

#endif
