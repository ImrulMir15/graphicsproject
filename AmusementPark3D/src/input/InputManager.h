#pragma once
#include "core/Common.h"

namespace ap3d {

/// Centralized input manager handling keyboard, mouse position, mouse buttons,
/// and mouse delta with raw input support. Frame-independent.
class InputManager {
public:
    InputManager() = default;

    /// Initialize callbacks on the given GLFW window.
    void init(GLFWwindow* window);

    /// Call once per frame before processing input.
    void update();

    // ---- Keyboard ----
    [[nodiscard]] bool isKeyDown(int key) const;
    [[nodiscard]] bool isKeyPressed(int key) const;  // Just pressed this frame
    [[nodiscard]] bool isKeyReleased(int key) const; // Just released this frame

    // ---- Mouse position ----
    [[nodiscard]] double mouseX() const { return m_mouseX; }
    [[nodiscard]] double mouseY() const { return m_mouseY; }
    [[nodiscard]] float mouseDeltaX() const { return m_mouseDeltaX; }
    [[nodiscard]] float mouseDeltaY() const { return m_mouseDeltaY; }

    // ---- Mouse buttons ----
    [[nodiscard]] bool isMouseButtonDown(int button) const;
    [[nodiscard]] bool isMouseButtonPressed(int button) const;
    [[nodiscard]] bool isMouseButtonReleased(int button) const;

    // ---- Mouse scroll ----
    [[nodiscard]] float scrollDelta() const { return m_scrollDelta; }

    // ---- Cursor control ----
    void setCursorLocked(bool locked);
    [[nodiscard]] bool isCursorLocked() const { return m_cursorLocked; }

    /// Sensitivity multiplier for mouse look.
    void setMouseSensitivity(float sens) { m_sensitivity = sens; }
    [[nodiscard]] float mouseSensitivity() const { return m_sensitivity; }

    /// Check if the UI wants to capture input (ImGui).
    void setUIWantsInput(bool wants) { m_uiWantsInput = wants; }
    [[nodiscard]] bool uiWantsInput() const { return m_uiWantsInput; }

private:
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    GLFWwindow* m_window = nullptr;

    // Keyboard state
    std::array<bool, GLFW_KEY_LAST + 1> m_keys{};
    std::array<bool, GLFW_KEY_LAST + 1> m_prevKeys{};

    // Mouse button state
    std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> m_mouseButtons{};
    std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> m_prevMouseButtons{};

    // Mouse position and delta
    double m_mouseX = 0.0;
    double m_mouseY = 0.0;
    double m_lastMouseX = 0.0;
    double m_lastMouseY = 0.0;
    float m_mouseDeltaX = 0.0f;
    float m_mouseDeltaY = 0.0f;
    bool m_firstMouse = true;

    // Scroll
    float m_scrollDelta = 0.0f;
    float m_scrollAccum = 0.0f;

    // Settings
    float m_sensitivity = 0.1f;
    bool m_cursorLocked = false;
    bool m_uiWantsInput = false;
};

} // namespace ap3d
