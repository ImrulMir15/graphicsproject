#pragma once
#include "renderer/Mesh.h"

namespace ap3d {

/// Factory for generating procedural geometry (all park objects use these).
struct ProceduralMeshes {
    /// Generate a unit cube centered at origin.
    static void createCube(std::vector<Vertex>& verts, std::vector<uint32_t>& indices);

    /// Generate a UV sphere.
    static void createSphere(std::vector<Vertex>& verts, std::vector<uint32_t>& indices,
        int sectors = 32, int stacks = 16, float radius = 1.0f);

    /// Generate a cylinder along Y axis.
    static void createCylinder(std::vector<Vertex>& verts, std::vector<uint32_t>& indices,
        float radius = 1.0f, float height = 1.0f, int segments = 32, bool caps = true);

    /// Generate a cone along Y axis.
    static void createCone(std::vector<Vertex>& verts, std::vector<uint32_t>& indices,
        float radius = 1.0f, float height = 1.0f, int segments = 32);

    /// Generate a torus (donut shape).
    static void createTorus(std::vector<Vertex>& verts, std::vector<uint32_t>& indices,
        float majorRadius = 1.0f, float minorRadius = 0.3f,
        int majorSegments = 32, int minorSegments = 16);

    /// Generate a flat quad (plane) on XZ plane.
    static void createPlane(std::vector<Vertex>& verts, std::vector<uint32_t>& indices,
        float width = 1.0f, float depth = 1.0f, int subdivX = 1, int subdivZ = 1);

    /// Generate a ring (annulus) on XZ plane.
    static void createRing(std::vector<Vertex>& verts, std::vector<uint32_t>& indices,
        float innerRadius = 0.5f, float outerRadius = 1.0f, int segments = 32);

    /// Generate a capsule (cylinder with hemisphere caps).
    static void createCapsule(std::vector<Vertex>& verts, std::vector<uint32_t>& indices,
        float radius = 0.5f, float height = 2.0f, int segments = 16, int rings = 8);

    /// Generate an arch shape.
    static void createArch(std::vector<Vertex>& verts, std::vector<uint32_t>& indices,
        float width = 4.0f, float height = 5.0f, float depth = 1.0f, int segments = 16);

    /// Apply a transformation to all vertices.
    static void transformVertices(std::vector<Vertex>& verts, const glm::mat4& transform);

    /// Apply a solid color to all vertices.
    static void colorVertices(std::vector<Vertex>& verts, const glm::vec3& color);

    /// Merge source vertices/indices into destination, appending with index offset.
    static void merge(std::vector<Vertex>& destVerts, std::vector<uint32_t>& destIndices,
        const std::vector<Vertex>& srcVerts, const std::vector<uint32_t>& srcIndices);
};

} // namespace ap3d
