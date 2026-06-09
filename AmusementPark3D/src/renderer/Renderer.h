#pragma once
#include "core/Common.h"
#include "renderer/Shader.h"
#include "renderer/Mesh.h"
#include "renderer/Texture.h"

namespace ap3d {

class Camera;

/// Light definition for Blinn-Phong shading.
struct Light {
    glm::vec3 position{0.0f, 10.0f, 0.0f};
    glm::vec3 color{1.0f};
    float ambient = 0.15f;
    float diffuse = 0.8f;
    float specular = 0.5f;
    float constant = 1.0f;
    float linear = 0.045f;
    float quadratic = 0.0075f;
    bool enabled = true;
};

/// Directional light (sun).
struct DirectionalLight {
    glm::vec3 direction{-0.3f, -1.0f, -0.5f};
    glm::vec3 color{1.0f, 0.95f, 0.85f};
    float ambient = 0.2f;
    float diffuse = 0.9f;
    float specular = 0.4f;
};

/// Material properties.
struct Material {
    glm::vec3 ambient{0.2f};
    glm::vec3 diffuse{0.8f};
    glm::vec3 specular{0.3f};
    float shininess = 32.0f;
    bool useVertexColor = true;
    const Texture* diffuseMap = nullptr;
    const Texture* specularMap = nullptr;
};

/// Render command for batched drawing.
struct RenderCommand {
    const Mesh* mesh = nullptr;
    glm::mat4 transform{1.0f};
    Material material;
    bool wireframe = false;
    bool disableCulling = false;
};

/// Simple frustum for culling.
struct Frustum {
    glm::vec4 planes[6]; // left, right, bottom, top, near, far

    void extract(const glm::mat4& viewProj);
    [[nodiscard]] bool testAABB(const glm::vec3& min, const glm::vec3& max) const;
};

/// Main rendering system with Blinn-Phong lighting.
class Renderer {
public:
    Renderer() = default;

    /// Initialize shaders and render state.
    bool init();

    /// Begin a new frame.
    void beginFrame(const Camera& camera, float aspectRatio);

    /// Submit a render command for this frame.
    void submit(const RenderCommand& cmd);

    /// Execute all submitted draw calls.
    void endFrame();

    /// Set the sun/directional light.
    void setSunLight(const DirectionalLight& light) { m_sunLight = light; }

    /// Add a point light.
    void addPointLight(const Light& light);

    /// Clear all point lights.
    void clearPointLights() { m_pointLights.clear(); }

    /// Set fog parameters.
    void setFog(const glm::vec3& color, float start, float end);

    /// Set sky color.
    void setSkyColor(const glm::vec3& color) { m_skyColor = color; }

    /// Get the default white texture.
    [[nodiscard]] const Texture& whiteTexture() const { return m_whiteTexture; }

    /// Get camera position.
    [[nodiscard]] const glm::vec3& cameraPosition() const { return m_viewPos; }

    /// Get draw call count for previous frame.
    [[nodiscard]] int drawCalls() const { return m_lastDrawCalls; }

    /// Get the lighting shader (for external setup).
    [[nodiscard]] Shader& lightingShader() { return m_lightingShader; }

private:
    Shader m_lightingShader;
    Texture m_whiteTexture;

    DirectionalLight m_sunLight;
    std::vector<Light> m_pointLights;
    std::vector<RenderCommand> m_commands;

    glm::mat4 m_view{1.0f};
    glm::mat4 m_projection{1.0f};
    glm::vec3 m_viewPos{0.0f};
    Frustum m_frustum;

    glm::vec3 m_skyColor{0.4f, 0.6f, 0.9f};
    glm::vec3 m_fogColor{0.5f, 0.6f, 0.8f};
    float m_fogStart = 80.0f;
    float m_fogEnd = 200.0f;

    int m_lastDrawCalls = 0;
};

} // namespace ap3d
