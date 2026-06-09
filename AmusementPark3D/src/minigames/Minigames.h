#pragma once
#include "core/Common.h"
#include "renderer/Mesh.h"
#include "renderer/Renderer.h"
#include "physics/PhysicsWorld.h"

namespace ap3d {

class InputManager;
class AudioManager;
class UISystem;

/// Minigame state.
enum class MinigameState { Inactive, Active, Won, Lost };

/// Base class for all minigames.
class Minigame {
public:
    Minigame(const std::string& name, const glm::vec3& position);
    virtual ~Minigame() = default;

    virtual void build(PhysicsWorld& physics) = 0;
    virtual void start() { m_state = MinigameState::Active; m_score = 0; m_timeLeft = m_maxTime; }
    virtual void update(float deltaTime, const InputManager& input, const glm::vec3& camPos,
        const glm::vec3& camFront, class ParticleSystem* particles = nullptr) = 0;
    virtual void render(Renderer& renderer) = 0;
    virtual void stop() { m_state = MinigameState::Inactive; }

    virtual float getThrowCharge() const { return 0.0f; }
    virtual bool isCharging() const { return false; }

    [[nodiscard]] const std::string& name() const { return m_name; }
    [[nodiscard]] const glm::vec3& position() const { return m_position; }
    [[nodiscard]] MinigameState state() const { return m_state; }
    [[nodiscard]] int score() const { return m_score; }
    [[nodiscard]] int targetScore() const { return m_targetScore; }
    [[nodiscard]] float timeLeft() const { return m_timeLeft; }

protected:
    std::string m_name;
    glm::vec3 m_position;
    MinigameState m_state = MinigameState::Inactive;
    int m_score = 0;
    int m_targetScore = 10;
    float m_timeLeft = 30.0f;
    float m_maxTime = 30.0f;
};

/// Shooting Gallery - click targets to score.
class ShootingGallery : public Minigame {
public:
    ShootingGallery(const glm::vec3& position);
    void build(PhysicsWorld& physics) override;
    void start() override;
    void update(float deltaTime, const InputManager& input, const glm::vec3& camPos,
        const glm::vec3& camFront, class ParticleSystem* particles = nullptr) override;
    void render(Renderer& renderer) override;

private:
    struct Target {
        glm::vec3 pos;
        float radius = 0.4f;
        bool active = true;
        float respawnTimer = 0.0f;
        float bobPhase = 0.0f;
    };

    static constexpr int NUM_TARGETS = 6;
    std::array<Target, NUM_TARGETS> m_targets;
    Mesh m_targetMesh;
    Mesh m_boothMesh;
    Mesh m_shelfMesh;
    float m_hitFlash = 0.0f;
};

/// Ring Toss - toss rings onto pegs.
class RingToss : public Minigame {
public:
    RingToss(const glm::vec3& position);
    void build(PhysicsWorld& physics) override;
    void start() override;
    void update(float deltaTime, const InputManager& input, const glm::vec3& camPos,
        const glm::vec3& camFront, class ParticleSystem* particles = nullptr) override;
    void render(Renderer& renderer) override;

    float getThrowCharge() const override { return m_throwCharge; }
    bool isCharging() const override { return m_isCharging; }

private:
    struct Peg {
        glm::vec3 pos;
        float radius = 0.15f;
        bool hasRing = false;
    };

    struct FlyingRing {
        glm::vec3 pos;
        glm::vec3 velocity;
        bool active = false;
        float life = 0.0f;
    };

    static constexpr int NUM_PEGS = 9;
    std::array<Peg, NUM_PEGS> m_pegs;
    std::vector<FlyingRing> m_flyingRings;
    Mesh m_pegMesh;
    Mesh m_ringMesh;
    Mesh m_boothMesh;
    Mesh m_tableMesh;
    float m_throwCooldown = 0.0f;
    int m_ringsLeft = 15;
    float m_throwCharge = 0.0f;
    bool m_isCharging = false;
};

/// Basketball Challenge - shoot hoops.
class BasketballChallenge : public Minigame {
public:
    BasketballChallenge(const glm::vec3& position);
    void build(PhysicsWorld& physics) override;
    void start() override;
    void update(float deltaTime, const InputManager& input, const glm::vec3& camPos,
        const glm::vec3& camFront, class ParticleSystem* particles = nullptr) override;
    void render(Renderer& renderer) override;

    float getThrowCharge() const override { return m_throwCharge; }
    bool isCharging() const override { return m_isCharging; }

private:
    struct Ball {
        glm::vec3 pos;
        glm::vec3 velocity;
        bool active = false;
        float life = 0.0f;
    };

    Mesh m_ballMesh;
    Mesh m_hoopMesh;
    Mesh m_backboardMesh;
    Mesh m_poleMesh;
    Mesh m_boothMesh;
    std::vector<Ball> m_balls;
    glm::vec3 m_hoopCenter;
    float m_hoopRadius = 0.6f;
    float m_throwCooldown = 0.0f;
    int m_ballsLeft = 20;
    float m_throwCharge = 0.0f;
    bool m_isCharging = false;
};

} // namespace ap3d
