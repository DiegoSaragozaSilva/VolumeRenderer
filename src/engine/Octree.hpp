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
    Octree() { maxDepth = -1;}
    Octree(int maxDepth);
    ~Octree();

    void setVolumeData(std::vector<glm::vec3> volumeData);
    void generateOctree();
    std::vector<Vertex> _generateMesh();
    std::vector<uint32_t> _generateIndices(uint32_t numVertices);
    void setMaxDepth(int maxDepth, bool recreate);
private:
    OctreeNode* root;
    int maxDepth;
    std::vector<glm::vec3> volumeData;
    
    void subdivideNode(OctreeNode* node, std::vector<glm::vec3> nodeVolume, uint32_t currentDepth);
    std::vector<Vertex> _getMeshFromNode(OctreeNode* node);
};
