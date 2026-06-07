// =============================================================================
// lighting.h - Multi-light system for 3D Space Station Survival
// Implements: Ambient, Diffuse, Specular, Emergency, Spotlights, Reactor glow
// =============================================================================
#ifndef LIGHTING_H
#define LIGHTING_H

#include <GL/glut.h>
#include <cmath>

// ─── Light toggle flags ───────────────────────────────────────────────────────
static bool g_mainLightsOn      = true;
static bool g_emergencyOn       = true;
static bool g_spotlightsOn      = true;
static bool g_reactorGlowOn     = true;
static bool g_flickerEnabled    = true;

// ─── Flicker / animation state ───────────────────────────────────────────────
static float g_flickerTimer     = 0.0f;
static float g_emergencyTimer   = 0.0f;   // drives red pulse
static float g_reactorPulse     = 0.0f;

// ─── Spotlight positions ──────────────────────────────────────────────────────
// 4 spotlights positioned at corridor junctions
static const float g_spotPos[4][4] = {
    { -10.0f, 8.0f,  0.0f, 1.0f },
    {  10.0f, 8.0f,  0.0f, 1.0f },
    {   0.0f, 8.0f, -15.0f, 1.0f },
    {   0.0f, 8.0f,  15.0f, 1.0f }
};
static const float g_spotDir[4][3] = {
    { 0.0f, -1.0f,  0.3f },
    { 0.0f, -1.0f, -0.3f },
    { 0.3f, -1.0f,  0.0f },
    {-0.3f, -1.0f,  0.0f }
};

// ─── Initialize the full lighting system ─────────────────────────────────────
inline void Lighting_Init() {
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_SMOOTH);

    // Global ambient (dim space-station feel)
    float globalAmb[] = { 0.08f, 0.08f, 0.12f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmb);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,     GL_TRUE);

    // ── GL_LIGHT0 : Main overhead white light ──
    float amb0[]  = { 0.15f, 0.15f, 0.20f, 1.0f };
    float dif0[]  = { 0.80f, 0.85f, 1.00f, 1.0f };
    float spec0[] = { 1.00f, 1.00f, 1.00f, 1.0f };
    float pos0[]  = { 0.0f, 12.0f, 0.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT,  amb0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  dif0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, spec0);
    glLightfv(GL_LIGHT0, GL_POSITION, pos0);
    glEnable(GL_LIGHT0);

    // ── GL_LIGHT1 : Emergency red pulsing light ──
    float amb1[]  = { 0.30f, 0.02f, 0.02f, 1.0f };
    float dif1[]  = { 1.00f, 0.05f, 0.05f, 1.0f };
    float spec1[] = { 1.00f, 0.10f, 0.10f, 1.0f };
    float pos1[]  = { 5.0f, 10.0f, 5.0f, 1.0f };
    glLightfv(GL_LIGHT1, GL_AMBIENT,  amb1);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  dif1);
    glLightfv(GL_LIGHT1, GL_SPECULAR, spec1);
    glLightfv(GL_LIGHT1, GL_POSITION, pos1);
    glEnable(GL_LIGHT1);

    // ── GL_LIGHT2 : Reactor blue-green glow ──
    float amb2[]  = { 0.00f, 0.10f, 0.20f, 1.0f };
    float dif2[]  = { 0.10f, 0.80f, 1.00f, 1.0f };
    float spec2[] = { 0.20f, 1.00f, 1.00f, 1.0f };
    float pos2[]  = { 0.0f, 2.0f, -20.0f, 1.0f };
    glLightfv(GL_LIGHT2, GL_AMBIENT,  amb2);
    glLightfv(GL_LIGHT2, GL_DIFFUSE,  dif2);
    glLightfv(GL_LIGHT2, GL_SPECULAR, spec2);
    glLightfv(GL_LIGHT2, GL_POSITION, pos2);
    glLightf (GL_LIGHT2, GL_CONSTANT_ATTENUATION,  0.5f);
    glLightf (GL_LIGHT2, GL_LINEAR_ATTENUATION,    0.05f);
    glLightf (GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.005f);
    glEnable(GL_LIGHT2);

    // ── GL_LIGHT3-6 : Spotlights ──
    for (int i = 0; i < 4; i++) {
        GLenum light = GL_LIGHT3 + i;
        float spotAmb[]  = { 0.05f, 0.05f, 0.10f, 1.0f };
        float spotDif[]  = { 0.70f, 0.70f, 0.90f, 1.0f };
        float spotSpec[] = { 0.80f, 0.80f, 1.00f, 1.0f };
        glLightfv(light, GL_AMBIENT,              spotAmb);
        glLightfv(light, GL_DIFFUSE,              spotDif);
        glLightfv(light, GL_SPECULAR,             spotSpec);
        glLightfv(light, GL_POSITION,             g_spotPos[i]);
        glLightfv(light, GL_SPOT_DIRECTION,       g_spotDir[i]);
        glLightf (light, GL_SPOT_CUTOFF,          35.0f);
        glLightf (light, GL_SPOT_EXPONENT,        6.0f);
        glLightf (light, GL_CONSTANT_ATTENUATION, 1.0f);
        glLightf (light, GL_LINEAR_ATTENUATION,   0.02f);
        glEnable(light);
    }
}

// ─── Update dynamic lights each frame ────────────────────────────────────────
inline void Lighting_Update(float dt) {
    g_flickerTimer   += dt;
    g_emergencyTimer += dt;
    g_reactorPulse   += dt;

    // ── Flicker: main light occasionally dims ──
    if (g_flickerEnabled && g_mainLightsOn) {
        float flicker = 1.0f;
        // pseudo-random flicker using sine composition
        float f = sinf(g_flickerTimer * 17.3f) *
                  sinf(g_flickerTimer *  7.1f);
        if (f > 0.85f) flicker = 0.3f; // sudden dim

        float dif0[] = {
            0.80f * flicker,
            0.85f * flicker,
            1.00f * flicker,
            1.0f
        };
        glLightfv(GL_LIGHT0, GL_DIFFUSE, dif0);
    }

    // ── Emergency: pulsing red ──
    if (g_emergencyOn) {
        float pulse = 0.5f + 0.5f * sinf(g_emergencyTimer * 4.0f); // 2 Hz
        float dif1[] = { pulse, 0.03f, 0.03f, 1.0f };
        glLightfv(GL_LIGHT1, GL_DIFFUSE, dif1);
        glEnable(GL_LIGHT1);
    } else {
        glDisable(GL_LIGHT1);
    }

    // ── Reactor glow: slow throb ──
    if (g_reactorGlowOn) {
        float throb = 0.6f + 0.4f * sinf(g_reactorPulse * 1.5f);
        float dif2[] = { 0.10f * throb, 0.80f * throb, throb, 1.0f };
        glLightfv(GL_LIGHT2, GL_DIFFUSE, dif2);
        glEnable(GL_LIGHT2);
    } else {
        glDisable(GL_LIGHT2);
    }

    // ── Spotlights ──
    for (int i = 0; i < 4; i++) {
        GLenum light = GL_LIGHT3 + i;
        if (g_spotlightsOn) glEnable(light);
        else                glDisable(light);
    }

    // ── Main lights ──
    if (!g_mainLightsOn) glDisable(GL_LIGHT0);
    else                  glEnable(GL_LIGHT0);
}

// ─── Material helpers ─────────────────────────────────────────────────────────
inline void SetMaterial_Metal() {
    float amb[]  = { 0.20f, 0.20f, 0.25f, 1.0f };
    float dif[]  = { 0.55f, 0.55f, 0.60f, 1.0f };
    float spec[] = { 0.90f, 0.90f, 0.95f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   amb);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   dif);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  spec);
    glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 80.0f);
}

inline void SetMaterial_GlowingBlue() {
    float emis[] = { 0.0f, 0.4f, 0.8f, 1.0f };
    float spec[] = { 0.5f, 1.0f, 1.0f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,  emis);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  spec);
    glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 120.0f);
}

inline void SetMaterial_GlowingRed() {
    float emis[] = { 0.8f, 0.0f, 0.0f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emis);
    glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 60.0f);
}

inline void SetMaterial_Reset() {
    float zero[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    float ones[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,  zero);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  zero);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   ones);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   ones);
    glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 10.0f);
}

// ─── Toggle helpers (called from input handler) ───────────────────────────────
inline void Lighting_ToggleMain()       { g_mainLightsOn   = !g_mainLightsOn; }
inline void Lighting_ToggleEmergency()  { g_emergencyOn    = !g_emergencyOn; }
inline void Lighting_ToggleSpotlights() { g_spotlightsOn   = !g_spotlightsOn; }
inline void Lighting_ToggleReactor()    { g_reactorGlowOn  = !g_reactorGlowOn; }
inline void Lighting_ToggleFlicker()    { g_flickerEnabled = !g_flickerEnabled; }

#endif // LIGHTING_H
