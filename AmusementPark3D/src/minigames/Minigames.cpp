#include "minigames/Minigames.h"
#include "input/InputManager.h"
#include "renderer/ProceduralMeshes.h"
#include "renderer/ParticleSystem.h"

namespace ap3d {

// ==================== Minigame Base ====================
Minigame::Minigame(const std::string& name, const glm::vec3& position)
    : m_name(name), m_position(position) {}

// ==================== Shooting Gallery ====================
ShootingGallery::ShootingGallery(const glm::vec3& position)
    : Minigame("Shooting Gallery", position) {
    m_targetScore = 10;
    m_maxTime = 30.0f;
}

void ShootingGallery::build(PhysicsWorld& physics) {
    std::vector<Vertex> v; std::vector<uint32_t> idx;

    ProceduralMeshes::createSphere(v, idx, 16, 8, 0.4f);
    m_targetMesh.upload(v, idx);

    ProceduralMeshes::createCube(v, idx);
    m_boothMesh.upload(v, idx);

    ProceduralMeshes::createCube(v, idx);
    m_shelfMesh.upload(v, idx);

    // Trigger zone
    Collider trigger;
    trigger.type = ColliderType::AABB;
    trigger.bounds.min = m_position - glm::vec3(3, 0, 2);
    trigger.bounds.max = m_position + glm::vec3(3, 3, 2);
    trigger.isTrigger = true;
    trigger.tag = "minigame_shooting";
    physics.addCollider(trigger);
}

void ShootingGallery::start() {
    Minigame::start();
    for (int i = 0; i < NUM_TARGETS; i++) {
        float x = m_position.x - 2.0f + (i % 3) * 2.0f;
        float y = m_position.y + 2.0f + (i / 3) * 1.5f;
        float z = m_position.z - 3.0f;
        m_targets[i] = {glm::vec3(x, y, z), 0.4f, true, 0.0f, static_cast<float>(i)};
    }
}

void ShootingGallery::update(float deltaTime, const InputManager& input,
    const glm::vec3& camPos, const glm::vec3& camFront, ParticleSystem* particles)
{
    if (m_state != MinigameState::Active) return;

    m_timeLeft -= deltaTime;
    m_hitFlash = std::max(0.0f, m_hitFlash - deltaTime * 4.0f);

    if (m_timeLeft <= 0.0f) {
        m_state = (m_score >= m_targetScore) ? MinigameState::Won : MinigameState::Lost;
        return;
    }

    // Respawn targets
    for (auto& t : m_targets) {
        if (!t.active) {
            t.respawnTimer -= deltaTime;
            if (t.respawnTimer <= 0) t.active = true;
        }
        t.bobPhase += deltaTime * 2.0f;
    }

    // Shoot on click
    if (input.isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
        glm::vec3 rayDir = glm::normalize(camFront);
        for (auto& t : m_targets) {
            if (!t.active) continue;
            glm::vec3 tPos = t.pos + glm::vec3(0, sinf(t.bobPhase) * 0.2f, 0);
            glm::vec3 oc = camPos - tPos;
            float a = glm::dot(rayDir, rayDir);
            float b = 2.0f * glm::dot(oc, rayDir);
            float c = glm::dot(oc, oc) - t.radius * t.radius;
            float disc = b * b - 4 * a * c;
            if (disc >= 0) {
                float hitT = (-b - sqrtf(disc)) / (2.0f * a);
                if (hitT > 0 && hitT < 50.0f) {
                    t.active = false;
                    t.respawnTimer = 1.5f;
                    m_score++;
                    m_hitFlash = 1.0f;
                    if (particles) {
                        particles->emitBurst(tPos, glm::vec4(1.0f, 0.8f, 0.2f, 1.0f), 20, 3.0f);
                    }
                    break;
                }
            }
        }
    }

    if (m_score >= m_targetScore) m_state = MinigameState::Won;
}

void ShootingGallery::render(Renderer& renderer) {
    // Booth back wall
    RenderCommand cmd;
    cmd.mesh = &m_boothMesh;
    cmd.transform = glm::translate(glm::mat4(1.0f), m_position + glm::vec3(0, 2.5f, -4.0f));
    cmd.transform = glm::scale(cmd.transform, glm::vec3(8.0f, 5.0f, 0.3f));
    cmd.material.diffuse = glm::vec3(0.18f, 0.08f, 0.05f); // Dark rich wood
    cmd.material.useVertexColor = false;
    renderer.submit(cmd);

    // Left Pillar
    cmd.transform = glm::translate(glm::mat4(1.0f), m_position + glm::vec3(-3.8f, 2.5f, -1.0f));
    cmd.transform = glm::scale(cmd.transform, glm::vec3(0.4f, 5.0f, 0.4f));
    cmd.material.diffuse = glm::vec3(0.85f, 0.15f, 0.15f); // Red pillar
    renderer.submit(cmd);

    // Right Pillar
    cmd.transform = glm::translate(glm::mat4(1.0f), m_position + glm::vec3(3.8f, 2.5f, -1.0f));
    cmd.transform = glm::scale(cmd.transform, glm::vec3(0.4f, 5.0f, 0.4f));
    renderer.submit(cmd);

    // Front counter table
    cmd.transform = glm::translate(glm::mat4(1.0f), m_position + glm::vec3(0, 0.9f, -1.0f));
    cmd.transform = glm::scale(cmd.transform, glm::vec3(8.0f, 0.1f, 1.2f));
    cmd.material.diffuse = glm::vec3(0.55f, 0.35f, 0.18f); // Wood top
    renderer.submit(cmd);

    // Front counter base
    cmd.transform = glm::translate(glm::mat4(1.0f), m_position + glm::vec3(0, 0.45f, -1.0f));
    cmd.transform = glm::scale(cmd.transform, glm::vec3(7.6f, 0.9f, 1.0f));
    cmd.material.diffuse = glm::vec3(0.15f, 0.15f, 0.2f); // Dark base
    renderer.submit(cmd);

    // Canopy Roof
    cmd.transform = glm::translate(glm::mat4(1.0f), m_position + glm::vec3(0, 5.0f, -2.5f));
    cmd.transform = glm::scale(cmd.transform, glm::vec3(8.2f, 0.3f, 3.5f));
    cmd.material.diffuse = glm::vec3(0.85f, 0.75f, 0.15f); // Yellow roof
    renderer.submit(cmd);

    // Sign on top
    cmd.transform = glm::translate(glm::mat4(1.0f), m_position + glm::vec3(0, 5.6f, -1.0f));
    cmd.transform = glm::scale(cmd.transform, glm::vec3(5.0f, 0.8f, 0.15f));
    cmd.material.diffuse = glm::vec3(0.1f, 0.6f, 0.85f); // Cyan sign
    renderer.submit(cmd);

    // Shelf
    cmd.mesh = &m_shelfMesh;
    cmd.transform = glm::translate(glm::mat4(1.0f), m_position + glm::vec3(0, 1.5f, -3.5f));
    cmd.transform = glm::scale(cmd.transform, glm::vec3(6.0f, 0.1f, 0.5f));
    cmd.material.diffuse = glm::vec3(0.5f, 0.35f, 0.15f);
    renderer.submit(cmd);

    // Upper shelf
    cmd.transform = glm::translate(glm::mat4(1.0f), m_position + glm::vec3(0, 3.0f, -3.5f));
    cmd.transform = glm::scale(cmd.transform, glm::vec3(6.0f, 0.1f, 0.5f));
    renderer.submit(cmd);

    // Targets
    if (m_state == MinigameState::Active) {
        for (const auto& t : m_targets) {
            if (!t.active) continue;
            glm::vec3 tPos = t.pos + glm::vec3(0, sinf(t.bobPhase) * 0.2f, 0);
            cmd.mesh = &m_targetMesh;
            cmd.transform = glm::translate(glm::mat4(1.0f), tPos);
            cmd.material.diffuse = glm::vec3(0.9f, 0.2f + m_hitFlash * 0.5f, 0.1f);
            cmd.material.shininess = 64.0f;
            renderer.submit(cmd);
        }
    }
}

// ==================== Ring Toss ====================
RingToss::RingToss(const glm::vec3& position)
    : Minigame("Ring Toss", position) {
    m_targetScore = 5;
    m_maxTime = 45.0f;
}

void RingToss::build(PhysicsWorld& physics) {
    std::vector<Vertex> v; std::vector<uint32_t> idx;

    ProceduralMeshes::createCylinder(v, idx, 0.15f, 0.8f, 8, true);
    m_pegMesh.upload(v, idx);

    ProceduralMeshes::createTorus(v, idx, 0.35f, 0.06f, 16, 8);
    m_ringMesh.upload(v, idx);

    ProceduralMeshes::createCube(v, idx);
    m_boothMesh.upload(v, idx);
    m_tableMesh.upload(v, idx);

    Collider trigger;
    trigger.type = ColliderType::AABB;
    trigger.bounds.min = m_position - glm::vec3(3, 0, 3);
    trigger.bounds.max = m_position + glm::vec3(3, 3, 3);
    trigger.isTrigger = true;
    trigger.tag = "minigame_ringtoss";
    physics.addCollider(trigger);
}

void RingToss::start() {
    Minigame::start();
    m_ringsLeft = 15;
    m_flyingRings.clear();
    m_throwCharge = 0.0f;
    m_isCharging = false;
    for (int i = 0; i < NUM_PEGS; i++) {
        int row = i / 3, col = i % 3;
        float x = m_position.x - 1.0f + col * 1.0f;
        float z = m_position.z - 3.0f + row * 1.0f;
        m_pegs[i] = {glm::vec3(x, m_position.y + 1.3f, z), 0.15f, false};
    }
}

void RingToss::update(float deltaTime, const InputManager& input,
    const glm::vec3& camPos, const glm::vec3& camFront, ParticleSystem* particles)
{
    if (m_state != MinigameState::Active) return;

    m_timeLeft -= deltaTime;
    m_throwCooldown = std::max(0.0f, m_throwCooldown - deltaTime);

    if (m_timeLeft <= 0.0f || (m_ringsLeft <= 0 && m_flyingRings.empty())) {
        m_state = (m_score >= m_targetScore) ? MinigameState::Won : MinigameState::Lost;
        m_isCharging = false;
        return;
    }

    // Charge and Throw ring
    if (m_throwCooldown <= 0 && m_ringsLeft > 0) {
        if (input.isMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT)) {
            m_isCharging = true;
            m_throwCharge = std::min(1.0f, m_throwCharge + deltaTime * 2.0f); // 0.5s to full charge
        } else if (m_isCharging) {
            // Release to throw
            FlyingRing ring;
            ring.pos = camPos + camFront * 0.5f;
            float power = 5.0f + m_throwCharge * 9.0f; // 5.0f to 14.0f
            ring.velocity = camFront * power + glm::vec3(0.0f, 1.5f + m_throwCharge * 2.5f, 0.0f);
            ring.active = true;
            ring.life = 0.0f;
            m_flyingRings.push_back(ring);
            
            m_isCharging = false;
            m_throwCharge = 0.0f;
            m_throwCooldown = 0.4f;
            m_ringsLeft--;
        }
    } else {
        m_isCharging = false;
        m_throwCharge = 0.0f;
    }

    // Update flying rings
    for (auto it = m_flyingRings.begin(); it != m_flyingRings.end(); ) {
        if (!it->active) { it = m_flyingRings.erase(it); continue; }
        it->velocity.y -= 9.8f * deltaTime;
        it->pos += it->velocity * deltaTime;
        it->life += deltaTime;

        // Check peg collision
        for (auto& peg : m_pegs) {
            if (peg.hasRing) continue;
            float dist2D = glm::distance(glm::vec2(it->pos.x, it->pos.z),
                                          glm::vec2(peg.pos.x, peg.pos.z));
            if (dist2D < 0.4f && it->pos.y < peg.pos.y + 0.5f && it->pos.y > peg.pos.y - 0.3f
                && it->velocity.y < 0) {
                peg.hasRing = true;
                it->active = false;
                m_score++;
                if (particles) {
                    particles->emitBurst(peg.pos + glm::vec3(0, 0.4f, 0), glm::vec4(0.2f, 0.8f, 1.0f, 1.0f), 15, 2.0f);
                }
                break;
            }
        }

        if (it->pos.y < m_position.y - 1.0f || it->life > 5.0f) it->active = false;
        if (!it->active) { it = m_flyingRings.erase(it); } else { ++it; }
    }

    if (m_score >= m_targetScore) m_state = MinigameState::Won;
}

void RingToss::render(Renderer& renderer) {
    RenderCommand cmd;
    cmd.material.useVertexColor = false;

    // Booth back wall
    cmd.mesh = &m_boothMesh;
    cmd.transform = glm::translate(glm::mat4(1.0f), m_position + glm::vec3(0, 2.5f, -5.0f));
    cmd.transform = glm::scale(cmd.transform, glm::vec3(6.0f, 5.0f, 0.3f));
    cmd.material.diffuse = glm::vec3(0.18f, 0.08f, 0.05f); // Dark rich wood
    renderer.submit(cmd);

    // Left Pillar
    cmd.transform = glm::translate(glm::mat4(1.0f), m_position + glm::vec3(-2.8f, 2.5f, -1.0f));
    cmd.transform = glm::scale(cmd.transform, glm::vec3(0.3f, 5.0f, 0.3f));
    cmd.material.diffuse = glm::vec3(0.15f, 0.75f, 0.35f); // Green pillar
    renderer.submit(cmd);

    // Right Pillar
    cmd.transform = glm::translate(glm::mat4(1.0f), m_position + glm::vec3(2.8f, 2.5f, -1.0f));
    cmd.transform = glm::scale(cmd.transform, glm::vec3(0.3f, 5.0f, 0.3f));
    renderer.submit(cmd);

    // Front counter table
    cmd.transform = glm::translate(glm::mat4(1.0f), m_position + glm::vec3(0, 0.9f, -1.0f));
    cmd.transform = glm::scale(cmd.transform, glm::vec3(6.0f, 0.1f, 1.0f));
    cmd.material.diffuse = glm::vec3(0.55f, 0.35f, 0.18f); // Wood top
    renderer.submit(cmd);

    // Front counter base
    cmd.transform = glm::translate(glm::mat4(1.0f), m_position + glm::vec3(0, 0.45f, -1.0f));
    cmd.transform = glm::scale(cmd.transform, glm::vec3(5.6f, 0.9f, 0.8f));
    cmd.material.diffuse = glm::vec3(0.15f, 0.15f, 0.2f); // Dark base
    renderer.submit(cmd);

    // Canopy Roof
    cmd.transform = glm::translate(glm::mat4(1.0f), m_position + glm::vec3(0, 5.0f, -3.0f));
    cmd.transform = glm::scale(cmd.transform, glm::vec3(6.2f, 0.3f, 4.5f));
    cmd.material.diffuse = glm::vec3(0.85f, 0.15f, 0.75f); // Purple roof
    renderer.submit(cmd);

    // Sign on top
    cmd.transform = glm::translate(glm::mat4(1.0f), m_position + glm::vec3(0, 5.6f, -1.0f));
    cmd.transform = glm::scale(cmd.transform, glm::vec3(4.0f, 0.8f, 0.15f));
    cmd.material.diffuse = glm::vec3(0.85f, 0.75f, 0.15f); // Gold sign
    renderer.submit(cmd);

    // Table for pegs
    cmd.mesh = &m_tableMesh;
    cmd.transform = glm::translate(glm::mat4(1.0f), m_position + glm::vec3(0, 0.9f, -2.5f));
    cmd.transform = glm::scale(cmd.transform, glm::vec3(4.0f, 0.1f, 4.0f));
    cmd.material.diffuse = glm::vec3(0.4f, 0.25f, 0.1f);
    renderer.submit(cmd);

    // Pegs
    for (const auto& peg : m_pegs) {
        cmd.mesh = &m_pegMesh;
        cmd.transform = glm::translate(glm::mat4(1.0f), peg.pos + glm::vec3(0, 0.4f, 0));
        cmd.material.diffuse = glm::vec3(0.8f, 0.7f, 0.2f);
        renderer.submit(cmd);

        if (peg.hasRing) {
            cmd.mesh = &m_ringMesh;
            cmd.transform = glm::translate(glm::mat4(1.0f), peg.pos + glm::vec3(0, 0.3f, 0));
            cmd.transform = glm::rotate(cmd.transform, HALF_PI, glm::vec3(1, 0, 0));
            cmd.material.diffuse = glm::vec3(0.1f, 0.6f, 0.9f);
            renderer.submit(cmd);
        }
    }

    // Flying rings
    for (const auto& ring : m_flyingRings) {
        if (!ring.active) continue;
        cmd.mesh = &m_ringMesh;
        cmd.transform = glm::translate(glm::mat4(1.0f), ring.pos);
        cmd.transform = glm::rotate(cmd.transform, ring.life * 5.0f, glm::vec3(1, 0, 0));
        cmd.material.diffuse = glm::vec3(0.1f, 0.8f, 0.3f);
        renderer.submit(cmd);
    }
}

// ==================== Basketball Challenge ====================
BasketballChallenge::BasketballChallenge(const glm::vec3& position)
    : Minigame("Basketball Challenge", position) {
    m_targetScore = 5;
    m_maxTime = 45.0f;
    m_hoopCenter = position + glm::vec3(0, 3.5f, -4.0f);
}

void BasketballChallenge::build(PhysicsWorld& physics) {
    std::vector<Vertex> v; std::vector<uint32_t> idx;

    ProceduralMeshes::createSphere(v, idx, 16, 8, 0.25f);
    m_ballMesh.upload(v, idx);

    ProceduralMeshes::createTorus(v, idx, m_hoopRadius, 0.04f, 24, 8);
    m_hoopMesh.upload(v, idx);

    ProceduralMeshes::createCube(v, idx);
    m_backboardMesh.upload(v, idx);
    m_boothMesh.upload(v, idx);

    ProceduralMeshes::createCylinder(v, idx, 0.1f, 3.5f, 8, true);
    m_poleMesh.upload(v, idx);

    Collider trigger;
    trigger.type = ColliderType::AABB;
    trigger.bounds.min = m_position - glm::vec3(3, 0, 2);
    trigger.bounds.max = m_position + glm::vec3(3, 4, 5);
    trigger.isTrigger = true;
    trigger.tag = "minigame_basketball";
    physics.addCollider(trigger);
}

void BasketballChallenge::start() {
    Minigame::start();
    m_ballsLeft = 20;
    m_balls.clear();
    m_throwCharge = 0.0f;
    m_isCharging = false;
}

void BasketballChallenge::update(float deltaTime, const InputManager& input,
    const glm::vec3& camPos, const glm::vec3& camFront, ParticleSystem* particles)
{
    if (m_state != MinigameState::Active) return;

    m_timeLeft -= deltaTime;
    m_throwCooldown = std::max(0.0f, m_throwCooldown - deltaTime);

    if (m_timeLeft <= 0.0f || (m_ballsLeft <= 0 && m_balls.empty())) {
        m_state = (m_score >= m_targetScore) ? MinigameState::Won : MinigameState::Lost;
        m_isCharging = false;
        return;
    }

    // Charge and Throw basketball
    if (m_throwCooldown <= 0 && m_ballsLeft > 0) {
        if (input.isMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT)) {
            m_isCharging = true;
            m_throwCharge = std::min(1.0f, m_throwCharge + deltaTime * 2.0f); // 0.5s to full charge
        } else if (m_isCharging) {
            // Release to throw
            Ball ball;
            ball.pos = camPos + camFront * 0.5f - glm::vec3(0, 0.2f, 0);
            float power = 6.0f + m_throwCharge * 12.0f; // 6.0f to 18.0f
            ball.velocity = camFront * power + glm::vec3(0, 1.5f + m_throwCharge * 4.5f, 0);
            ball.active = true;
            ball.life = 0.0f;
            m_balls.push_back(ball);
            
            m_isCharging = false;
            m_throwCharge = 0.0f;
            m_throwCooldown = 0.5f;
            m_ballsLeft--;
        }
    } else {
        m_isCharging = false;
        m_throwCharge = 0.0f;
    }

    // Update balls
    for (auto it = m_balls.begin(); it != m_balls.end(); ) {
        if (!it->active) { it = m_balls.erase(it); continue; }
        it->velocity.y -= 9.8f * deltaTime;
        it->pos += it->velocity * deltaTime;
        it->life += deltaTime;

        // Check hoop scoring
        float dist2D = glm::distance(glm::vec2(it->pos.x, it->pos.z),
                                      glm::vec2(m_hoopCenter.x, m_hoopCenter.z));
        if (dist2D < m_hoopRadius * 0.6f &&
            fabsf(it->pos.y - m_hoopCenter.y) < 0.3f && it->velocity.y < 0) {
            m_score++;
            it->active = false;
            if (particles) {
                particles->emitBurst(m_hoopCenter, glm::vec4(1.0f, 0.3f, 0.1f, 1.0f), 25, 4.0f);
            }
        }

        // Floor bounce / remove
        if (it->pos.y < m_position.y) {
            it->velocity.y = -it->velocity.y * 0.4f;
            it->pos.y = m_position.y + 0.25f;
            if (fabsf(it->velocity.y) < 0.5f) it->active = false;
        }

        if (it->life > 5.0f) it->active = false;
        if (!it->active) { it = m_balls.erase(it); } else { ++it; }
    }

    if (m_score >= m_targetScore) m_state = MinigameState::Won;
}

void BasketballChallenge::render(Renderer& renderer) {
    RenderCommand cmd;
    cmd.material.useVertexColor = false;

    // Booth back wall
    cmd.mesh = &m_boothMesh;
    cmd.transform = glm::translate(glm::mat4(1.0f), m_position + glm::vec3(0, 2.5f, -5.0f));
    cmd.transform = glm::scale(cmd.transform, glm::vec3(6.0f, 5.0f, 0.3f));
    cmd.material.diffuse = glm::vec3(0.18f, 0.08f, 0.05f); // Dark rich wood
    renderer.submit(cmd);

    // Left side net fence
    cmd.transform = glm::translate(glm::mat4(1.0f), m_position + glm::vec3(-3.0f, 2.0f, -2.5f));
    cmd.transform = glm::scale(cmd.transform, glm::vec3(0.1f, 4.0f, 5.0f));
    cmd.material.diffuse = glm::vec3(0.15f, 0.15f, 0.2f); // Dark metal grid
    renderer.submit(cmd);

    // Right side net fence
    cmd.transform = glm::translate(glm::mat4(1.0f), m_position + glm::vec3(3.0f, 2.0f, -2.5f));
    cmd.transform = glm::scale(cmd.transform, glm::vec3(0.1f, 4.0f, 5.0f));
    renderer.submit(cmd);

    // Front counter table
    cmd.transform = glm::translate(glm::mat4(1.0f), m_position + glm::vec3(0, 0.9f, 0.0f));
    cmd.transform = glm::scale(cmd.transform, glm::vec3(6.0f, 0.1f, 0.8f));
    cmd.material.diffuse = glm::vec3(0.55f, 0.35f, 0.18f); // Wood top
    renderer.submit(cmd);

    // Front counter base
    cmd.transform = glm::translate(glm::mat4(1.0f), m_position + glm::vec3(0, 0.45f, 0.0f));
    cmd.transform = glm::scale(cmd.transform, glm::vec3(5.6f, 0.9f, 0.6f));
    cmd.material.diffuse = glm::vec3(0.15f, 0.15f, 0.2f); // Dark base
    renderer.submit(cmd);

    // Sign on top
    cmd.transform = glm::translate(glm::mat4(1.0f), m_position + glm::vec3(0, 5.6f, -2.5f));
    cmd.transform = glm::scale(cmd.transform, glm::vec3(5.0f, 0.8f, 0.15f));
    cmd.material.diffuse = glm::vec3(0.85f, 0.3f, 0.1f); // Vibrant orange sign
    renderer.submit(cmd);

    // Backboard
    cmd.mesh = &m_backboardMesh;
    cmd.transform = glm::translate(glm::mat4(1.0f), m_hoopCenter + glm::vec3(0, 0.5f, -0.3f));
    cmd.transform = glm::scale(cmd.transform, glm::vec3(1.5f, 1.2f, 0.1f));
    cmd.material.diffuse = glm::vec3(0.9f, 0.9f, 0.95f);
    renderer.submit(cmd);

    // Pole
    cmd.mesh = &m_poleMesh;
    cmd.transform = glm::translate(glm::mat4(1.0f),
        glm::vec3(m_hoopCenter.x, m_position.y + 1.75f, m_hoopCenter.z - 0.4f));
    cmd.material.diffuse = glm::vec3(0.5f, 0.5f, 0.55f);
    renderer.submit(cmd);

    // Hoop
    cmd.mesh = &m_hoopMesh;
    cmd.transform = glm::translate(glm::mat4(1.0f), m_hoopCenter);
    cmd.transform = glm::rotate(cmd.transform, HALF_PI, glm::vec3(1, 0, 0));
    cmd.material.diffuse = glm::vec3(0.9f, 0.3f, 0.1f);
    renderer.submit(cmd);

    // Balls
    for (const auto& ball : m_balls) {
        if (!ball.active) continue;
        cmd.mesh = &m_ballMesh;
        cmd.transform = glm::translate(glm::mat4(1.0f), ball.pos);
        cmd.material.diffuse = glm::vec3(0.9f, 0.5f, 0.1f);
        cmd.material.shininess = 64.0f;
        renderer.submit(cmd);
    }
}

} // namespace ap3d
