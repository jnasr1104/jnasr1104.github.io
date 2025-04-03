// ============================================================================
// ViewManager.cpp - Enhanced with Comments and Clean Code Structure
// Manages camera movement, input events, projection matrices, and viewport setup
// ============================================================================

#include "ViewManager.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// ============================== Internal Globals ==============================
namespace {
    const int WINDOW_WIDTH = 1000;
    const int WINDOW_HEIGHT = 800;
    const char* g_ViewName = "view";
    const char* g_ProjectionName = "projection";

    Camera* g_pCamera = nullptr;  // Global camera pointer
    float gLastX = WINDOW_WIDTH / 2.0f;
    float gLastY = WINDOW_HEIGHT / 2.0f;
    bool gFirstMouse = true;

    float gDeltaTime = 0.0f;     // Time between current frame and last frame
    float gLastFrame = 0.0f;

    bool bOrthographicProjection = false; // Projection mode toggle
}

// ============================== Constructor ==============================
// Initializes shader pointer and sets default camera parameters
ViewManager::ViewManager(ShaderManager* pShaderManager) {
    m_pShaderManager = pShaderManager;
    m_pWindow = nullptr;

    g_pCamera = new Camera();
    g_pCamera->Position = glm::vec3(0.0f, 5.0f, 12.0f);
    g_pCamera->Front = glm::vec3(0.0f, -0.5f, -2.0f);
    g_pCamera->Up = glm::vec3(0.0f, 1.0f, 0.0f);
    g_pCamera->Zoom = 80;
    g_pCamera->MovementSpeed = 20;
}

// ============================== Destructor ==============================
// Releases allocated memory
ViewManager::~ViewManager() {
    m_pShaderManager = nullptr;
    m_pWindow = nullptr;
    if (g_pCamera) {
        delete g_pCamera;
        g_pCamera = nullptr;
    }
}

// ============================== CreateDisplayWindow ==============================
// Creates the GLFW display window, sets mouse and scroll callbacks
GLFWwindow* ViewManager::CreateDisplayWindow(const char* windowTitle) {
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, windowTitle, nullptr, nullptr);
    if (!window) {
        std::cout << "[ERROR] Failed to create GLFW window\n";
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, &ViewManager::Mouse_Position_Callback);
    glfwSetScrollCallback(window, &ViewManager::Mouse_Scroll_Wheel_Callback);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_pWindow = window;
    return window;
}

// ============================== Mouse Position Callback ==============================
// Handles mouse movement and camera rotation
void ViewManager::Mouse_Position_Callback(GLFWwindow* window, double xMousePos, double yMousePos) {
    if (gFirstMouse) {
        gLastX = xMousePos;
        gLastY = yMousePos;
        gFirstMouse = false;
    }
    float xOffset = xMousePos - gLastX;
    float yOffset = gLastY - yMousePos;
    gLastX = xMousePos;
    gLastY = yMousePos;
    g_pCamera->ProcessMouseMovement(xOffset, yOffset);
}

// ============================== Mouse Scroll Callback ==============================
// Handles camera zoom with scroll wheel
void ViewManager::Mouse_Scroll_Wheel_Callback(GLFWwindow* window, double x, double yScrollDistance) {
    g_pCamera->ProcessMouseScroll(yScrollDistance);
}

// ============================== ProcessKeyboardEvents ==============================
// Detects keyboard events and updates camera movement or projection type
void ViewManager::ProcessKeyboardEvents() {
    if (glfwGetKey(m_pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_pWindow, true);

    if (!g_pCamera) return;

    // Movement controls
    if (glfwGetKey(m_pWindow, GLFW_KEY_W) == GLFW_PRESS) g_pCamera->ProcessKeyboard(FORWARD, gDeltaTime);
    if (glfwGetKey(m_pWindow, GLFW_KEY_S) == GLFW_PRESS) g_pCamera->ProcessKeyboard(BACKWARD, gDeltaTime);
    if (glfwGetKey(m_pWindow, GLFW_KEY_A) == GLFW_PRESS) g_pCamera->ProcessKeyboard(LEFT, gDeltaTime);
    if (glfwGetKey(m_pWindow, GLFW_KEY_D) == GLFW_PRESS) g_pCamera->ProcessKeyboard(RIGHT, gDeltaTime);
    if (glfwGetKey(m_pWindow, GLFW_KEY_Q) == GLFW_PRESS) g_pCamera->ProcessKeyboard(UP, gDeltaTime);
    if (glfwGetKey(m_pWindow, GLFW_KEY_E) == GLFW_PRESS) g_pCamera->ProcessKeyboard(DOWN, gDeltaTime);

    // Projection mode switch
    if (glfwGetKey(m_pWindow, GLFW_KEY_O) == GLFW_PRESS) {
        bOrthographicProjection = true;
        g_pCamera->Position = glm::vec3(-5.0f, 5.0f, 10.0f);
        g_pCamera->Front = glm::vec3(0.0f, 0.0f, -1.0f);
        g_pCamera->Up = glm::vec3(0.0f, 1.0f, 0.0f);
    }
    if (glfwGetKey(m_pWindow, GLFW_KEY_P) == GLFW_PRESS) {
        bOrthographicProjection = false;
        g_pCamera->Position = glm::vec3(0.0f, 5.5f, 8.0f);
        g_pCamera->Front = glm::vec3(0.0f, -0.5f, -2.0f);
        g_pCamera->Up = glm::vec3(0.0f, 1.0f, 0.0f);
        g_pCamera->Zoom = 125;
    }
}

// ============================== PrepareSceneView ==============================
// Calculates view and projection matrices and updates shaders
void ViewManager::PrepareSceneView() {
    float currentFrame = glfwGetTime();
    gDeltaTime = currentFrame - gLastFrame;
    gLastFrame = currentFrame;

    ProcessKeyboardEvents();

    glm::mat4 view = g_pCamera->GetViewMatrix();
    glm::mat4 projection;

    if (!bOrthographicProjection) {
        projection = glm::perspective(glm::radians(g_pCamera->Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
    }
    else {
        double scale = 0.0;
        if (WINDOW_WIDTH > WINDOW_HEIGHT) {
            scale = (double)WINDOW_HEIGHT / (double)WINDOW_WIDTH;
            projection = glm::ortho(-5.0f, 5.0f, -5.0f * (float)scale, 5.0f * (float)scale, 0.1f, 100.0f);
        }
        else if (WINDOW_WIDTH < WINDOW_HEIGHT) {
            scale = (double)WINDOW_WIDTH / (double)WINDOW_HEIGHT;
            projection = glm::ortho(-5.0f * (float)scale, 5.0f * (float)scale, -5.0f, 5.0f, 0.1f, 100.0f);
        }
        else {
            projection = glm::ortho(0.0f, 0.0f, 0.0f, 0.0f, 0.1f, 100.0f);
        }
    }

    if (m_pShaderManager) {
        m_pShaderManager->setMat4Value(g_ViewName, view);
        m_pShaderManager->setMat4Value(g_ProjectionName, projection);
        m_pShaderManager->setVec3Value("viewPosition", g_pCamera->Position);
        m_pShaderManager->setVec3Value("spotLight.position", g_pCamera->Position);
        m_pShaderManager->setVec3Value("spotLight.direction", g_pCamera->Front);
    }
}