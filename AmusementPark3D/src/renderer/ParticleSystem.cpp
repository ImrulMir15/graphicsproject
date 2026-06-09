#include "renderer/ParticleSystem.h"
#include "renderer/ProceduralMeshes.h"

namespace ap3d {

void ParticleSystem::init() {
    std::vector<Vertex> v;
    std::vector<uint32_t> idx;
    ProceduralMeshes::createCube(v, idx);
    // Color it white initially so it can be multiplied by the material diffuse color
    ProceduralMeshes::colorVertices(v, glm::vec3(1.0f));
    m_particleMesh.upload(v, idx);
}

void ParticleSystem::update(float dt) {
    for (auto it = m_particles.begin(); it != m_particles.end(); ) {
        it->lifetime -= dt;
        if (it->lifetime <= 0.0f) {
            it = m_particles.erase(it);
        } else {
            // Apply physics: movement & gravity
            it->position += it->velocity * dt;
            it->velocity.y -= 4.9f * dt; // Gravity
            ++it;
        }
    }
}

void ParticleSystem::render(Renderer& renderer) {
    RenderCommand cmd;
    cmd.mesh = &m_particleMesh;
    cmd.material.useVertexColor = false;
    cmd.material.diffuseMap = nullptr;
    cmd.material.specularMap = nullptr;
    cmd.material.shininess = 32.0f;

    for (const auto& p : m_particles) {
        float lifeRatio = p.lifetime / p.maxLifetime;
        float currentSize = p.size * lifeRatio;

        cmd.transform = glm::translate(glm::mat4(1.0f), p.position);
        cmd.transform = glm::scale(cmd.transform, glm::vec3(currentSize));
        
        // Color fades out with life
        cmd.material.diffuse = glm::vec3(p.color) * lifeRatio;
        cmd.material.ambient = cmd.material.diffuse * 0.5f;

        renderer.submit(cmd);
    }
}

void ParticleSystem::emit(const glm::vec3& position, const glm::vec3& velocity, const glm::vec4& color, float size, float lifetime) {
    // Cap particle count to prevent performance issues
    if (m_particles.size() >= 1000) return;

    Particle p;
    p.position = position;
    p.velocity = velocity;
    p.color = color;
    p.size = size;
    p.lifetime = lifetime;
    p.maxLifetime = lifetime;
    m_particles.push_back(p);
}

void ParticleSystem::emitBurst(const glm::vec3& position, const glm::vec4& color, int count, float speed) {
    for (int i = 0; i < count; i++) {
        // Distribute velocity randomly on a hemisphere/sphere
        float theta = static_cast<float>(rand()) / RAND_MAX * 2.0f * 3.14159f;
        float phi = static_cast<float>(rand()) / RAND_MAX * 3.14159f;
        
        glm::vec3 velocity(
            sinf(phi) * cosf(theta) * speed,
            fabsf(cosf(phi)) * speed * 1.5f + 1.0f, // Upward bias
            sinf(phi) * sinf(theta) * speed
        );

        float size = 0.08f + static_cast<float>(rand()) / RAND_MAX * 0.12f;
        float lifetime = 0.5f + static_cast<float>(rand()) / RAND_MAX * 1.0f;

        emit(position, velocity, color, size, lifetime);
    }
}

} // namespace ap3d
