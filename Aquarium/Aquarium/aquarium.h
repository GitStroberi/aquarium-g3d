#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <GL/glew.h>
#include <glfw3.h>
#include "camera.h"

class Aquarium {
public:
    Aquarium(int width, int height, const char* title);
    ~Aquarium();

    void run();

private:
    GLFWwindow* window;
    Camera camera;

    void initialize();
    void setupShaders();
    void setupModels();
    void render();
};
