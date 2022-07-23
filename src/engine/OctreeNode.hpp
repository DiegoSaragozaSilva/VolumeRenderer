#pragma once

#include <glm/glm.hpp>
#include <array>
#include <cstdint>

class OctreeNode {
public:
    OctreeNode();
    ~OctreeNode();

    glm::vec3 minPoint, maxPoint;

    void addChild(OctreeNode* child, uint8_t index);
    OctreeNode* getChild(uint8_t index);
private:
    uint8_t data;
    std::array<OctreeNode*, 8> childs;
};
