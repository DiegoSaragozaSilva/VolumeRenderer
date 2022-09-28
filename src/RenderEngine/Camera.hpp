#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera();
    ~Camera();

    void setPosition(glm::vec3 position);
    void setUpVector(glm::vec3 up);
    void setFocusPoint(glm::vec3 focusPoint);
    void setAspectRatio(float aspectRatio);
    void setFOV(float fov);
    void setNearPlane(float nearPlane);
    void setFarPlane(float farPlane);

    glm::mat4 getViewMatrix();
    glm::mat4 getProjectionMatrix();

    void generateViewMatrix();
    void generateProjectionMatrix();
private:
    float aspectRatio, fov, nearPlane, farPlane;
    glm::vec3 position, up, focusPoint;
    glm::mat4 viewMatrix, projectionMatrix;
};

#endif
