#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

struct AABB {
    glm::vec3 min;
    glm::vec3 max;
    glm::vec3 center;
};

#endif