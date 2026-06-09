#include "physics/PhysicsWorld.h"

namespace ap3d {

int PhysicsWorld::addCollider(const Collider& collider) {
    m_colliders.push_back(collider);
    return static_cast<int>(m_colliders.size()) - 1;
}

void PhysicsWorld::removeCollider(int index) {
    if (index >= 0 && index < static_cast<int>(m_colliders.size())) {
        m_colliders.erase(m_colliders.begin() + index);
    }
}

glm::vec3 PhysicsWorld::resolveMovement(const glm::vec3& currentPos, const glm::vec3& desiredPos,
    float playerRadius, float playerHeight)
{
    glm::vec3 resolved = desiredPos;

    // Multiple iteration passes for stable collision response
    for (int pass = 0; pass < 4; pass++) {
        bool hadCollision = false;

        for (const auto& col : m_colliders) {
            if (col.isTrigger) continue; // Skip triggers

            CollisionResult result;

            switch (col.type) {
                case ColliderType::AABB:
                    result = testSphereAABB(resolved, playerRadius, col.bounds);
                    break;
                case ColliderType::Sphere:
                    result = testSphereSphere(resolved, playerRadius, col.center, col.radius);
                    break;
                case ColliderType::Cylinder:
                    result = testSphereCylinder(resolved, playerRadius, col.center, col.radius, col.height);
                    break;
            }

            if (result.collided) {
                resolved += result.penetration;
                hadCollision = true;
            }
        }

        if (!hadCollision) break;
    }

    // Floor constraint - keep player above ground
    if (resolved.y < playerHeight * 0.5f) {
        resolved.y = playerHeight * 0.5f;
    }

    return resolved;
}

bool PhysicsWorld::raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance,
    CollisionResult& result) const
{
    float closestT = maxDistance;
    bool hit = false;

    for (const auto& col : m_colliders) {
        if (col.type == ColliderType::AABB) {
            float t;
            glm::vec3 hitPoint;
            if (testRayAABB(origin, direction, col.bounds, t, hitPoint) && t < closestT) {
                closestT = t;
                result.collided = true;
                result.contactPoint = hitPoint;
                result.hitCollider = &col;
                hit = true;
            }
        } else if (col.type == ColliderType::Sphere) {
            // Ray-sphere intersection
            glm::vec3 oc = origin - col.center;
            float a = glm::dot(direction, direction);
            float b = 2.0f * glm::dot(oc, direction);
            float c = glm::dot(oc, oc) - col.radius * col.radius;
            float discriminant = b * b - 4 * a * c;
            if (discriminant >= 0) {
                float t = (-b - sqrtf(discriminant)) / (2.0f * a);
                if (t > 0 && t < closestT) {
                    closestT = t;
                    result.collided = true;
                    result.contactPoint = origin + direction * t;
                    result.hitCollider = &col;
                    hit = true;
                }
            }
        }
    }

    return hit;
}

std::vector<const Collider*> PhysicsWorld::findTriggers(const glm::vec3& position, float radius) const {
    std::vector<const Collider*> triggers;

    for (const auto& col : m_colliders) {
        if (!col.isTrigger) continue;

        bool inside = false;
        switch (col.type) {
            case ColliderType::AABB: {
                // Expand AABB by player radius
                AABB expanded = col.bounds;
                expanded.min -= glm::vec3(radius);
                expanded.max += glm::vec3(radius);
                inside = expanded.contains(position);
                break;
            }
            case ColliderType::Sphere: {
                float dist = glm::distance(position, col.center);
                inside = dist < (col.radius + radius);
                break;
            }
            case ColliderType::Cylinder: {
                glm::vec2 xz1(position.x, position.z);
                glm::vec2 xz2(col.center.x, col.center.z);
                float dist2D = glm::distance(xz1, xz2);
                inside = dist2D < (col.radius + radius) &&
                         position.y > col.center.y - col.height * 0.5f &&
                         position.y < col.center.y + col.height * 0.5f;
                break;
            }
        }

        if (inside) {
            triggers.push_back(&col);
        }
    }

    return triggers;
}

// ---- Collision Tests ----

CollisionResult PhysicsWorld::testSphereAABB(const glm::vec3& sphereCenter, float sphereRadius, const AABB& box) {
    CollisionResult result;

    // Find closest point on AABB to sphere center
    glm::vec3 closest = glm::clamp(sphereCenter, box.min, box.max);

    glm::vec3 diff = sphereCenter - closest;
    float distSq = glm::dot(diff, diff);

    if (distSq < sphereRadius * sphereRadius) {
        result.collided = true;
        float dist = sqrtf(distSq);
        if (dist > 0.0001f) {
            glm::vec3 normal = diff / dist;
            float penetration = sphereRadius - dist;
            result.penetration = normal * penetration;
        } else {
            // Sphere center is inside AABB - push out along shortest axis
            glm::vec3 toMin = sphereCenter - box.min;
            glm::vec3 toMax = box.max - sphereCenter;
            float minPen = toMin.x;
            glm::vec3 normal(-1, 0, 0);

            if (toMin.y < minPen) { minPen = toMin.y; normal = glm::vec3(0, -1, 0); }
            if (toMin.z < minPen) { minPen = toMin.z; normal = glm::vec3(0, 0, -1); }
            if (toMax.x < minPen) { minPen = toMax.x; normal = glm::vec3(1, 0, 0); }
            if (toMax.y < minPen) { minPen = toMax.y; normal = glm::vec3(0, 1, 0); }
            if (toMax.z < minPen) { minPen = toMax.z; normal = glm::vec3(0, 0, 1); }

            result.penetration = normal * (minPen + sphereRadius);
        }
        result.contactPoint = closest;
    }

    return result;
}

CollisionResult PhysicsWorld::testSphereSphere(const glm::vec3& c1, float r1, const glm::vec3& c2, float r2) {
    CollisionResult result;

    glm::vec3 diff = c1 - c2;
    float dist = glm::length(diff);
    float overlap = (r1 + r2) - dist;

    if (overlap > 0 && dist > 0.0001f) {
        result.collided = true;
        glm::vec3 normal = diff / dist;
        result.penetration = normal * overlap;
        result.contactPoint = c2 + normal * r2;
    }

    return result;
}

CollisionResult PhysicsWorld::testSphereCylinder(const glm::vec3& sphereCenter, float sphereRadius,
    const glm::vec3& cylCenter, float cylRadius, float cylHeight)
{
    CollisionResult result;

    // Check height bounds
    float halfH = cylHeight * 0.5f;
    if (sphereCenter.y < cylCenter.y - halfH - sphereRadius ||
        sphereCenter.y > cylCenter.y + halfH + sphereRadius) {
        return result;
    }

    // 2D circle test on XZ plane
    glm::vec2 sphereXZ(sphereCenter.x, sphereCenter.z);
    glm::vec2 cylXZ(cylCenter.x, cylCenter.z);
    glm::vec2 diff2D = sphereXZ - cylXZ;
    float dist2D = glm::length(diff2D);

    float overlap = (sphereRadius + cylRadius) - dist2D;
    if (overlap > 0 && dist2D > 0.0001f) {
        result.collided = true;
        glm::vec2 normal2D = diff2D / dist2D;
        result.penetration = glm::vec3(normal2D.x, 0, normal2D.y) * overlap;
        result.contactPoint = glm::vec3(cylXZ.x + normal2D.x * cylRadius, sphereCenter.y,
                                        cylXZ.y + normal2D.y * cylRadius);
    }

    return result;
}

bool PhysicsWorld::testRayAABB(const glm::vec3& origin, const glm::vec3& dir, const AABB& box,
    float& tMin, glm::vec3& hitPoint)
{
    glm::vec3 invDir = 1.0f / dir;

    float t1 = (box.min.x - origin.x) * invDir.x;
    float t2 = (box.max.x - origin.x) * invDir.x;
    float t3 = (box.min.y - origin.y) * invDir.y;
    float t4 = (box.max.y - origin.y) * invDir.y;
    float t5 = (box.min.z - origin.z) * invDir.z;
    float t6 = (box.max.z - origin.z) * invDir.z;

    float tMinCalc = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
    float tMaxCalc = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

    if (tMaxCalc < 0 || tMinCalc > tMaxCalc) return false;

    tMin = tMinCalc < 0 ? tMaxCalc : tMinCalc;
    hitPoint = origin + dir * tMin;
    return true;
}

} // namespace ap3d
