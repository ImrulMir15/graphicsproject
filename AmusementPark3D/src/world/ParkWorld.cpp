#include "world/ParkWorld.h"
#include "renderer/ProceduralMeshes.h"

namespace ap3d {

void ParkWorld::build(PhysicsWorld& physics) {
    buildSky();
    buildGround();
    buildWalls(physics);
    buildPaths();
    buildEntranceGate(physics);
    buildTicketBooth(physics);
    buildFoodCourt(physics);
    buildBenches(physics);
    buildLampPosts();
    buildTrees(physics);
    buildGardens(physics);
    buildFountain(physics);
    std::cout << "[ParkWorld] Built with " << physics.colliderCount() << " colliders\n";
}

void ParkWorld::buildGround() {
    std::vector<Vertex> v; std::vector<uint32_t> idx;
    ProceduralMeshes::createPlane(v, idx, PARK_WIDTH, PARK_DEPTH, 4, 4);
    ProceduralMeshes::colorVertices(v, glm::vec3(0.2f, 0.55f, 0.15f));
    m_groundMesh.upload(v, idx);
    m_grassTexture.createNoise(256, glm::vec3(0.25f, 0.52f, 0.20f), 0.2f, 2.0f);
}

void ParkWorld::buildWalls(PhysicsWorld& physics) {
    std::vector<Vertex> v; std::vector<uint32_t> idx;
    ProceduralMeshes::createCube(v, idx);
    m_wallMesh.upload(v, idx);

    float hw = PARK_WIDTH / 2.0f;
    float hd = PARK_DEPTH / 2.0f;
    float wh = WALL_HEIGHT;
    float wt = 0.5f;

    // Four walls with colliders
    struct WallDef { glm::vec3 pos; glm::vec3 scale; };
    WallDef walls[] = {
        {{0,     wh/2, -hd}, {PARK_WIDTH, wh, wt}},  // Back
        {{0,     wh/2,  hd}, {PARK_WIDTH, wh, wt}},  // Front (with gap for entrance)
        {{-hw,   wh/2,  0},  {wt, wh, PARK_DEPTH}},  // Left
        {{ hw,   wh/2,  0},  {wt, wh, PARK_DEPTH}},  // Right
    };

    for (const auto& w : walls) {
        Collider col;
        col.type = ColliderType::AABB;
        col.bounds.min = w.pos - w.scale * 0.5f;
        col.bounds.max = w.pos + w.scale * 0.5f;
        col.tag = "wall";
        physics.addCollider(col);
    }
}

void ParkWorld::buildPaths() {
    std::vector<Vertex> v; std::vector<uint32_t> idx;
    ProceduralMeshes::createPlane(v, idx, 5.0f, PARK_DEPTH * 0.8f, 1, 1);
    ProceduralMeshes::colorVertices(v, glm::vec3(0.6f, 0.55f, 0.45f));
    m_pathMesh.upload(v, idx);
    m_pathTexture.createNoise(256, glm::vec3(0.55f, 0.52f, 0.48f), 0.15f, 1.5f);
}

void ParkWorld::buildEntranceGate(PhysicsWorld& physics) {
    std::vector<Vertex> v; std::vector<uint32_t> idx;

    ProceduralMeshes::createCube(v, idx);
    m_gatePillarMesh.upload(v, idx);

    ProceduralMeshes::createArch(v, idx, 8.0f, 7.0f, 1.2f, 12);
    m_gateArchMesh.upload(v, idx);

    ProceduralMeshes::createCube(v, idx);
    m_gateSignMesh.upload(v, idx);

    float hw = PARK_WIDTH / 2.0f;
    float hd = PARK_DEPTH / 2.0f;

    Collider leftPillar;
    leftPillar.type = ColliderType::AABB;
    leftPillar.bounds.min = glm::vec3(-5.5f, 0, hd - 1);
    leftPillar.bounds.max = glm::vec3(-3.5f, 7, hd + 1);
    leftPillar.tag = "gate";
    physics.addCollider(leftPillar);

    Collider rightPillar;
    rightPillar.type = ColliderType::AABB;
    rightPillar.bounds.min = glm::vec3(3.5f, 0, hd - 1);
    rightPillar.bounds.max = glm::vec3(5.5f, 7, hd + 1);
    rightPillar.tag = "gate";
    physics.addCollider(rightPillar);
}

void ParkWorld::buildTicketBooth(PhysicsWorld& physics) {
    std::vector<Vertex> v; std::vector<uint32_t> idx;

    ProceduralMeshes::createCube(v, idx);
    m_boothMesh.upload(v, idx);

    ProceduralMeshes::createCube(v, idx);
    m_boothRoofMesh.upload(v, idx);

    ProceduralMeshes::createCube(v, idx);
    m_boothWindowMesh.upload(v, idx);

    float hd = PARK_DEPTH / 2.0f;
    Collider col;
    col.type = ColliderType::AABB;
    col.bounds.min = glm::vec3(8.0f, 0, hd - 3.0f);
    col.bounds.max = glm::vec3(12.0f, 3.5f, hd + 0.5f);
    col.tag = "ticket_booth";
    physics.addCollider(col);
}

void ParkWorld::buildFoodCourt(PhysicsWorld& physics) {
    std::vector<Vertex> v; std::vector<uint32_t> idx;

    // Table mesh (wood cylinder)
    ProceduralMeshes::createCylinder(v, idx, 0.5f, 0.8f, 8, true);
    m_tableMesh.upload(v, idx);

    // Stool/chair mesh (wood cube)
    ProceduralMeshes::createCube(v, idx);
    m_chairMesh.upload(v, idx);

    // Rebuild m_counterMesh as a composite mesh containing the three themed shops!
    std::vector<Vertex> shopVerts;
    std::vector<uint32_t> shopIdx;

    // Colors for the shops
    glm::vec3 phWall(0.85f, 0.15f, 0.15f); // Red
    glm::vec3 phRoof(0.55f, 0.10f, 0.10f); // Dark Red
    
    glm::vec3 brWall(0.95f, 0.65f, 0.75f); // Pink
    glm::vec3 brRoof(0.15f, 0.55f, 0.85f); // Light Blue
    
    glm::vec3 ddWall(0.92f, 0.55f, 0.15f); // Orange
    glm::vec3 ddRoof(0.45f, 0.25f, 0.12f); // Dark Brown

    float zOffsets[] = { -9.0f, -2.0f, 5.0f };
    std::string names[] = { "Pizza Hut", "Baskin Robbins", "Dunkin Donuts" };

    for (int sIdx = 0; sIdx < 3; sIdx++) {
        float zOffset = zOffsets[sIdx];
        std::string name = names[sIdx];
        glm::vec3 wallColor = (sIdx == 0) ? phWall : ((sIdx == 1) ? brWall : ddWall);
        glm::vec3 roofColor = (sIdx == 0) ? phRoof : ((sIdx == 1) ? brRoof : ddRoof);

        // 1. Main building structure (Cube)
        v.clear(); idx.clear();
        ProceduralMeshes::createCube(v, idx);
        ProceduralMeshes::colorVertices(v, wallColor);
        glm::mat4 baseTrans = glm::translate(glm::mat4(1.0f), glm::vec3(-38.0f, 1.5f, zOffset));
        baseTrans = glm::scale(baseTrans, glm::vec3(3.0f, 3.0f, 4.0f));
        ProceduralMeshes::transformVertices(v, baseTrans);
        ProceduralMeshes::merge(shopVerts, shopIdx, v, idx);

        // 2. Service counter ledge (Cube in front of building)
        v.clear(); idx.clear();
        ProceduralMeshes::createCube(v, idx);
        ProceduralMeshes::colorVertices(v, glm::vec3(0.85f, 0.85f, 0.88f)); // Clean counter shelf
        glm::mat4 shelfTrans = glm::translate(glm::mat4(1.0f), glm::vec3(-36.3f, 1.0f, zOffset));
        shelfTrans = glm::scale(shelfTrans, glm::vec3(0.4f, 0.1f, 3.0f));
        ProceduralMeshes::transformVertices(v, shelfTrans);
        ProceduralMeshes::merge(shopVerts, shopIdx, v, idx);

        // 3. Service window cutout area
        v.clear(); idx.clear();
        ProceduralMeshes::createCube(v, idx);
        ProceduralMeshes::colorVertices(v, glm::vec3(0.08f, 0.08f, 0.1f));
        glm::mat4 cutTrans = glm::translate(glm::mat4(1.0f), glm::vec3(-36.45f, 1.8f, zOffset));
        cutTrans = glm::scale(cutTrans, glm::vec3(0.1f, 1.2f, 2.6f));
        ProceduralMeshes::transformVertices(v, cutTrans);
        ProceduralMeshes::merge(shopVerts, shopIdx, v, idx);

        // 4. Roof
        if (sIdx == 0) {
            // Pizza Hut: Sloped pyramid roof
            v.clear(); idx.clear();
            ProceduralMeshes::createCone(v, idx, 2.8f, 1.5f, 4); // 4-sided pyramid cone
            ProceduralMeshes::colorVertices(v, roofColor);
            glm::mat4 roofTrans = glm::translate(glm::mat4(1.0f), glm::vec3(-38.0f, 3.0f, zOffset));
            roofTrans = glm::rotate(roofTrans, HALF_PI * 0.5f, glm::vec3(0, 1, 0));
            ProceduralMeshes::transformVertices(v, roofTrans);
            ProceduralMeshes::merge(shopVerts, shopIdx, v, idx);
        } else if (sIdx == 1) {
            // Baskin Robbins: Curved dome roof
            v.clear(); idx.clear();
            ProceduralMeshes::createSphere(v, idx, 16, 8, 2.1f);
            ProceduralMeshes::colorVertices(v, roofColor);
            glm::mat4 roofTrans = glm::translate(glm::mat4(1.0f), glm::vec3(-38.0f, 2.9f, zOffset));
            roofTrans = glm::scale(roofTrans, glm::vec3(0.7f, 0.4f, 0.95f));
            ProceduralMeshes::transformVertices(v, roofTrans);
            ProceduralMeshes::merge(shopVerts, shopIdx, v, idx);
        } else {
            // Dunkin Donuts: Flat roof border
            v.clear(); idx.clear();
            ProceduralMeshes::createCube(v, idx);
            ProceduralMeshes::colorVertices(v, roofColor);
            glm::mat4 roofTrans = glm::translate(glm::mat4(1.0f), glm::vec3(-38.0f, 3.1f, zOffset));
            roofTrans = glm::scale(roofTrans, glm::vec3(3.2f, 0.2f, 4.2f));
            ProceduralMeshes::transformVertices(v, roofTrans);
            ProceduralMeshes::merge(shopVerts, shopIdx, v, idx);
        }

        // 5. Giant Sign / Logo Billboard on top
        if (sIdx == 0) {
            // Pizza Hut: slice sign
            v.clear(); idx.clear();
            ProceduralMeshes::createCube(v, idx);
            ProceduralMeshes::colorVertices(v, glm::vec3(0.9f, 0.1f, 0.15f));
            glm::mat4 signTrans = glm::translate(glm::mat4(1.0f), glm::vec3(-37.5f, 4.3f, zOffset));
            signTrans = glm::scale(signTrans, glm::vec3(0.2f, 0.8f, 2.0f));
            ProceduralMeshes::transformVertices(v, signTrans);
            ProceduralMeshes::merge(shopVerts, shopIdx, v, idx);
        } else if (sIdx == 1) {
            // Baskin Robbins: Oval disk
            v.clear(); idx.clear();
            ProceduralMeshes::createSphere(v, idx, 12, 6, 0.5f);
            ProceduralMeshes::colorVertices(v, glm::vec3(0.95f, 0.25f, 0.65f));
            glm::mat4 signTrans = glm::translate(glm::mat4(1.0f), glm::vec3(-37.5f, 4.1f, zOffset));
            signTrans = glm::scale(signTrans, glm::vec3(0.4f, 1.2f, 1.2f));
            ProceduralMeshes::transformVertices(v, signTrans);
            ProceduralMeshes::merge(shopVerts, shopIdx, v, idx);
        } else {
            // Dunkin Donuts: Donut!
            v.clear(); idx.clear();
            ProceduralMeshes::createTorus(v, idx, 0.6f, 0.25f, 16, 8);
            ProceduralMeshes::colorVertices(v, glm::vec3(0.85f, 0.45f, 0.2f));
            glm::mat4 signTrans = glm::translate(glm::mat4(1.0f), glm::vec3(-37.5f, 4.2f, zOffset));
            signTrans = glm::rotate(signTrans, HALF_PI, glm::vec3(0, 1, 0));
            ProceduralMeshes::transformVertices(v, signTrans);
            ProceduralMeshes::merge(shopVerts, shopIdx, v, idx);
            
            // frosting
            v.clear(); idx.clear();
            ProceduralMeshes::createTorus(v, idx, 0.6f, 0.12f, 16, 8);
            ProceduralMeshes::colorVertices(v, glm::vec3(0.95f, 0.35f, 0.65f));
            glm::mat4 frostTrans = glm::translate(glm::mat4(1.0f), glm::vec3(-37.45f, 4.2f, zOffset));
            frostTrans = glm::rotate(frostTrans, HALF_PI, glm::vec3(0, 1, 0));
            ProceduralMeshes::transformVertices(v, frostTrans);
            ProceduralMeshes::merge(shopVerts, shopIdx, v, idx);
        }
    }

    m_counterMesh.upload(shopVerts, shopIdx);

    ProceduralMeshes::createPlane(v, idx, 6.0f, 3.0f, 1, 1);
    m_awningMesh.upload(v, idx);

    // Food court collider
    Collider col;
    col.type = ColliderType::AABB;
    col.bounds.min = glm::vec3(-40.0f, 0, -13.0f);
    col.bounds.max = glm::vec3(-35.0f, 3.5f, 9.0f);
    col.tag = "food_court";
    physics.addCollider(col);
}

void ParkWorld::buildBenches(PhysicsWorld& physics) {
    std::vector<Vertex> v; std::vector<uint32_t> idx;
    ProceduralMeshes::createCube(v, idx);
    m_benchMesh.upload(v, idx);

    // Place benches along paths
    m_benchPositions = {
        {4.0f, 0, 20.0f}, {4.0f, 0, 10.0f}, {4.0f, 0, 0.0f}, {4.0f, 0, -10.0f},
        {-4.0f, 0, 20.0f}, {-4.0f, 0, 10.0f}, {-4.0f, 0, 0.0f}, {-4.0f, 0, -10.0f},
        {15.0f, 0, 5.0f}, {-15.0f, 0, 5.0f}, {15.0f, 0, -15.0f}, {-15.0f, 0, -15.0f},
    };
    m_benchRotations.resize(m_benchPositions.size());
    for (size_t i = 0; i < m_benchPositions.size(); i++) {
        m_benchRotations[i] = (i < 4) ? HALF_PI : (i < 8) ? -HALF_PI : 0.0f;
        Collider col;
        col.type = ColliderType::AABB;
        col.bounds.min = m_benchPositions[i] - glm::vec3(0.8f, 0, 0.3f);
        col.bounds.max = m_benchPositions[i] + glm::vec3(0.8f, 0.6f, 0.3f);
        col.tag = "bench";
        physics.addCollider(col);
    }
}

void ParkWorld::buildLampPosts() {
    std::vector<Vertex> v; std::vector<uint32_t> idx;

    ProceduralMeshes::createCylinder(v, idx, 0.08f, 4.0f, 6, true);
    m_lampPostMesh.upload(v, idx);

    ProceduralMeshes::createSphere(v, idx, 8, 4, 0.25f);
    m_lampBulbMesh.upload(v, idx);

    m_lampPositions = {
        {6.0f, 0, 30.0f}, {-6.0f, 0, 30.0f},
        {6.0f, 0, 15.0f}, {-6.0f, 0, 15.0f},
        {6.0f, 0, 0.0f},  {-6.0f, 0, 0.0f},
        {6.0f, 0, -15.0f},{-6.0f, 0, -15.0f},
        {6.0f, 0, -30.0f},{-6.0f, 0, -30.0f},
        {20.0f, 0, 0.0f}, {-20.0f, 0, 0.0f},
        {30.0f, 0, 15.0f},{-30.0f, 0, 15.0f},
        {30.0f, 0, -15.0f},{-30.0f, 0, -15.0f},
    };
}

void ParkWorld::buildTrees(PhysicsWorld& physics) {
    std::vector<Vertex> v; std::vector<uint32_t> idx;

    ProceduralMeshes::createCylinder(v, idx, 0.2f, 2.0f, 8, true);
    m_treeTrunkMesh.upload(v, idx);

    ProceduralMeshes::createSphere(v, idx, 12, 6, 1.5f);
    m_treeCanopyMesh.upload(v, idx);

    // Seed-based positioning for natural look
    unsigned int seed = 42;
    auto rng = [&seed]() -> float {
        seed = seed * 1103515245 + 12345;
        return static_cast<float>((seed >> 16) & 0x7FFF) / 32767.0f;
    };

    float hw = PARK_WIDTH / 2.0f - 5.0f;
    float hd = PARK_DEPTH / 2.0f - 5.0f;

    for (int i = 0; i < 30; i++) {
        float x = (rng() * 2.0f - 1.0f) * hw;
        float z = (rng() * 2.0f - 1.0f) * hd;

        // Avoid center path area
        if (fabsf(x) < 8.0f && fabsf(z) < hd * 0.8f) continue;
        // Avoid ride areas
        if (fabsf(x - 25.0f) < 15.0f && fabsf(z) < 20.0f) continue;
        if (fabsf(x + 25.0f) < 15.0f && fabsf(z) < 20.0f) continue;

        float scale = 0.8f + rng() * 0.6f;
        m_treePositions.push_back(glm::vec3(x, 0, z));
        m_treeScales.push_back(scale);

        Collider col;
        col.type = ColliderType::Cylinder;
        col.center = glm::vec3(x, 1.0f, z);
        col.radius = 0.4f;
        col.height = 2.0f;
        col.tag = "tree";
        physics.addCollider(col);
    }
}

void ParkWorld::buildGardens(PhysicsWorld& physics) {
    std::vector<Vertex> finalVerts;
    std::vector<uint32_t> finalIdx;

    // 1. Stone Wall Ring
    std::vector<Vertex> v; std::vector<uint32_t> idx;
    ProceduralMeshes::createCylinder(v, idx, 1.5f, 0.3f, 16, true);
    ProceduralMeshes::colorVertices(v, glm::vec3(0.45f, 0.45f, 0.48f)); // Slate stone grey
    ProceduralMeshes::merge(finalVerts, finalIdx, v, idx);

    // 2. Dirt/Soil
    v.clear(); idx.clear();
    ProceduralMeshes::createCylinder(v, idx, 1.42f, 0.31f, 12, true);
    ProceduralMeshes::colorVertices(v, glm::vec3(0.25f, 0.16f, 0.08f)); // Organic brown soil
    ProceduralMeshes::merge(finalVerts, finalIdx, v, idx);

    // 3. Stems and flowers (procedurally distributed inside the bed)
    glm::vec3 flowerColors[] = {
        {0.92f, 0.15f, 0.15f}, // Vivid Red
        {0.98f, 0.85f, 0.05f}, // Sun Yellow
        {0.15f, 0.55f, 0.95f}, // Sky Blue
        {0.95f, 0.25f, 0.65f}, // Hot Pink
        {0.98f, 0.45f, 0.08f}  // Bright Orange
    };

    uint32_t seed = 54321;
    auto randFloat = [&]() {
        seed = seed * 1103515245 + 12345;
        return static_cast<float>(seed & 0xffff) / 65535.0f;
    };

    for (int i = 0; i < 14; i++) {
        float angle = randFloat() * TWO_PI;
        float r = 0.15f + randFloat() * 1.15f;
        float fx = r * cosf(angle);
        float fz = r * sinf(angle);
        float h = 0.20f + randFloat() * 0.20f; // stem height

        // 1. Stem (green cylinder)
        v.clear(); idx.clear();
        ProceduralMeshes::createCylinder(v, idx, 0.015f, h, 6, true);
        ProceduralMeshes::colorVertices(v, glm::vec3(0.12f, 0.48f, 0.16f));
        glm::mat4 stemTrans = glm::translate(glm::mat4(1.0f), glm::vec3(fx, 0.155f + h * 0.5f, fz));
        ProceduralMeshes::transformVertices(v, stemTrans);
        ProceduralMeshes::merge(finalVerts, finalIdx, v, idx);

        // 2. Leaves (2 green angled boxes)
        for (int l = 0; l < 2; l++) {
            v.clear(); idx.clear();
            ProceduralMeshes::createCube(v, idx);
            ProceduralMeshes::colorVertices(v, glm::vec3(0.10f, 0.42f, 0.14f));
            glm::mat4 leafTrans = glm::translate(glm::mat4(1.0f), glm::vec3(fx, 0.155f + h * 0.4f, fz));
            leafTrans = glm::rotate(leafTrans, l * PI + 0.5f, glm::vec3(0, 1, 0));
            leafTrans = glm::rotate(leafTrans, 0.5f, glm::vec3(0, 0, 1));
            leafTrans = glm::translate(leafTrans, glm::vec3(0.04f, 0.0f, 0.0f));
            leafTrans = glm::scale(leafTrans, glm::vec3(0.08f, 0.01f, 0.03f));
            ProceduralMeshes::transformVertices(v, leafTrans);
            ProceduralMeshes::merge(finalVerts, finalIdx, v, idx);
        }

        // 3. Flower Center Core (yellow sphere)
        v.clear(); idx.clear();
        ProceduralMeshes::createSphere(v, idx, 8, 4, 0.04f);
        ProceduralMeshes::colorVertices(v, glm::vec3(0.98f, 0.85f, 0.05f));
        glm::mat4 coreTrans = glm::translate(glm::mat4(1.0f), glm::vec3(fx, 0.155f + h, fz));
        ProceduralMeshes::transformVertices(v, coreTrans);
        ProceduralMeshes::merge(finalVerts, finalIdx, v, idx);

        // 4. Petals (6 distinct boxes rotated in a circle and angled upwards)
        glm::vec3 petalCol = flowerColors[i % 5];
        for (int p = 0; p < 6; p++) {
            v.clear(); idx.clear();
            ProceduralMeshes::createCube(v, idx);
            ProceduralMeshes::colorVertices(v, petalCol);
            
            glm::mat4 petalTrans = glm::translate(glm::mat4(1.0f), glm::vec3(fx, 0.155f + h, fz));
            petalTrans = glm::rotate(petalTrans, p * (TWO_PI / 6.0f), glm::vec3(0, 1, 0));
            petalTrans = glm::rotate(petalTrans, 0.25f, glm::vec3(0, 0, 1));
            petalTrans = glm::translate(petalTrans, glm::vec3(0.06f, 0.0f, 0.0f));
            petalTrans = glm::scale(petalTrans, glm::vec3(0.12f, 0.01f, 0.04f));
            
            ProceduralMeshes::transformVertices(v, petalTrans);
            ProceduralMeshes::merge(finalVerts, finalIdx, v, idx);
        }
    }

    m_flowerBedMesh.upload(finalVerts, finalIdx);

    m_flowerPositions = {
        {12.0f, 0, 25.0f}, {-12.0f, 0, 25.0f},
        {12.0f, 0, -25.0f}, {-12.0f, 0, -25.0f},
        {0.0f, 0, -35.0f}, {20.0f, 0, -5.0f}, {-20.0f, 0, -5.0f},
    };

    // Trash cans
    v.clear(); idx.clear();
    ProceduralMeshes::createCylinder(v, idx, 0.25f, 0.8f, 10, true);
    m_trashCanMesh.upload(v, idx);

    m_trashCanPositions = {
        {-22.0f, 0.0f, -22.0f},
        {-36.0f, 0.0f, 22.0f},
        {36.0f, 0.0f, -22.0f},
        {-8.0f, 0.0f, 32.0f},
        {8.0f, 0.0f, 32.0f},
        {-28.0f, 0.0f, -2.0f},
        {22.0f, 0.0f, 22.0f}
    };

    for (const auto& pos : m_trashCanPositions) {
        Collider col;
        col.type = ColliderType::Cylinder;
        col.center = pos + glm::vec3(0, 0.4f, 0);
        col.radius = 0.35f;
        col.height = 0.8f;
        col.tag = "trash_can";
        physics.addCollider(col);
    }
}

void ParkWorld::buildFountain(PhysicsWorld& physics) {
    std::vector<Vertex> finalBaseVerts;
    std::vector<uint32_t> finalBaseIdx;

    // 1. Basin Outer Wall
    std::vector<Vertex> v; std::vector<uint32_t> idx;
    ProceduralMeshes::createCylinder(v, idx, 3.0f, 0.6f, 24, true);
    ProceduralMeshes::colorVertices(v, glm::vec3(0.55f, 0.55f, 0.58f)); // Polished concrete wall
    ProceduralMeshes::merge(finalBaseVerts, finalBaseIdx, v, idx);

    // 2. Basin Water Surface
    v.clear(); idx.clear();
    ProceduralMeshes::createCylinder(v, idx, 2.85f, 0.58f, 20, true);
    ProceduralMeshes::colorVertices(v, glm::vec3(0.2f, 0.55f, 0.85f)); // Shimmering pool water
    ProceduralMeshes::merge(finalBaseVerts, finalBaseIdx, v, idx);

    // 3. Central Step
    v.clear(); idx.clear();
    ProceduralMeshes::createCylinder(v, idx, 0.8f, 0.7f, 12, true);
    ProceduralMeshes::colorVertices(v, glm::vec3(0.45f, 0.45f, 0.47f)); // Center platform
    ProceduralMeshes::merge(finalBaseVerts, finalBaseIdx, v, idx);

    // 4. Water Jets (Sprays)
    for (int i = 0; i < 8; ++i) {
        float angle = i * (TWO_PI / 8.0f);
        glm::vec3 start(0.0f, 0.8f, 0.0f);
        glm::vec3 end(2.2f * cosf(angle), 0.29f, 2.2f * sinf(angle));
        
        v.clear(); idx.clear();
        ProceduralMeshes::createCylinder(v, idx, 0.04f, glm::length(end - start), 4, true);
        ProceduralMeshes::colorVertices(v, glm::vec3(0.7f, 0.85f, 1.0f)); // Light blue water spray
        
        glm::vec3 dir = glm::normalize(end - start);
        glm::vec3 up(0, 1, 0);
        glm::vec3 right = glm::normalize(glm::cross(dir, up));
        if (glm::length(right) < 0.01f) {
            right = glm::vec3(1, 0, 0);
        }
        glm::vec3 actualUp = glm::normalize(glm::cross(right, dir));
        
        glm::mat4 rot(1.0f);
        rot[0] = glm::vec4(right, 0.0f);
        rot[1] = glm::vec4(dir, 0.0f);
        rot[2] = glm::vec4(actualUp, 0.0f);
        
        glm::mat4 trans = glm::translate(glm::mat4(1.0f), (start + end) * 0.5f) * rot;
        ProceduralMeshes::transformVertices(v, trans);
        ProceduralMeshes::merge(finalBaseVerts, finalBaseIdx, v, idx);
    }

    m_fountainBaseMesh.upload(finalBaseVerts, finalBaseIdx);

    // Central Pillar
    v.clear(); idx.clear();
    ProceduralMeshes::createCylinder(v, idx, 0.3f, 2.0f, 8, true);
    ProceduralMeshes::colorVertices(v, glm::vec3(0.65f, 0.65f, 0.7f));
    m_fountainPillarMesh.upload(v, idx);

    // Water Sphere on Top
    v.clear(); idx.clear();
    ProceduralMeshes::createSphere(v, idx, 12, 6, 0.5f);
    ProceduralMeshes::colorVertices(v, glm::vec3(0.3f, 0.65f, 0.95f)); // Deep blue water
    m_fountainWaterMesh.upload(v, idx);

    // Fence post
    v.clear(); idx.clear();
    ProceduralMeshes::createCylinder(v, idx, 0.04f, 0.9f, 6, true);
    m_fencePostMesh.upload(v, idx);

    Collider col;
    col.type = ColliderType::Cylinder;
    col.center = glm::vec3(0, 0.5f, -5.0f);
    col.radius = 3.5f;
    col.height = 2.0f;
    col.tag = "fountain";
    physics.addCollider(col);
}

void ParkWorld::render(Renderer& renderer) {
    // Sky Dome
    RenderCommand skyCmd;
    skyCmd.mesh = &m_skyDomeMesh;
    skyCmd.transform = glm::translate(glm::mat4(1.0f), renderer.cameraPosition());
    skyCmd.material.ambient = glm::vec3(1.0f);
    skyCmd.material.diffuse = glm::vec3(1.0f); // Allow vertex colors to show
    skyCmd.material.specular = glm::vec3(0.0f);
    skyCmd.material.useVertexColor = true;
    skyCmd.material.shininess = 1.0f;
    skyCmd.disableCulling = true; // Make sure the inside of the sphere renders
    renderer.submit(skyCmd);

    RenderCommand cmd;

    // Ground
    cmd.mesh = &m_groundMesh;
    cmd.transform = glm::mat4(1.0f);
    cmd.material.diffuse = glm::vec3(0.25f, 0.55f, 0.18f);
    cmd.material.useVertexColor = false;
    cmd.material.diffuseMap = &m_grassTexture;
    cmd.material.shininess = 8.0f;
    renderer.submit(cmd);

    // Main path (central walkway)
    cmd.mesh = &m_pathMesh;
    cmd.transform = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0.02f, 0));
    cmd.material.diffuse = glm::vec3(0.6f, 0.55f, 0.45f);
    cmd.material.diffuseMap = &m_pathTexture;
    renderer.submit(cmd);

    // Cross path
    cmd.transform = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0.021f, 0));
    cmd.transform = glm::rotate(cmd.transform, HALF_PI, glm::vec3(0, 1, 0));
    renderer.submit(cmd);

    float hw = PARK_WIDTH / 2.0f;
    float hd = PARK_DEPTH / 2.0f;
    cmd.material.diffuseMap = nullptr;

    // Walls
    cmd.mesh = &m_wallMesh;
    cmd.material.diffuse = glm::vec3(0.55f, 0.45f, 0.35f);
    struct { glm::vec3 pos; glm::vec3 scale; } walls[] = {
        {{0, WALL_HEIGHT/2, -hd}, {PARK_WIDTH, WALL_HEIGHT, 0.5f}},
        {{0, WALL_HEIGHT/2, hd}, {PARK_WIDTH, WALL_HEIGHT, 0.5f}},
        {{-hw, WALL_HEIGHT/2, 0}, {0.5f, WALL_HEIGHT, PARK_DEPTH}},
        {{hw, WALL_HEIGHT/2, 0}, {0.5f, WALL_HEIGHT, PARK_DEPTH}},
    };
    for (const auto& w : walls) {
        cmd.transform = glm::translate(glm::mat4(1.0f), w.pos);
        cmd.transform = glm::scale(cmd.transform, w.scale);
        renderer.submit(cmd);
    }

    // Entrance gate
    cmd.mesh = &m_gateArchMesh;
    cmd.transform = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, hd));
    cmd.transform = glm::rotate(cmd.transform, HALF_PI, glm::vec3(0, 1, 0));
    cmd.material.diffuse = glm::vec3(0.7f, 0.5f, 0.2f);
    renderer.submit(cmd);

    // Gate sign
    cmd.mesh = &m_gateSignMesh;
    cmd.transform = glm::translate(glm::mat4(1.0f), glm::vec3(0, 8.0f, hd));
    cmd.transform = glm::scale(cmd.transform, glm::vec3(10.0f, 1.5f, 0.3f));
    cmd.material.diffuse = glm::vec3(0.8f, 0.2f, 0.15f);
    renderer.submit(cmd);

    // Ticket booth
    cmd.mesh = &m_boothMesh;
    cmd.transform = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 1.5f, hd - 2.0f));
    cmd.transform = glm::scale(cmd.transform, glm::vec3(3.0f, 3.0f, 2.5f));
    cmd.material.diffuse = glm::vec3(0.3f, 0.5f, 0.7f);
    renderer.submit(cmd);

    // Booth roof
    cmd.mesh = &m_boothRoofMesh;
    cmd.transform = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 3.3f, hd - 2.0f));
    cmd.transform = glm::scale(cmd.transform, glm::vec3(3.5f, 0.4f, 3.0f));
    cmd.material.diffuse = glm::vec3(0.7f, 0.2f, 0.15f);
    renderer.submit(cmd);

    // Themed Food Court Shops (Pizza Hut, Baskin Robbins, Dunkin Donuts)
    cmd.mesh = &m_counterMesh;
    cmd.transform = glm::mat4(1.0f);
    cmd.material.diffuse = glm::vec3(1.0f);
    cmd.material.useVertexColor = true;
    renderer.submit(cmd);

    // Food court tables and chairs
    for (int i = 0; i < 4; i++) {
        float tz = -6.0f + i * 3.5f;
        glm::vec3 tablePos(-35.0f, 0.0f, tz);

        // Table
        cmd.mesh = &m_tableMesh;
        cmd.transform = glm::translate(glm::mat4(1.0f), tablePos + glm::vec3(0.0f, 0.4f, 0.0f));
        cmd.material.diffuse = glm::vec3(0.65f, 0.45f, 0.25f);
        cmd.material.useVertexColor = false;
        renderer.submit(cmd);

        // Stools around table
        cmd.mesh = &m_chairMesh;
        cmd.material.diffuse = glm::vec3(0.45f, 0.3f, 0.15f);
        glm::vec3 chairOffsets[] = {
            {-0.9f, 0.25f, 0.0f},
            {0.9f, 0.25f, 0.0f},
            {0.0f, 0.25f, -0.9f},
            {0.0f, 0.25f, 0.9f}
        };
        for (const auto& offset : chairOffsets) {
            cmd.transform = glm::translate(glm::mat4(1.0f), tablePos + offset);
            cmd.transform = glm::scale(cmd.transform, glm::vec3(0.4f, 0.5f, 0.4f));
            renderer.submit(cmd);
        }
    }

    // Benches
    cmd.mesh = &m_benchMesh;
    for (size_t i = 0; i < m_benchPositions.size(); i++) {
        cmd.transform = glm::translate(glm::mat4(1.0f), m_benchPositions[i] + glm::vec3(0, 0.3f, 0));
        cmd.transform = glm::rotate(cmd.transform, m_benchRotations[i], glm::vec3(0, 1, 0));
        cmd.transform = glm::scale(cmd.transform, glm::vec3(1.5f, 0.5f, 0.5f));
        cmd.material.diffuse = glm::vec3(0.5f, 0.35f, 0.15f);
        cmd.material.useVertexColor = false;
        renderer.submit(cmd);
    }

    // Lamp posts
    for (const auto& lp : m_lampPositions) {
        cmd.mesh = &m_lampPostMesh;
        cmd.transform = glm::translate(glm::mat4(1.0f), lp + glm::vec3(0, 2.0f, 0));
        cmd.material.diffuse = glm::vec3(0.3f, 0.3f, 0.35f);
        cmd.material.useVertexColor = false;
        renderer.submit(cmd);

        cmd.mesh = &m_lampBulbMesh;
        cmd.transform = glm::translate(glm::mat4(1.0f), lp + glm::vec3(0, 4.2f, 0));
        cmd.material.diffuse = glm::vec3(1.0f, 0.95f, 0.7f);
        cmd.material.shininess = 128.0f;
        renderer.submit(cmd);
    }

    // Trees
    for (size_t i = 0; i < m_treePositions.size(); i++) {
        float s = m_treeScales[i];
        // Trunk
        cmd.mesh = &m_treeTrunkMesh;
        cmd.transform = glm::translate(glm::mat4(1.0f), m_treePositions[i] + glm::vec3(0, 1.0f * s, 0));
        cmd.transform = glm::scale(cmd.transform, glm::vec3(s, s, s));
        cmd.material.diffuse = glm::vec3(0.4f, 0.25f, 0.1f);
        cmd.material.shininess = 8.0f;
        cmd.material.useVertexColor = false;
        renderer.submit(cmd);

        // Canopy
        cmd.mesh = &m_treeCanopyMesh;
        cmd.transform = glm::translate(glm::mat4(1.0f), m_treePositions[i] + glm::vec3(0, 3.0f * s, 0));
        cmd.transform = glm::scale(cmd.transform, glm::vec3(s, s * 1.2f, s));
        cmd.material.diffuse = glm::vec3(0.15f + i * 0.005f, 0.5f + i * 0.003f, 0.1f);
        renderer.submit(cmd);
    }

    // Flower beds
    for (const auto& fp : m_flowerPositions) {
        cmd.mesh = &m_flowerBedMesh;
        cmd.transform = glm::translate(glm::mat4(1.0f), fp + glm::vec3(0, 0.15f, 0));
        cmd.material.diffuse = glm::vec3(1.0f);
        cmd.material.useVertexColor = true;
        renderer.submit(cmd);
    }

    // Fountain
    cmd.mesh = &m_fountainBaseMesh;
    cmd.transform = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0.3f, -5.0f));
    cmd.material.diffuse = glm::vec3(1.0f);
    cmd.material.useVertexColor = true;
    cmd.material.shininess = 64.0f;
    renderer.submit(cmd);

    cmd.mesh = &m_fountainPillarMesh;
    cmd.transform = glm::translate(glm::mat4(1.0f), glm::vec3(0, 1.5f, -5.0f));
    cmd.material.diffuse = glm::vec3(1.0f);
    cmd.material.useVertexColor = true;
    renderer.submit(cmd);

    cmd.mesh = &m_fountainWaterMesh;
    cmd.transform = glm::translate(glm::mat4(1.0f), glm::vec3(0, 2.8f, -5.0f));
    cmd.material.diffuse = glm::vec3(1.0f);
    cmd.material.useVertexColor = true;
    cmd.material.specular = glm::vec3(0.8f);
    cmd.material.shininess = 128.0f;
    renderer.submit(cmd);

    // Fountain Fence posts
    cmd.mesh = &m_fencePostMesh;
    cmd.material.diffuse = glm::vec3(0.12f, 0.12f, 0.15f); // Black iron
    cmd.material.specular = glm::vec3(0.3f);
    cmd.material.shininess = 16.0f;
    for (int i = 0; i < 16; ++i) {
        float angle = i * (2.0f * 3.14159265f / 16.0f);
        glm::vec3 pos(3.3f * cosf(angle), 0.45f, -5.0f + 3.3f * sinf(angle));
        cmd.transform = glm::translate(glm::mat4(1.0f), pos);
        renderer.submit(cmd);
    }

    // Trash cans
    cmd.mesh = &m_trashCanMesh;
    cmd.material.diffuse = glm::vec3(0.1f, 0.25f, 0.18f); // Classic park dark green bins
    cmd.material.specular = glm::vec3(0.2f);
    cmd.material.shininess = 8.0f;
    for (const auto& pos : m_trashCanPositions) {
        cmd.transform = glm::translate(glm::mat4(1.0f), pos + glm::vec3(0.0f, 0.4f, 0.0f));
        renderer.submit(cmd);
    }
}

void ParkWorld::setupLights(Renderer& renderer, float totalTime) {
    renderer.clearPointLights();

    // Lamp post lights
    for (const auto& lp : m_lampPositions) {
        Light light;
        light.position = lp + glm::vec3(0, 4.5f, 0);
        light.color = glm::vec3(1.0f, 0.9f, 0.6f);
        light.ambient = 0.05f;
        light.diffuse = 0.6f;
        light.specular = 0.3f;
        light.linear = 0.09f;
        light.quadratic = 0.032f;
        renderer.addPointLight(light);
    }
}

void ParkWorld::buildSky() {
    std::vector<Vertex> v; std::vector<uint32_t> idx;
    float skyRadius = 400.0f;
    ProceduralMeshes::createSphere(v, idx, 32, 16, skyRadius);
    
    // Premium theme-park sunset colors
    glm::vec3 zenithColor(0.04f, 0.08f, 0.28f);  // Deep twilight indigo
    glm::vec3 midColor(0.75f, 0.18f, 0.42f);     // Vivid pink/magenta sunset glow
    glm::vec3 horizonColor(0.98f, 0.55f, 0.15f); // Bright golden orange
    glm::vec3 nadirColor(0.02f, 0.05f, 0.15f);   // Deep ground shadow blue
    
    glm::vec3 sunDir = glm::normalize(glm::vec3(0.3f, 0.8f, 0.5f)); // matches Engine's sun light direction
    
    for (auto& vertex : v) {
        float normalizedY = vertex.position.y / skyRadius; // [-1.0, 1.0]
        
        // Base sky gradient
        glm::vec3 baseColor;
        if (normalizedY >= 0.0f) {
            if (normalizedY < 0.25f) {
                float t = normalizedY / 0.25f;
                baseColor = glm::mix(horizonColor, midColor, t);
            } else {
                float t = (normalizedY - 0.25f) / 0.75f;
                baseColor = glm::mix(midColor, zenithColor, powf(t, 0.7f));
            }
        } else {
            float t = -normalizedY;
            baseColor = glm::mix(horizonColor, nadirColor, powf(t, 0.5f));
        }
        
        // Sun glow & disc
        glm::vec3 normPos = glm::normalize(vertex.position);
        float sunDot = glm::dot(normPos, sunDir);
        
        if (sunDot > 0.0f) {
            float sunGlow = powf(sunDot, 24.0f); // wide atmospheric glow
            float sunDisc = powf(sunDot, 350.0f); // sharp sun disc
            
            baseColor += glm::vec3(1.0f, 0.9f, 0.7f) * (sunGlow * 0.35f + sunDisc * 0.9f);
        }
        
        vertex.color = glm::clamp(baseColor, 0.0f, 1.0f);
    }
    m_skyDomeMesh.upload(v, idx);
}

} // namespace ap3d
