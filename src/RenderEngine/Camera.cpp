#include "Camera.hpp"

Camera::Camera() {
    // Standard camera configuration
    aspectRatio = 4.0f / 3.0f;
    nearPlane = 0.1f;
    farPlane = 100.0f;
    isOrbital = false;

    pivot = glm::vec3(0, 0, 0);

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

void Camera::setPivot(glm::vec3 pivot) {
    this->pivot = pivot;
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

glm::vec3 Camera::getPivotPoint() {
    return pivot;
}

glm::vec3 Camera::getRightVector() {
    return glm::transpose(viewMatrix)[0];
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

void Camera::generateViewMatrix() {
    if (!isOrbital) {
        glm::mat4 rotation = glm::mat4(1.0f);
        rotation = glm::rotate(rotation, yaw, glm::vec3(0.0f, 1.0f, 0.0f));
        rotation = glm::rotate(rotation, pitch, glm::vec3(-1.0f, 0.0f, 0.0f));

        front = glm::normalize(rotation * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));
        glm::vec3 right = glm::normalize(rotation * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)) * glm::tan(glm::radians(fov) * 0.5f) * aspectRatio;
        up = glm::normalize(glm::cross(front, right)) * glm::tan(glm::radians(fov) * 0.5f);

        viewMatrix = glm::lookAt(position, position + front, up);
    }
    else {
        glm::mat4 rotation = glm::mat4(1.0f);
        rotation = glm::rotate(rotation, yaw, glm::vec3(0.0f, 1.0f, 0.0f));
        rotation = glm::rotate(rotation, pitch, glm::vec3(-1.0f, 0.0f, 0.0f));

        front = glm::normalize(rotation * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));
        glm::vec3 right = glm::normalize(rotation * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f)) * glm::tan(glm::radians(fov) * 0.5f) * aspectRatio;
        up = glm::normalize(glm::cross(front, right)) * glm::tan(glm::radians(fov) * 0.5f);

        glm::vec4 focus = glm::vec4(position - pivot, 1);
    
        rotation = glm::mat4(1.0f);
        rotation = glm::rotate(rotation, yaw, up);
        focus = rotation * focus;

        rotation = glm::mat4(1.0f);
        rotation = glm::rotate(rotation, pitch, right);
        focus = rotation * focus;

        position = pivot + glm::vec3(focus);
        viewMatrix = glm::lookAt(position, pivot, glm::vec3(0, 1, 0));
    }
}

void Camera::generateProjectionMatrix() {
    // Projection matrix generation
    projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}
