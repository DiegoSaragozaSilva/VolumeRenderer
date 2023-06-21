#include "src/RenderEngine/RenderEngine.hpp"
#include "src/RenderEngine/Octree.hpp"

#define CAMERA_SPEED 10.0f
#define CAMERA_ROTATE_SPEED 0.01f
#define CAMERA_ZOOM_SPEED 10.0f
#define ORBITAL_RADIUS 100.0f

glm::vec2 previousMousePos = {0.0f, 0.0f};

RenderEngine* renderEngine;

void processInput(Window* window) {
  if (glfwGetKey(window->getWindow(), GLFW_KEY_W) == GLFW_PRESS) {           
      glm::vec3 oldPos = renderEngine->camera->getPosition();
      glm::vec3 newPos = oldPos - renderEngine->camera->getFrontVector() * CAMERA_SPEED * (float)renderEngine->getDeltaTime(); 
      newPos = glm::vec3(
        glm::clamp(newPos.x, -ORBITAL_RADIUS, ORBITAL_RADIUS),
        glm::clamp(newPos.y, -ORBITAL_RADIUS, ORBITAL_RADIUS),
        glm::clamp(newPos.z, -ORBITAL_RADIUS, ORBITAL_RADIUS)
      );

      renderEngine->camera->setPosition(newPos);

      renderEngine->camera->generateViewMatrix();
      renderEngine->camera->generateProjectionMatrix();
  }

  if (glfwGetKey(window->getWindow(), GLFW_KEY_S) == GLFW_PRESS) {
      glm::vec3 oldPos = renderEngine->camera->getPosition();
      glm::vec3 newPos = oldPos + renderEngine->camera->getFrontVector() * CAMERA_SPEED * (float)renderEngine->getDeltaTime();
      newPos = glm::vec3(
        glm::clamp(newPos.x, -ORBITAL_RADIUS, ORBITAL_RADIUS),
        glm::clamp(newPos.y, -ORBITAL_RADIUS, ORBITAL_RADIUS),
        glm::clamp(newPos.z, -ORBITAL_RADIUS, ORBITAL_RADIUS)
      );  

      renderEngine->camera->setPosition(newPos);

      renderEngine->camera->generateViewMatrix();
  }

  if (glfwGetKey(window->getWindow(), GLFW_KEY_D) == GLFW_PRESS) {           
      glm::vec3 oldPos = renderEngine->camera->getPosition();
      glm::vec3 newPos = oldPos - renderEngine->camera->getRightVector() * CAMERA_SPEED * (float)renderEngine->getDeltaTime();
      newPos = glm::vec3(
        glm::clamp(newPos.x, -ORBITAL_RADIUS, ORBITAL_RADIUS),
        glm::clamp(newPos.y, -ORBITAL_RADIUS, ORBITAL_RADIUS),
        glm::clamp(newPos.z, -ORBITAL_RADIUS, ORBITAL_RADIUS)
      );

      renderEngine->camera->setPosition(newPos);

      renderEngine->camera->generateViewMatrix();
      renderEngine->camera->generateProjectionMatrix();
  }

  if (glfwGetKey(window->getWindow(), GLFW_KEY_A) == GLFW_PRESS) {
      glm::vec3 oldPos = renderEngine->camera->getPosition();
      glm::vec3 newPos = oldPos + renderEngine->camera->getRightVector() * CAMERA_SPEED * (float)renderEngine->getDeltaTime();
      newPos = glm::vec3(
        glm::clamp(newPos.x, -ORBITAL_RADIUS, ORBITAL_RADIUS),
        glm::clamp(newPos.y, -ORBITAL_RADIUS, ORBITAL_RADIUS),
        glm::clamp(newPos.z, -ORBITAL_RADIUS, ORBITAL_RADIUS)
      );

      renderEngine->camera->setPosition(newPos);

      renderEngine->camera->generateViewMatrix();
      renderEngine->camera->generateProjectionMatrix();
  }

  if (glfwGetKey(window->getWindow(), GLFW_KEY_Q) == GLFW_PRESS) {           
      glm::vec3 oldPos = renderEngine->camera->getPosition();
      glm::vec3 newPos = oldPos - renderEngine->camera->getUpVector() * CAMERA_SPEED * (float)renderEngine->getDeltaTime();
      newPos = glm::vec3(
        glm::clamp(newPos.x, -ORBITAL_RADIUS, ORBITAL_RADIUS),
        glm::clamp(newPos.y, -ORBITAL_RADIUS, ORBITAL_RADIUS),
        glm::clamp(newPos.z, -ORBITAL_RADIUS, ORBITAL_RADIUS)
      );
      renderEngine->camera->setPosition(newPos);

      renderEngine->camera->generateViewMatrix();
      renderEngine->camera->generateProjectionMatrix();
  }

  if (glfwGetKey(window->getWindow(), GLFW_KEY_E) == GLFW_PRESS) {
      glm::vec3 oldPos = renderEngine->camera->getPosition();
      glm::vec3 newPos = oldPos + renderEngine->camera->getUpVector() * CAMERA_SPEED * (float)renderEngine->getDeltaTime();
      newPos = glm::vec3(
        glm::clamp(newPos.x, -ORBITAL_RADIUS, ORBITAL_RADIUS),
        glm::clamp(newPos.y, -ORBITAL_RADIUS, ORBITAL_RADIUS),
        glm::clamp(newPos.z, -ORBITAL_RADIUS, ORBITAL_RADIUS)
      ); 

      renderEngine->camera->setPosition(newPos);

      renderEngine->camera->generateViewMatrix();
      renderEngine->camera->generateProjectionMatrix();
  }

  if (glfwGetMouseButton(window->getWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
      double x, y;
      glfwGetCursorPos(window->getWindow(), &x, &y);
      glm::vec2 currentMousePos = glm::vec2(x, y);

      if (previousMousePos == glm::vec2(-1.0f, -1.0f))
        previousMousePos = currentMousePos;
      
      glm::vec3 delta = glm::vec3(currentMousePos - previousMousePos, 0.0f);
      delta *= renderEngine->getDeltaTime();

      glm::vec3 movementDelta = (renderEngine->camera->getUpVector() + renderEngine->camera->getRightVector()) * delta;
      glm::vec3 oldPos = renderEngine->camera->getPosition();
      glm::vec3 newPos = oldPos + movementDelta * CAMERA_SPEED; 
      newPos = glm::vec3(
        glm::clamp(newPos.x, -ORBITAL_RADIUS, ORBITAL_RADIUS),
        glm::clamp(newPos.y, -ORBITAL_RADIUS, ORBITAL_RADIUS),
        glm::clamp(newPos.z, -ORBITAL_RADIUS, ORBITAL_RADIUS)
      );  

      renderEngine->camera->setPosition(newPos);

      renderEngine->camera->generateViewMatrix();
      renderEngine->camera->generateProjectionMatrix();

      previousMousePos = currentMousePos;
  }

  if (glfwGetMouseButton(window->getWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
    previousMousePos = glm::vec2(-1.0f, -1.0f);
  }
}

int main() {
    renderEngine = new RenderEngine();
    renderEngine->init();

    while (!renderEngine->window->shouldClose()) {
        processInput(renderEngine->window);
        renderEngine->renderFrame();
    }

    delete renderEngine;

    return 1;
}
