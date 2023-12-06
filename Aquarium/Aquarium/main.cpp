#include "aquarium.h"

#pragma comment (lib, "glfw3dll.lib")
#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "OpenGL32.lib")

int main() {
    Aquarium aquarium(1280, 720, "Aquarium Simulation");
    aquarium.run();
    return 0;
}