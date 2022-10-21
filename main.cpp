#include <iostream>
#include "src/RenderEngine/RenderEngine.hpp"

#define CAMERA_SPEED 50.0f
#define CAMERA_ROTATE_SPEED 0.01f

glm::vec2 previousMousePos = {-1.0f, -1.0f};
RenderEngine* renderEngine;

void processInput(Window* window) {
    // Keyboard movement
    if (glfwGetKey(window->getWindow(), GLFW_KEY_W) == GLFW_PRESS) {
        glm::vec3 oldPosition = renderEngine->camera->getPosition();
        glm::vec3 frontVector = renderEngine->camera->getFrontVector();
        renderEngine->camera->setPosition(oldPosition + (float)(CAMERA_SPEED * renderEngine->getDeltaTime()) * frontVector);
        renderEngine->camera->generateViewMatrix();
    }

    if (glfwGetKey(window->getWindow(), GLFW_KEY_S) == GLFW_PRESS) {
        glm::vec3 oldPosition = renderEngine->camera->getPosition();
        glm::vec3 frontVector = renderEngine->camera->getFrontVector();
        renderEngine->camera->setPosition(oldPosition - (float)(CAMERA_SPEED * renderEngine->getDeltaTime()) * frontVector);
        renderEngine->camera->generateViewMatrix();
    }

    if (glfwGetKey(window->getWindow(), GLFW_KEY_D) == GLFW_PRESS) {
        glm::vec3 oldPosition = renderEngine->camera->getPosition();
        glm::vec3 frontVector = renderEngine->camera->getFrontVector();
        glm::vec3 upVector = renderEngine->camera->getUpVector();
        glm::vec3 rightVector = glm::normalize(glm::cross(frontVector, upVector));
        renderEngine->camera->setPosition(oldPosition + (float)(CAMERA_SPEED * renderEngine->getDeltaTime()) * rightVector);
        renderEngine->camera->generateViewMatrix();
    }
    
    if (glfwGetKey(window->getWindow(), GLFW_KEY_A) == GLFW_PRESS) {
        glm::vec3 oldPosition = renderEngine->camera->getPosition();
        glm::vec3 frontVector = renderEngine->camera->getFrontVector();
        glm::vec3 upVector = renderEngine->camera->getUpVector();
        glm::vec3 rightVector = glm::normalize(glm::cross(frontVector, upVector));
        renderEngine->camera->setPosition(oldPosition - (float)(CAMERA_SPEED * renderEngine->getDeltaTime()) * rightVector);
        renderEngine->camera->generateViewMatrix();
    }

    // Mouse rotation
    if (glfwGetMouseButton(window->getWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        double mouseX, mouseY;
        glfwGetCursorPos(window->getWindow(), &mouseX, &mouseY);
        glm::vec2 mousePos = {(float)mouseX, (float)mouseY};

        if (previousMousePos.x == -1.0f && previousMousePos.y == -1.0f)
            previousMousePos = mousePos;

        glm::vec2 delta = {mousePos.x - previousMousePos.x, mousePos.y - previousMousePos.y};
        delta *= CAMERA_ROTATE_SPEED;

        float newYaw = renderEngine->camera->getYaw() - delta.x;
        float newPitch = renderEngine->camera->getPitch() - delta.y;
        newYaw = glm::mod(newYaw, glm::pi<float>() * 2.0f);
        newPitch = glm::clamp(newPitch, -(glm::pi<float>() * 0.5f), glm::pi<float>() * 0.5f);

        renderEngine->camera->setYaw(newYaw);
        renderEngine->camera->setPitch(newPitch);
        renderEngine->camera->generateViewMatrix();

        previousMousePos = mousePos;
    }
    if (glfwGetMouseButton(window->getWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
        // Reset previous mouse position
        previousMousePos.x = -1.0f;
        previousMousePos.y = -1.0f;
    }
}

int main() {
    renderEngine = new RenderEngine();

    while (!renderEngine->window->shouldClose()) {
        processInput(renderEngine->window);

        renderEngine->renderFrame();
    }

    delete renderEngine;

    return 1;
}
