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
    if (depth >= maxDepth) return;

    Voxel nodeVoxel = node->getVoxel();
    AABB nodeAABB = nodeVoxel.aabb;
    glm::vec3 aabbSize = nodeAABB.max - nodeAABB.min;
    for (float j = nodeAABB.min.y; j < nodeAABB.max.y - LENGTH_EPSILON; j += aabbSize.y / 2.0f)
        for (float k = nodeAABB.min.z; k < nodeAABB.max.z - LENGTH_EPSILON; k += aabbSize.z / 2.0f) 
            for (float i = nodeAABB.min.x; i < nodeAABB.max.x - LENGTH_EPSILON; i += aabbSize.x / 2.0f) {
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
                    subNodeVoxel.color = getVoxelDataAverageColor(subData);

                    ONode* subNode = new ONode();
                    subNode->setVoxel(subNodeVoxel);
                    node->addChild(subNode);
                    subdivideNode(subNode, subData, depth + 1);
                }
                else {
                    Voxel subNodeVoxel;
                    subNodeVoxel.aabb = subAABB;
                    subNodeVoxel.normal = glm::vec3(0.0f);
                    subNodeVoxel.color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

                    ONode* subNode = new ONode();
                    subNode->setVoxel(subNodeVoxel);
                    node->addChild(subNode);
                    subdivideNode(subNode, subData, depth + 1);
                }
            }
}

ImageData Octree::compressToImage(uint32_t depth) {
    uint32_t textureSize = std::pow(2, depth - 1);
    std::vector<uint8_t> voxelData(textureSize * textureSize * textureSize * 4);
    std::fill(voxelData.begin(), voxelData.end(), 0);

    traverseGettingLeaves(root, 1, depth, voxelData);

    ImageData octreeImageData;
    octreeImageData.name = "Octree Image Data";
    octreeImageData.width = textureSize;
    octreeImageData.height = textureSize;
    octreeImageData.depth = textureSize;
    octreeImageData.data = voxelData;
    return octreeImageData;
}

void Octree::traverseGettingLeaves(ONode* node, uint32_t depth, uint32_t maxTraverseDepth, std::vector<uint8_t>& voxelData) {
    if (depth > maxTraverseDepth || depth > maxDepth)
        return;
    
    if (node != nullptr && depth == maxTraverseDepth) {
        Voxel voxel = node->getVoxel();
        glm::vec4 voxelColor = voxel.color;
        
        Voxel rootVoxel = this->root->getVoxel(); 
        
        glm::vec3 offset = glm::abs(rootVoxel.aabb.min);
        glm::vec3 shiftedMin = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 shiftedMax = rootVoxel.aabb.max + offset;
        glm::vec3 shiftedPoint = voxel.aabb.center + offset;

        float gridSize = std::pow(2.0f, depth - 1);
        glm::vec3 voxelSize = glm::vec3(shiftedMax / gridSize);
        glm::vec3 voxelIndex = glm::floor(shiftedPoint / voxelSize);

        int index = voxelIndex.x + voxelIndex.z * gridSize + voxelIndex.y * gridSize * gridSize;
        
        voxelData[index * 4 + 0] = (uint8_t)(voxelColor.x * 255.0f);
        voxelData[index * 4 + 1] = (uint8_t)(voxelColor.y * 255.0f);
        voxelData[index * 4 + 2] = (uint8_t)(voxelColor.z * 255.0f);
        voxelData[index * 4 + 3] = (uint8_t)(voxelColor.w * 255.0f);

        return;
    }
    
    for (ONode* child : node->children)
        traverseGettingLeaves(child, depth + 1, maxTraverseDepth, voxelData);
}

Mesh* Octree::getDebugMesh(uint32_t depth) {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<Material> materials;
    traverseGettingMeshes(root, 1, depth, vertices, indices);

    Mesh* debugRenderMesh = new Mesh();
    debugRenderMesh->setVertices(vertices);
    debugRenderMesh->setIndices(indices);
    return debugRenderMesh;
}

void Octree::traverseGettingMeshes(ONode* node, uint32_t depth, uint32_t maxTraverseDepth, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices) {
    if (node == nullptr) return; 

    float ratio = 2.0f * (depth - 1) / (maxDepth - 1);
    float r = std::max(0.0f, ratio - 1.0f);
    float b = std::max(0.0f, 1.0f - ratio);
    float g = 1.0f - b - r;

    AABB nodeAABB = node->getVoxel().aabb;
    Mesh* debugBox = Utils::getDebugBoxMesh(nodeAABB, glm::vec4(r, g, b, 1.0f));
    std::vector<Vertex> debugVertices = debugBox->getVertices();
    std::vector<uint32_t> debugIndices = debugBox->getIndices();

    vertices.insert(vertices.end(), debugVertices.begin(), debugVertices.end());

    std::transform(debugIndices.begin(), debugIndices.end(), debugIndices.begin(), [vertices](uint32_t x) {return x + vertices.size();});
    indices.insert(indices.end(), debugIndices.begin(), debugIndices.end());

    for (ONode* child : node->children)
        traverseGettingMeshes(child, depth + 1, maxTraverseDepth, vertices, indices);
}

glm::vec3 Octree::getVoxelDataAverageNormal(std::vector<Voxel> data) {
    glm::vec3 averageNormal = glm::vec3(0.0f);
    for (Voxel v : data)
        averageNormal += v.normal;
    return averageNormal * (1.0f / data.size());
}

glm::vec4 Octree::getVoxelDataAverageColor(std::vector<Voxel> data) {
    glm::vec4 averageColor = glm::vec4(0.0f);
    for (Voxel v : data)
        averageColor += v.color;

    averageColor /= data.size();

    return averageColor;   
}

ONode* Octree::getRoot() {
    return root;
}
