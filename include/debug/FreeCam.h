#pragma once
#include <glm/glm.hpp>

namespace debug
{

}

struct GLFWwindow; // forward declare to avoid forcing the inclusion of glfw on all sites

class FreeCam {
public:
    // Constructor: base speed (units/s) and mouse sensitivity (degrees/pixel)
    FreeCam(float speed = 5.0f, float sensitivity = 0.12f);

    // Call this from your update loop every frame.
    // - window: GLFWwindow pointer (use your input wrapper if you don't have GLFW)
    // - dt: delta time in seconds
    // - applyCamera: a lambda function or callback that applies the position/orientation to the actual camera.
    //
    // Simple example of applyCamera:
    //   [](const glm::vec3& pos, const glm::vec3& forward, const glm::vec3& up){
    //       camera.setPosition(pos);
    //       camera.lookAt(pos + forward, up);
    //   }
    void UpdateGLFW(GLFWwindow* window, float dt, const std::function<void(const glm::vec3&, const glm::vec3&, const glm::vec3&)>& applyCamera);

    bool IsActive() const { return active; }

private:
    void activate(GLFWwindow* window);
    void deactivate(GLFWwindow* window);

    glm::vec3 pos;
    float yaw;   // degrees, around Y
    float pitch; // degrees, around X

    float speed;
    float sensitivity;

    bool active;

    // para captura de cursor
    double lastMouseX, lastMouseY;
    bool firstMouse;

    // Saving/restoring the camera before activation
    glm::vec3 savedPos;
    glm::vec3 savedForward;
    glm::vec3 savedUp;
    bool hasSaved;
};