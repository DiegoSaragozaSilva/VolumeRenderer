#include "Camera.hpp"

Camera::Camera() {
    // Standard camera configuration
    aspectRatio = 4.0f / 3.0f;
    nearPlane = 0.1f;
    farPlane = 100.0f;

    #ifndef NDEBUG
        spdlog::info("New camera successfully created.");
    #endif
}

Camera::~Camera() {};

void Camera::setPosition(glm::vec3 position) {
    this->position = position;
}

void Camera::setUpVector(glm::vec3 up) {
    this->up = up;
}

void Camera::setFocusPoint(glm::vec3 focusPoint) {
    this->focusPoint = focusPoint;
}

void Camera::setAspectRatio(float aspectRatio) {
    this->aspectRatio = aspectRatio;
}

void Camera::setFOV(float fov) {
    this->fov = fov;
}

void Camera::setNearPlane(float nearPlane) {
    this->nearPlane = nearPlane;
}

void Camera::setFarPlane(float farPlane) {
    this->farPlane = farPlane;
}

glm::mat4 Camera::getViewMatrix() {
    return viewMatrix;
}

glm::mat4 Camera::getProjectionMatrix() {
    return projectionMatrix;
}

void Camera::generateViewMatrix() {
    // View matrix generation
    viewMatrix = glm::lookAt(position, focusPoint, up);
}

void Camera::generateProjectionMatrix() {
    // Projection matrix generation
    projectionMatrix = glm::perspective(fov, aspectRatio, nearPlane, farPlane);
}
