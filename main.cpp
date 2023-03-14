#include "src/RenderEngine/RenderEngine.hpp"
#include "src/RenderEngine/Octree.hpp"

#define FREE_CAMERA

#define CAMERA_SPEED 10.0f
#define CAMERA_ROTATE_SPEED 0.01f
#define CAMERA_ZOOM_SPEED 15.0f

bool arcEnabled = false;
glm::vec2 lastMousePos = glm::vec2(0.0f);
glm::vec2 cameraRotation = glm::vec2(0.0f);
RenderEngine* renderEngine;

void mouseCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        lastMousePos = glm::vec2(x, y);
        arcEnabled = true;
    }
 
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        arcEnabled = false;
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

}

void processInput(GLFWwindow* window) {
    // Arcball camera
    if (arcEnabled) {
        double x, y;
        glfwGetCursorPos(window, &x, &y);

        double dx = x - lastMousePos.x;
        double dy = y - lastMousePos.y;

        int width, height;
        glfwGetWindowSize(window, &width, &height);

        double scaleX = std::abs(dx) / width;
        double scaleY = std::abs(dy) / height;

        if (dx < 0) {
            renderEngine->camera->rotateWorld((float)glm::radians(-CAMERA_ROTATE_SPEED * scaleX), glm::vec3(0, 1, 0));
            cameraRotation.x -= CAMERA_ROTATE_SPEED * scaleX;
        }
        else if (dx > 0) {
            renderEngine->camera->rotateWorld((float)glm::radians(CAMERA_ROTATE_SPEED * scaleX), glm::vec3(0, 1, 0));
            cameraRotation.x += CAMERA_ROTATE_SPEED * scaleX; 
        }

        float rotation = CAMERA_ROTATE_SPEED * scaleY;
        if (dy < 0) {
            if (cameraRotation.y + rotation > 90.0f)
               rotation = 90.0f - cameraRotation.y;

            renderEngine->camera->rotateView((float)glm::radians(rotation), glm::vec3(1, 0, 0));
            cameraRotation.y += rotation;
        }
        else if (dy > 0) {
            if (cameraRotation.y - rotation < -90.0f)
               rotation = 90.0f + cameraRotation.y;

            renderEngine->camera->rotateView((float)glm::radians(-rotation), glm::vec3(1, 0, 0));
            cameraRotation.y -= rotation;
        }

        lastMousePos.x = x;
        lastMousePos.y = y;
    }
}

int main() {
    renderEngine = new RenderEngine();

    renderEngine->window->setMouseCallback(&mouseCallback);
    renderEngine->window->setKeyboardCallback(&keyboardCallback);

    renderEngine->init();
    while (!renderEngine->window->shouldClose()) {
        processInput(renderEngine->window->getWindow());
        renderEngine->renderFrame();
    }

    delete renderEngine;

    return 1;
}
