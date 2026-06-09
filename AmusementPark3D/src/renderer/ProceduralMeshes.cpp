#include "renderer/ProceduralMeshes.h"

namespace ap3d {

void ProceduralMeshes::createCube(std::vector<Vertex>& verts, std::vector<uint32_t>& indices) {
    verts.clear();
    indices.clear();

    // 6 faces, 4 vertices each, proper normals
    struct FaceData { glm::vec3 normal; glm::vec3 tangent; glm::vec3 bitangent; };
    const FaceData faces[6] = {
        {{ 0, 0, 1}, { 1, 0, 0}, {0, 1, 0}},  // Front
        {{ 0, 0,-1}, {-1, 0, 0}, {0, 1, 0}},  // Back
        {{ 1, 0, 0}, { 0, 0,-1}, {0, 1, 0}},  // Right
        {{-1, 0, 0}, { 0, 0, 1}, {0, 1, 0}},  // Left
        {{ 0, 1, 0}, { 1, 0, 0}, {0, 0,-1}},  // Top
        {{ 0,-1, 0}, { 1, 0, 0}, {0, 0, 1}},  // Bottom
    };

    for (int i = 0; i < 6; i++) {
        const auto& f = faces[i];
        uint32_t base = static_cast<uint32_t>(verts.size());

        glm::vec3 v0 = (-f.tangent - f.bitangent + f.normal) * 0.5f;
        glm::vec3 v1 = ( f.tangent - f.bitangent + f.normal) * 0.5f;
        glm::vec3 v2 = ( f.tangent + f.bitangent + f.normal) * 0.5f;
        glm::vec3 v3 = (-f.tangent + f.bitangent + f.normal) * 0.5f;

        verts.push_back({v0, f.normal, {0, 0}, {1, 1, 1}});
        verts.push_back({v1, f.normal, {1, 0}, {1, 1, 1}});
        verts.push_back({v2, f.normal, {1, 1}, {1, 1, 1}});
        verts.push_back({v3, f.normal, {0, 1}, {1, 1, 1}});

        indices.push_back(base + 0);
        indices.push_back(base + 1);
        indices.push_back(base + 2);
        indices.push_back(base + 0);
        indices.push_back(base + 2);
        indices.push_back(base + 3);
    }
}

void ProceduralMeshes::createSphere(std::vector<Vertex>& verts, std::vector<uint32_t>& indices,
    int sectors, int stacks, float radius)
{
    verts.clear();
    indices.clear();

    for (int i = 0; i <= stacks; i++) {
        float stackAngle = PI / 2.0f - PI * static_cast<float>(i) / static_cast<float>(stacks);
        float xy = radius * cosf(stackAngle);
        float z = radius * sinf(stackAngle);

        for (int j = 0; j <= sectors; j++) {
            float sectorAngle = TWO_PI * static_cast<float>(j) / static_cast<float>(sectors);
            float x = xy * cosf(sectorAngle);
            float y = xy * sinf(sectorAngle);

            glm::vec3 pos(x, z, y);
            glm::vec3 normal = glm::normalize(pos);
            glm::vec2 uv(static_cast<float>(j) / sectors, static_cast<float>(i) / stacks);

            verts.push_back({pos, normal, uv, {1, 1, 1}});
        }
    }

    for (int i = 0; i < stacks; i++) {
        int k1 = i * (sectors + 1);
        int k2 = k1 + sectors + 1;

        for (int j = 0; j < sectors; j++, k1++, k2++) {
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }
            if (i != (stacks - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
}

void ProceduralMeshes::createCylinder(std::vector<Vertex>& verts, std::vector<uint32_t>& indices,
    float radius, float height, int segments, bool caps)
{
    verts.clear();
    indices.clear();

    float halfH = height / 2.0f;

    // Side vertices
    for (int i = 0; i <= segments; i++) {
        float angle = TWO_PI * static_cast<float>(i) / static_cast<float>(segments);
        float x = cosf(angle) * radius;
        float z = sinf(angle) * radius;
        float u = static_cast<float>(i) / static_cast<float>(segments);
        glm::vec3 normal = glm::normalize(glm::vec3(x, 0, z));

        verts.push_back({{x, -halfH, z}, normal, {u, 0}, {1, 1, 1}});
        verts.push_back({{x,  halfH, z}, normal, {u, 1}, {1, 1, 1}});
    }

    // Side indices
    for (int i = 0; i < segments; i++) {
        uint32_t base = i * 2;
        indices.push_back(base);
        indices.push_back(base + 1);
        indices.push_back(base + 2);
        indices.push_back(base + 1);
        indices.push_back(base + 3);
        indices.push_back(base + 2);
    }

    if (caps) {
        // Top cap
        uint32_t topCenter = static_cast<uint32_t>(verts.size());
        verts.push_back({{0, halfH, 0}, {0, 1, 0}, {0.5f, 0.5f}, {1, 1, 1}});

        for (int i = 0; i <= segments; i++) {
            float angle = TWO_PI * static_cast<float>(i) / static_cast<float>(segments);
            float x = cosf(angle) * radius;
            float z = sinf(angle) * radius;
            verts.push_back({{x, halfH, z}, {0, 1, 0},
                {cosf(angle) * 0.5f + 0.5f, sinf(angle) * 0.5f + 0.5f}, {1, 1, 1}});
        }

        for (int i = 0; i < segments; i++) {
            indices.push_back(topCenter);
            indices.push_back(topCenter + 2 + i);
            indices.push_back(topCenter + 1 + i);
        }

        // Bottom cap
        uint32_t botCenter = static_cast<uint32_t>(verts.size());
        verts.push_back({{0, -halfH, 0}, {0, -1, 0}, {0.5f, 0.5f}, {1, 1, 1}});

        for (int i = 0; i <= segments; i++) {
            float angle = TWO_PI * static_cast<float>(i) / static_cast<float>(segments);
            float x = cosf(angle) * radius;
            float z = sinf(angle) * radius;
            verts.push_back({{x, -halfH, z}, {0, -1, 0},
                {cosf(angle) * 0.5f + 0.5f, sinf(angle) * 0.5f + 0.5f}, {1, 1, 1}});
        }

        for (int i = 0; i < segments; i++) {
            indices.push_back(botCenter);
            indices.push_back(botCenter + 1 + i);
            indices.push_back(botCenter + 2 + i);
        }
    }
}

void ProceduralMeshes::createCone(std::vector<Vertex>& verts, std::vector<uint32_t>& indices,
    float radius, float height, int segments)
{
    verts.clear();
    indices.clear();

    float halfH = height / 2.0f;
    float slope = radius / height;

    // Tip vertex
    uint32_t tipIdx = 0;
    verts.push_back({{0, halfH, 0}, {0, 1, 0}, {0.5f, 1.0f}, {1, 1, 1}});

    // Base ring
    for (int i = 0; i <= segments; i++) {
        float angle = TWO_PI * static_cast<float>(i) / static_cast<float>(segments);
        float x = cosf(angle) * radius;
        float z = sinf(angle) * radius;
        glm::vec3 normal = glm::normalize(glm::vec3(cosf(angle), slope, sinf(angle)));
        float u = static_cast<float>(i) / static_cast<float>(segments);
        verts.push_back({{x, -halfH, z}, normal, {u, 0}, {1, 1, 1}});
    }

    // Side triangles
    for (int i = 0; i < segments; i++) {
        indices.push_back(tipIdx);
        indices.push_back(1 + i + 1);
        indices.push_back(1 + i);
    }

    // Bottom cap
    uint32_t botCenter = static_cast<uint32_t>(verts.size());
    verts.push_back({{0, -halfH, 0}, {0, -1, 0}, {0.5f, 0.5f}, {1, 1, 1}});

    for (int i = 0; i <= segments; i++) {
        float angle = TWO_PI * static_cast<float>(i) / static_cast<float>(segments);
        float x = cosf(angle) * radius;
        float z = sinf(angle) * radius;
        verts.push_back({{x, -halfH, z}, {0, -1, 0},
            {cosf(angle) * 0.5f + 0.5f, sinf(angle) * 0.5f + 0.5f}, {1, 1, 1}});
    }

    for (int i = 0; i < segments; i++) {
        indices.push_back(botCenter);
        indices.push_back(botCenter + 1 + i);
        indices.push_back(botCenter + 2 + i);
    }
}

void ProceduralMeshes::createTorus(std::vector<Vertex>& verts, std::vector<uint32_t>& indices,
    float majorRadius, float minorRadius, int majorSegments, int minorSegments)
{
    verts.clear();
    indices.clear();

    for (int i = 0; i <= majorSegments; i++) {
        float u = static_cast<float>(i) / static_cast<float>(majorSegments);
        float theta = u * TWO_PI;
        float cosTheta = cosf(theta);
        float sinTheta = sinf(theta);

        for (int j = 0; j <= minorSegments; j++) {
            float v = static_cast<float>(j) / static_cast<float>(minorSegments);
            float phi = v * TWO_PI;
            float cosPhi = cosf(phi);
            float sinPhi = sinf(phi);

            float x = (majorRadius + minorRadius * cosPhi) * cosTheta;
            float y = minorRadius * sinPhi;
            float z = (majorRadius + minorRadius * cosPhi) * sinTheta;

            glm::vec3 center(majorRadius * cosTheta, 0, majorRadius * sinTheta);
            glm::vec3 pos(x, y, z);
            glm::vec3 normal = glm::normalize(pos - center);

            verts.push_back({pos, normal, {u, v}, {1, 1, 1}});
        }
    }

    for (int i = 0; i < majorSegments; i++) {
        for (int j = 0; j < minorSegments; j++) {
            uint32_t a = i * (minorSegments + 1) + j;
            uint32_t b = a + minorSegments + 1;
            uint32_t c = a + 1;
            uint32_t d = b + 1;

            indices.push_back(a);
            indices.push_back(b);
            indices.push_back(c);
            indices.push_back(c);
            indices.push_back(b);
            indices.push_back(d);
        }
    }
}

void ProceduralMeshes::createPlane(std::vector<Vertex>& verts, std::vector<uint32_t>& indices,
    float width, float depth, int subdivX, int subdivZ)
{
    verts.clear();
    indices.clear();

    float halfW = width / 2.0f;
    float halfD = depth / 2.0f;

    for (int z = 0; z <= subdivZ; z++) {
        for (int x = 0; x <= subdivX; x++) {
            float u = static_cast<float>(x) / static_cast<float>(subdivX);
            float v = static_cast<float>(z) / static_cast<float>(subdivZ);
            float px = -halfW + u * width;
            float pz = -halfD + v * depth;

            verts.push_back({{px, 0, pz}, {0, 1, 0}, {u * width / 4.0f, v * depth / 4.0f}, {1, 1, 1}});
        }
    }

    for (int z = 0; z < subdivZ; z++) {
        for (int x = 0; x < subdivX; x++) {
            uint32_t tl = z * (subdivX + 1) + x;
            uint32_t tr = tl + 1;
            uint32_t bl = tl + (subdivX + 1);
            uint32_t br = bl + 1;

            indices.push_back(tl);
            indices.push_back(bl);
            indices.push_back(tr);
            indices.push_back(tr);
            indices.push_back(bl);
            indices.push_back(br);
        }
    }
}

void ProceduralMeshes::createRing(std::vector<Vertex>& verts, std::vector<uint32_t>& indices,
    float innerRadius, float outerRadius, int segments)
{
    verts.clear();
    indices.clear();

    for (int i = 0; i <= segments; i++) {
        float angle = TWO_PI * static_cast<float>(i) / static_cast<float>(segments);
        float c = cosf(angle);
        float s = sinf(angle);
        float u = static_cast<float>(i) / static_cast<float>(segments);

        verts.push_back({{c * innerRadius, 0, s * innerRadius}, {0, 1, 0}, {u, 0}, {1, 1, 1}});
        verts.push_back({{c * outerRadius, 0, s * outerRadius}, {0, 1, 0}, {u, 1}, {1, 1, 1}});
    }

    for (int i = 0; i < segments; i++) {
        uint32_t base = i * 2;
        indices.push_back(base);
        indices.push_back(base + 2);
        indices.push_back(base + 1);
        indices.push_back(base + 1);
        indices.push_back(base + 2);
        indices.push_back(base + 3);
    }
}

void ProceduralMeshes::createCapsule(std::vector<Vertex>& verts, std::vector<uint32_t>& indices,
    float radius, float height, int segments, int rings)
{
    verts.clear();
    indices.clear();

    float cylinderHeight = height - 2.0f * radius;
    if (cylinderHeight < 0) cylinderHeight = 0;
    float halfCyl = cylinderHeight / 2.0f;

    // Top hemisphere
    for (int i = 0; i <= rings; i++) {
        float phi = HALF_PI * static_cast<float>(i) / static_cast<float>(rings);
        float y = radius * cosf(phi) + halfCyl;
        float r = radius * sinf(phi);

        for (int j = 0; j <= segments; j++) {
            float theta = TWO_PI * static_cast<float>(j) / static_cast<float>(segments);
            float x = r * cosf(theta);
            float z = r * sinf(theta);
            glm::vec3 pos(x, y, z);
            glm::vec3 normal = glm::normalize(glm::vec3(x, y - halfCyl, z));
            float u = static_cast<float>(j) / segments;
            float v = static_cast<float>(i) / (rings * 2 + 1);
            verts.push_back({pos, normal, {u, v}, {1, 1, 1}});
        }
    }

    // Cylinder body
    for (int i = 0; i <= 1; i++) {
        float y = halfCyl - i * cylinderHeight;
        for (int j = 0; j <= segments; j++) {
            float theta = TWO_PI * static_cast<float>(j) / static_cast<float>(segments);
            float x = radius * cosf(theta);
            float z = radius * sinf(theta);
            glm::vec3 normal = glm::normalize(glm::vec3(x, 0, z));
            float u = static_cast<float>(j) / segments;
            float v = 0.5f;
            verts.push_back({{x, y, z}, normal, {u, v}, {1, 1, 1}});
        }
    }

    // Bottom hemisphere
    for (int i = 0; i <= rings; i++) {
        float phi = HALF_PI + HALF_PI * static_cast<float>(i) / static_cast<float>(rings);
        float y = radius * cosf(phi) - halfCyl;
        float r = radius * sinf(phi);

        for (int j = 0; j <= segments; j++) {
            float theta = TWO_PI * static_cast<float>(j) / static_cast<float>(segments);
            float x = r * cosf(theta);
            float z = r * sinf(theta);
            glm::vec3 pos(x, y, z);
            glm::vec3 normal = glm::normalize(glm::vec3(x, y + halfCyl, z));
            float u = static_cast<float>(j) / segments;
            float v = 1.0f - static_cast<float>(i) / (rings * 2 + 1);
            verts.push_back({pos, normal, {u, v}, {1, 1, 1}});
        }
    }

    // Generate indices for all rows
    int totalRows = rings + 1 + rings; // top hemi + cylinder + bottom hemi
    int vertsPerRow = segments + 1;
    for (int i = 0; i < totalRows; i++) {
        for (int j = 0; j < segments; j++) {
            uint32_t a = i * vertsPerRow + j;
            uint32_t b = a + vertsPerRow;
            uint32_t c = a + 1;
            uint32_t d = b + 1;

            if (a < verts.size() && b < verts.size() && c < verts.size() && d < verts.size()) {
                indices.push_back(a);
                indices.push_back(b);
                indices.push_back(c);
                indices.push_back(c);
                indices.push_back(b);
                indices.push_back(d);
            }
        }
    }
}

void ProceduralMeshes::createArch(std::vector<Vertex>& verts, std::vector<uint32_t>& indices,
    float width, float height, float depth, int segments)
{
    verts.clear();
    indices.clear();

    float halfW = width / 2.0f;
    float halfD = depth / 2.0f;
    float archRadius = halfW;
    float pillarHeight = height - archRadius;

    // Left pillar (cube stretched)
    std::vector<Vertex> pillarV;
    std::vector<uint32_t> pillarI;
    createCube(pillarV, pillarI);
    glm::mat4 leftPillar = glm::translate(glm::mat4(1.0f), glm::vec3(-halfW + halfD, pillarHeight / 2.0f, 0.0f));
    leftPillar = glm::scale(leftPillar, glm::vec3(depth, pillarHeight, depth));
    transformVertices(pillarV, leftPillar);
    merge(verts, indices, pillarV, pillarI);

    // Right pillar
    createCube(pillarV, pillarI);
    glm::mat4 rightPillar = glm::translate(glm::mat4(1.0f), glm::vec3(halfW - halfD, pillarHeight / 2.0f, 0.0f));
    rightPillar = glm::scale(rightPillar, glm::vec3(depth, pillarHeight, depth));
    transformVertices(pillarV, rightPillar);
    merge(verts, indices, pillarV, pillarI);

    // Arch curve (semicircle of boxes)
    for (int i = 0; i <= segments; i++) {
        float angle = PI * static_cast<float>(i) / static_cast<float>(segments);
        float x = cosf(angle) * archRadius;
        float y = sinf(angle) * archRadius + pillarHeight;

        std::vector<Vertex> blockV;
        std::vector<uint32_t> blockI;
        createCube(blockV, blockI);
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(-x, y, 0.0f));
        float blockAngle = angle - HALF_PI;
        transform = glm::rotate(transform, blockAngle, glm::vec3(0, 0, 1));
        transform = glm::scale(transform, glm::vec3(width / segments * 1.2f, depth, depth));
        transformVertices(blockV, transform);
        merge(verts, indices, blockV, blockI);
    }
}

void ProceduralMeshes::transformVertices(std::vector<Vertex>& verts, const glm::mat4& transform) {
    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(transform)));
    for (auto& v : verts) {
        v.position = glm::vec3(transform * glm::vec4(v.position, 1.0f));
        v.normal = glm::normalize(normalMatrix * v.normal);
    }
}

void ProceduralMeshes::colorVertices(std::vector<Vertex>& verts, const glm::vec3& color) {
    for (auto& v : verts) {
        v.color = color;
    }
}

void ProceduralMeshes::merge(std::vector<Vertex>& destVerts, std::vector<uint32_t>& destIndices,
    const std::vector<Vertex>& srcVerts, const std::vector<uint32_t>& srcIndices)
{
    uint32_t offset = static_cast<uint32_t>(destVerts.size());
    destVerts.insert(destVerts.end(), srcVerts.begin(), srcVerts.end());
    for (auto idx : srcIndices) {
        destIndices.push_back(idx + offset);
    }
}

} // namespace ap3d
