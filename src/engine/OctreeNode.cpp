#include "OctreeNode.hpp"

OctreeNode::OctreeNode() {
    for (uint8_t i = 0; i < 8; i++)
        childs[i] = nullptr;
}

OctreeNode::~OctreeNode() {}

void OctreeNode::addChild(OctreeNode* child, uint8_t index) {
    childs[index] = child;
}

OctreeNode* OctreeNode::getChild(uint8_t index) {
    return childs[index];
}
