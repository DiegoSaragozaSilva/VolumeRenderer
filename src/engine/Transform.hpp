#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Transform {
public:
    Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
    ~Transform();

    glm::vec3 getPosition();
    glm::vec3 getRotation();
    glm::vec3 getScale();
    void setPosition(glm::vec3 position);
    void setRotation(glm::vec3 rotation);
    void setScale(glm::vec3 scale);
    glm::mat4 getModelMatrix();
private:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};
