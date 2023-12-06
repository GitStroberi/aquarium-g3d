#include "aquarium.h"
#include "shader.h"
#include "model.h"

Aquarium::Aquarium(int width, int height, const char* title) {
    // Initialize GLFW, GLEW, and create a window
    if (!glfwInit())
        exit(EXIT_FAILURE);

    window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Error initializing GLEW!" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Set up other initializations
    initialize();
}

Aquarium::~Aquarium() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Aquarium::run() {
    while (!glfwWindowShouldClose(window)) {
        render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void Aquarium::initialize() {
    // Set up OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set up shaders and models
    setupShaders();
    setupModels();
}

void Aquarium::setupShaders() {
    // Implement shader loading and compilation
    // Use Shader class to manage shaders
}

void Aquarium::setupModels() {
    // Implement loading and initializing 3D models
    // Use Model class to manage models
}

void Aquarium::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Implement rendering logic here

    // Example: render a model
    // model.render(camera.getViewMatrix(), camera.getProjectionMatrix());

    glfwPollEvents();
}
