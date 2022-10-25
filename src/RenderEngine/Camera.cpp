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

void Camera::setFrontVector(glm::vec3 front) {
    this->front = front;
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

void Camera::setPitch(float pitch) {
    this->pitch = pitch;
}

void Camera::setYaw(float yaw) {
    this->yaw = yaw;
}

glm::mat4 Camera::getViewMatrix() {
    return viewMatrix;
}

glm::mat4 Camera::getProjectionMatrix() {
    return projectionMatrix;
}

glm::vec3 Camera::getPosition() {
    return position;
}

glm::vec3 Camera::getUpVector() {
    return up;
}

glm::vec3 Camera::getFrontVector() {
    return front;
}

float Camera::getPitch() {
    return pitch;
}

float Camera::getYaw() {
    return yaw;
}

void Camera::generateViewMatrix() {
    // View matrix generation
    glm::mat4 translation = glm::mat4(1.0f);
    translation = glm::rotate(translation, yaw, glm::vec3(0.0f, 1.0f, 0.0f));
    translation = glm::rotate(translation, pitch, glm::vec3(-1.0f, 0.0f, 0.0f));

    front = glm::normalize(translation * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));
    glm::vec3 right = glm::normalize(translation * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)) * glm::tan(glm::radians(fov) * 0.5f) * aspectRatio;
    up = glm::normalize(glm::cross(front, right)) * glm::tan(glm::radians(fov) * 0.5f);

    viewMatrix = glm::lookAt(position, position + front, up);
}

void Camera::generateProjectionMatrix() {
    // Projection matrix generation
    projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}
