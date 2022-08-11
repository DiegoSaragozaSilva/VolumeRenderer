#include "Octree.hpp"

Octree::Octree(std::vector<glm::vec3> volume, int maxDepth) {
    this->maxDepth = maxDepth;

    generateOctree(volume);
}

Octree::~Octree() {}

void Octree::generateOctree(std::vector<glm::vec3> volume) { 
    root = new OctreeNode();

    root->minPoint = glm::vec3(std::numeric_limits<float>::max());
    root->maxPoint = glm::vec3(std::numeric_limits<float>::min());
 
    for (const auto& point : volume) {
        root->minPoint.x = point.x < root->minPoint.x ? point.x : root->minPoint.x;
        root->minPoint.y = point.y < root->minPoint.y ? point.y : root->minPoint.y;
        root->minPoint.z = point.z < root->minPoint.z ? point.z : root->minPoint.z;
    
        root->maxPoint.x = point.x > root->maxPoint.x ? point.x : root->maxPoint.x;
        root->maxPoint.y = point.y > root->maxPoint.y ? point.y : root->maxPoint.y;
        root->maxPoint.z = point.z > root->maxPoint.z ? point.z : root->maxPoint.z;
    }

    subdivideNode(root, volume, 0);
}

void Octree::subdivideNode(OctreeNode* node, std::vector<glm::vec3> nodeVolume, uint32_t currentDepth) {
    if (currentDepth == maxDepth) {
        node->isLeaf = true;
        return;
    }

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

std::vector<uint32_t> Octree::compressToTexture() {
    uint32_t numNodes = getNodeSize(root);

    uint32_t index = 0;
    std::vector<uint32_t> data = compressNode(root, &index); 
    return data; 
}

std::vector<uint32_t> Octree::compressNode(OctreeNode* node, uint32_t* nodeIndex) {
    std::vector<uint32_t> compressedNode;
    std::vector<uint32_t> compressedChilds;
    for (uint32_t i = 0; i < 8; i++) {
        OctreeNode* child = node->getChild(i);
        if (child != nullptr) {
            *nodeIndex += 1;
            uint32_t lastIndex = *nodeIndex;
            std::vector<uint32_t> compressedChild = compressNode(child, nodeIndex);
            compressedChilds.insert(compressedChilds.end(), compressedChild.begin(), compressedChild.end());
            
            if (lastIndex == *nodeIndex)
                compressedNode.insert(compressedNode.end(), (0xF010F0 << 8) | 0xFF);
            else 
                compressedNode.insert(compressedNode.end(), (lastIndex << 8) | 0x7F);
        }
        else
            compressedNode.insert(compressedNode.end(), 0);
    }
    
    compressedNode.insert(compressedNode.end(), compressedChilds.begin(), compressedChilds.end());
    return compressedNode;
}

uint32_t Octree::getNodeSize(OctreeNode* node) {
    uint32_t numChilds = 0;
    for (uint32_t i = 0; i < 8; i++) {
        OctreeNode* child = node->getChild(i);
        if (child != nullptr) {
            numChilds++;
            numChilds += getNodeSize(child);
        }
    }
    return numChilds;
}
