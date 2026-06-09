#pragma once
#include "core/Common.h"
#include "core/Timer.h"
#include "core/Window.h"
#include "input/InputManager.h"
#include "camera/Camera.h"
#include "renderer/Renderer.h"
#include "physics/PhysicsWorld.h"
#include "audio/AudioManager.h"
#include "ui/UISystem.h"
#include "world/ParkWorld.h"
#include "rides/Rides.h"
#include "minigames/Minigames.h"
#include "renderer/ParticleSystem.h"

namespace ap3d {

/// Main engine class that owns and orchestrates all subsystems.
class Engine {
public:
    Engine() = default;
    ~Engine() = default;

    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    /// Initialize all systems and build the world.
    bool init();

    /// Run the main game loop.
    void run();

    /// Shutdown all systems.
    void shutdown();

    /// Get the particle system
    [[nodiscard]] ParticleSystem& particles() { return m_particles; }

private:
    void processInput();
    void update();
    void render();
    void checkInteractions();
    void handleMinigameState();

    // Core systems
    Window m_window;
    Timer m_timer;
    InputManager m_input;
    Camera m_camera;
    Renderer m_renderer;
    PhysicsWorld m_physics;
    AudioManager m_audio;
    UISystem m_ui;
    ParticleSystem m_particles;

    // World
    ParkWorld m_park;

    // Rides
    std::vector<std::unique_ptr<Ride>> m_rides;

    // Minigames
    std::vector<std::unique_ptr<Minigame>> m_minigames;
    Minigame* m_activeMinigame = nullptr;

    // Audio resources
    SoundBuffer m_ambientBuffer = 0;
    SoundSource m_ambientSource = 0;
    SoundBuffer m_hitBuffer = 0;
    SoundSource m_hitSource = 0;

    // State
    bool m_running = false;
    bool m_cursorLocked = true;
    float m_playerRadius = 0.4f;
    float m_playerHeight = 1.7f;
};

} // namespace ap3d
