#pragma once
#include "core/Common.h"

namespace ap3d {

class InputManager;

/// First-person camera with mouse look, pitch clamping, smooth movement,
/// and collision-aware positioning.
class Camera {
public:
    Camera();

    /// Process input to update camera position and orientation.
    void update(const InputManager& input, float deltaTime);

    /// Set camera position directly (used for collision response).
    void setPosition(const glm::vec3& pos) { m_position = pos; }
    void setYaw(float yaw) { m_yaw = yaw; updateVectors(); }
    void setPitch(float pitch) { m_pitch = glm::clamp(pitch, -89.0f, 89.0f); updateVectors(); }

    // ---- Getters ----
    [[nodiscard]] const glm::vec3& position() const { return m_position; }
    [[nodiscard]] const glm::vec3& front() const { return m_front; }
    [[nodiscard]] const glm::vec3& right() const { return m_right; }
    [[nodiscard]] const glm::vec3& up() const { return m_up; }
    [[nodiscard]] float yaw() const { return m_yaw; }
    [[nodiscard]] float pitch() const { return m_pitch; }
    [[nodiscard]] float fov() const { return m_fov; }
    [[nodiscard]] float nearPlane() const { return m_nearPlane; }
    [[nodiscard]] float farPlane() const { return m_farPlane; }
    [[nodiscard]] float moveSpeed() const { return m_moveSpeed; }

    /// Get the view matrix.
    [[nodiscard]] glm::mat4 viewMatrix() const;

    /// Get the projection matrix for the given aspect ratio.
    [[nodiscard]] glm::mat4 projectionMatrix(float aspectRatio) const;

    // ---- Settings ----
    void setMoveSpeed(float speed) { m_moveSpeed = speed; }
    void setFov(float fov) { m_fov = fov; }
    void setNearFar(float nearP, float farP) { m_nearPlane = nearP; m_farPlane = farP; }

    /// Get the desired (pre-collision) position for the current frame.
    [[nodiscard]] const glm::vec3& desiredPosition() const { return m_desiredPosition; }

    /// Enable/disable mouse look processing.
    void setMouseLookEnabled(bool enabled) { m_mouseLookEnabled = enabled; }
    [[nodiscard]] bool isMouseLookEnabled() const { return m_mouseLookEnabled; }

private:
    void updateVectors();

    glm::vec3 m_position{0.0f, 1.7f, 0.0f}; // Eye height ~1.7m
    glm::vec3 m_front{0.0f, 0.0f, -1.0f};
    glm::vec3 m_right{1.0f, 0.0f, 0.0f};
    glm::vec3 m_up{0.0f, 1.0f, 0.0f};
    glm::vec3 m_worldUp{0.0f, 1.0f, 0.0f};
    glm::vec3 m_desiredPosition{0.0f, 1.7f, 0.0f};

    float m_yaw = -90.0f;    // Look along -Z by default
    float m_pitch = 0.0f;
    float m_fov = 60.0f;
    float m_nearPlane = 0.1f;
    float m_farPlane = 500.0f;
    float m_moveSpeed = 5.0f;

    bool m_mouseLookEnabled = true;

    // Head bobbing
    float m_bobTime = 0.0f;
    float m_bobOffset = 0.0f;
    float m_bobSpeed = 12.0f;
    float m_bobAmount = 0.06f; // Subtle bobbing
};

} // namespace ap3d
