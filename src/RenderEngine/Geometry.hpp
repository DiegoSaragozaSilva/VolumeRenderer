#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include <glm/glm.hpp>

struct AABB {
    glm::vec3 min;
    glm::vec3 max;
    glm::vec3 center;
};

struct Voxel {
    glm::vec4 color;
    glm::vec3 normal;
    glm::vec3 position;
    AABB aabb;

    bool operator<(Voxel& a) {
        return glm::all(glm::lessThan(position, a.position));
    }

    bool operator==(Voxel& a) {
        return glm::all(glm::equal(position, a.position));
    }
};

bool isPointInsideAABB(AABB aabb, glm::vec3 point);

#endif
