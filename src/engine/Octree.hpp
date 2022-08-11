#pragma once

#define DEFAULT_MAX_DEPTH 5
#define MIN_POINTS 1

#include <limits>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>

#include "OctreeNode.hpp"
#include "../core/Vertex.hpp"

class Octree {
public:
    Octree(std::vector<glm::vec3> volume, int maxDepth);
    ~Octree();

    void generateOctree(std::vector<glm::vec3> volume);
    std::vector<uint32_t> compressToTexture();
private:
    OctreeNode* root;
    int maxDepth;
        
    void subdivideNode(OctreeNode* node, std::vector<glm::vec3> nodeVolume, uint32_t currentDepth);
    std::vector<uint32_t> compressNode(OctreeNode* node, uint32_t* nodeIndex);
    uint32_t getNodeSize(OctreeNode* node);
};
