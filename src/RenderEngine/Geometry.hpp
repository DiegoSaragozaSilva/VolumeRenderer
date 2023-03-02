#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include <glm/glm.hpp>

struct AABB {
    glm::vec3 min;
    glm::vec3 max;
    glm::vec3 center;
};

struct Voxel {
    /*
        DATA SENT TO THE GPU

        renderData
        8 bits + 24 bits = 32 bits
        | MaterialID | R | G | B |

        normal
        4 bytes = 32 bits
        | nx | ny | nz |
    */
    uint32_t renderData;
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
