#pragma once
#include "core/Common.h"

namespace ap3d {

/// Axis-Aligned Bounding Box.
struct AABB {
    glm::vec3 min{0.0f};
    glm::vec3 max{0.0f};

    [[nodiscard]] glm::vec3 center() const { return (min + max) * 0.5f; }
    [[nodiscard]] glm::vec3 extents() const { return (max - min) * 0.5f; }
    [[nodiscard]] glm::vec3 size() const { return max - min; }

    [[nodiscard]] bool intersects(const AABB& other) const {
        return (min.x <= other.max.x && max.x >= other.min.x) &&
               (min.y <= other.max.y && max.y >= other.min.y) &&
               (min.z <= other.max.z && max.z >= other.min.z);
    }

    [[nodiscard]] bool contains(const glm::vec3& point) const {
        return point.x >= min.x && point.x <= max.x &&
               point.y >= min.y && point.y <= max.y &&
               point.z >= min.z && point.z <= max.z;
    }

    /// Expand the AABB to include a point.
    void expand(const glm::vec3& point) {
        min = glm::min(min, point);
        max = glm::max(max, point);
    }
};

/// Collider types.
enum class ColliderType {
    AABB,
    Sphere,
    Cylinder
};

/// Static collider for world objects.
struct Collider {
    ColliderType type = ColliderType::AABB;
    AABB bounds;
    glm::vec3 center{0.0f};
    float radius = 0.0f;     // For sphere/cylinder
    float height = 0.0f;     // For cylinder
    bool isStatic = true;
    bool isTrigger = false;   // Trigger zones (ride entrance, minigame zone)
    int userData = -1;        // Index into ride/minigame arrays
    std::string tag;          // Identifier tag
};

/// Collision result.
struct CollisionResult {
    bool collided = false;
    glm::vec3 penetration{0.0f};  // Direction and depth to resolve
    glm::vec3 contactPoint{0.0f};
    const Collider* hitCollider = nullptr;
};

/// Physics world managing collision detection and response.
class PhysicsWorld {
public:
    PhysicsWorld() = default;

    /// Add a static collider. Returns the collider index.
    int addCollider(const Collider& collider);

    /// Remove a collider by index.
    void removeCollider(int index);

    /// Get a collider by index.
    [[nodiscard]] Collider& getCollider(int index) { return m_colliders[index]; }
    [[nodiscard]] const Collider& getCollider(int index) const { return m_colliders[index]; }

    /// Test a moving sphere against all static colliders.
    /// Returns the corrected position after collision response.
    glm::vec3 resolveMovement(const glm::vec3& currentPos, const glm::vec3& desiredPos,
        float playerRadius, float playerHeight);

    /// Test a ray against all colliders. Returns true if hit.
    bool raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance,
        CollisionResult& result) const;

    /// Find all trigger zones the player is inside.
    std::vector<const Collider*> findTriggers(const glm::vec3& position, float radius) const;

    /// Get the number of colliders.
    [[nodiscard]] size_t colliderCount() const { return m_colliders.size(); }

    /// Clear all colliders.
    void clear() { m_colliders.clear(); }

private:
    /// Test sphere vs AABB.
    static CollisionResult testSphereAABB(const glm::vec3& sphereCenter, float sphereRadius, const AABB& box);

    /// Test sphere vs sphere.
    static CollisionResult testSphereSphere(const glm::vec3& c1, float r1, const glm::vec3& c2, float r2);

    /// Test sphere vs cylinder.
    static CollisionResult testSphereCylinder(const glm::vec3& sphereCenter, float sphereRadius,
        const glm::vec3& cylCenter, float cylRadius, float cylHeight);

    /// Test ray vs AABB.
    static bool testRayAABB(const glm::vec3& origin, const glm::vec3& dir, const AABB& box,
        float& tMin, glm::vec3& hitPoint);

    std::vector<Collider> m_colliders;
};

} // namespace ap3d
