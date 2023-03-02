#include "Geometry.hpp"

bool isPointInsideAABB(AABB aabb, glm::vec3 point) {
    return (point.x >= aabb.min.x && point.y >= aabb.min.y && point.z >= aabb.min.z) &&
           (point.x <= aabb.max.x && point.y <= aabb.max.y && point.z <= aabb.max.z);
}
