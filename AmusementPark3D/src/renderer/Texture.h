#pragma once
#include "core/Common.h"

namespace ap3d {

/// OpenGL 2D texture wrapper with RAII.
class Texture {
public:
    Texture() = default;
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    /// Load a texture from file using stb_image.
    bool loadFromFile(const std::string& path, bool srgb = true);

    /// Create a solid color 1x1 texture.
    void createSolidColor(float r, float g, float b, float a = 1.0f);

    /// Create a procedural checkerboard texture.
    void createCheckerboard(int size, int checkSize,
        const glm::vec3& color1, const glm::vec3& color2);

    /// Create a procedural noise/granularity texture.
    void createNoise(int size, const glm::vec3& baseColor, float noiseAmount, float granularity = 1.0f);

    /// Bind the texture to the given unit.
    void bind(GLuint unit = 0) const;

    /// Unbind.
    static void unbind(GLuint unit = 0);

    [[nodiscard]] GLuint id() const { return m_texture; }
    [[nodiscard]] int width() const { return m_width; }
    [[nodiscard]] int height() const { return m_height; }
    [[nodiscard]] bool isValid() const { return m_texture != 0; }

private:
    GLuint m_texture = 0;
    int m_width = 0;
    int m_height = 0;
    int m_channels = 0;
};

} // namespace ap3d
