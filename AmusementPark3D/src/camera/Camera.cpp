#include "camera/Camera.h"
#include "input/InputManager.h"

namespace ap3d {

Camera::Camera() {
    updateVectors();
}

void Camera::update(const InputManager& input, float deltaTime) {
    // ---- Mouse look ----
    if (m_mouseLookEnabled && input.isCursorLocked() && !input.uiWantsInput()) {
        float dx = input.mouseDeltaX() * input.mouseSensitivity();
        float dy = input.mouseDeltaY() * input.mouseSensitivity();

        m_yaw += dx;
        m_pitch += dy;

        // Clamp pitch to prevent flipping
        m_pitch = glm::clamp(m_pitch, -89.0f, 89.0f);

        // Wrap yaw
        if (m_yaw > 360.0f) m_yaw -= 360.0f;
        if (m_yaw < -360.0f) m_yaw += 360.0f;

        updateVectors();
    }

    // ---- Keyboard movement ----
    if (!input.uiWantsInput()) {
        float velocity = m_moveSpeed * deltaTime;

        // Horizontal movement only (project front onto XZ plane)
        glm::vec3 flatFront = glm::normalize(glm::vec3(m_front.x, 0.0f, m_front.z));
        glm::vec3 flatRight = glm::normalize(glm::vec3(m_right.x, 0.0f, m_right.z));

        glm::vec3 movement{0.0f};

        if (input.isKeyDown(GLFW_KEY_W)) movement += flatFront * velocity;
        if (input.isKeyDown(GLFW_KEY_S)) movement -= flatFront * velocity;
        if (input.isKeyDown(GLFW_KEY_A)) movement -= flatRight * velocity;
        if (input.isKeyDown(GLFW_KEY_D)) movement += flatRight * velocity;
        if (input.isKeyDown(GLFW_KEY_SPACE)) movement.y += velocity;
        if (input.isKeyDown(GLFW_KEY_LEFT_SHIFT)) movement.y -= velocity;

        m_desiredPosition = m_position + movement;
        m_position = m_desiredPosition; // Will be corrected by collision system
    }

    // ---- Scroll zoom ----
    if (!input.uiWantsInput()) {
        float scroll = input.scrollDelta();
        if (scroll != 0.0f) {
            m_fov -= scroll * 2.0f;
            m_fov = glm::clamp(m_fov, 20.0f, 120.0f);
        }
    }

    // ---- Head Bobbing ----
    bool isMoving = false;
    if (!input.uiWantsInput()) {
        if (input.isKeyDown(GLFW_KEY_W) || input.isKeyDown(GLFW_KEY_S) ||
            input.isKeyDown(GLFW_KEY_A) || input.isKeyDown(GLFW_KEY_D)) {
            isMoving = true;
        }
    }

    if (isMoving) {
        m_bobTime += deltaTime * m_bobSpeed;
        m_bobOffset = sin(m_bobTime) * m_bobAmount;
    } else {
        m_bobOffset = glm::mix(m_bobOffset, 0.0f, deltaTime * 8.0f);
        m_bobTime = 0.0f;
    }
}

glm::mat4 Camera::viewMatrix() const {
    glm::vec3 bobbedPos = m_position;
    bobbedPos.y += m_bobOffset;
    return glm::lookAt(bobbedPos, bobbedPos + m_front, m_up);
}

glm::mat4 Camera::projectionMatrix(float aspectRatio) const {
    return glm::perspective(glm::radians(m_fov), aspectRatio, m_nearPlane, m_farPlane);
}

void Camera::updateVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front = glm::normalize(front);

    m_right = glm::normalize(glm::cross(m_front, m_worldUp));
    m_up = glm::normalize(glm::cross(m_right, m_front));
}

} // namespace ap3d
