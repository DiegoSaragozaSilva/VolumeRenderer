#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <spdlog/spdlog.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

class Camera {
public:
    Camera();
    ~Camera();

    void setPosition(glm::vec3 position);
    void setUpVector(glm::vec3 up);
    void setFrontVector(glm::vec3 front);
    void setAspectRatio(float aspectRatio);
    void setFOV(float fov);
    void setNearPlane(float nearPlane);
    void setFarPlane(float farPlane);
    void setPitch(float pitch);
    void setYaw(float yaw);

    glm::vec3 getPosition();
    glm::vec3 getUpVector();
    glm::vec3 getRightVector();
    glm::vec3 getFrontVector();
    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix();
    glm::mat4 getWorldMatrix();
    float getPitch();
    float getYaw();
    float getFOV();

    void updateVectors();
    void generateViewMatrix();
    void generateProjectionMatrix();
    void generateWorldMatrix();
private:
    float aspectRatio, fov, nearPlane, farPlane, pitch, yaw;
    glm::vec3 position, up, front, right;
    glm::mat4 worldMatrix, viewMatrix, projectionMatrix;
};

#endif
