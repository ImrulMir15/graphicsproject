#include "core/Window.h"

namespace ap3d {

// Static instance for callbacks (avoids user pointer conflict with InputManager)
static Window* s_windowInstance = nullptr;

Window::~Window() {
    s_windowInstance = nullptr;
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();
}

bool Window::init(const Config& config) {
    if (!glfwInit()) {
        std::cerr << "[Window] Failed to initialize GLFW\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_SAMPLES, config.samples);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    GLFWmonitor* monitor = nullptr;
    int winWidth = config.width;
    int winHeight = config.height;

    if (config.fullscreen) {
        monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        winWidth = mode->width;
        winHeight = mode->height;
    }

    m_window = glfwCreateWindow(winWidth, winHeight, config.title.c_str(), monitor, nullptr);
    if (!m_window) {
        std::cerr << "[Window] Failed to create GLFW window\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);

    // Load OpenGL functions via GLAD2
    int version = gladLoadGL(glfwGetProcAddress);
    if (!version) {
        std::cerr << "[Window] Failed to initialize GLAD\n";
        return false;
    }
    std::cout << "[Window] OpenGL " << GLAD_VERSION_MAJOR(version) << "."
              << GLAD_VERSION_MINOR(version) << " loaded\n";

    // Get actual framebuffer size
    glfwGetFramebufferSize(m_window, &m_width, &m_height);
    glViewport(0, 0, m_width, m_height);

    // Store static instance and set framebuffer callback
    // NOTE: glfwSetWindowUserPointer is reserved for InputManager
    s_windowInstance = this;
    glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);

    // VSync
    glfwSwapInterval(config.vsync ? 1 : 0);

    // Enable multisampling
    if (config.samples > 1) {
        glEnable(GL_MULTISAMPLE);
    }

    // Raw mouse motion if supported
    if (glfwRawMouseMotionSupported()) {
        glfwSetInputMode(m_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }

    return true;
}

void Window::swapBuffers() {
    glfwSwapBuffers(m_window);
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(m_window);
}

void Window::close() {
    glfwSetWindowShouldClose(m_window, GLFW_TRUE);
}

void Window::framebufferSizeCallback(GLFWwindow* /*window*/, int width, int height) {
    if (s_windowInstance) {
        s_windowInstance->m_width = width;
        s_windowInstance->m_height = height;
        s_windowInstance->m_resized = true;
        glViewport(0, 0, width, height);
    }
}

} // namespace ap3d

