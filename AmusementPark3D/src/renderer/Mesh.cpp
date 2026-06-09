#include "renderer/Mesh.h"

namespace ap3d {

Mesh::~Mesh() {
    cleanup();
}

Mesh::Mesh(Mesh&& other) noexcept
    : m_vao(other.m_vao), m_vbo(other.m_vbo), m_ebo(other.m_ebo),
      m_vertexCount(other.m_vertexCount), m_indexCount(other.m_indexCount),
      m_hasIndices(other.m_hasIndices), m_boundsMin(other.m_boundsMin), m_boundsMax(other.m_boundsMax)
{
    other.m_vao = 0;
    other.m_vbo = 0;
    other.m_ebo = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        cleanup();
        m_vao = other.m_vao;
        m_vbo = other.m_vbo;
        m_ebo = other.m_ebo;
        m_vertexCount = other.m_vertexCount;
        m_indexCount = other.m_indexCount;
        m_hasIndices = other.m_hasIndices;
        m_boundsMin = other.m_boundsMin;
        m_boundsMax = other.m_boundsMax;
        other.m_vao = 0;
        other.m_vbo = 0;
        other.m_ebo = 0;
    }
    return *this;
}

void Mesh::upload(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {
    cleanup();

    m_vertexCount = static_cast<uint32_t>(vertices.size());
    m_indexCount = static_cast<uint32_t>(indices.size());
    m_hasIndices = !indices.empty();

    // Compute AABB
    if (!vertices.empty()) {
        m_boundsMin = vertices[0].position;
        m_boundsMax = vertices[0].position;
        for (const auto& v : vertices) {
            m_boundsMin = glm::min(m_boundsMin, v.position);
            m_boundsMax = glm::max(m_boundsMax, v.position);
        }
    }

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)),
        vertices.data(), GL_STATIC_DRAW);

    if (m_hasIndices) {
        glGenBuffers(1, &m_ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(indices.size() * sizeof(uint32_t)),
            indices.data(), GL_STATIC_DRAW);
    }

    // Position (location 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        reinterpret_cast<void*>(offsetof(Vertex, position)));

    // Normal (location 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        reinterpret_cast<void*>(offsetof(Vertex, normal)));

    // TexCoord (location 2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        reinterpret_cast<void*>(offsetof(Vertex, texCoord)));

    // Color (location 3)
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        reinterpret_cast<void*>(offsetof(Vertex, color)));

    glBindVertexArray(0);
}

void Mesh::uploadVerticesOnly(const std::vector<Vertex>& vertices) {
    upload(vertices, {});
}

void Mesh::draw(GLenum mode) const {
    if (m_vao == 0) return;

    glBindVertexArray(m_vao);
    if (m_hasIndices) {
        glDrawElements(mode, static_cast<GLsizei>(m_indexCount), GL_UNSIGNED_INT, nullptr);
    } else {
        glDrawArrays(mode, 0, static_cast<GLsizei>(m_vertexCount));
    }
    glBindVertexArray(0);
}

void Mesh::cleanup() {
    if (m_ebo) { glDeleteBuffers(1, &m_ebo); m_ebo = 0; }
    if (m_vbo) { glDeleteBuffers(1, &m_vbo); m_vbo = 0; }
    if (m_vao) { glDeleteVertexArrays(1, &m_vao); m_vao = 0; }
}

} // namespace ap3d
