#include "renderer/Texture.h"
#include "stb/stb_image.h"

namespace ap3d {

Texture::~Texture() {
    if (m_texture) {
        glDeleteTextures(1, &m_texture);
    }
}

Texture::Texture(Texture&& other) noexcept
    : m_texture(other.m_texture), m_width(other.m_width),
      m_height(other.m_height), m_channels(other.m_channels)
{
    other.m_texture = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        if (m_texture) glDeleteTextures(1, &m_texture);
        m_texture = other.m_texture;
        m_width = other.m_width;
        m_height = other.m_height;
        m_channels = other.m_channels;
        other.m_texture = 0;
    }
    return *this;
}

bool Texture::loadFromFile(const std::string& path, bool srgb) {
    stbi_set_flip_vertically_on_load(1);

    unsigned char* data = stbi_load(path.c_str(), &m_width, &m_height, &m_channels, 0);
    if (!data) {
        std::cerr << "[Texture] Failed to load: " << path << " - " << stbi_failure_reason() << "\n";
        return false;
    }

    GLenum internalFormat = GL_RGB8;
    GLenum dataFormat = GL_RGB;
    if (m_channels == 4) {
        internalFormat = srgb ? GL_SRGB8_ALPHA8 : GL_RGBA8;
        dataFormat = GL_RGBA;
    } else if (m_channels == 3) {
        internalFormat = srgb ? GL_SRGB8 : GL_RGB8;
        dataFormat = GL_RGB;
    } else if (m_channels == 1) {
        internalFormat = GL_R8;
        dataFormat = GL_RED;
    }

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    glTexImage2D(GL_TEXTURE_2D, 0, static_cast<GLint>(internalFormat),
        m_width, m_height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Anisotropic filtering if available
    float maxAniso = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAniso);
    if (maxAniso > 0.0f) {
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, std::min(maxAniso, 8.0f));
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);

    std::cout << "[Texture] Loaded: " << path << " (" << m_width << "x" << m_height
              << ", " << m_channels << " channels)\n";
    return true;
}

void Texture::createSolidColor(float r, float g, float b, float a) {
    unsigned char pixel[4] = {
        static_cast<unsigned char>(r * 255.0f),
        static_cast<unsigned char>(g * 255.0f),
        static_cast<unsigned char>(b * 255.0f),
        static_cast<unsigned char>(a * 255.0f)
    };

    m_width = 1;
    m_height = 1;
    m_channels = 4;

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::createCheckerboard(int size, int checkSize,
    const glm::vec3& color1, const glm::vec3& color2)
{
    m_width = size;
    m_height = size;
    m_channels = 3;

    std::vector<unsigned char> data(size * size * 3);

    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            bool isColor1 = ((x / checkSize) + (y / checkSize)) % 2 == 0;
            const auto& color = isColor1 ? color1 : color2;
            int idx = (y * size + x) * 3;
            data[idx + 0] = static_cast<unsigned char>(color.r * 255.0f);
            data[idx + 1] = static_cast<unsigned char>(color.g * 255.0f);
            data[idx + 2] = static_cast<unsigned char>(color.b * 255.0f);
        }
    }

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::createNoise(int size, const glm::vec3& baseColor, float noiseAmount, float granularity) {
    m_width = size;
    m_height = size;
    m_channels = 3;

    std::vector<unsigned char> data(size * size * 3);

    // Seed-based pseudo-random generator
    unsigned int seed = 12345;
    auto randomFloat = [&seed]() -> float {
        seed = seed * 1103515245 + 12345;
        return static_cast<float>((seed >> 16) & 0x7FFF) / 32767.0f;
    };

    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            // Base pixel color with small random deviations
            float factor = 1.0f + (randomFloat() - 0.5f) * noiseAmount;
            
            // Add low-frequency wave noise for larger variations (blotches)
            float fx = static_cast<float>(x) / size;
            float fy = static_cast<float>(y) / size;
            float wave = sinf(fx * 10.0f * granularity) * cosf(fy * 10.0f * granularity) * 0.05f;
            wave += sinf(fx * 25.0f * granularity + fy * 15.0f * granularity) * 0.03f;
            
            float finalFactor = glm::clamp(factor + wave, 0.0f, 1.0f);
            glm::vec3 color = baseColor * finalFactor;

            int idx = (y * size + x) * 3;
            data[idx + 0] = static_cast<unsigned char>(glm::clamp(color.r, 0.0f, 1.0f) * 255.0f);
            data[idx + 1] = static_cast<unsigned char>(glm::clamp(color.g, 0.0f, 1.0f) * 255.0f);
            data[idx + 2] = static_cast<unsigned char>(glm::clamp(color.b, 0.0f, 1.0f) * 255.0f);
        }
    }

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::bind(GLuint unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_texture);
}

void Texture::unbind(GLuint unit) {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, 0);
}

} // namespace ap3d
