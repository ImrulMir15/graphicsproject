#include "ui/UISystem.h"
#include "camera/Camera.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace ap3d {

UISystem::~UISystem() {
    shutdown();
}

bool UISystem::init(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Set up dark style with custom colors
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 8.0f;
    style.FrameRounding = 4.0f;
    style.PopupRounding = 4.0f;
    style.ScrollbarRounding = 4.0f;
    style.GrabRounding = 4.0f;
    style.WindowBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;
    style.WindowPadding = ImVec2(12, 12);
    style.FramePadding = ImVec2(8, 4);
    style.ItemSpacing = ImVec2(8, 6);

    // Custom color scheme
    auto& colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.10f, 0.85f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.14f, 1.0f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.12f, 0.10f, 0.22f, 1.0f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.12f, 0.12f, 0.18f, 1.0f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.18f, 0.16f, 0.28f, 1.0f);
    colors[ImGuiCol_Button] = ImVec4(0.25f, 0.20f, 0.45f, 1.0f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.35f, 0.28f, 0.60f, 1.0f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.40f, 0.32f, 0.70f, 1.0f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.50f, 0.40f, 0.85f, 1.0f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.60f, 0.50f, 0.95f, 1.0f);
    colors[ImGuiCol_Header] = ImVec4(0.20f, 0.16f, 0.35f, 1.0f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.30f, 0.24f, 0.50f, 1.0f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.35f, 0.28f, 0.55f, 1.0f);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    m_initialized = true;
    std::cout << "[UI] Initialized successfully\n";
    return true;
}

void UISystem::beginFrame() {
    if (!m_initialized) return;
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UISystem::render(float fps, int drawCalls, const Camera& camera, float deltaTime) {
    if (!m_initialized) return;

    ImGuiIO& io = ImGui::GetIO();
    m_wantsCaptureInput = io.WantCaptureMouse || io.WantCaptureKeyboard;

    if (m_showHUD) {
        renderHUD(fps, drawCalls, camera);
        renderCrosshair();
    }

    renderNotifications(deltaTime);

    if (m_showMinigameHUD) {
        renderMinigameHUD();
    }

    if (m_showRidePrompt) {
        renderRidePrompt();
    }

    if (m_showSettings) {
        renderSettings();
    }
}

void UISystem::endFrame() {
    if (!m_initialized) return;
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UISystem::shutdown() {
    if (!m_initialized) return;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    m_initialized = false;
}

void UISystem::showNotification(const std::string& message, float duration) {
    m_notifications.push_back({message, duration});
}

void UISystem::setMinigameInfo(const std::string& name, int score, int targetScore, float timeLeft) {
    m_showMinigameHUD = true;
    m_minigameName = name;
    m_minigameScore = score;
    m_minigameTargetScore = targetScore;
    m_minigameTimeLeft = timeLeft;
}

void UISystem::showRidePrompt(const std::string& rideName) {
    m_showRidePrompt = true;
    m_ridePromptName = rideName;
}

// ---- Private rendering methods ----

void UISystem::renderHUD(float fps, int drawCalls, const Camera& camera) {
    ImGuiIO& io = ImGui::GetIO();

    // Top-left stats
    ImGui::SetNextWindowPos(ImVec2(10, 10));
    ImGui::SetNextWindowSize(ImVec2(260, 0));
    ImGui::Begin("##HUD", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoInputs);

    ImGui::TextColored(ImVec4(0.9f, 0.7f, 0.3f, 1.0f), "Amusement Park 3D");
    ImGui::Separator();
    ImGui::Text("FPS: %.0f", fps);
    ImGui::Text("Draw Calls: %d", drawCalls);
    ImGui::Text("Pos: %.1f, %.1f, %.1f",
        camera.position().x, camera.position().y, camera.position().z);

    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.6f, 0.8f, 1.0f, 1.0f), "Controls");
    ImGui::Text("WASD - Move");
    ImGui::Text("Mouse - Look");
    ImGui::Text("E - Interact");
    ImGui::Text("ESC - Settings");
    ImGui::Text("TAB - Toggle HUD");

    ImGui::End();
}

void UISystem::renderNotifications(float deltaTime) {
    ImGuiIO& io = ImGui::GetIO();
    float screenW = io.DisplaySize.x;

    float yOffset = 60.0f;
    for (auto it = m_notifications.begin(); it != m_notifications.end(); ) {
        it->timeLeft -= deltaTime;
        if (it->timeLeft <= 0) {
            it = m_notifications.erase(it);
            continue;
        }

        float alpha = std::min(it->timeLeft, 1.0f);

        ImGui::SetNextWindowPos(ImVec2(screenW / 2.0f, yOffset), ImGuiCond_Always, ImVec2(0.5f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);

        std::string id = "##notif_" + std::to_string(yOffset);
        ImGui::Begin(id.c_str(), nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);

        ImGui::TextColored(ImVec4(1.0f, 0.9f, 0.3f, 1.0f), "%s", it->message.c_str());
        ImGui::End();

        ImGui::PopStyleVar();

        yOffset += 40.0f;
        ++it;
    }
}

void UISystem::renderMinigameHUD() {
    ImGuiIO& io = ImGui::GetIO();
    float screenW = io.DisplaySize.x;

    ImGui::SetNextWindowPos(ImVec2(screenW / 2.0f, 10.0f), ImGuiCond_Always, ImVec2(0.5f, 0.0f));
    ImGui::SetNextWindowSize(ImVec2(350, 0));
    ImGui::Begin("##MinigameHUD", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoInputs);

    ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.2f, 1.0f), "%s", m_minigameName.c_str());
    ImGui::Separator();

    // Score bar
    float progress = m_minigameTargetScore > 0 ?
        static_cast<float>(m_minigameScore) / m_minigameTargetScore : 0.0f;
    ImGui::Text("Score: %d / %d", m_minigameScore, m_minigameTargetScore);
    ImGui::ProgressBar(progress, ImVec2(-1, 0), "");

    // Throw power bar
    if (m_minigameIsCharging) {
        ImGui::TextColored(ImVec4(0.3f, 0.9f, 1.0f, 1.0f), "Throw Power:");
        ImVec4 barColor(m_minigameCharge, 1.0f - m_minigameCharge * 0.5f, 0.2f, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, barColor);
        ImGui::ProgressBar(m_minigameCharge, ImVec2(-1, 0), "HOLD LEFT MOUSE TO CHARGE");
        ImGui::PopStyleColor();
    }

    // Timer
    ImGui::Text("Time: %.1f s", m_minigameTimeLeft);

    // Color-coded timer
    if (m_minigameTimeLeft < 10.0f) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), " HURRY!");
    }

    ImGui::Separator();
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Press Q to quit minigame");
    ImGui::End();
}

void UISystem::renderRidePrompt() {
    ImGuiIO& io = ImGui::GetIO();
    float screenW = io.DisplaySize.x;
    float screenH = io.DisplaySize.y;

    ImGui::SetNextWindowPos(ImVec2(screenW / 2.0f, screenH * 0.7f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::Begin("##RidePrompt", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);

    ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.5f, 1.0f),
        "Press E to interact with %s", m_ridePromptName.c_str());
    ImGui::End();
}

void UISystem::renderSettings() {
    ImGuiIO& io = ImGui::GetIO();
    float screenW = io.DisplaySize.x;
    float screenH = io.DisplaySize.y;

    ImGui::SetNextWindowPos(ImVec2(screenW / 2.0f, screenH / 2.0f), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(400, 0), ImGuiCond_Appearing);

    ImGui::Begin("Settings", &m_showSettings);

    ImGui::TextColored(ImVec4(0.9f, 0.7f, 0.3f, 1.0f), "Game Settings");
    ImGui::Separator();

    ImGui::SliderFloat("Mouse Sensitivity", &mouseSensitivity, 0.01f, 0.5f, "%.3f");
    ImGui::SliderFloat("Move Speed", &moveSpeed, 1.0f, 20.0f, "%.1f");
    ImGui::SliderFloat("Master Volume", &masterVolume, 0.0f, 1.0f, "%.2f");

    ImGui::Separator();
    ImGui::Text("Debug");
    ImGui::Checkbox("Show Wireframe", &showWireframe);
    ImGui::Checkbox("Show Colliders", &showColliders);

    ImGui::Separator();
    if (ImGui::Button("Close", ImVec2(-1, 30))) {
        m_showSettings = false;
    }

    ImGui::End();
}

void UISystem::renderCrosshair() {
    ImGuiIO& io = ImGui::GetIO();
    float cx = io.DisplaySize.x / 2.0f;
    float cy = io.DisplaySize.y / 2.0f;
    float size = 10.0f;
    float thickness = 2.0f;

    ImDrawList* drawList = ImGui::GetForegroundDrawList();
    ImU32 color = IM_COL32(255, 255, 255, 180);

    drawList->AddLine(ImVec2(cx - size, cy), ImVec2(cx + size, cy), color, thickness);
    drawList->AddLine(ImVec2(cx, cy - size), ImVec2(cx, cy + size), color, thickness);
    drawList->AddCircle(ImVec2(cx, cy), 3.0f, color, 12, thickness);
}

} // namespace ap3d
