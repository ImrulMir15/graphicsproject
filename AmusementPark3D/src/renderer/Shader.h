#pragma once
#include "core/Common.h"

namespace ap3d {

/// GLSL shader program wrapper supporting vertex + fragment shaders.
class Shader {
public:
    Shader() = default;
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    /// Compile and link shaders from source strings.
    bool loadFromSource(const std::string& vertexSrc, const std::string& fragmentSrc);

    /// Compile and link shaders from file paths.
    bool loadFromFiles(const std::string& vertexPath, const std::string& fragmentPath);

    /// Activate this shader program.
    void use() const;

    /// Get the OpenGL program ID.
    [[nodiscard]] GLuint id() const { return m_program; }

    // ---- Uniform setters ----
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setMat3(const std::string& name, const glm::mat3& value) const;
    void setMat4(const std::string& name, const glm::mat4& value) const;

private:
    [[nodiscard]] GLint getUniformLocation(const std::string& name) const;
    static bool compileShader(GLuint shader, const std::string& source);
    static std::string readFile(const std::string& path);

    GLuint m_program = 0;
    mutable std::unordered_map<std::string, GLint> m_uniformCache;
};

} // namespace ap3d
