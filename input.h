// =============================================================================
// input.h - Keyboard and Mouse Input Handler for 3D Space Station Survival
// Handles: WASD movement, arrow camera, mode switches, toggles
// =============================================================================
#ifndef INPUT_H
#define INPUT_H

#include <GL/glut.h>
#include "camera.h"
#include "lighting.h"
#include "animation.h"

// ─── Key state array (for smooth held-key movement) ───────────────────────────
static bool g_keys[256] = { false };
static bool g_specialKeys[256] = { false };

// ─── Mouse state ─────────────────────────────────────────────────────────────
static int  g_lastMouseX = -1;
static int  g_lastMouseY = -1;
static bool g_mouseCaptured = true;
static int  g_windowW = 1280;
static int  g_windowH = 720;

// ─── Survival / gameplay state ───────────────────────────────────────────────
static float g_oxygenLevel    = 100.0f;  // 0..100
static float g_reactorHealth  = 100.0f;  // 0..100
static bool  g_reactorMode    = true;    // Starts in failure mode!
static bool  g_gameOver       = false;
static bool  g_missionCompleted = false; // New mission state
static float g_missionTimer   = 0.0f;   // seconds survived
static float g_reactorCooldownTimer = 0.0f; // Time since reactor was shut down

// ─── Key down ────────────────────────────────────────────────────────────────
inline void Input_KeyDown(unsigned char key, int /*x*/, int /*y*/) {
    g_keys[key] = true;

    switch (key) {
    // Camera mode switching
    case '1': Camera_SetMode(CAM_FIRST_PERSON);  break;
    case '2': Camera_SetMode(CAM_THIRD_PERSON);  break;
    case '3': Camera_SetMode(CAM_FREE);          break;

    // Lighting toggles
    case 'l': case 'L': Lighting_ToggleMain();      break;
    case 'e': case 'E': Lighting_ToggleEmergency(); break;
    case 'k': case 'K': Lighting_ToggleSpotlights(); break;
    case 'f': case 'F': Lighting_ToggleFlicker();   break;

    // Reactor interaction (only when near the reactor room Z < -15)
    case 'r': case 'R':
        if (g_camera.posZ < -15.0f && abs((int)g_camera.posX) < 15) {
            g_reactorMode = !g_reactorMode;
            Lighting_ToggleReactor();
            if (!g_reactorMode) {
                g_missionCompleted = true; // Shut down successfully!
                g_alarmActive = false;     // Turn off alarms
                g_reactorCooldownTimer = 0.0f; // Reset cooldown
            }
        }
        break;

    // Door interaction (nearest door)
    case 'o': case 'O':
        Anim_ToggleDoor(0); // toggle first door; scene picks nearest
        break;

    // Alarm
    case 'g': case 'G': Anim_ToggleAlarm(); break;

    // Pause
    case 'p': case 'P': Anim_TogglePause(); break;

    // Mouse capture toggle
    case 'm': case 'M':
        g_mouseCaptured = !g_mouseCaptured;
        if (g_mouseCaptured)
            glutSetCursor(GLUT_CURSOR_NONE);
        else
            glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
        break;

    // Exit
    case 27: exit(0); break; // ESC

    default: break;
    }
}

// ─── Key up ──────────────────────────────────────────────────────────────────
inline void Input_KeyUp(unsigned char key, int /*x*/, int /*y*/) {
    g_keys[key] = false;
}

// ─── Special keys ─────────────────────────────────────────────────────────────
inline void Input_SpecialDown(int key, int /*x*/, int /*y*/) {
    g_specialKeys[key] = true;
}
inline void Input_SpecialUp(int key, int /*x*/, int /*y*/) {
    g_specialKeys[key] = false;
}

// ─── Mouse motion (passive = mouse captured / FPS look) ──────────────────────
inline void Input_MouseMotion(int x, int y) {
    if (!g_mouseCaptured) { g_lastMouseX = x; g_lastMouseY = y; return; }
    if (g_lastMouseX == -1) { g_lastMouseX = x; g_lastMouseY = y; return; }

    int dx = x - g_lastMouseX;
    int dy = y - g_lastMouseY;

    // Warp back to center to keep mouse "grabbed"
    int cx = g_windowW / 2, cy = g_windowH / 2;
    if (abs(dx) > 1 || abs(dy) > 1) {
        Camera_RotateYaw  ((float)dx * g_camera.sensitivity);
        Camera_RotatePitch((float)(-dy) * g_camera.sensitivity);
        glutWarpPointer(cx, cy);
        g_lastMouseX = cx;
        g_lastMouseY = cy;
    }
}

inline void Input_PassiveMotion(int x, int y) {
    Input_MouseMotion(x, y);
}

// ─── Process held keys (call once per frame) ─────────────────────────────────
inline void Input_ProcessHeldKeys() {
    float speed = g_camera.moveSpeed;

    if (g_keys['w'] || g_keys['W']) Camera_MoveForward (speed);
    if (g_keys['s'] || g_keys['S']) Camera_MoveBackward(speed);
    if (g_keys['a'] || g_keys['A']) Camera_StrafeLeft  (speed);
    if (g_keys['d'] || g_keys['D']) Camera_StrafeRight (speed);
    if (g_keys['q'] || g_keys['Q']) g_camera.posY += speed * 0.5f;
    if (g_keys['z'] || g_keys['Z']) g_camera.posY -= speed * 0.5f;

    // Arrow keys for camera rotation
    if (g_specialKeys[GLUT_KEY_LEFT])  Camera_ArrowLeft();
    if (g_specialKeys[GLUT_KEY_RIGHT]) Camera_ArrowRight();
    if (g_specialKeys[GLUT_KEY_UP])    Camera_ArrowUp();
    if (g_specialKeys[GLUT_KEY_DOWN])  Camera_ArrowDown();

    // Clamp inside station bounds, UNLESS walking out the docking bay (Spacewalk!)
    if (g_camera.posZ > 24.5f && g_doors[3].openAmount > 0.5f) {
        // Outside the station (Zero-G Spacewalk bounds - huge area!)
        Camera_ClampToBounds(-1000.0f, 1000.0f, -1000.0f, 1000.0f, 24.5f, 1000.0f);
    } else {
        // Inside the station
        Camera_ClampToBounds(-28.0f, 28.0f, 0.5f, 14.0f, -38.0f, 24.8f);
    }
}

// ─── Update survival systems ──────────────────────────────────────────────────
inline void Input_UpdateSurvival(float dt) {
    if (g_gameOver) return;

    g_missionTimer += dt;

    // Oxygen slowly drains; reactor failure speeds it up slightly
    float drainRate = g_reactorMode ? 0.5f : 0.2f; // Drains much slower now (was 2.5/0.8)
    g_oxygenLevel -= drainRate * dt;
    if (g_oxygenLevel < 0.0f) g_oxygenLevel = 0.0f;

    // Reactor health degrades when reactor mode active
    if (g_reactorMode) {
        g_reactorHealth -= 0.8f * dt; // Gives them ~125 seconds to reach the console! (was 20s)
        if (g_reactorHealth < 0.0f) g_reactorHealth = 0.0f;
    } else if (g_missionCompleted) {
        // Reactor reignition logic!
        g_reactorCooldownTimer += dt;
        if (g_reactorCooldownTimer >= 120.0f) {
            g_reactorMode = true;           // Reignite!
            g_missionCompleted = false;     // Mission failed again
            g_alarmActive = true;           // Trigger alarm
            Lighting_ToggleReactor();       // Make it glow red
            g_reactorCooldownTimer = 0.0f;
        }
    }

    // Check game-over conditions
    if (g_oxygenLevel <= 0.0f || g_reactorHealth <= 0.0f) {
        g_gameOver = true;
        g_alarmActive = true; // Force alarm on
    }
}

// ─── Handle resize ────────────────────────────────────────────────────────────
inline void Input_Reshape(int w, int h) {
    g_windowW = w;
    g_windowH = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0f, (float)w / (float)(h ? h : 1), 0.1f, 8000.0f); // Massive render distance for giant Earth
    glMatrixMode(GL_MODELVIEW);
}

#endif // INPUT_H
