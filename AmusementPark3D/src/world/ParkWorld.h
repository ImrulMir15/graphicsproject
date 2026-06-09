#pragma once
#include "core/Common.h"
#include "renderer/Mesh.h"
#include "renderer/Renderer.h"
#include "renderer/Texture.h"
#include "physics/PhysicsWorld.h"

namespace ap3d {

/// The unified amusement park world - one continuous coordinate system.
class ParkWorld {
public:
    ParkWorld() = default;

    /// Build all park geometry, scenery, and colliders.
    void build(PhysicsWorld& physics);

    /// Render the entire park environment.
    void render(Renderer& renderer);

    /// Setup park lights.
    void setupLights(Renderer& renderer, float totalTime);

    /// Get park boundary dimensions.
    [[nodiscard]] float parkWidth() const { return PARK_WIDTH; }
    [[nodiscard]] float parkDepth() const { return PARK_DEPTH; }

private:
    void buildGround();
    void buildWalls(PhysicsWorld& physics);
    void buildPaths();
    void buildEntranceGate(PhysicsWorld& physics);
    void buildTicketBooth(PhysicsWorld& physics);
    void buildFoodCourt(PhysicsWorld& physics);
    void buildBenches(PhysicsWorld& physics);
    void buildLampPosts();
    void buildTrees(PhysicsWorld& physics);
    void buildGardens(PhysicsWorld& physics);
    void buildFountain(PhysicsWorld& physics);
    void buildSky();

    static constexpr float PARK_WIDTH = 120.0f;
    static constexpr float PARK_DEPTH = 100.0f;
    static constexpr float WALL_HEIGHT = 4.0f;

    // Sky
    Mesh m_skyDomeMesh;

    // Ground
    Mesh m_groundMesh;
    Texture m_grassTexture;

    // Paths
    Mesh m_pathMesh;
    Texture m_pathTexture;

    // Walls
    Mesh m_wallMesh;

    // Entrance gate
    Mesh m_gatePillarMesh;
    Mesh m_gateArchMesh;
    Mesh m_gateSignMesh;

    // Ticket booth
    Mesh m_boothMesh;
    Mesh m_boothRoofMesh;
    Mesh m_boothWindowMesh;

    // Food court
    Mesh m_tableMesh;
    Mesh m_chairMesh;
    Mesh m_counterMesh;
    Mesh m_awningMesh;

    // Scenery
    Mesh m_benchMesh;
    Mesh m_lampPostMesh;
    Mesh m_lampBulbMesh;
    Mesh m_treeTrunkMesh;
    Mesh m_treeCanopyMesh;
    Mesh m_flowerBedMesh;
    Mesh m_trashCanMesh;
    Mesh m_fencePostMesh;
    Mesh m_fountainBaseMesh;
    Mesh m_fountainPillarMesh;
    Mesh m_fountainWaterMesh;

    // Positions for batch rendering
    std::vector<glm::vec3> m_benchPositions;
    std::vector<float> m_benchRotations;
    std::vector<glm::vec3> m_lampPositions;
    std::vector<glm::vec3> m_treePositions;
    std::vector<float> m_treeScales;
    std::vector<glm::vec3> m_flowerPositions;
    std::vector<glm::vec3> m_trashCanPositions;
};

} // namespace ap3d
