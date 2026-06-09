#include "input/InputManager.h"

namespace ap3d {

void InputManager::init(GLFWwindow* window) {
    m_window = window;
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetScrollCallback(window, scrollCallback);

    // Initialize mouse position
    glfwGetCursorPos(window, &m_mouseX, &m_mouseY);
    m_lastMouseX = m_mouseX;
    m_lastMouseY = m_mouseY;
    m_firstMouse = true;

    m_keys.fill(false);
    m_prevKeys.fill(false);
    m_mouseButtons.fill(false);
    m_prevMouseButtons.fill(false);
}

void InputManager::update() {
    // Store previous state for edge detection
    m_prevKeys = m_keys;
    m_prevMouseButtons = m_mouseButtons;

    // Poll new events
    glfwPollEvents();

    // Compute mouse delta from accumulated cursor position
    if (m_firstMouse) {
        m_mouseDeltaX = 0.0f;
        m_mouseDeltaY = 0.0f;
        m_lastMouseX = m_mouseX;
        m_lastMouseY = m_mouseY;
        m_firstMouse = false;
    } else {
        m_mouseDeltaX = static_cast<float>(m_mouseX - m_lastMouseX);
        m_mouseDeltaY = static_cast<float>(m_lastMouseY - m_mouseY); // Inverted Y
        m_lastMouseX = m_mouseX;
        m_lastMouseY = m_mouseY;
    }

    // Consume scroll delta
    m_scrollDelta = m_scrollAccum;
    m_scrollAccum = 0.0f;
}

bool InputManager::isKeyDown(int key) const {
    if (key < 0 || key > GLFW_KEY_LAST) return false;
    return m_keys[key];
}

bool InputManager::isKeyPressed(int key) const {
    if (key < 0 || key > GLFW_KEY_LAST) return false;
    return m_keys[key] && !m_prevKeys[key];
}

bool InputManager::isKeyReleased(int key) const {
    if (key < 0 || key > GLFW_KEY_LAST) return false;
    return !m_keys[key] && m_prevKeys[key];
}

bool InputManager::isMouseButtonDown(int button) const {
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) return false;
    return m_mouseButtons[button];
}

bool InputManager::isMouseButtonPressed(int button) const {
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) return false;
    return m_mouseButtons[button] && !m_prevMouseButtons[button];
}

bool InputManager::isMouseButtonReleased(int button) const {
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) return false;
    return !m_mouseButtons[button] && m_prevMouseButtons[button];
}

void InputManager::setCursorLocked(bool locked) {
    m_cursorLocked = locked;
    if (m_window) {
        glfwSetInputMode(m_window, GLFW_CURSOR,
            locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

        if (locked) {
            // Reset mouse delta to prevent jump
            glfwGetCursorPos(m_window, &m_mouseX, &m_mouseY);
            m_lastMouseX = m_mouseX;
            m_lastMouseY = m_mouseY;
            m_mouseDeltaX = 0.0f;
            m_mouseDeltaY = 0.0f;
        }
    }
}

// ---- Static GLFW Callbacks ----

void InputManager::keyCallback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/) {
    auto* self = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (!self || key < 0 || key > GLFW_KEY_LAST) return;

    if (action == GLFW_PRESS) {
        self->m_keys[key] = true;
    } else if (action == GLFW_RELEASE) {
        self->m_keys[key] = false;
    }
}

void InputManager::mouseButtonCallback(GLFWwindow* window, int button, int action, int /*mods*/) {
    auto* self = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (!self || button < 0 || button > GLFW_MOUSE_BUTTON_LAST) return;

    if (action == GLFW_PRESS) {
        self->m_mouseButtons[button] = true;
    } else if (action == GLFW_RELEASE) {
        self->m_mouseButtons[button] = false;
    }
}

void InputManager::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    auto* self = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (!self) return;

    self->m_mouseX = xpos;
    self->m_mouseY = ypos;
}

void InputManager::scrollCallback(GLFWwindow* window, double /*xoffset*/, double yoffset) {
    auto* self = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (!self) return;

    self->m_scrollAccum += static_cast<float>(yoffset);
}

} // namespace ap3d
