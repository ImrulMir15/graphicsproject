#pragma once
#include "core/Common.h"
#include "renderer/Mesh.h"
#include "renderer/Renderer.h"

namespace ap3d {

struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec4 color;
    float size;
    float lifetime;
    float maxLifetime;
};

class ParticleSystem {
public:
    ParticleSystem() = default;
    ~ParticleSystem() = default;

    void init();
    void update(float dt);
    void render(Renderer& renderer);

    void emit(const glm::vec3& position, const glm::vec3& velocity, const glm::vec4& color, float size, float lifetime);
    void emitBurst(const glm::vec3& position, const glm::vec4& color, int count, float speed);

    void clear() { m_particles.clear(); }

private:
    std::vector<Particle> m_particles;
    Mesh m_particleMesh;
};

} // namespace ap3d
