#pragma once
#include "core/Common.h"
#include "renderer/Mesh.h"
#include "renderer/Renderer.h"
#include "physics/PhysicsWorld.h"

namespace ap3d {

class AudioManager;

/// Base class for all amusement park rides.
class Ride {
public:
    Ride(const std::string& name, const glm::vec3& position);
    virtual ~Ride() = default;

    /// Build the ride's geometry and colliders.
    virtual void build(PhysicsWorld& physics) = 0;

    /// Update animation state.
    virtual void update(float deltaTime, float totalTime) = 0;

    /// Submit render commands.
    virtual void render(Renderer& renderer) = 0;

    /// Setup audio for this ride.
    virtual void setupAudio(AudioManager& audio) {}

    /// Toggle the ride on/off.
    void toggle() { m_running = !m_running; }
    void setRunning(bool running) { m_running = running; }

    [[nodiscard]] const std::string& name() const { return m_name; }
    [[nodiscard]] const glm::vec3& position() const { return m_position; }
    [[nodiscard]] bool isRunning() const { return m_running; }

protected:
    /// Helper to submit a colored mesh at a transform.
    void submitMesh(Renderer& renderer, const Mesh& mesh, const glm::mat4& transform,
        const glm::vec3& color, float shininess = 32.0f);

    std::string m_name;
    glm::vec3 m_position;
    bool m_running = true;
    float m_animTime = 0.0f;
};

/// Ferris Wheel ride with rotating gondolas.
class FerrisWheel : public Ride {
public:
    FerrisWheel(const glm::vec3& position);
    void build(PhysicsWorld& physics) override;
    void update(float deltaTime, float totalTime) override;
    void render(Renderer& renderer) override;

private:
    static constexpr int NUM_GONDOLAS = 12;
    static constexpr float WHEEL_RADIUS = 12.0f;
    static constexpr float ROTATION_SPEED = 0.3f;

    Mesh m_supportMesh;
    Mesh m_rimMesh;
    Mesh m_spokeMesh;
    Mesh m_gondolaMesh;
    Mesh m_hubMesh;
    Mesh m_baseMesh;
    float m_rotation = 0.0f;
};

/// Roller Coaster with track and moving cars.
class RollerCoaster : public Ride {
public:
    RollerCoaster(const glm::vec3& position);
    void build(PhysicsWorld& physics) override;
    void update(float deltaTime, float totalTime) override;
    void render(Renderer& renderer) override;

private:
    struct TrackPoint {
        glm::vec3 position;
        glm::vec3 tangent;
    };

    void generateTrack();

    std::vector<TrackPoint> m_trackPoints;
    Mesh m_trackMesh;
    Mesh m_supportMesh;
    Mesh m_carMesh;
    float m_carT = 0.0f; // Position along track [0, 1]
    static constexpr float CAR_SPEED = 0.05f;
};

/// Carousel (merry-go-round) with rotating horses.
class Carousel : public Ride {
public:
    Carousel(const glm::vec3& position);
    void build(PhysicsWorld& physics) override;
    void update(float deltaTime, float totalTime) override;
    void render(Renderer& renderer) override;

private:
    static constexpr int NUM_HORSES = 8;
    static constexpr float PLATFORM_RADIUS = 5.0f;
    static constexpr float ROTATION_SPEED = 0.8f;

    Mesh m_platformMesh;
    Mesh m_roofMesh;
    Mesh m_poleMesh;
    Mesh m_horseMesh;
    Mesh m_centerPoleMesh;
    float m_rotation = 0.0f;
};

/// Swing Ride with chains and seats that swing outward.
class SwingRide : public Ride {
public:
    SwingRide(const glm::vec3& position);
    void build(PhysicsWorld& physics) override;
    void update(float deltaTime, float totalTime) override;
    void render(Renderer& renderer) override;

private:
    static constexpr int NUM_SWINGS = 10;
    static constexpr float TOP_RADIUS = 4.0f;
    static constexpr float ROTATION_SPEED = 1.2f;

    Mesh m_towerMesh;
    Mesh m_topMesh;
    Mesh m_chainMesh;
    Mesh m_seatMesh;
    float m_rotation = 0.0f;
    float m_swingAngle = 0.0f;
};

/// Drop Tower (vertical ride that drops).
class DropTower : public Ride {
public:
    DropTower(const glm::vec3& position);
    void build(PhysicsWorld& physics) override;
    void update(float deltaTime, float totalTime) override;
    void render(Renderer& renderer) override;

private:
    static constexpr float TOWER_HEIGHT = 30.0f;
    static constexpr float RIDE_SPEED = 8.0f;

    Mesh m_towerMesh;
    Mesh m_platformMesh;
    Mesh m_seatRingMesh;
    Mesh m_baseMesh;
    float m_carHeight = 0.0f;

    enum class Phase { Rising, Pause, Dropping, Landed };
    Phase m_phase = Phase::Rising;
    float m_phaseTimer = 0.0f;
};

} // namespace ap3d
