#include "ONode.hpp"

ONode::ONode() {
    this->isLeaf = false;
}

ONode::~ONode() {
    for (const auto child : children)
        delete child;
}

void ONode::addChild(ONode* child) {
    children.push_back(child);
}

//void ONode::setVoxelRenderData(uint8_t materialID, uint8_t r, uint8_t g, uint8_t b) {
//    voxel.renderData = 0x0000;
//    voxel.renderData += materialID << 24;
//    voxel.renderData += r          << 16;
//    voxel.renderData += g          << 8;
//    voxel.renderData += b;
//}
//
//void ONode::setVoxelNormal(glm::vec3 normal) {
//    voxel.normal = normal;
//}

void ONode::setVoxel(Voxel voxel) {
    this->voxel = voxel;
}

Voxel ONode::getVoxel() {
    return voxel;
}
