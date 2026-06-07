// =============================================================================
// animation.h - Animation state machine for 3D Space Station Survival
// Controls: doors, robotic arm, rotating machinery, alarm, spacecraft
// =============================================================================
#ifndef ANIMATION_H
#define ANIMATION_H

#include <GL/glut.h>
#include <cmath>

// ─── Global pause flag ────────────────────────────────────────────────────────
static bool g_animPaused = false;

// ─── Rotation angles for continuous objects ───────────────────────────────────
static float g_radarAngle      = 0.0f;   // radar dish yaw
static float g_ringAngle       = 0.0f;   // station ring
static float g_solarAngle      = 0.0f;   // solar panel tilt
static float g_turbineAngle    = 0.0f;   // turbine/fan spin
static float g_satelliteAngle  = 0.0f;   // satellite orbit
static float g_asteroidAngle   = 0.0f;   // asteroid belt

// ─── Robotic arm joint angles ─────────────────────────────────────────────────
static float g_armBase     = 0.0f;   // base rotation
static float g_armShoulder = 30.0f;  // shoulder pitch
static float g_armElbow    = -45.0f; // elbow pitch
static float g_armWrist    = 20.0f;  // wrist roll

static float g_armBaseDir     =  1.0f;
static float g_armShoulderDir =  1.0f;
static float g_armElbowDir    = -1.0f;
static float g_armWristDir    =  1.0f;

// ─── Door state ───────────────────────────────────────────────────────────────
enum DoorState { DOOR_CLOSED, DOOR_OPENING, DOOR_OPEN, DOOR_CLOSING };

struct Door {
    DoorState state;
    float     openAmount;   // 0.0 = closed, 1.0 = fully open
    float     speed;        // units per second
};

static const int NUM_DOORS = 4;
static Door g_doors[NUM_DOORS];

inline void Anim_InitDoors() {
    for (int i = 0; i < NUM_DOORS; i++) {
        g_doors[i].state      = DOOR_CLOSED;
        g_doors[i].openAmount = 0.0f;
        g_doors[i].speed      = 0.8f;
    }
}

// ─── Alarm / flickering state ─────────────────────────────────────────────────
static float g_alarmTimer     = 0.0f;
static bool  g_alarmActive    = true;
static float g_alarmIntensity = 0.0f; // 0..1 drives red overlay in HUD

// ─── Spacecraft / escape pod movement ────────────────────────────────────────
static float g_shuttleX   = 60.0f;
static float g_shuttleZ   = 0.0f;
static float g_shuttleDir = -1.0f; // moving direction

// ─── Particle state (debris/smoke) ───────────────────────────────────────────
#define MAX_PARTICLES 200
struct Particle {
    float x, y, z;
    float vx, vy, vz;
    float life;      // 0..1
    float r, g, b;
};
static Particle g_particles[MAX_PARTICLES];
static int g_particleCount = 0;

// ─── Spawn a particle at position ────────────────────────────────────────────
inline void Particle_Spawn(float px, float py, float pz,
                            float r, float g, float b) {
    if (g_particleCount >= MAX_PARTICLES) return;
    Particle& p = g_particles[g_particleCount++];
    auto rand01 = []() { return (rand() % 1000) / 1000.0f; };
    p.x = px; p.y = py; p.z = pz;
    p.vx = (rand01() - 0.5f) * 0.05f;
    p.vy = rand01() * 0.03f + 0.01f;
    p.vz = (rand01() - 0.5f) * 0.05f;
    p.life = 1.0f;
    p.r = r; p.g = g; p.b = b;
}

// ─── Master update (called every frame) ──────────────────────────────────────
inline void Anim_Update(float dt) {
    if (g_animPaused) return;

    // ── Continuous rotators ──
    g_radarAngle     += dt * 60.0f;   // 60°/s
    g_ringAngle      += dt * 8.0f;    // slow ring
    g_solarAngle     += dt * 15.0f;   // solar panels nod
    g_turbineAngle   += dt * 360.0f;  // fast turbine
    g_satelliteAngle += dt * 5.0f;    // orbit
    g_asteroidAngle  += dt * 3.0f;    // belt rotation

    if (g_radarAngle     > 360.0f) g_radarAngle     -= 360.0f;
    if (g_ringAngle      > 360.0f) g_ringAngle      -= 360.0f;
    if (g_solarAngle     > 360.0f) g_solarAngle     -= 360.0f;
    if (g_turbineAngle   > 360.0f) g_turbineAngle   -= 360.0f;
    if (g_satelliteAngle > 360.0f) g_satelliteAngle -= 360.0f;
    if (g_asteroidAngle  > 360.0f) g_asteroidAngle  -= 360.0f;

    // ── Robotic arm oscillation ──
    g_armBase     += dt * 20.0f * g_armBaseDir;
    g_armShoulder += dt * 15.0f * g_armShoulderDir;
    g_armElbow    += dt * 25.0f * g_armElbowDir;
    g_armWrist    += dt * 35.0f * g_armWristDir;

    if (g_armBase >  45.0f || g_armBase < -45.0f)     g_armBaseDir     *= -1.0f;
    if (g_armShoulder > 60.0f || g_armShoulder < 10.0f) g_armShoulderDir *= -1.0f;
    if (g_armElbow > -20.0f || g_armElbow < -80.0f)   g_armElbowDir    *= -1.0f;
    if (g_armWrist >  45.0f || g_armWrist < -45.0f)   g_armWristDir    *= -1.0f;

    // ── Door animation ──
    for (int i = 0; i < NUM_DOORS; i++) {
        Door& d = g_doors[i];
        if (d.state == DOOR_OPENING) {
            d.openAmount += dt * d.speed;
            if (d.openAmount >= 1.0f) {
                d.openAmount = 1.0f;
                d.state = DOOR_OPEN;
            }
        } else if (d.state == DOOR_CLOSING) {
            d.openAmount -= dt * d.speed;
            if (d.openAmount <= 0.0f) {
                d.openAmount = 0.0f;
                d.state = DOOR_CLOSED;
            }
        }
    }

    // ── Alarm pulse ──
    if (g_alarmActive) {
        g_alarmTimer += dt;
        g_alarmIntensity = 0.5f + 0.5f * sinf(g_alarmTimer * 5.0f); // 2.5 Hz
    } else {
        g_alarmIntensity = 0.0f;
        g_alarmTimer = 0.0f;
    }

    // ── Shuttle drift ──
    g_shuttleX += dt * 2.5f * g_shuttleDir;
    if (g_shuttleX > 80.0f || g_shuttleX < 40.0f) g_shuttleDir *= -1.0f;

    // ── Particles ──
    for (int i = 0; i < g_particleCount; ) {
        Particle& p = g_particles[i];
        p.x += p.vx; p.y += p.vy; p.z += p.vz;
        p.vy -= 0.001f; // weak gravity
        p.life -= dt * 0.5f;
        if (p.life <= 0.0f) {
            // Remove by swapping with last
            g_particles[i] = g_particles[--g_particleCount];
        } else {
            i++;
        }
    }

    // Continuously spawn smoke from reactor exhaust
    static float spawnTimer = 0.0f;
    spawnTimer += dt;
    if (spawnTimer > 0.05f) {
        Particle_Spawn(0.0f, 4.0f, -20.0f, 0.4f, 0.4f, 0.5f); // grey smoke
        spawnTimer = 0.0f;
    }
}

// ─── Door toggle ─────────────────────────────────────────────────────────────
inline void Anim_ToggleDoor(int idx) {
    if (idx < 0 || idx >= NUM_DOORS) return;
    Door& d = g_doors[idx];
    if (d.state == DOOR_CLOSED || d.state == DOOR_CLOSING)
        d.state = DOOR_OPENING;
    else
        d.state = DOOR_CLOSING;
}

// ─── Alarm toggle ────────────────────────────────────────────────────────────
inline void Anim_ToggleAlarm() { g_alarmActive = !g_alarmActive; }

// ─── Pause toggle ────────────────────────────────────────────────────────────
inline void Anim_TogglePause() { g_animPaused = !g_animPaused; }

#endif // ANIMATION_H
