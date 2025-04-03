// Refactored MainCode.cpp with Enhancements and Comments
#include <iostream>
#include <memory>
#include <cstdlib>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "SceneManager.h"
#include "ViewManager.h"
#include "ShapeMeshes.h"
#include "ShaderManager.h"

// Application class encapsulates the entire lifecycle of the OpenGL application
class Application {
public:
    Application(const char* title) : windowTitle(title) {}
    ~Application() = default;

    bool Initialize();       // Initializes GLFW, GLEW, and project components
    void Run();              // Executes the render loop
    void Cleanup();          // Cleans up memory and shuts down GLFW

private:
    bool InitializeGLFW();   // Initializes GLFW with appropriate hints
    bool InitializeGLEW();   // Initializes GLEW and checks for errors

    const char* windowTitle; // Window title text
    GLFWwindow* window = nullptr; // Pointer to GLFW window

    // Smart pointers for automatic memory management
    std::unique_ptr<SceneManager> sceneManager;
    std::unique_ptr<ViewManager> viewManager;
    std::unique_ptr<ShaderManager> shaderManager;
};

// Sets up GLFW window hints and initializes the GLFW library
bool Application::InitializeGLFW() {
    if (!glfwInit()) return false;

#ifdef __APPLE__
    // macOS-specific OpenGL version hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    // Windows/Linux OpenGL version hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
    return true;
}

// Initializes the GLEW library to enable OpenGL extensions
bool Application::InitializeGLEW() {
    GLenum result = glewInit();
    if (result != GLEW_OK) {
        std::cerr << "GLEW Init Failed: " << glewGetErrorString(result) << std::endl;
        return false;
    }
    // Display successful OpenGL initialization information
    std::cout << "OpenGL Initialized\nVersion: " << glGetString(GL_VERSION) << "\n";
    return true;
}

// Initializes the core components of the application
bool Application::Initialize() {
    if (!InitializeGLFW()) return false; // Initialize GLFW

    // Create ShaderManager and ViewManager
    shaderManager = std::make_unique<ShaderManager>();
    viewManager = std::make_unique<ViewManager>(shaderManager.get());

    // Create the application window
    window = viewManager->CreateDisplayWindow(windowTitle);
    if (!InitializeGLEW()) return false; // Initialize GLEW

    // Load and compile shaders
    shaderManager->LoadShaders("shaders/vertexShader.glsl", "shaders/fragmentShader.glsl");
    shaderManager->use();

    // Initialize SceneManager and prepare the 3D scene
    sceneManager = std::make_unique<SceneManager>(shaderManager.get());
    sceneManager->PrepareScene();

    return true;
}

// Runs the main render loop until the window is closed
void Application::Run() {
    while (!glfwWindowShouldClose(window)) {
        // Enable depth testing for correct 3D rendering
        glEnable(GL_DEPTH_TEST);

        // Clear the screen and depth buffer
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Prepare camera view and render scene objects
        viewManager->PrepareSceneView();
        sceneManager->RenderScene();

        // Display the rendered frame
        glfwSwapBuffers(window);

        // Handle user input and window events
        glfwPollEvents();
    }
}

// Cleans up allocated resources and terminates GLFW
void Application::Cleanup() {
    sceneManager.reset();
    viewManager.reset();
    shaderManager.reset();
    glfwTerminate();
}

// Main entry point of the application
int main(int argc, char* argv[]) {
    Application app("7-1 FinalProject and Milestones");

    // Initialize application components and run if successful
    if (!app.Initialize()) return EXIT_FAILURE;

    app.Run();
    app.Cleanup();

    return EXIT_SUCCESS;
}