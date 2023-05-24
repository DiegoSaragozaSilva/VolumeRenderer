#ifndef _OCTREE_HPP_
#define _OCTREE_HPP_

#define LENGTH_EPSILON 1e-3

#include <algorithm>
#include <vector>
#include <limits>
#include <glm/gtx/string_cast.hpp>

#include "ONode.hpp"
#include "Utils.hpp"
#include "Geometry.hpp"

class Octree {
public:
    Octree();
    ~Octree();

    void build(std::vector<Voxel> data, uint32_t maxDepth);
    ImageData compressToImage(uint32_t depth);
    Mesh* getDebugMesh(uint32_t depth);
    ONode* getRoot();
private:
    ONode* root;
    uint32_t maxDepth;

    void subdivideNode(ONode* node, std::vector<Voxel> data, uint32_t depth);
    void traverseGettingMeshes(ONode* node, uint32_t depth, uint32_t maxTraverseDepth, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
    void traverseGettingLeaves(ONode* node, uint32_t depth, uint32_t maxTraverseDepth, std::vector<uint8_t>& volumeData); 
    glm::vec3 getVoxelDataAverageNormal(std::vector<Voxel> data);
    glm::vec4 getVoxelDataAverageColor(std::vector<Voxel> data);
};

#endif
