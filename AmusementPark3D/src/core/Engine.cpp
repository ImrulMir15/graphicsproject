#include "core/Engine.h"

namespace ap3d {

// Portable string prefix check (safe even if C++20 starts_with isn't available)
static bool hasPrefix(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

bool Engine::init() {
    // Window
    Window::Config winCfg;
    winCfg.width = 1600;
    winCfg.height = 900;
    winCfg.title = "AmusementPark3D - 3D Amusement Park Simulator";
    winCfg.vsync = true;
    winCfg.samples = 4;

    if (!m_window.init(winCfg)) {
        std::cerr << "[Engine] Window init failed\n";
        return false;
    }

    // Input
    m_input.init(m_window.handle());
    m_input.setCursorLocked(true);
    m_input.setMouseSensitivity(0.1f);

    // Renderer
    if (!m_renderer.init()) {
        std::cerr << "[Engine] Renderer init failed\n";
        return false;
    }

    // Set sun light
    DirectionalLight sun;
    sun.direction = glm::vec3(-0.3f, -0.8f, -0.5f);
    sun.color = glm::vec3(1.0f, 0.95f, 0.85f);
    sun.ambient = 0.2f;
    sun.diffuse = 0.9f;
    sun.specular = 0.4f;
    m_renderer.setSunLight(sun);
    m_renderer.setSkyColor(glm::vec3(0.98f, 0.55f, 0.15f));
    m_renderer.setFog(glm::vec3(0.98f, 0.55f, 0.15f), 120.0f, 400.0f);

    // UI
    m_ui.init(m_window.handle());

    // Particles
    m_particles.init();

    // Audio
    m_audio.init();
    if (m_audio.isAvailable()) {
        m_ambientBuffer = m_audio.createToneBuffer(220.0f, 4.0f);
        m_ambientSource = m_audio.createSource(true, 0.1f);
        m_audio.play(m_ambientSource, m_ambientBuffer, 0.08f);

        m_hitBuffer = m_audio.createChimeBuffer(880.0f, 0.3f);
        m_hitSource = m_audio.createSource(false, 0.5f);
    }

    // Build park world
    m_park.build(m_physics);

    // Create rides - all in the same unified coordinate system

    m_rides.push_back(std::make_unique<FerrisWheel>(glm::vec3(30.0f, 0.0f, -10.0f)));
    m_rides.push_back(std::make_unique<RollerCoaster>(glm::vec3(0.0f, 0.0f, -25.0f)));
    m_rides.push_back(std::make_unique<Carousel>(glm::vec3(-25.0f, 0.0f, 15.0f)));
    m_rides.push_back(std::make_unique<SwingRide>(glm::vec3(25.0f, 0.0f, 20.0f)));
    m_rides.push_back(std::make_unique<DropTower>(glm::vec3(-30.0f, 0.0f, -25.0f)));

    for (auto& ride : m_rides) {
        ride->build(m_physics);
    }

    // Create minigames
    m_minigames.push_back(std::make_unique<ShootingGallery>(glm::vec3(40.0f, 0.0f, 5.0f)));
    m_minigames.push_back(std::make_unique<RingToss>(glm::vec3(-40.0f, 0.0f, 25.0f)));
    m_minigames.push_back(std::make_unique<BasketballChallenge>(glm::vec3(40.0f, 0.0f, -25.0f)));

    for (auto& mg : m_minigames) {
        mg->build(m_physics);
    }

    // Camera starting position (just inside the entrance)
    m_camera.setPosition(glm::vec3(0.0f, m_playerHeight, m_park.parkDepth() / 2.0f - 5.0f));
    m_camera.setYaw(-90.0f);

    std::cout << "[Engine] Initialization complete\n";
    std::cout << "[Engine] Total colliders: " << m_physics.colliderCount() << "\n";
    std::cout << "[Engine] Rides: " << m_rides.size() << "\n";
    std::cout << "[Engine] Minigames: " << m_minigames.size() << "\n";

    m_running = true;
    return true;
}

void Engine::run() {
    while (m_running && !m_window.shouldClose()) {
        m_timer.tick();
        m_input.update();

        processInput();
        update();
        render();

        m_window.swapBuffers();
    }
}

void Engine::shutdown() {
    m_audio.shutdown();
    m_ui.shutdown();
    m_running = false;
    std::cout << "[Engine] Shutdown complete\n";
}

void Engine::processInput() {
    // ESC - toggle settings / unlock cursor
    if (m_input.isKeyPressed(GLFW_KEY_ESCAPE)) {
        if (m_activeMinigame) {
            // Quit minigame
            m_activeMinigame->stop();
            m_activeMinigame = nullptr;
            m_ui.clearMinigameInfo();
            m_ui.showNotification("Minigame ended");
        } else {
            m_ui.toggleSettings();
            bool settingsOpen = m_ui.isSettingsOpen();
            m_input.setCursorLocked(!settingsOpen);
            m_cursorLocked = !settingsOpen;
        }
    }

    // TAB - toggle HUD
    if (m_input.isKeyPressed(GLFW_KEY_TAB)) {
        static bool hudVisible = true;
        hudVisible = !hudVisible;
        m_ui.setHUDVisible(hudVisible);
    }

    // Q - quit active minigame
    if (m_input.isKeyPressed(GLFW_KEY_Q) && m_activeMinigame) {
        m_activeMinigame->stop();
        m_activeMinigame = nullptr;
        m_ui.clearMinigameInfo();
        m_ui.showNotification("Minigame ended");
    }

    // E - interact
    if (m_input.isKeyPressed(GLFW_KEY_E)) {
        checkInteractions();
    }

    // Apply UI settings
    m_input.setMouseSensitivity(m_ui.mouseSensitivity);
    m_camera.setMoveSpeed(m_ui.moveSpeed);
    if (m_audio.isAvailable()) {
        m_audio.setMasterVolume(m_ui.masterVolume);
    }

    // Pass UI capture state to input (UI only gets capture when settings menu is open/cursor is unlocked)
    bool uiNeedsInput = m_ui.isSettingsOpen() || (!m_input.isCursorLocked() && m_ui.wantsCaptureInput());
    m_input.setUIWantsInput(uiNeedsInput);
}

void Engine::update() {
    float dt = m_timer.deltaTime();
    float total = m_timer.totalTime();

    // Update particles
    m_particles.update(dt);

    // Emit fountain water particles
    for (int i = 0; i < 3; i++) {
        float theta = static_cast<float>(rand()) / RAND_MAX * 2.0f * 3.14159265f;
        float radius = static_cast<float>(rand()) / RAND_MAX * 0.2f;
        glm::vec3 pos(radius * cosf(theta), 2.8f, -5.0f + radius * sinf(theta));
        
        float vx = (static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f) * 0.8f;
        float vy = 3.5f + static_cast<float>(rand()) / RAND_MAX * 2.5f;
        float vz = (static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f) * 0.8f;
        glm::vec3 vel(vx, vy, vz);
        
        m_particles.emit(pos, vel, glm::vec4(0.4f, 0.7f, 1.0f, 0.8f), 0.08f, 1.2f);
    }

    // Camera update
    m_camera.update(m_input, dt);

    // Collision resolution for camera/player
    glm::vec3 resolved = m_physics.resolveMovement(
        m_camera.position(), m_camera.desiredPosition(),
        m_playerRadius, m_playerHeight);
    m_camera.setPosition(resolved);

    // Update rides
    for (auto& ride : m_rides) {
        ride->update(dt, total);
    }

    // Update active minigame
    handleMinigameState();
    if (m_activeMinigame && m_activeMinigame->state() == MinigameState::Active) {
        m_activeMinigame->update(dt, m_input, m_camera.position(), m_camera.front(), &m_particles);
        m_ui.setMinigameInfo(m_activeMinigame->name(), m_activeMinigame->score(),
            m_activeMinigame->targetScore(), m_activeMinigame->timeLeft());
        m_ui.setMinigameCharge(m_activeMinigame->getThrowCharge(), m_activeMinigame->isCharging());
    }

    // Check trigger zones for ride/minigame prompts
    auto triggers = m_physics.findTriggers(m_camera.position(), m_playerRadius);
    bool nearInteractable = false;
    for (const auto* trig : triggers) {
        if (hasPrefix(trig->tag, "ride_") || hasPrefix(trig->tag, "minigame_")) {
            std::string name = trig->tag.substr(trig->tag.find('_') + 1);
            m_ui.showRidePrompt(name);
            nearInteractable = true;
            break;
        }
    }
    if (!nearInteractable) {
        m_ui.hideRidePrompt();
    }

    // Audio listener
    if (m_audio.isAvailable()) {
        m_audio.setListenerPosition(m_camera.position(), m_camera.front(), m_camera.up());
    }
}

void Engine::render() {
    // Resize handling
    if (m_window.wasResized()) {
        glViewport(0, 0, m_window.width(), m_window.height());
        m_window.resetResizedFlag();
    }

    m_renderer.beginFrame(m_camera, m_window.aspectRatio());

    // Setup park lights
    m_park.setupLights(m_renderer, m_timer.totalTime());

    // Render world
    m_park.render(m_renderer);

    // Render rides
    for (auto& ride : m_rides) {
        ride->render(m_renderer);
    }

    // Render minigames
    for (auto& mg : m_minigames) {
        mg->render(m_renderer);
    }

    // Render particles
    m_particles.render(m_renderer);

    m_renderer.endFrame();

    // UI
    m_ui.beginFrame();
    m_ui.render(m_timer.fps(), m_renderer.drawCalls(), m_camera, m_timer.deltaTime());
    m_ui.endFrame();
}

void Engine::checkInteractions() {
    if (m_activeMinigame) return; // Already in a minigame

    auto triggers = m_physics.findTriggers(m_camera.position(), m_playerRadius);
    for (const auto* trig : triggers) {
        // Check minigame triggers
        if (hasPrefix(trig->tag, "minigame_")) {
            for (auto& mg : m_minigames) {
                std::string trigName = trig->tag.substr(9); // after "minigame_"
                if (mg->name().find("Shooting") != std::string::npos && trigName == "shooting") {
                    m_activeMinigame = mg.get();
                } else if (mg->name().find("Ring") != std::string::npos && trigName == "ringtoss") {
                    m_activeMinigame = mg.get();
                } else if (mg->name().find("Basketball") != std::string::npos && trigName == "basketball") {
                    m_activeMinigame = mg.get();
                }
            }
            if (m_activeMinigame) {
                m_activeMinigame->start();
                m_ui.showNotification("Started: " + m_activeMinigame->name());
                if (m_audio.isAvailable()) {
                    m_audio.play(m_hitSource, m_hitBuffer, 0.5f);
                }
                return;
            }
        }

        // Check ride triggers
        if (hasPrefix(trig->tag, "ride_")) {
            std::string rideName = trig->tag.substr(5);
            for (auto& ride : m_rides) {
                if (ride->name().find(rideName) != std::string::npos ||
                    rideName.find("ferris") != std::string::npos ||
                    rideName.find("carousel") != std::string::npos) {
                    ride->toggle();
                    std::string state = ride->isRunning() ? "Started" : "Stopped";
                    m_ui.showNotification(state + ": " + ride->name());
                    if (m_audio.isAvailable()) {
                        m_audio.play(m_hitSource, m_hitBuffer, 0.4f);
                    }
                    return;
                }
            }
        }
    }

    m_ui.showNotification("Nothing to interact with here");
}

void Engine::handleMinigameState() {
    if (!m_activeMinigame) return;

    auto state = m_activeMinigame->state();
    if (state == MinigameState::Won) {
        m_ui.showNotification("You won " + m_activeMinigame->name() + "!", 4.0f);
        if (m_audio.isAvailable()) {
            m_audio.play(m_hitSource, m_hitBuffer, 0.7f);
        }
        m_activeMinigame->stop();
        m_activeMinigame = nullptr;
        m_ui.clearMinigameInfo();
    } else if (state == MinigameState::Lost) {
        m_ui.showNotification("Time's up! Better luck next time.", 4.0f);
        m_activeMinigame->stop();
        m_activeMinigame = nullptr;
        m_ui.clearMinigameInfo();
    }
}

} // namespace ap3d
