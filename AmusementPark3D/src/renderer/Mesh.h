#pragma once
#include "core/Common.h"

namespace ap3d {

/// Vertex structure for all meshes.
struct Vertex {
    glm::vec3 position{0.0f};
    glm::vec3 normal{0.0f, 1.0f, 0.0f};
    glm::vec2 texCoord{0.0f};
    glm::vec3 color{1.0f};
};

/// GPU mesh wrapper managing VAO, VBO, EBO with RAII.
class Mesh {
public:
    Mesh() = default;
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    /// Upload vertex and index data to the GPU.
    void upload(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices);

    /// Upload vertex data only (no indices, uses glDrawArrays).
    void uploadVerticesOnly(const std::vector<Vertex>& vertices);

    /// Draw the mesh.
    void draw(GLenum mode = GL_TRIANGLES) const;

    /// Check if the mesh has data uploaded.
    [[nodiscard]] bool isValid() const { return m_vao != 0; }

    /// Get the number of vertices/indices.
    [[nodiscard]] uint32_t vertexCount() const { return m_vertexCount; }
    [[nodiscard]] uint32_t indexCount() const { return m_indexCount; }

    /// Get the AABB bounds (computed on upload).
    [[nodiscard]] const glm::vec3& boundsMin() const { return m_boundsMin; }
    [[nodiscard]] const glm::vec3& boundsMax() const { return m_boundsMax; }

private:
    void cleanup();

    GLuint m_vao = 0;
    GLuint m_vbo = 0;
    GLuint m_ebo = 0;
    uint32_t m_vertexCount = 0;
    uint32_t m_indexCount = 0;
    bool m_hasIndices = false;

    glm::vec3 m_boundsMin{0.0f};
    glm::vec3 m_boundsMax{0.0f};
};

} // namespace ap3d
