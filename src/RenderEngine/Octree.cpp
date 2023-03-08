#include "Octree.hpp"

Octree::Octree() {
    this->root= nullptr;
}

Octree::~Octree() {
    delete root;
}

void Octree::build(std::vector<Voxel> data, uint32_t maxDepth) {
    // Initiate the octree root voxel
    Voxel rootVoxel;
    rootVoxel.aabb.min = glm::vec3(std::numeric_limits<float>::max());
    rootVoxel.aabb.max = glm::vec3(std::numeric_limits<float>::min());
    rootVoxel.aabb.center = glm::vec3(0.0f, 0.0f, 0.0f);

    for (const auto voxel : data) {
        if (rootVoxel.aabb.min.x > voxel.position.x) rootVoxel.aabb.min.x = voxel.position.x;
        if (rootVoxel.aabb.min.y > voxel.position.y) rootVoxel.aabb.min.y = voxel.position.y;
        if (rootVoxel.aabb.min.z > voxel.position.z) rootVoxel.aabb.min.z = voxel.position.z;

        if (rootVoxel.aabb.max.x < voxel.position.x) rootVoxel.aabb.max.x = voxel.position.x;
        if (rootVoxel.aabb.max.y < voxel.position.y) rootVoxel.aabb.max.y = voxel.position.y;
        if (rootVoxel.aabb.max.z < voxel.position.z) rootVoxel.aabb.max.z = voxel.position.z;
    }

    rootVoxel.aabb.min -= LENGTH_EPSILON;
    rootVoxel.aabb.max += LENGTH_EPSILON;
    rootVoxel.aabb.center = (rootVoxel.aabb.min + rootVoxel.aabb.max) / 2.0f;

    root = new ONode();
    root->setVoxel(rootVoxel);
    this->maxDepth = maxDepth;
    subdivideNode(root, data, 1);
}

void Octree::subdivideNode(ONode* node, std::vector<Voxel> data, uint32_t depth) {
    if (depth >= maxDepth || data.size() == 0) return;

    Voxel nodeVoxel = node->getVoxel();
    AABB nodeAABB = nodeVoxel.aabb;
    glm::vec3 aabbSize = nodeAABB.max - nodeAABB.min;
    for (float j = nodeAABB.min.y; j < nodeAABB.max.y; j += aabbSize.y / 2.0f)
        for (float k = nodeAABB.min.z; k < nodeAABB.max.z; k += aabbSize.z / 2.0f) 
            for (float i = nodeAABB.min.x; i < nodeAABB.max.x; i += aabbSize.x / 2.0f) {
                AABB subAABB;
                subAABB.min = {i, j ,k};
                subAABB.max = subAABB.min + aabbSize / 2.0f;
                subAABB.center = (subAABB.min + subAABB.max) / 2.0f;

                // Check for each voxel inside the parent node
                std::vector<Voxel> subData;
                for (const auto voxel : data)
                    if (isPointInsideAABB(subAABB, voxel.position))
                        subData.push_back(voxel);

                if (subData.size() > 0) {
                    Voxel subNodeVoxel;
                    subNodeVoxel.aabb = subAABB;
                    subNodeVoxel.normal = getVoxelDataAverageNormal(subData);
                    subNodeVoxel.renderData = 0x00FF8040;

                    ONode* subNode = new ONode();
                    subNode->setVoxel(subNodeVoxel);
                    node->addChild(subNode);
                    subdivideNode(subNode, subData, depth + 1);
                }
            }
}

Mesh* Octree::compressToMesh(uint32_t depth) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<Material> materials;
    traverseGettingLeaves(root, 1, depth, vertices, indices);

    Mesh* volumeRenderMesh = new Mesh();
    volumeRenderMesh->setVertices(vertices);
    volumeRenderMesh->setIndices(indices);
    return volumeRenderMesh;
}

void Octree::traverseGettingLeaves(ONode* node, uint32_t depth, uint32_t maxTraverseDepth, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices) {
    if (depth > maxTraverseDepth || depth > maxDepth)
        return;

    if (node->children.size() == 0) {
        // Is leaf
        Voxel voxel = node->getVoxel();

        uint32_t voxelColor = voxel.renderData;
        glm::vec3 color = glm::vec3((voxelColor & 0x00110000) >> 16, (voxelColor & 0x00001100) >> 8, voxelColor & 0x0000011);        

        Vertex v = {
            .position = voxel.aabb.center,
            .normal = voxel.normal,
            .color = color
        };
        vertices.push_back(v);
        indices.push_back(indices.size());
        return;
    }
    
    for (ONode* child : node->children)
        traverseGettingLeaves(child, depth + 1, maxTraverseDepth, vertices, indices);
}

std::vector<Mesh*> Octree::getDebugMeshes() {
    std::vector<Mesh*> meshes;
    traverseGettingMeshes(root, 1, meshes);
    return meshes;
}

void Octree::traverseGettingMeshes(ONode* node, uint32_t depth, std::vector<Mesh*>& meshes) {
    if (node == nullptr) return;

    float ratio = 2.0f * (depth - 1) / (maxDepth - 1);
    float r = std::max(0.0f, ratio - 1.0f);
    float b = std::max(0.0f, 1.0f - ratio);
    float g = 1.0f - b - r;

    AABB nodeAABB = node->getVoxel().aabb;
    meshes.push_back(Utils::getDebugBoxMesh(nodeAABB, glm::vec3(r, g, b)));
    for (ONode* child : node->children)
        traverseGettingMeshes(child, depth + 1, meshes);
}

glm::vec3 Octree::getVoxelDataAverageNormal(std::vector<Voxel> data) {
    glm::vec3 averageNormal = glm::vec3(0.0f);
    for (Voxel v : data)
        averageNormal += v.normal;
    return glm::normalize(averageNormal);
}

ONode* Octree::getRoot() {
    return root;
}
