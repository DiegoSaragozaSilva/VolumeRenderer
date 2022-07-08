#include "Transform.hpp"

Transform::Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) {
    this->position = position;
    this->rotation = rotation;
    this->scale = scale;
}

Transform::~Transform() {}

glm::vec3 Transform::getPosition() {
    return position;
}

glm::vec3 Transform::getRotation() {
    return rotation;
}

glm::vec3 Transform::getScale() {
    return scale;
}

void Transform::setPosition(glm::vec3 position) {
    this->position = position;
}

void Transform::setRotation(glm::vec3 rotation) {
    this->rotation = rotation;
}

void Transform::setScale(glm::vec3 scale) {
    this->scale = scale;
}

glm::mat4 Transform::getModelMatrix() {
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    modelMatrix = glm::scale(modelMatrix, scale);
    return modelMatrix;
}
