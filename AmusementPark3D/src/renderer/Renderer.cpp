#include "renderer/Renderer.h"
#include "camera/Camera.h"

namespace ap3d {

// ===================== Frustum =====================

void Frustum::extract(const glm::mat4& vp) {
    // Left
    planes[0] = glm::vec4(vp[0][3]+vp[0][0], vp[1][3]+vp[1][0], vp[2][3]+vp[2][0], vp[3][3]+vp[3][0]);
    // Right
    planes[1] = glm::vec4(vp[0][3]-vp[0][0], vp[1][3]-vp[1][0], vp[2][3]-vp[2][0], vp[3][3]-vp[3][0]);
    // Bottom
    planes[2] = glm::vec4(vp[0][3]+vp[0][1], vp[1][3]+vp[1][1], vp[2][3]+vp[2][1], vp[3][3]+vp[3][1]);
    // Top
    planes[3] = glm::vec4(vp[0][3]-vp[0][1], vp[1][3]-vp[1][1], vp[2][3]-vp[2][1], vp[3][3]-vp[3][1]);
    // Near
    planes[4] = glm::vec4(vp[0][3]+vp[0][2], vp[1][3]+vp[1][2], vp[2][3]+vp[2][2], vp[3][3]+vp[3][2]);
    // Far
    planes[5] = glm::vec4(vp[0][3]-vp[0][2], vp[1][3]-vp[1][2], vp[2][3]-vp[2][2], vp[3][3]-vp[3][2]);

    // Normalize
    for (auto& p : planes) {
        float len = glm::length(glm::vec3(p));
        if (len > 0.0001f) p /= len;
    }
}

bool Frustum::testAABB(const glm::vec3& aabbMin, const glm::vec3& aabbMax) const {
    for (const auto& plane : planes) {
        glm::vec3 p(
            plane.x > 0 ? aabbMax.x : aabbMin.x,
            plane.y > 0 ? aabbMax.y : aabbMin.y,
            plane.z > 0 ? aabbMax.z : aabbMin.z
        );
        if (glm::dot(glm::vec3(plane), p) + plane.w < 0.0f) {
            return false;
        }
    }
    return true;
}

// ===================== Renderer =====================

static const char* VERT_SHADER_SRC = R"glsl(
#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aColor;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat3 uNormalMatrix;

out vec3 vFragPos;
out vec3 vNormal;
out vec2 vTexCoord;
out vec3 vColor;
out float vFogFactor;

uniform float uFogStart;
uniform float uFogEnd;

void main() {
    vec4 worldPos = uModel * vec4(aPos, 1.0);
    vFragPos = worldPos.xyz;
    vNormal = normalize(uNormalMatrix * aNormal);
    vTexCoord = aTexCoord;
    vColor = aColor;

    vec4 viewPos = uView * worldPos;
    float dist = length(viewPos.xyz);
    vFogFactor = clamp((uFogEnd - dist) / (uFogEnd - uFogStart), 0.0, 1.0);

    gl_Position = uProjection * viewPos;
}
)glsl";

static const char* FRAG_SHADER_SRC = R"glsl(
#version 460 core
in vec3 vFragPos;
in vec3 vNormal;
in vec2 vTexCoord;
in vec3 vColor;
in float vFogFactor;

out vec4 FragColor;

// Material
uniform vec3 uMatAmbient;
uniform vec3 uMatDiffuse;
uniform vec3 uMatSpecular;
uniform float uMatShininess;
uniform bool uUseVertexColor;
uniform sampler2D uDiffuseMap;
uniform sampler2D uSpecularMap;
uniform bool uHasDiffuseMap;
uniform bool uHasSpecularMap;

// View position
uniform vec3 uViewPos;

// Directional light (sun)
struct DirLight {
    vec3 direction;
    vec3 color;
    float ambient;
    float diffuse;
    float specular;
};
uniform DirLight uSunLight;

// Point lights
#define MAX_POINT_LIGHTS 16
struct PointLight {
    vec3 position;
    vec3 color;
    float ambient;
    float diffuse;
    float specular;
    float constant;
    float linear;
    float quadratic;
    bool enabled;
};
uniform PointLight uPointLights[MAX_POINT_LIGHTS];
uniform int uNumPointLights;

// Fog
uniform vec3 uFogColor;

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 baseColor, vec3 specColor) {
    vec3 lightDir = normalize(-light.direction);
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), uMatShininess);

    vec3 ambient  = light.ambient  * light.color * baseColor;
    vec3 diffuse  = light.diffuse  * diff * light.color * baseColor;
    vec3 specular = light.specular * spec * light.color * specColor;

    return ambient + diffuse + specular;
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 baseColor, vec3 specColor) {
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), uMatShininess);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    vec3 ambient  = light.ambient  * light.color * baseColor * attenuation;
    vec3 diffuse  = light.diffuse  * diff * light.color * baseColor * attenuation;
    vec3 specular = light.specular * spec * light.color * specColor * attenuation;

    return ambient + diffuse + specular;
}

void main() {
    vec3 norm = normalize(vNormal);
    vec3 viewDir = normalize(uViewPos - vFragPos);

    // Base color
    vec3 baseColor = uUseVertexColor ? vColor * uMatDiffuse : uMatDiffuse;
    if (uHasDiffuseMap) {
        baseColor *= texture(uDiffuseMap, vTexCoord).rgb;
    }

    // Specular color
    vec3 specColor = uMatSpecular;
    if (uHasSpecularMap) {
        specColor *= texture(uSpecularMap, vTexCoord).rgb;
    }

    // Lighting
    vec3 result = calcDirLight(uSunLight, norm, viewDir, baseColor, specColor);

    for (int i = 0; i < uNumPointLights && i < MAX_POINT_LIGHTS; i++) {
        if (uPointLights[i].enabled) {
            result += calcPointLight(uPointLights[i], norm, vFragPos, viewDir, baseColor, specColor);
        }
    }

    // Fog
    result = mix(uFogColor, result, vFogFactor);

    // Tone mapping (simple Reinhard)
    result = result / (result + vec3(1.0));
    // Gamma correction
    result = pow(result, vec3(1.0 / 2.2));

    FragColor = vec4(result, 1.0);
}
)glsl";

bool Renderer::init() {
    if (!m_lightingShader.loadFromSource(VERT_SHADER_SRC, FRAG_SHADER_SRC)) {
        std::cerr << "[Renderer] Failed to compile lighting shader\n";
        return false;
    }

    // Create default white texture
    m_whiteTexture.createSolidColor(1.0f, 1.0f, 1.0f, 1.0f);

    // OpenGL state
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    std::cout << "[Renderer] Initialized successfully\n";
    return true;
}

void Renderer::beginFrame(const Camera& camera, float aspectRatio) {
    m_view = camera.viewMatrix();
    m_projection = camera.projectionMatrix(aspectRatio);
    m_viewPos = camera.position();
    m_frustum.extract(m_projection * m_view);
    m_commands.clear();

    glClearColor(m_skyColor.r, m_skyColor.g, m_skyColor.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::submit(const RenderCommand& cmd) {
    m_commands.push_back(cmd);
}

void Renderer::endFrame() {
    m_lightingShader.use();
    m_lightingShader.setMat4("uView", m_view);
    m_lightingShader.setMat4("uProjection", m_projection);
    m_lightingShader.setVec3("uViewPos", m_viewPos);

    // Sun light
    m_lightingShader.setVec3("uSunLight.direction", m_sunLight.direction);
    m_lightingShader.setVec3("uSunLight.color", m_sunLight.color);
    m_lightingShader.setFloat("uSunLight.ambient", m_sunLight.ambient);
    m_lightingShader.setFloat("uSunLight.diffuse", m_sunLight.diffuse);
    m_lightingShader.setFloat("uSunLight.specular", m_sunLight.specular);

    // Point lights
    int numLights = std::min(static_cast<int>(m_pointLights.size()), 16);
    m_lightingShader.setInt("uNumPointLights", numLights);
    for (int i = 0; i < numLights; i++) {
        std::string prefix = "uPointLights[" + std::to_string(i) + "].";
        m_lightingShader.setVec3(prefix + "position", m_pointLights[i].position);
        m_lightingShader.setVec3(prefix + "color", m_pointLights[i].color);
        m_lightingShader.setFloat(prefix + "ambient", m_pointLights[i].ambient);
        m_lightingShader.setFloat(prefix + "diffuse", m_pointLights[i].diffuse);
        m_lightingShader.setFloat(prefix + "specular", m_pointLights[i].specular);
        m_lightingShader.setFloat(prefix + "constant", m_pointLights[i].constant);
        m_lightingShader.setFloat(prefix + "linear", m_pointLights[i].linear);
        m_lightingShader.setFloat(prefix + "quadratic", m_pointLights[i].quadratic);
        m_lightingShader.setBool(prefix + "enabled", m_pointLights[i].enabled);
    }

    // Fog
    m_lightingShader.setVec3("uFogColor", m_fogColor);
    m_lightingShader.setFloat("uFogStart", m_fogStart);
    m_lightingShader.setFloat("uFogEnd", m_fogEnd);

    int drawCalls = 0;

    for (const auto& cmd : m_commands) {
        if (!cmd.mesh || !cmd.mesh->isValid()) continue;

        // Frustum culling - transform AABB correctly by transforming all 8 corners
        glm::vec3 originalMin = cmd.mesh->boundsMin();
        glm::vec3 originalMax = cmd.mesh->boundsMax();
        glm::vec3 corners[8] = {
            {originalMin.x, originalMin.y, originalMin.z},
            {originalMax.x, originalMin.y, originalMin.z},
            {originalMin.x, originalMax.y, originalMin.z},
            {originalMax.x, originalMax.y, originalMin.z},
            {originalMin.x, originalMin.y, originalMax.z},
            {originalMax.x, originalMin.y, originalMax.z},
            {originalMin.x, originalMax.y, originalMax.z},
            {originalMax.x, originalMax.y, originalMax.z}
        };

        glm::vec3 aabbMin(1e9f);
        glm::vec3 aabbMax(-1e9f);
        for (int c = 0; c < 8; c++) {
            glm::vec3 p = glm::vec3(cmd.transform * glm::vec4(corners[c], 1.0f));
            aabbMin = glm::min(aabbMin, p);
            aabbMax = glm::max(aabbMax, p);
        }

        if (!m_frustum.testAABB(aabbMin, aabbMax)) continue;

        // Set transform
        m_lightingShader.setMat4("uModel", cmd.transform);
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(cmd.transform)));
        m_lightingShader.setMat3("uNormalMatrix", normalMatrix);

        // Set material
        m_lightingShader.setVec3("uMatAmbient", cmd.material.ambient);
        m_lightingShader.setVec3("uMatDiffuse", cmd.material.diffuse);
        m_lightingShader.setVec3("uMatSpecular", cmd.material.specular);
        m_lightingShader.setFloat("uMatShininess", cmd.material.shininess);
        m_lightingShader.setBool("uUseVertexColor", cmd.material.useVertexColor);

        // Bind textures
        if (cmd.material.diffuseMap && cmd.material.diffuseMap->isValid()) {
            cmd.material.diffuseMap->bind(0);
            m_lightingShader.setInt("uDiffuseMap", 0);
            m_lightingShader.setBool("uHasDiffuseMap", true);
        } else {
            m_whiteTexture.bind(0);
            m_lightingShader.setInt("uDiffuseMap", 0);
            m_lightingShader.setBool("uHasDiffuseMap", false);
        }

        if (cmd.material.specularMap && cmd.material.specularMap->isValid()) {
            cmd.material.specularMap->bind(1);
            m_lightingShader.setInt("uSpecularMap", 1);
            m_lightingShader.setBool("uHasSpecularMap", true);
        } else {
            m_lightingShader.setBool("uHasSpecularMap", false);
        }

        if (cmd.disableCulling) {
            glDisable(GL_CULL_FACE);
        }

        if (cmd.wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }

        cmd.mesh->draw();
        drawCalls++;

        if (cmd.wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        if (cmd.disableCulling) {
            glEnable(GL_CULL_FACE);
        }
    }

    m_lastDrawCalls = drawCalls;
}

void Renderer::addPointLight(const Light& light) {
    if (m_pointLights.size() < 16) {
        m_pointLights.push_back(light);
    }
}

void Renderer::setFog(const glm::vec3& color, float start, float end) {
    m_fogColor = color;
    m_fogStart = start;
    m_fogEnd = end;
}

} // namespace ap3d
