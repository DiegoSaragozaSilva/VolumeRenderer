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

glm::mat4 Camera::getWorldMatrix() {
    return worldMatrix;
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

glm::vec3 Camera::getRightVector() {
    return right;
}

float Camera::getPitch() {
    return pitch;
}

float Camera::getYaw() {
    return yaw;
}

float Camera::getFOV() {
    return fov;
}


void Camera::updateVectors() {
  front = glm::normalize(position - glm::vec3(0.0f, 0.0f, 0.0f));
  right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
  up    = glm::normalize(glm::cross(right, front));

  std::cout << glm::to_string(front) << std::endl;
  std::cout << glm::to_string(right) << std::endl;
  std::cout << glm::to_string(up) << std::endl;
}

void Camera::generateViewMatrix() {
    viewMatrix = glm::lookAt(position, glm::vec3(0.0f, 0.0f, 0.0f), up);
}

void Camera::generateProjectionMatrix() {
    projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}

void Camera::generateWorldMatrix() {
    glm::mat4 translation = glm::mat4(1.0f);
    translation = glm::translate(translation, glm::vec3(0.0f, 0.0f, 0.0f));

    worldMatrix = translation;
}
