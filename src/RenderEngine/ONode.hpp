#ifndef _ONODE_HPP_
#define _ONODE_HPP_

#include <stdint.h>
#include <vector>
#include <glm/glm.hpp>

#include "Geometry.hpp"

class ONode {
public:
    ONode();
    ~ONode();

    std::vector<ONode*> children;

    void addChild(ONode* child);
    void setVoxel(Voxel voxel);
    Voxel getVoxel();
private:
    bool isLeaf;
    Voxel voxel;
};

#endif
