// =============================================================================
// main.cpp - Entry Point for "3D Space Station Survival"
// Academic-grade OpenGL/GLUT 3D project
// Author: Computer Graphics Course Project
// =============================================================================
//
// CONTROLS:
//   W/A/S/D       - Move forward/left/back/right
//   Q/Z           - Move up/down (free camera)
//   Arrow Keys    - Rotate camera
//   Mouse         - Look around (FPS style)
//   1/2/3         - Switch camera: First-person / Third-person / Free
//   L             - Toggle main lights
//   E             - Toggle emergency lights
//   K             - Toggle spotlights
//   F             - Toggle light flicker
//   R             - Toggle reactor mode (drains reactor health faster)
//   O             - Open/close doors
//   G             - Toggle alarm
//   P             - Pause/resume animation
//   M             - Toggle mouse capture
//   ESC           - Exit
//
// =============================================================================

#ifdef _WIN32
#include <windows.h>
#endif

#include <GL/glut.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>

// Project headers
#include "camera.h"
#include "lighting.h"
#include "textures.h"
#include "animation.h"
#include "input.h"
#include "models.h"
#include "scene.h"

// ─── Timing ──────────────────────────────────────────────────────────────────
static int   g_lastTime  = 0;
static float g_deltaTime = 0.0f;
static int   g_doorCycle = 0;   // which door to toggle next

// ─── HUD text rendering ─────────────────────────────────────────────────────
static void DrawText2D(float x, float y, const char* str, void* font = GLUT_BITMAP_HELVETICA_18) {
    glRasterPos2f(x, y);
    for (const char* c = str; *c; c++)
        glutBitmapCharacter(font, *c);
}

// ─── HUD overlay (oxygen, reactor, mission timer, controls) ──────────────────
static void RenderHUD() {
    // Switch to 2D orthographic
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, g_windowW, 0, g_windowH);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);

    // ── Alarm red overlay flash ──
    if (g_alarmActive) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(1.0f, 0.0f, 0.0f, g_alarmIntensity * 0.08f);
        glBegin(GL_QUADS);
        glVertex2f(0, 0); glVertex2f((float)g_windowW, 0);
        glVertex2f((float)g_windowW, (float)g_windowH); glVertex2f(0, (float)g_windowH);
        glEnd();
        glDisable(GL_BLEND);
    }

    // ── Game-over overlay ──
    if (g_gameOver) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor4f(0.0f, 0.0f, 0.0f, 0.6f);
        glBegin(GL_QUADS);
        glVertex2f(0, 0); glVertex2f((float)g_windowW, 0);
        glVertex2f((float)g_windowW, (float)g_windowH); glVertex2f(0, (float)g_windowH);
        glEnd();
        glDisable(GL_BLEND);
        glColor3f(1.0f, 0.2f, 0.2f);
        DrawText2D(g_windowW/2.0f - 100, g_windowH/2.0f + 20, "SYSTEM FAILURE", GLUT_BITMAP_TIMES_ROMAN_24);
        glColor3f(0.8f, 0.8f, 0.8f);
        DrawText2D(g_windowW/2.0f - 120, g_windowH/2.0f - 20, "Life support offline. Mission failed.");
    }

    // ── Mission Completed overlay ──
    if (g_missionCompleted) {
        glColor3f(0.2f, 1.0f, 0.2f);
        DrawText2D(g_windowW/2.0f - 150, g_windowH/2.0f + 20, "MISSION ACCOMPLISHED!", GLUT_BITMAP_TIMES_ROMAN_24);
        glColor3f(0.8f, 1.0f, 0.8f);
        DrawText2D(g_windowW/2.0f - 160, g_windowH/2.0f - 20, "Reactor stabilized. You saved the station!");
    }

    // ── HUD panel (top-left) ──
    float panelX = 15.0f, panelY = (float)g_windowH - 30.0f;
    char buf[128];

    // Title
    glColor3f(0.0f, 0.85f, 1.0f);
    DrawText2D(panelX, panelY, "[ SPACE STATION SURVIVAL ]");
    panelY -= 22;

    // Camera mode
    const char* modeStr = (g_camera.mode == CAM_FIRST_PERSON) ? "FIRST PERSON" :
                          (g_camera.mode == CAM_THIRD_PERSON) ? "THIRD PERSON" : "FREE CAM";
    glColor3f(0.6f, 0.9f, 0.6f);
    sprintf(buf, "Camera: %s", modeStr);
    DrawText2D(panelX, panelY, buf, GLUT_BITMAP_HELVETICA_12);
    panelY -= 18;

    // Oxygen bar
    glColor3f(0.3f, 0.9f, 1.0f);
    sprintf(buf, "O2: %.0f%%", g_oxygenLevel);
    DrawText2D(panelX, panelY, buf, GLUT_BITMAP_HELVETICA_12);
    // Draw bar
    float barX = panelX + 60, barW = 120;
    glColor3f(0.15f, 0.15f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(barX, panelY - 2); glVertex2f(barX+barW, panelY - 2);
    glVertex2f(barX+barW, panelY+12); glVertex2f(barX, panelY+12);
    glEnd();
    float fillW = barW * g_oxygenLevel / 100.0f;
    if (g_oxygenLevel > 50) glColor3f(0.1f, 0.8f, 1.0f);
    else if (g_oxygenLevel > 20) glColor3f(1.0f, 0.8f, 0.0f);
    else glColor3f(1.0f, 0.1f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(barX, panelY - 2); glVertex2f(barX+fillW, panelY - 2);
    glVertex2f(barX+fillW, panelY+12); glVertex2f(barX, panelY+12);
    glEnd();
    panelY -= 18;

    // Reactor health
    glColor3f(0.2f, 1.0f, 0.6f);
    sprintf(buf, "Reactor: %.0f%%", g_reactorHealth);
    DrawText2D(panelX, panelY, buf, GLUT_BITMAP_HELVETICA_12);
    float rFillW = barW * g_reactorHealth / 100.0f;
    glColor3f(0.15f, 0.15f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(barX, panelY - 2); glVertex2f(barX+barW, panelY - 2);
    glVertex2f(barX+barW, panelY+12); glVertex2f(barX, panelY+12);
    glEnd();
    if (g_reactorHealth > 50) glColor3f(0.1f, 1.0f, 0.5f);
    else if (g_reactorHealth > 20) glColor3f(1.0f, 0.6f, 0.0f);
    else glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(barX, panelY - 2); glVertex2f(barX+rFillW, panelY - 2);
    glVertex2f(barX+rFillW, panelY+12); glVertex2f(barX, panelY+12);
    glEnd();
    panelY -= 18;

    // Mission timer
    glColor3f(0.8f, 0.8f, 0.8f);
    int mins = (int)(g_missionTimer / 60.0f);
    int secs = (int)g_missionTimer % 60;
    sprintf(buf, "Mission Time: %02d:%02d", mins, secs);
    DrawText2D(panelX, panelY, buf, GLUT_BITMAP_HELVETICA_12);
    panelY -= 18;

    // Lights status
    glColor3f(0.7f, 0.7f, 0.4f);
    sprintf(buf, "Lights:%s  Emergency:%s  Reactor:%s",
            g_mainLightsOn ? "ON" : "OFF",
            g_emergencyOn  ? "ON" : "OFF",
            g_reactorGlowOn? "ON" : "OFF");
    DrawText2D(panelX, panelY, buf, GLUT_BITMAP_HELVETICA_12);
    panelY -= 18;

    // Animation status
    glColor3f(0.6f, 0.6f, 0.6f);
    sprintf(buf, "Animation:%s  Alarm:%s",
            g_animPaused ? "PAUSED" : "RUNNING",
            g_alarmActive ? "ACTIVE" : "OFF");
    DrawText2D(panelX, panelY, buf, GLUT_BITMAP_HELVETICA_12);

    // ── Minimap (bottom-right corner) ──
    float mmX = g_windowW - 140.0f, mmY = 20.0f, mmS = 120.0f;
    // Background
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.1f, 0.2f, 0.6f);
    glBegin(GL_QUADS);
    glVertex2f(mmX, mmY); glVertex2f(mmX+mmS, mmY);
    glVertex2f(mmX+mmS, mmY+mmS); glVertex2f(mmX, mmY+mmS);
    glEnd();
    glDisable(GL_BLEND);
    // Border
    glColor3f(0.0f, 0.8f, 1.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(mmX, mmY); glVertex2f(mmX+mmS, mmY);
    glVertex2f(mmX+mmS, mmY+mmS); glVertex2f(mmX, mmY+mmS);
    glEnd();
    // Player dot
    float px = mmX + mmS/2 + (g_camera.posX / 30.0f) * (mmS/2);
    float py = mmY + mmS/2 - (g_camera.posZ / 30.0f) * (mmS/2);
    glColor3f(0.0f, 1.0f, 0.0f);
    glPointSize(5.0f);
    glBegin(GL_POINTS);
    glVertex2f(px, py);
    glEnd();
    // Direction indicator
    float dirLen = 8.0f;
    Camera_UpdateVectors();
    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f(px, py);
    glVertex2f(px + g_camera.dirX * dirLen, py - g_camera.dirZ * dirLen);
    glEnd();
    // Room labels
    glColor3f(0.5f, 0.7f, 0.9f);
    DrawText2D(mmX + 35, mmY + mmS - 15, "MAP", GLUT_BITMAP_HELVETICA_12);

    // ── Controls hint (bottom-left) ──
    glColor3f(0.4f, 0.5f, 0.6f);
    float hintY = 20.0f;
    DrawText2D(15, hintY + 54, "WASD:Move  Arrows:Look  1/2/3:Cam", GLUT_BITMAP_HELVETICA_12);
    DrawText2D(15, hintY + 36, "L:Lights  E:Emergency  R:Reactor  O:Door", GLUT_BITMAP_HELVETICA_12);
    DrawText2D(15, hintY + 18, "G:Alarm  P:Pause  F:Flicker  M:Mouse", GLUT_BITMAP_HELVETICA_12);
    DrawText2D(15, hintY,      "ESC:Exit  Q/Z:Up/Down", GLUT_BITMAP_HELVETICA_12);

    // ── Mission Objectives ──
    if (!g_missionCompleted && !g_gameOver) {
        glColor3f(1.0f, 0.8f, 0.0f);
        DrawText2D(g_windowW/2.0f - 220, g_windowH - 40, "MISSION: Find the Reactor console (Z < -15) and press 'R' to shut it down!", GLUT_BITMAP_HELVETICA_18);
    } else if (g_missionCompleted) {
        glColor3f(0.2f, 1.0f, 0.2f);
        DrawText2D(g_windowW/2.0f - 160, g_windowH - 40, "MISSION COMPLETE: You can now freely explore or go Spacewalking!", GLUT_BITMAP_HELVETICA_18);
    }

    // ── Reactor warning ──
    if (g_reactorMode && g_reactorHealth < 50.0f && !g_gameOver) {
        float blink = sinf(g_missionTimer * 6.0f);
        if (blink > 0) {
            glColor3f(1.0f, 0.2f, 0.0f);
            DrawText2D(g_windowW/2.0f - 100, g_windowH/2.0f + 60,
                       "!! REACTOR CRITICAL !!", GLUT_BITMAP_TIMES_ROMAN_24);
        }
    }

    // Restore matrices
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

// ─── Crosshair ───────────────────────────────────────────────────────────────
static void DrawCrosshair() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, g_windowW, 0, g_windowH);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    float cx = g_windowW / 2.0f, cy = g_windowH / 2.0f;
    glColor3f(0.0f, 1.0f, 0.8f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2f(cx - 10, cy); glVertex2f(cx + 10, cy);
    glVertex2f(cx, cy - 10); glVertex2f(cx, cy + 10);
    glEnd();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

// ─── Custom door toggle handling (cycle through doors with O) ────────────────
static void HandleDoorKey() {
    // Override the default single-door toggle from input.h
    // Instead cycle through all doors
}

// ─── Display callback ────────────────────────────────────────────────────────
static void Display() {
    // Calculate delta time
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    g_deltaTime = (currentTime - g_lastTime) / 1000.0f;
    if (g_deltaTime > 0.05f) g_deltaTime = 0.05f; // clamp
    g_lastTime = currentTime;

    // Update systems
    Input_ProcessHeldKeys();
    Anim_Update(g_deltaTime);
    Lighting_Update(g_deltaTime);
    Input_UpdateSurvival(g_deltaTime);

    // Clear
    glClearColor(0.0f, 0.0f, 0.02f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set up modelview with camera
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    Camera_Apply();

    // Render the 3D scene
    Scene_Render();

    // 2D overlays
    DrawCrosshair();
    RenderHUD();

    glutSwapBuffers();
}

// ─── Idle callback (continuous redraw) ───────────────────────────────────────
static void Idle() {
    glutPostRedisplay();
}

// ─── Extended door toggle (override 'O' to cycle doors) ─────────────────────
static void KeyDownOverride(unsigned char key, int x, int y) {
    if (key == 'o' || key == 'O') {
        Anim_ToggleDoor(g_doorCycle);
        g_doorCycle = (g_doorCycle + 1) % NUM_DOORS;
        return; // don't pass to default handler for this key
    }
    Input_KeyDown(key, x, y);
}

// =============================================================================
// MAIN ENTRY POINT
// =============================================================================
int main(int argc, char** argv) {
    // ── Initialize GLUT ──
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(1280, 720);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("3D Space Station Survival - OpenGL/GLUT");

    // ── OpenGL state ──
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    // glEnable(GL_MULTISAMPLE); // Disabled for compatibility with some Windows OpenGL headers
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Enable fog for atmosphere
    glEnable(GL_FOG);
    float fogColor[] = { 0.02f, 0.02f, 0.05f, 1.0f };
    glFogfv(GL_FOG_COLOR, fogColor);
    glFogi(GL_FOG_MODE, GL_EXP2);
    glFogf(GL_FOG_DENSITY, 0.008f);

    // ── Initialize subsystems ──
    Camera_Init();
    Lighting_Init();
    Textures_Init();
    Anim_InitDoors();
    g_lastTime = glutGet(GLUT_ELAPSED_TIME);

    // ── Hide cursor for FPS look ──
    glutSetCursor(GLUT_CURSOR_NONE);

    // ── Set projection ──
    Input_Reshape(1280, 720);

    // ── Register callbacks ──
    glutDisplayFunc(Display);
    glutIdleFunc(Idle);
    glutReshapeFunc(Input_Reshape);
    glutKeyboardFunc(KeyDownOverride);
    glutKeyboardUpFunc(Input_KeyUp);
    glutSpecialFunc(Input_SpecialDown);
    glutSpecialUpFunc(Input_SpecialUp);
    glutMotionFunc(Input_MouseMotion);
    glutPassiveMotionFunc(Input_PassiveMotion);

    // ── Print startup info ──
    printf("========================================\n");
    printf("  3D SPACE STATION SURVIVAL\n");
    printf("  OpenGL/GLUT Academic Project\n");
    printf("========================================\n");
    printf("CONTROLS:\n");
    printf("  WASD        - Move\n");
    printf("  Arrow Keys  - Look around\n");
    printf("  Mouse       - FPS look\n");
    printf("  1/2/3       - Camera modes\n");
    printf("  L           - Toggle main lights\n");
    printf("  E           - Toggle emergency lights\n");
    printf("  K           - Toggle spotlights\n");
    printf("  F           - Toggle flicker\n");
    printf("  R           - Reactor mode\n");
    printf("  O           - Open/close doors\n");
    printf("  G           - Toggle alarm\n");
    printf("  P           - Pause animation\n");
    printf("  M           - Toggle mouse capture\n");
    printf("  Q/Z         - Move up/down\n");
    printf("  ESC         - Exit\n");
    printf("========================================\n");

    // ── Enter main loop ──
    glutMainLoop();
    return 0;
}
