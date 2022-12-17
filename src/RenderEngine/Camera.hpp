#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera();
    ~Camera();

    bool isOrbital;

    void setPosition(glm::vec3 position);
    void setUpVector(glm::vec3 up);
    void setFrontVector(glm::vec3 front);
    void setPivot(glm::vec3 pivot);
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
    glm::vec3 getPivotPoint();
    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix();
    float getPitch();
    float getYaw();
    float getFOV();

    void generateViewMatrix();
    void generateProjectionMatrix();
private:
    float aspectRatio, fov, nearPlane, farPlane, pitch, yaw;
    glm::vec3 position, up, front, pivot;
    glm::mat4 viewMatrix, projectionMatrix;
};

#endif
