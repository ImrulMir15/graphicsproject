#pragma once
#include "core/Common.h"

namespace ap3d {

/// GLFW window wrapper handling creation, resizing, and frame buffer management.
class Window {
public:
    struct Config {
        int width = 1920;
        int height = 1080;
        std::string title = "AmusementPark3D";
        bool vsync = true;
        bool fullscreen = false;
        int samples = 4; // MSAA samples
    };

    Window() = default;
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    /// Initialize the window with the given configuration.
    bool init(const Config& config);

    /// Swap buffers and poll events.
    void swapBuffers();

    /// Check if the window should close.
    [[nodiscard]] bool shouldClose() const;

    /// Request window close.
    void close();

    /// Get the GLFW window handle.
    [[nodiscard]] GLFWwindow* handle() const { return m_window; }

    /// Get current framebuffer dimensions.
    [[nodiscard]] int width() const { return m_width; }
    [[nodiscard]] int height() const { return m_height; }
    [[nodiscard]] float aspectRatio() const {
        return m_height > 0 ? static_cast<float>(m_width) / static_cast<float>(m_height) : 1.0f;
    }

    /// Check if window was resized this frame.
    [[nodiscard]] bool wasResized() const { return m_resized; }
    void resetResizedFlag() { m_resized = false; }

private:
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

    GLFWwindow* m_window = nullptr;
    int m_width = 0;
    int m_height = 0;
    bool m_resized = false;
};

} // namespace ap3d
