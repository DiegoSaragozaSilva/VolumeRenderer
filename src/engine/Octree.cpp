#include "Octree.hpp"

Octree::Octree(int maxDepth) {
    this->maxDepth = maxDepth;
}

Octree::~Octree() {}

void Octree::setVolumeData(std::vector<glm::vec3> volumeData) {
    this->volumeData = volumeData;
}

void Octree::generateOctree() { 
    if (volumeData.size() == 0) {
        spdlog::warn("Octree volume data is not set. Octree will not be constructed!");
        return;
    }

    spdlog::info("Generating octree with max depth {}...", maxDepth == -1 ? DEFAULT_MAX_DEPTH : maxDepth);
    root = new OctreeNode();

    root->minPoint = glm::vec3(std::numeric_limits<float>::max());
    root->maxPoint = glm::vec3(std::numeric_limits<float>::min());
 
    for (const auto& point : volumeData) {
        root->minPoint.x = point.x < root->minPoint.x ? point.x : root->minPoint.x;
        root->minPoint.y = point.y < root->minPoint.y ? point.y : root->minPoint.y;
        root->minPoint.z = point.z < root->minPoint.z ? point.z : root->minPoint.z;
    
        root->maxPoint.x = point.x > root->maxPoint.x ? point.x : root->maxPoint.x;
        root->maxPoint.y = point.y > root->maxPoint.y ? point.y : root->maxPoint.y;
        root->maxPoint.z = point.z > root->maxPoint.z ? point.z : root->maxPoint.z;
    }

    subdivideNode(root, volumeData, 0);
    spdlog::info("Octree generated successfully!");
}

void Octree::subdivideNode(OctreeNode* node, std::vector<glm::vec3> nodeVolume, uint32_t currentDepth) {
    if (maxDepth == -1 && currentDepth == DEFAULT_MAX_DEPTH)
        return;
    if (maxDepth != - 1 && currentDepth == maxDepth)
        return;

    glm::vec3 volumeLength = {
        fabs(node->maxPoint.x - node->minPoint.x),
        fabs(node->maxPoint.y - node->minPoint.y),
        fabs(node->maxPoint.z - node->minPoint.z)
    };

    uint8_t index = 0;
    for (float y = node->minPoint.y; y < node->maxPoint.y; y += volumeLength.y / 2.f)
        for (float z = node->minPoint.z; z < node->maxPoint.z; z += volumeLength.z / 2.f)
            for (float x = node->minPoint.x; x < node->maxPoint.x; x += volumeLength.x / 2.f) {                
                glm::vec3 childMinPoint = {x, y, z};
                glm::vec3 childMaxPoint = {x + volumeLength.x / 2.f, y + volumeLength.y / 2.f, z + volumeLength.z / 2.f};
                std::vector<glm::vec3> childVolume;
                for (const auto& point : nodeVolume) {
                    if (glm::all(glm::greaterThanEqual(point, childMinPoint)) && glm::all(glm::lessThanEqual(point, childMaxPoint)))
                        childVolume.push_back(point);
                }
                
                if (childVolume.size() == 0) continue;
   
                OctreeNode* child = new OctreeNode();
                child->minPoint = childMinPoint;
                child->maxPoint = childMaxPoint;

                node->addChild(child, index);
                subdivideNode(child, childVolume, currentDepth + 1);
                index++;
            }
}

std::vector<Vertex> Octree::_generateMesh() {
    if (root == nullptr) {
        spdlog::warn("No octree data. No mesh data will be generated!");
        return {};
    }

    return _getMeshFromNode(root); 
}

std::vector<Vertex> Octree::_getMeshFromNode(OctreeNode* node) {
    if (node == nullptr)
        return {};

    bool isLeaf = true;
    for (uint8_t i = 0; i < 8; i++)
        if (node->getChild(i) != nullptr) {
            isLeaf = false;
            break;
        }

    std::vector<Vertex> nodeVertices;
    if (!isLeaf) {
        for (uint8_t i = 0; i < 8; i++) {
            std::vector<Vertex> childVertices = _getMeshFromNode(node->getChild(i));
            nodeVertices.insert(nodeVertices.end(), childVertices.begin(), childVertices.end());
        }

        return nodeVertices;
    }

    glm::vec3 volumeLength = {
        fabs(node->maxPoint.x - node->minPoint.x),
        fabs(node->maxPoint.y - node->minPoint.y),
        fabs(node->maxPoint.z - node->minPoint.z)
    };

    for (float y = node->minPoint.y; y <= node->maxPoint.y; y += volumeLength.y)
        for (float z = node->minPoint.z; z <= node->maxPoint.z; z += volumeLength.z)
            for (float x = node->minPoint.x; x <= node->maxPoint.x; x += volumeLength.x) {
                glm::vec3 pos = {x, y, z};
                glm::vec3 color = {0.5f, 1.f, 0.25f};
                glm::vec2 uv = {0.0f, 0.0f}; 

                Vertex v = Vertex(pos, color, uv);
                nodeVertices.push_back(v);
            }

    return nodeVertices;
}

std::vector<uint32_t> Octree::_generateIndices(uint32_t numVertices) {
    std::vector<uint32_t> indices;
    for (uint32_t i = 0, j = 0; i < numVertices / 8; i++, j = i * 8) {
        // front face
        indices.insert(indices.end(), {0 + j, 1 + j, 5 + j});
        indices.insert(indices.end(), {5 + j, 4 + j, 0 + j});

        // back face
        indices.insert(indices.end(), {3 + j, 2 + j, 6 + j});
        indices.insert(indices.end(), {6 + j, 7 + j, 3 + j});

        // top face
        indices.insert(indices.end(), {4 + j, 5 + j, 7 + j});
        indices.insert(indices.end(), {7 + j, 6 + j, 4 + j});
        
        // bottom face
        indices.insert(indices.end(), {0 + j, 2 + j, 3 + j});
        indices.insert(indices.end(), {3 + j, 1 + j, 0 + j});
    
        // left face
        indices.insert(indices.end(), {2 + j, 0 + j, 4 + j});
        indices.insert(indices.end(), {4 + j, 6 + j, 2 + j});
    
        // right face
        indices.insert(indices.end(), {1 + j, 3 + j, 7 + j});
        indices.insert(indices.end(), {7 + j, 5 + j, 1 + j});
    }

    return indices;
}

void Octree::setMaxDepth(int maxDepth, bool recreate) {
    this->maxDepth = maxDepth;

    if (recreate)
        generateOctree();
}
