#include "debug/FreeCam.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <functional>
#include <cmath>

namespace debug
{

}


FreeCam::FreeCam(float speed_, float sensitivity_)
    : pos(0.0f, 0.0f, 0.0f),
    yaw(90.0f), // mirar hacia -Z por convención OpenGL (opcional)
    pitch(0.0f),
    speed(speed_),
    sensitivity(sensitivity_),
    active(false),
    lastMouseX(0.0),
    lastMouseY(0.0),
    firstMouse(true),
    hasSaved(false)
{}

void FreeCam::activate(GLFWwindow* window) {
    if (active) return;
    active = true;
    firstMouse = true;
    // disable cursor for mouse capture
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // (optional) start pos/yaw/pitch from the current camera if you have access
}

void FreeCam::deactivate(GLFWwindow* window) {
    if (!active) return;
    active = false;
    // restore cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    // Restore previous camera if saved
    if (hasSaved) {
        pos = savedPos;
        // reconstruct yaw/pitch from forward (optional)
        // savedForward / savedUp are used only if you want to restore the external camera exactly.
    }
}

// dt in seconds. applyCamera receives (pos, forward, up) and must apply these to the actual Camera.
void FreeCam::UpdateGLFW(GLFWwindow* window, float dt, const std::function<void(const glm::vec3&, const glm::vec3&, const glm::vec3&)>& applyCamera) {
    if (!window) return;

    // X key: activate while pressed
    int xState = glfwGetKey(window, GLFW_KEY_X);
    if (xState == GLFW_PRESS) {
        if (!active) {
            // Attempt to save external camera state: the callback does not return state,
            // So if you want to save the current camera, get it before calling UpdateGLFW.
            hasSaved = false; // If you want to save, configure before calling
            activate(window);
        }
    }
    else {
        if (active) {
            deactivate(window);
            // If you want the external camera to revert to the old mode when you release X,
            // You have to apply savedPos/savedForward here with applyCamera.
            if (hasSaved) {
                applyCamera(savedPos, savedForward, glm::vec3(0.0f, 1.0f, 0.0f));
            }
            return;
        }
    }

    if (!active) {
        // Nothing to do if it's not active
        return;
    }

    // Mouse reading
    double mx, my;
    glfwGetCursorPos(window, &mx, &my);
    if (firstMouse) {
        lastMouseX = mx;
        lastMouseY = my;
        firstMouse = false;
    }
    double dx = mx - lastMouseX;
    double dy = lastMouseY - my; // invest AND for typical behavior
    lastMouseX = mx;
    lastMouseY = my;

    yaw += static_cast<float>(dx) * sensitivity;
    pitch += static_cast<float>(dy) * sensitivity;
    // clamp pitch
    if (pitch > 89.9f) pitch = 89.9f;
    if (pitch < -89.9f) pitch = -89.9f;

    // calculate orientation vectors
    float yawRad = glm::radians(yaw);
    float pitchRad = glm::radians(pitch);
    glm::vec3 forward;
    forward.x = cos(pitchRad) * cos(yawRad);
    forward.y = sin(pitchRad);
    forward.z = cos(pitchRad) * sin(yawRad);
    forward = glm::normalize(forward);

    glm::vec3 up(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::normalize(glm::cross(forward, up));

    // Speed ​​modified by Shift
    float moveSpeed = speed * dt;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) moveSpeed *= 3.0f;

    // WASD + QE to go up/down
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) pos += forward * moveSpeed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) pos -= forward * moveSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) pos -= right * moveSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) pos += right * moveSpeed;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) pos -= up * moveSpeed;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) pos += up * moveSpeed;

    // Apply to the real camera via callback
    applyCamera(pos, forward, up);
}