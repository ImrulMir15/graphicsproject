#include "rides/Rides.h"
#include "renderer/ProceduralMeshes.h"

namespace ap3d {

// ==================== Ride Base ====================

Ride::Ride(const std::string& name, const glm::vec3& position)
    : m_name(name), m_position(position) {}

void Ride::submitMesh(Renderer& renderer, const Mesh& mesh, const glm::mat4& transform,
    const glm::vec3& color, float shininess) {
    RenderCommand cmd;
    cmd.mesh = &mesh;
    cmd.transform = transform;
    cmd.material.diffuse = color;
    cmd.material.ambient = color * 0.2f;
    cmd.material.specular = glm::vec3(0.3f);
    cmd.material.shininess = shininess;
    cmd.material.useVertexColor = false;
    renderer.submit(cmd);
}

// ==================== Ferris Wheel ====================

FerrisWheel::FerrisWheel(const glm::vec3& position) : Ride("Ferris Wheel", position) {}

void FerrisWheel::build(PhysicsWorld& physics) {
    std::vector<Vertex> v; std::vector<uint32_t> idx;

    // Support pillars (two A-frame supports)
    ProceduralMeshes::createCube(v, idx);
    m_supportMesh.upload(v, idx);

    // Wheel rim (torus)
    ProceduralMeshes::createTorus(v, idx, WHEEL_RADIUS, 0.4f, 48, 12);
    m_rimMesh.upload(v, idx);

    // Spoke
    ProceduralMeshes::createCylinder(v, idx, 0.1f, WHEEL_RADIUS * 2.0f, 8, true);
    m_spokeMesh.upload(v, idx);

    // Gondola (box)
    ProceduralMeshes::createCube(v, idx);
    m_gondolaMesh.upload(v, idx);

    // Hub (center)
    ProceduralMeshes::createCylinder(v, idx, 0.8f, 1.5f, 16, true);
    m_hubMesh.upload(v, idx);

    // Base platform
    ProceduralMeshes::createCylinder(v, idx, 3.0f, 0.3f, 24, true);
    m_baseMesh.upload(v, idx);

    // Collider
    Collider col;
    col.type = ColliderType::Cylinder;
    col.center = m_position;
    col.radius = WHEEL_RADIUS + 2.0f;
    col.height = WHEEL_RADIUS * 2.0f + 5.0f;
    col.tag = "ferris_wheel";
    physics.addCollider(col);

    // Trigger zone
    Collider trigger;
    trigger.type = ColliderType::Cylinder;
    trigger.center = m_position + glm::vec3(WHEEL_RADIUS + 3.0f, 1.0f, 0.0f);
    trigger.radius = 2.5f;
    trigger.height = 3.0f;
    trigger.isTrigger = true;
    trigger.tag = "ride_ferris_wheel";
    physics.addCollider(trigger);
}

void FerrisWheel::update(float deltaTime, float totalTime) {
    m_animTime = totalTime;
    if (m_running) {
        m_rotation += ROTATION_SPEED * deltaTime;
        if (m_rotation > TWO_PI) m_rotation -= TWO_PI;
    }
}

void FerrisWheel::render(Renderer& renderer) {
    glm::vec3 p = m_position;
    float hubY = p.y + WHEEL_RADIUS + 4.0f;

    // Base platform
    glm::mat4 base = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y + 0.15f, p.z));
    submitMesh(renderer, m_baseMesh, base, glm::vec3(0.4f, 0.4f, 0.45f));

    // Left support
    glm::mat4 ls = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y + hubY * 0.5f, p.z - 1.5f));
    ls = glm::scale(ls, glm::vec3(1.2f, hubY, 1.0f));
    ls = glm::rotate(ls, 0.05f, glm::vec3(0, 0, 1));
    submitMesh(renderer, m_supportMesh, ls, glm::vec3(0.12f, 0.22f, 0.38f));

    // Right support
    glm::mat4 rs = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y + hubY * 0.5f, p.z + 1.5f));
    rs = glm::scale(rs, glm::vec3(1.2f, hubY, 1.0f));
    rs = glm::rotate(rs, -0.05f, glm::vec3(0, 0, 1));
    submitMesh(renderer, m_supportMesh, rs, glm::vec3(0.12f, 0.22f, 0.38f));

    // Hub
    glm::mat4 hub = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, hubY, p.z));
    hub = glm::rotate(hub, HALF_PI, glm::vec3(1, 0, 0));
    submitMesh(renderer, m_hubMesh, hub, glm::vec3(0.8f, 0.7f, 0.5f)); // Brass hub

    // Wheel rim
    glm::mat4 rim = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, hubY, p.z));
    rim = glm::rotate(rim, m_rotation, glm::vec3(0, 0, 1));
    rim = glm::rotate(rim, HALF_PI, glm::vec3(1, 0, 0));
    submitMesh(renderer, m_rimMesh, rim, glm::vec3(0.88f, 0.88f, 0.86f)); // Off-white steel

    // Spokes
    for (int i = 0; i < 6; i++) {
        float angle = m_rotation + PI * i / 6.0f;
        glm::mat4 spoke = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, hubY, p.z));
        spoke = glm::rotate(spoke, angle, glm::vec3(0, 0, 1));
        submitMesh(renderer, m_spokeMesh, spoke, glm::vec3(0.75f, 0.75f, 0.78f));
    }

    // Gondolas
    for (int i = 0; i < NUM_GONDOLAS; i++) {
        float angle = m_rotation + TWO_PI * i / NUM_GONDOLAS;
        float gx = p.x + cosf(angle) * WHEEL_RADIUS;
        float gy = hubY + sinf(angle) * WHEEL_RADIUS;
        glm::mat4 gondola = glm::translate(glm::mat4(1.0f), glm::vec3(gx, gy, p.z));
        gondola = glm::scale(gondola, glm::vec3(1.2f, 1.5f, 1.2f));
        glm::vec3 color = (i % 3 == 0) ? glm::vec3(0.6f, 0.15f, 0.18f) : // Crimson
                          (i % 3 == 1) ? glm::vec3(0.12f, 0.35f, 0.22f) : // Forest Green
                                         glm::vec3(0.15f, 0.3f, 0.55f); // Royal Blue
        submitMesh(renderer, m_gondolaMesh, gondola, color);
    }

    // Blinking lights around the rim (24 bulbs)
    for (int i = 0; i < 24; i++) {
        float angle = m_rotation + TWO_PI * i / 24.0f;
        float bx = p.x + cosf(angle) * WHEEL_RADIUS;
        float by = hubY + sinf(angle) * WHEEL_RADIUS;
        
        // Slightly offset outward in Z so they display clearly on the rim edge
        glm::mat4 bulb = glm::translate(glm::mat4(1.0f), glm::vec3(bx, by, p.z + 0.45f));
        bulb = glm::scale(bulb, glm::vec3(0.2f, 0.2f, 0.2f));

        glm::vec3 lightColor;
        int pattern = (i + int(m_animTime * 4.0f)) % 4;
        if (pattern == 0)      lightColor = glm::vec3(0.95f, 0.15f, 0.15f); // Red
        else if (pattern == 1) lightColor = glm::vec3(0.98f, 0.85f, 0.05f); // Yellow
        else if (pattern == 2) lightColor = glm::vec3(0.15f, 0.85f, 0.15f); // Green
        else                   lightColor = glm::vec3(0.15f, 0.55f, 0.95f); // Blue

        submitMesh(renderer, m_gondolaMesh, bulb, lightColor, 128.0f); // High shininess to look glowing
        
        // Backside bulb
        glm::mat4 bulbBack = glm::translate(glm::mat4(1.0f), glm::vec3(bx, by, p.z - 0.45f));
        bulbBack = glm::scale(bulbBack, glm::vec3(0.2f, 0.2f, 0.2f));
        submitMesh(renderer, m_gondolaMesh, bulbBack, lightColor, 128.0f);
    }
}

// ==================== Roller Coaster ====================

RollerCoaster::RollerCoaster(const glm::vec3& position) : Ride("Roller Coaster", position) {}

void RollerCoaster::generateTrack() {
    m_trackPoints.clear();
    int numPoints = 200;
    for (int i = 0; i <= numPoints; i++) {
        float t = static_cast<float>(i) / numPoints;
        float angle = t * TWO_PI * 2.0f; // 2.0 full loops so start and end meet perfectly
        float r = 12.0f + 4.0f * sinf(angle * 2.0f); // smooth periodic radius
        float x = m_position.x + r * cosf(angle);
        float z = m_position.z + r * sinf(angle);
        // Height formula that matches perfectly at t=0 and t=1 in both value and derivative (slope)
        float y = m_position.y + 3.0f + 6.0f * sinf(t * TWO_PI * 2.0f) * (1.0f + 0.25f * cosf(t * TWO_PI * 4.0f));
        if (y < m_position.y + 0.8f) y = m_position.y + 0.8f;

        TrackPoint tp;
        tp.position = glm::vec3(x, y, z);
        m_trackPoints.push_back(tp);
    }
    // Compute tangents
    for (size_t i = 0; i < m_trackPoints.size(); i++) {
        size_t next = (i + 1) % m_trackPoints.size();
        m_trackPoints[i].tangent = glm::normalize(m_trackPoints[next].position - m_trackPoints[i].position);
    }
}

void RollerCoaster::build(PhysicsWorld& physics) {
    generateTrack();

    std::vector<Vertex> allVerts;
    std::vector<uint32_t> allIdx;

    auto addBox = [&](const glm::vec3& p0, const glm::vec3& p1,
                      const glm::vec3& r0, const glm::vec3& u0,
                      const glm::vec3& r1, const glm::vec3& u1,
                      float w, float h, const glm::vec3& color) {
        glm::vec3 c0 = p0 - r0 * w - u0 * h;
        glm::vec3 c1 = p0 + r0 * w - u0 * h;
        glm::vec3 c2 = p0 + r0 * w + u0 * h;
        glm::vec3 c3 = p0 - r0 * w + u0 * h;

        glm::vec3 c4 = p1 - r1 * w - u1 * h;
        glm::vec3 c5 = p1 + r1 * w - u1 * h;
        glm::vec3 c6 = p1 + r1 * w + u1 * h;
        glm::vec3 c7 = p1 - r1 * w + u1 * h;

        auto addQuad = [&](const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, const glm::vec3& d) {
            uint32_t base = static_cast<uint32_t>(allVerts.size());
            glm::vec3 normal = glm::normalize(glm::cross(c - a, b - a));
            allVerts.push_back({a, normal, {0, 0}, color});
            allVerts.push_back({b, normal, {1, 0}, color});
            allVerts.push_back({c, normal, {1, 1}, color});
            allVerts.push_back({d, normal, {0, 1}, color});
            allIdx.insert(allIdx.end(), {base, base+1, base+2, base, base+2, base+3});
        };

        addQuad(c0, c1, c5, c4); // bottom
        addQuad(c1, c2, c6, c5); // right
        addQuad(c2, c3, c7, c6); // top
        addQuad(c3, c0, c4, c7); // left
    };

    glm::vec3 railColor(0.85f, 0.15f, 0.15f); // Red rails
    glm::vec3 tieColor(0.22f, 0.22f, 0.25f);  // Dark steel cross-ties
    glm::vec3 spineColor(0.45f, 0.45f, 0.48f); // Steel grey spine

    for (size_t i = 0; i < m_trackPoints.size() - 1; i++) {
        size_t next = (i + 1) % m_trackPoints.size();
        const auto& p0 = m_trackPoints[i];
        const auto& p1 = m_trackPoints[next];

        glm::vec3 up(0, 1, 0);
        glm::vec3 r0 = glm::normalize(glm::cross(p0.tangent, up));
        glm::vec3 u0 = glm::normalize(glm::cross(r0, p0.tangent));
        glm::vec3 r1 = glm::normalize(glm::cross(p1.tangent, up));
        glm::vec3 u1 = glm::normalize(glm::cross(r1, p1.tangent));

        // 1. Left Rail (tube)
        addBox(p0.position - r0 * 0.6f, p1.position - r1 * 0.6f, r0, u0, r1, u1, 0.08f, 0.08f, railColor);

        // 2. Right Rail (tube)
        addBox(p0.position + r0 * 0.6f, p1.position + r1 * 0.6f, r0, u0, r1, u1, 0.08f, 0.08f, railColor);

        // 3. Central Spine (thicker tube underneath)
        addBox(p0.position - u0 * 0.35f, p1.position - u1 * 0.35f, r0, u0, r1, u1, 0.15f, 0.12f, spineColor);

        // 4. Cross ties (sleepers) connecting rails to the spine
        if (i % 2 == 0) {
            glm::vec3 tieCenter = p0.position - u0 * 0.1f;
            glm::vec3 tieL = tieCenter - r0 * 0.65f;
            glm::vec3 tieR = tieCenter + r0 * 0.65f;
            addBox(tieL, tieR, p0.tangent, u0, p0.tangent, u0, 0.05f, 0.04f, tieColor);

            // Connect sleeper to spine
            glm::vec3 connTop = tieCenter;
            glm::vec3 connBot = p0.position - u0 * 0.35f;
            addBox(connTop, connBot, r0, p0.tangent, r0, p0.tangent, 0.05f, 0.05f, tieColor);
        }
    }

    m_trackMesh.upload(allVerts, allIdx);

    // Support pillars
    std::vector<Vertex> sv; std::vector<uint32_t> si;
    ProceduralMeshes::createCylinder(sv, si, 0.15f, 1.0f, 8, true);
    m_supportMesh.upload(sv, si);

    // Car
    ProceduralMeshes::createCube(sv, si);
    m_carMesh.upload(sv, si);

    // Colliders along the track
    for (size_t i = 0; i < m_trackPoints.size(); i += 10) {
        Collider col;
        col.type = ColliderType::Cylinder;
        col.center = m_trackPoints[i].position;
        col.radius = 2.0f;
        col.height = 4.0f;
        col.tag = "roller_coaster";
        physics.addCollider(col);
    }
}

void RollerCoaster::update(float deltaTime, float /*totalTime*/) {
    if (m_running) {
        m_carT += CAR_SPEED * deltaTime;
        if (m_carT > 1.0f) m_carT -= 1.0f;
    }
}

void RollerCoaster::render(Renderer& renderer) {
    // Track
    RenderCommand trackCmd;
    trackCmd.mesh = &m_trackMesh;
    trackCmd.transform = glm::mat4(1.0f);
    trackCmd.material.diffuse = glm::vec3(1.0f); // Multiplier for vertex colors
    trackCmd.material.specular = glm::vec3(0.5f);
    trackCmd.material.shininess = 64.0f;
    trackCmd.material.useVertexColor = true; // Enable vertex coloring for rails/ties/spine
    renderer.submit(trackCmd);

    // Support pillars (every 15th point to reduce clutter)
    for (size_t i = 0; i < m_trackPoints.size(); i += 15) {
        float h = m_trackPoints[i].position.y - m_position.y;
        if (h > 1.0f) {
            // Steel pillar
            glm::mat4 sup = glm::translate(glm::mat4(1.0f),
                glm::vec3(m_trackPoints[i].position.x, m_position.y + h * 0.5f, m_trackPoints[i].position.z));
            sup = glm::scale(sup, glm::vec3(0.25f, h, 0.25f)); // Even thinner for realistic steel supports
            submitMesh(renderer, m_supportMesh, sup, glm::vec3(0.22f, 0.22f, 0.25f), 32.0f); // Metallic grey

            // Concrete base block
            glm::mat4 base = glm::translate(glm::mat4(1.0f),
                glm::vec3(m_trackPoints[i].position.x, m_position.y + 0.25f, m_trackPoints[i].position.z));
            base = glm::scale(base, glm::vec3(0.8f, 0.5f, 0.8f));
            submitMesh(renderer, m_carMesh, base, glm::vec3(0.55f, 0.55f, 0.57f), 8.0f); // Concrete grey base
        }
    }

    // Car along track
    if (!m_trackPoints.empty()) {
        size_t idx = static_cast<size_t>(m_carT * (m_trackPoints.size() - 1));
        idx = std::min(idx, m_trackPoints.size() - 1);
        const auto& tp = m_trackPoints[idx];
        
        // Calculate tangent-based orientation matrix (pitch, yaw, roll)
        glm::vec3 forward = tp.tangent;
        glm::vec3 up(0.0f, 1.0f, 0.0f);
        glm::vec3 right = glm::normalize(glm::cross(forward, up));
        if (glm::length(right) < 0.01f) right = glm::vec3(1, 0, 0);
        glm::vec3 actualUp = glm::normalize(glm::cross(right, forward));
        
        glm::mat4 rot(1.0f);
        rot[0] = glm::vec4(forward, 0.0f);
        rot[1] = glm::vec4(actualUp, 0.0f);
        rot[2] = glm::vec4(right, 0.0f);

        glm::mat4 car = glm::translate(glm::mat4(1.0f), tp.position) * rot;
        car = glm::scale(car, glm::vec3(2.0f, 1.0f, 1.2f));
        submitMesh(renderer, m_carMesh, car, glm::vec3(0.92f, 0.7f, 0.08f), 64.0f); // Bright yellow coaster car
    }
}

// ==================== Carousel ====================

Carousel::Carousel(const glm::vec3& position) : Ride("Carousel", position) {}

void Carousel::build(PhysicsWorld& physics) {
    std::vector<Vertex> v; std::vector<uint32_t> idx;

    ProceduralMeshes::createCylinder(v, idx, PLATFORM_RADIUS, 0.4f, 32, true);
    m_platformMesh.upload(v, idx);

    ProceduralMeshes::createCone(v, idx, PLATFORM_RADIUS + 0.5f, 3.0f, 32);
    m_roofMesh.upload(v, idx);

    ProceduralMeshes::createCylinder(v, idx, 0.08f, 3.5f, 6, true);
    m_poleMesh.upload(v, idx);

    // Horse shape (elongated cube for simplicity)
    ProceduralMeshes::createCube(v, idx);
    m_horseMesh.upload(v, idx);

    ProceduralMeshes::createCylinder(v, idx, 0.3f, 4.5f, 12, true);
    m_centerPoleMesh.upload(v, idx);

    Collider col;
    col.type = ColliderType::Cylinder;
    col.center = m_position + glm::vec3(0, 2, 0);
    col.radius = PLATFORM_RADIUS + 1.0f;
    col.height = 5.0f;
    col.tag = "carousel";
    physics.addCollider(col);

    Collider trigger;
    trigger.type = ColliderType::Cylinder;
    trigger.center = m_position + glm::vec3(PLATFORM_RADIUS + 2.5f, 1, 0);
    trigger.radius = 2.0f;
    trigger.height = 3.0f;
    trigger.isTrigger = true;
    trigger.tag = "ride_carousel";
    physics.addCollider(trigger);
}

void Carousel::update(float deltaTime, float /*totalTime*/) {
    if (m_running) {
        m_rotation += ROTATION_SPEED * deltaTime;
        if (m_rotation > TWO_PI) m_rotation -= TWO_PI;
    }
}

void Carousel::render(Renderer& renderer) {
    glm::vec3 p = m_position;

    // Platform
    glm::mat4 plat = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y + 0.5f, p.z));
    plat = glm::rotate(plat, m_rotation, glm::vec3(0, 1, 0));
    submitMesh(renderer, m_platformMesh, plat, glm::vec3(0.35f, 0.2f, 0.12f), 32.0f); // Polished mahogany wood

    // Center pole
    glm::mat4 cp = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y + 2.75f, p.z));
    submitMesh(renderer, m_centerPoleMesh, cp, glm::vec3(0.88f, 0.72f, 0.25f), 128.0f); // Polished brass/gold

    // Roof
    glm::mat4 roof = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y + 5.5f, p.z));
    roof = glm::rotate(roof, m_rotation * 0.2f, glm::vec3(0, 1, 0));
    submitMesh(renderer, m_roofMesh, roof, glm::vec3(0.6f, 0.12f, 0.15f), 16.0f); // Royal Crimson

    // Horses and poles
    for (int i = 0; i < NUM_HORSES; i++) {
        float angle = m_rotation + TWO_PI * i / NUM_HORSES;
        float r = PLATFORM_RADIUS * 0.7f;
        float hx = p.x + cosf(angle) * r;
        float hz = p.z + sinf(angle) * r;
        float bobY = sinf(m_rotation * 3.0f + i * 1.5f) * 0.3f;

        // Pole
        glm::mat4 pole = glm::translate(glm::mat4(1.0f), glm::vec3(hx, p.y + 2.5f + bobY, hz));
        submitMesh(renderer, m_poleMesh, pole, glm::vec3(0.85f, 0.7f, 0.2f), 96.0f); // Shiny brass poles

        // Horse
        glm::mat4 horse = glm::translate(glm::mat4(1.0f), glm::vec3(hx, p.y + 1.5f + bobY, hz));
        horse = glm::rotate(horse, angle + HALF_PI, glm::vec3(0, 1, 0));
        horse = glm::scale(horse, glm::vec3(0.5f, 0.8f, 1.2f));
        glm::vec3 hColor = (i % 2 == 0) ? glm::vec3(0.92f, 0.9f, 0.85f) : glm::vec3(0.45f, 0.25f, 0.15f); // Ivory or Chestnut
        submitMesh(renderer, m_horseMesh, horse, hColor, 8.0f);
    }
}

// ==================== Swing Ride ====================

SwingRide::SwingRide(const glm::vec3& position) : Ride("Swing Ride", position) {}

void SwingRide::build(PhysicsWorld& physics) {
    std::vector<Vertex> v; std::vector<uint32_t> idx;

    // Central tower
    ProceduralMeshes::createCylinder(v, idx, 0.5f, 8.0f, 12, true);
    m_towerMesh.upload(v, idx);

    // Top rotating disc
    ProceduralMeshes::createCylinder(v, idx, TOP_RADIUS + 1.0f, 0.5f, 24, true);
    m_topMesh.upload(v, idx);

    // Chain (short thick cylinder segment used per chain link)
    ProceduralMeshes::createCylinder(v, idx, 0.04f, 1.0f, 6, true);
    m_chainMesh.upload(v, idx);

    // Seat (a small box)
    ProceduralMeshes::createCube(v, idx);
    m_seatMesh.upload(v, idx);

    Collider col;
    col.type = ColliderType::Cylinder;
    col.center = m_position + glm::vec3(0, 5, 0);
    col.radius = TOP_RADIUS + 5.0f;
    col.height = 12.0f;
    col.tag = "swing_ride";
    physics.addCollider(col);
}

void SwingRide::update(float deltaTime, float /*totalTime*/) {
    if (m_running) {
        m_rotation += ROTATION_SPEED * deltaTime;
        if (m_rotation > TWO_PI) m_rotation -= TWO_PI;
        m_swingAngle = glm::mix(m_swingAngle, 0.45f, deltaTime * 2.0f);
    } else {
        m_swingAngle = glm::mix(m_swingAngle, 0.0f, deltaTime * 2.0f);
    }
}

void SwingRide::render(Renderer& renderer) {
    glm::vec3 p = m_position;
    float topY = p.y + 8.5f;

    // Tower
    glm::mat4 tower = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y + 4.0f, p.z));
    submitMesh(renderer, m_towerMesh, tower, glm::vec3(0.9f, 0.85f, 0.75f), 32.0f); // Warm cream tower

    // Top disc (rotates)
    glm::mat4 top = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, topY, p.z));
    top = glm::rotate(top, m_rotation, glm::vec3(0, 1, 0));
    submitMesh(renderer, m_topMesh, top, glm::vec3(0.15f, 0.45f, 0.45f), 64.0f); // Sleek copper teal

    // Swings: chains + seats
    float chainLength = 3.5f;
    for (int i = 0; i < NUM_SWINGS; i++) {
        float angle = m_rotation + TWO_PI * i / NUM_SWINGS;

        // Attachment point on the rotating disc edge
        float attachX = p.x + cosf(angle) * TOP_RADIUS;
        float attachZ = p.z + sinf(angle) * TOP_RADIUS;

        // Radial outward direction
        float outX = cosf(angle);
        float outZ = sinf(angle);

        // Seat position: hang down chainLength, swing outward by swingAngle
        float swingOffset = sinf(m_swingAngle) * chainLength;
        float seatX = attachX + outX * swingOffset;
        float seatZ = attachZ + outZ * swingOffset;
        float seatY = topY - cosf(m_swingAngle) * chainLength;

        // Draw chain segments from attachment to seat
        int chainSegments = 4;
        for (int c = 0; c < chainSegments; c++) {
            float t0 = static_cast<float>(c) / chainSegments;
            float t1 = static_cast<float>(c + 1) / chainSegments;
            float cx0 = attachX + (seatX - attachX) * t0;
            float cy0 = topY + (seatY - topY) * t0;
            float cz0 = attachZ + (seatZ - attachZ) * t0;
            float cx1 = attachX + (seatX - attachX) * t1;
            float cy1 = topY + (seatY - topY) * t1;
            float cz1 = attachZ + (seatZ - attachZ) * t1;

            // Position chain segment at midpoint, orient along direction
            glm::vec3 segMid((cx0+cx1)*0.5f, (cy0+cy1)*0.5f, (cz0+cz1)*0.5f);
            glm::vec3 segDir = glm::normalize(glm::vec3(cx1-cx0, cy1-cy0, cz1-cz0));
            float segLen = glm::distance(glm::vec3(cx0,cy0,cz0), glm::vec3(cx1,cy1,cz1));

            // Align Y-axis along segDir using Rodrigues' rotation formula
            glm::vec3 startDir(0.0f, 1.0f, 0.0f);
            glm::mat4 rot(1.0f);
            float d = glm::dot(startDir, segDir);
            if (d < 0.999f && d > -0.999f) {
                glm::vec3 cross = glm::cross(startDir, segDir);
                float s = glm::length(cross);
                glm::mat3 kskew(
                    0.0f, -cross.z, cross.y,
                    cross.z, 0.0f, -cross.x,
                    -cross.y, cross.x, 0.0f
                );
                glm::mat3 rot3 = glm::mat3(1.0f) + kskew + kskew * kskew * ((1.0f - d) / (s * s));
                rot = glm::mat4(rot3);
            } else if (d < -0.999f) {
                rot = glm::rotate(glm::mat4(1.0f), PI, glm::vec3(1.0f, 0.0f, 0.0f));
            }

            glm::mat4 chainM = glm::translate(glm::mat4(1.0f), segMid);
            chainM = chainM * rot;
            chainM = glm::scale(chainM, glm::vec3(1.0f, segLen, 1.0f));
            submitMesh(renderer, m_chainMesh, chainM, glm::vec3(0.35f, 0.35f, 0.37f), 32.0f); // Realistic dark steel
        }

        // Seat
        glm::mat4 seat = glm::translate(glm::mat4(1.0f), glm::vec3(seatX, seatY, seatZ));
        seat = glm::scale(seat, glm::vec3(0.5f, 0.12f, 0.5f));
        glm::vec3 seatColor = (i % 2 == 0) ? glm::vec3(0.15f, 0.5f, 0.5f) : glm::vec3(0.85f, 0.3f, 0.2f);
        submitMesh(renderer, m_seatMesh, seat, seatColor, 16.0f);
    }
}

// ==================== Drop Tower ====================

DropTower::DropTower(const glm::vec3& position) : Ride("Drop Tower", position) {}

void DropTower::build(PhysicsWorld& physics) {
    std::vector<Vertex> finalVerts;
    std::vector<uint32_t> finalIdx;

    // 4 vertical steel pillars at corners: (-0.6, -0.6), (0.6, -0.6), (0.6, 0.6), (-0.6, 0.6)
    glm::vec3 pillars[] = {
        {-0.6f, 0.0f, -0.6f},
        {0.6f, 0.0f, -0.6f},
        {0.6f, 0.0f, 0.6f},
        {-0.6f, 0.0f, 0.6f}
    };
    std::vector<Vertex> v; std::vector<uint32_t> idx;
    
    // Vertical columns
    for (int i = 0; i < 4; i++) {
        v.clear(); idx.clear();
        ProceduralMeshes::createCylinder(v, idx, 0.08f, TOWER_HEIGHT, 8, true);
        ProceduralMeshes::colorVertices(v, glm::vec3(0.75f, 0.75f, 0.78f)); // Steel gray
        glm::mat4 trans = glm::translate(glm::mat4(1.0f), pillars[i] + glm::vec3(0.0f, TOWER_HEIGHT * 0.5f, 0.0f));
        ProceduralMeshes::transformVertices(v, trans);
        ProceduralMeshes::merge(finalVerts, finalIdx, v, idx);
    }

    // Horizontal and diagonal cross braces (every 2.5m)
    float step = 2.5f;
    for (float y = 0.0f; y < TOWER_HEIGHT; y += step) {
        float yCenter = y + step * 0.5f;
        if (yCenter > TOWER_HEIGHT) break;

        for (int f = 0; f < 4; f++) {
            glm::vec3 p0 = pillars[f];
            glm::vec3 p1 = pillars[(f + 1) % 4];

            // 1. Horizontal brace at the top of this section
            {
                glm::vec3 start = p0 + glm::vec3(0, y + step, 0);
                glm::vec3 end = p1 + glm::vec3(0, y + step, 0);
                v.clear(); idx.clear();
                ProceduralMeshes::createCylinder(v, idx, 0.05f, glm::distance(start, end), 6, true);
                ProceduralMeshes::colorVertices(v, glm::vec3(0.85f, 0.85f, 0.88f)); // Sleek warning white/steel
                
                glm::vec3 dir = glm::normalize(end - start);
                glm::vec3 up(0, 1, 0);
                glm::vec3 right = glm::normalize(glm::cross(dir, up));
                glm::vec3 actualUp = glm::normalize(glm::cross(right, dir));
                glm::mat4 rot(1.0f);
                rot[0] = glm::vec4(right, 0);
                rot[1] = glm::vec4(dir, 0);
                rot[2] = glm::vec4(actualUp, 0);
                
                glm::mat4 trans = glm::translate(glm::mat4(1.0f), (start + end) * 0.5f) * rot;
                ProceduralMeshes::transformVertices(v, trans);
                ProceduralMeshes::merge(finalVerts, finalIdx, v, idx);
            }

            // 2. Diagonal brace /
            {
                glm::vec3 start = p0 + glm::vec3(0, y, 0);
                glm::vec3 end = p1 + glm::vec3(0, y + step, 0);
                v.clear(); idx.clear();
                ProceduralMeshes::createCylinder(v, idx, 0.04f, glm::distance(start, end), 6, true);
                ProceduralMeshes::colorVertices(v, glm::vec3(0.65f, 0.65f, 0.68f));
                
                glm::vec3 dir = glm::normalize(end - start);
                glm::vec3 up(0, 1, 0);
                glm::vec3 right = glm::normalize(glm::cross(dir, up));
                if (glm::length(right) < 0.01f) right = glm::vec3(1, 0, 0);
                glm::vec3 actualUp = glm::normalize(glm::cross(right, dir));
                glm::mat4 rot(1.0f);
                rot[0] = glm::vec4(right, 0);
                rot[1] = glm::vec4(dir, 0);
                rot[2] = glm::vec4(actualUp, 0);
                
                glm::mat4 trans = glm::translate(glm::mat4(1.0f), (start + end) * 0.5f) * rot;
                ProceduralMeshes::transformVertices(v, trans);
                ProceduralMeshes::merge(finalVerts, finalIdx, v, idx);
            }

            // 3. Diagonal brace
            {
                glm::vec3 start = p0 + glm::vec3(0, y + step, 0);
                glm::vec3 end = p1 + glm::vec3(0, y, 0);
                v.clear(); idx.clear();
                ProceduralMeshes::createCylinder(v, idx, 0.04f, glm::distance(start, end), 6, true);
                ProceduralMeshes::colorVertices(v, glm::vec3(0.65f, 0.65f, 0.68f));
                
                glm::vec3 dir = glm::normalize(end - start);
                glm::vec3 up(0, 1, 0);
                glm::vec3 right = glm::normalize(glm::cross(dir, up));
                if (glm::length(right) < 0.01f) right = glm::vec3(1, 0, 0);
                glm::vec3 actualUp = glm::normalize(glm::cross(right, dir));
                glm::mat4 rot(1.0f);
                rot[0] = glm::vec4(right, 0);
                rot[1] = glm::vec4(dir, 0);
                rot[2] = glm::vec4(actualUp, 0);
                
                glm::mat4 trans = glm::translate(glm::mat4(1.0f), (start + end) * 0.5f) * rot;
                ProceduralMeshes::transformVertices(v, trans);
                ProceduralMeshes::merge(finalVerts, finalIdx, v, idx);
            }
        }
    }

    m_towerMesh.upload(finalVerts, finalIdx);

    ProceduralMeshes::createCylinder(v, idx, 2.5f, 0.6f, 16, true);
    m_platformMesh.upload(v, idx);

    ProceduralMeshes::createTorus(v, idx, 2.0f, 0.4f, 24, 8);
    m_seatRingMesh.upload(v, idx);

    ProceduralMeshes::createCylinder(v, idx, 3.5f, 0.5f, 24, true);
    m_baseMesh.upload(v, idx);

    Collider col;
    col.type = ColliderType::AABB;
    col.bounds.min = m_position - glm::vec3(3.5f, 0, 3.5f);
    col.bounds.max = m_position + glm::vec3(3.5f, TOWER_HEIGHT + 2, 3.5f);
    col.tag = "drop_tower";
    physics.addCollider(col);
}

void DropTower::update(float deltaTime, float totalTime) {
    m_animTime = totalTime;
    if (!m_running) return;
    m_phaseTimer += deltaTime;

    switch (m_phase) {
        case Phase::Rising:
            m_carHeight += RIDE_SPEED * 0.4f * deltaTime;
            if (m_carHeight >= TOWER_HEIGHT - 2.0f) {
                m_carHeight = TOWER_HEIGHT - 2.0f;
                m_phase = Phase::Pause;
                m_phaseTimer = 0;
            }
            break;
        case Phase::Pause:
            if (m_phaseTimer > 2.0f) {
                m_phase = Phase::Dropping;
                m_phaseTimer = 0;
            }
            break;
        case Phase::Dropping:
            m_carHeight -= RIDE_SPEED * 2.5f * deltaTime;
            if (m_carHeight <= 1.0f) {
                m_carHeight = 1.0f;
                m_phase = Phase::Landed;
                m_phaseTimer = 0;
            }
            break;
        case Phase::Landed:
            if (m_phaseTimer > 1.5f) {
                m_phase = Phase::Rising;
                m_phaseTimer = 0;
            }
    }
}

void DropTower::render(Renderer& renderer) {
    glm::vec3 p = m_position;

    // Base
    glm::mat4 base = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y + 0.25f, p.z));
    submitMesh(renderer, m_baseMesh, base, glm::vec3(0.35f, 0.35f, 0.37f), 16.0f); // Dark concrete

    // Tower columns & lattice (uses vertex colors)
    RenderCommand towerCmd;
    towerCmd.mesh = &m_towerMesh;
    towerCmd.transform = glm::translate(glm::mat4(1.0f), p);
    towerCmd.material.diffuse = glm::vec3(1.0f);
    towerCmd.material.ambient = glm::vec3(0.2f);
    towerCmd.material.specular = glm::vec3(0.4f);
    towerCmd.material.shininess = 64.0f;
    towerCmd.material.useVertexColor = true;
    renderer.submit(towerCmd);

    // Moving platform
    glm::mat4 plat = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y + m_carHeight, p.z));
    submitMesh(renderer, m_platformMesh, plat, glm::vec3(0.2f, 0.2f, 0.22f), 64.0f); // Dark carbon/steel

    // Seat ring on platform
    glm::mat4 ring = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y + m_carHeight + 0.5f, p.z));
    submitMesh(renderer, m_seatRingMesh, ring, glm::vec3(0.95f, 0.35f, 0.05f), 96.0f); // Safety Orange

    // Top warning cap
    glm::mat4 cap = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, p.y + TOWER_HEIGHT + 0.5f, p.z));
    cap = glm::scale(cap, glm::vec3(2.5f, 1.0f, 2.5f));
    // Let's use a solid box (since m_towerMesh is now the complex lattice)
    // Actually we can reuse m_baseMesh (a cylinder) or just draw it normally
    submitMesh(renderer, m_baseMesh, cap, glm::vec3(0.92f, 0.75f, 0.1f), 64.0f); // Warning Yellow
}

} // namespace ap3d
