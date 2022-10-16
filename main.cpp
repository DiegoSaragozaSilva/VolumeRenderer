#include "src/RenderEngine/RenderEngine.hpp"

int main() {
    RenderEngine* renderEngine = new RenderEngine();

    while (!renderEngine->windowShouldClose()) {
        renderEngine->renderFrame();
    }

    delete renderEngine;

    return 1;
}
