#pragma once
#include "core/Common.h"

struct GLFWwindow;

namespace ap3d {

class Camera;
class InputManager;
class Renderer;

/// Dear ImGui based UI system for the park.
class UISystem {
public:
    UISystem() = default;
    ~UISystem();

    /// Initialize Dear ImGui with the GLFW window.
    bool init(GLFWwindow* window);

    /// Begin a new UI frame (call before any ImGui commands).
    void beginFrame();

    /// Render UI elements.
    void render(float fps, int drawCalls, const Camera& camera, float deltaTime);

    /// End the UI frame and issue draw commands.
    void endFrame();

    /// Shutdown ImGui.
    void shutdown();

    /// Check if UI wants mouse/keyboard input.
    [[nodiscard]] bool wantsCaptureInput() const { return m_wantsCaptureInput; }

    /// Show/hide the main HUD.
    void setHUDVisible(bool visible) { m_showHUD = visible; }

    /// Show a notification message.
    void showNotification(const std::string& message, float duration = 3.0f);

    /// Set the current minigame info to display.
    void setMinigameInfo(const std::string& name, int score, int targetScore, float timeLeft);
    void setMinigameCharge(float charge, bool isCharging) {
        m_minigameCharge = charge;
        m_minigameIsCharging = isCharging;
    }
    void clearMinigameInfo() { m_showMinigameHUD = false; m_minigameIsCharging = false; }

    /// Show ride interaction prompt.
    void showRidePrompt(const std::string& rideName);
    void hideRidePrompt() { m_showRidePrompt = false; }

    /// Toggle settings menu.
    void toggleSettings() { m_showSettings = !m_showSettings; }
    [[nodiscard]] bool isSettingsOpen() const { return m_showSettings; }

    // Settings values
    float mouseSensitivity = 0.1f;
    float masterVolume = 0.7f;
    float moveSpeed = 5.0f;
    bool showWireframe = false;
    bool showColliders = false;

private:
    void renderHUD(float fps, int drawCalls, const Camera& camera);
    void renderNotifications(float deltaTime);
    void renderMinigameHUD();
    void renderRidePrompt();
    void renderSettings();
    void renderCrosshair();

    bool m_initialized = false;
    bool m_wantsCaptureInput = false;
    bool m_showHUD = true;
    bool m_showSettings = false;
    bool m_showMinigameHUD = false;
    bool m_showRidePrompt = false;

    // Notifications
    struct Notification {
        std::string message;
        float timeLeft = 0.0f;
    };
    std::vector<Notification> m_notifications;

    // Minigame HUD data
    std::string m_minigameName;
    int m_minigameScore = 0;
    int m_minigameTargetScore = 0;
    float m_minigameTimeLeft = 0.0f;
    float m_minigameCharge = 0.0f;
    bool m_minigameIsCharging = false;

    // Ride prompt
    std::string m_ridePromptName;
};

} // namespace ap3d
