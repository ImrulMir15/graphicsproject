// =============================================================================
// scene.h - Scene composition & rendering for 3D Space Station Survival
// Builds the entire space station environment from reusable model functions
// =============================================================================
#ifndef SCENE_H
#define SCENE_H

#include "models.h"
#include "textures.h"
#include "lighting.h"
#include "animation.h"

// ─── Draw the station shell (floor, walls, ceiling) ──────────────────────────
inline void Scene_DrawStationShell() {
    // === FLOOR ===
    BindTex(TEX_METAL_FLOOR);
    SetMaterial_Metal();
    glColor3f(0.5f, 0.55f, 0.6f);
    // Main floor - large quad
    glPushMatrix();
    glBegin(GL_QUADS);
    glNormal3f(0,1,0);
    float fs = 30.0f; // half-size
    glTexCoord2f(0,0);    glVertex3f(-fs, 0, -fs);
    glTexCoord2f(8,0);    glVertex3f( fs, 0, -fs);
    glTexCoord2f(8,8);    glVertex3f( fs, 0,  fs);
    glTexCoord2f(0,8);    glVertex3f(-fs, 0,  fs);
    glEnd();
    glPopMatrix();

    // === CEILING ===
    BindTex(TEX_METAL_WALL);
    glColor3f(0.4f, 0.4f, 0.45f);
    glBegin(GL_QUADS);
    glNormal3f(0,-1,0);
    float ch = 10.0f; // ceiling height
    glTexCoord2f(0,0); glVertex3f(-fs, ch,  fs);
    glTexCoord2f(6,0); glVertex3f( fs, ch,  fs);
    glTexCoord2f(6,6); glVertex3f( fs, ch, -fs);
    glTexCoord2f(0,6); glVertex3f(-fs, ch, -fs);
    glEnd();

    // === WALLS ===
    glColor3f(0.45f, 0.45f, 0.52f);

    // Front wall (Z = -fs) with window cutout gaps
    glBegin(GL_QUADS);
    glNormal3f(0,0,1);
    // Left section
    glTexCoord2f(0,0); glVertex3f(-fs, 0, -fs);
    glTexCoord2f(2,0); glVertex3f(-8,  0, -fs);
    glTexCoord2f(2,2); glVertex3f(-8, ch, -fs);
    glTexCoord2f(0,2); glVertex3f(-fs,ch, -fs);
    // Right section
    glTexCoord2f(0,0); glVertex3f( 8,  0, -fs);
    glTexCoord2f(2,0); glVertex3f( fs, 0, -fs);
    glTexCoord2f(2,2); glVertex3f( fs,ch, -fs);
    glTexCoord2f(0,2); glVertex3f( 8, ch, -fs);
    // Above window
    glTexCoord2f(0,0); glVertex3f(-8, 7,  -fs);
    glTexCoord2f(2,0); glVertex3f( 8, 7,  -fs);
    glTexCoord2f(2,1); glVertex3f( 8, ch, -fs);
    glTexCoord2f(0,1); glVertex3f(-8, ch, -fs);
    // Below window
    glTexCoord2f(0,0); glVertex3f(-8, 0, -fs);
    glTexCoord2f(2,0); glVertex3f( 8, 0, -fs);
    glTexCoord2f(2,1); glVertex3f( 8, 2, -fs);
    glTexCoord2f(0,1); glVertex3f(-8, 2, -fs);
    glEnd();

    // Back wall (Z = +fs)
    glBegin(GL_QUADS);
    glNormal3f(0,0,-1);
    glTexCoord2f(0,0); glVertex3f( fs, 0,  fs);
    glTexCoord2f(4,0); glVertex3f(-fs, 0,  fs);
    glTexCoord2f(4,3); glVertex3f(-fs, ch, fs);
    glTexCoord2f(0,3); glVertex3f( fs, ch, fs);
    glEnd();

    // Left wall (X = -fs)
    glBegin(GL_QUADS);
    glNormal3f(1,0,0);
    // Left section of left wall
    glTexCoord2f(0,0); glVertex3f(-fs, 0,  fs);
    glTexCoord2f(2,0); glVertex3f(-fs, 0,  8);
    glTexCoord2f(2,2); glVertex3f(-fs, ch, 8);
    glTexCoord2f(0,2); glVertex3f(-fs, ch, fs);
    // Right section
    glTexCoord2f(0,0); glVertex3f(-fs, 0, -8);
    glTexCoord2f(2,0); glVertex3f(-fs, 0, -fs);
    glTexCoord2f(2,2); glVertex3f(-fs, ch,-fs);
    glTexCoord2f(0,2); glVertex3f(-fs, ch,-8);
    // Above window
    glTexCoord2f(0,0); glVertex3f(-fs, 7,  8);
    glTexCoord2f(2,0); glVertex3f(-fs, 7, -8);
    glTexCoord2f(2,1); glVertex3f(-fs, ch,-8);
    glTexCoord2f(0,1); glVertex3f(-fs, ch, 8);
    // Below window
    glTexCoord2f(0,0); glVertex3f(-fs, 0,  8);
    glTexCoord2f(2,0); glVertex3f(-fs, 0, -8);
    glTexCoord2f(2,1); glVertex3f(-fs, 2, -8);
    glTexCoord2f(0,1); glVertex3f(-fs, 2,  8);
    glEnd();

    // Right wall (X = +fs)
    glBegin(GL_QUADS);
    glNormal3f(-1,0,0);
    glTexCoord2f(0,0); glVertex3f( fs, 0, -fs);
    glTexCoord2f(4,0); glVertex3f( fs, 0,  fs);
    glTexCoord2f(4,3); glVertex3f( fs, ch, fs);
    glTexCoord2f(0,3); glVertex3f( fs, ch,-fs);
    glEnd();

    UnbindTex();
    SetMaterial_Reset();
}

// ─── Draw inner walls / partitions to create rooms ───────────────────────────
inline void Scene_DrawPartitions() {
    BindTex(TEX_METAL_WALL);
    SetMaterial_Metal();
    glColor3f(0.42f, 0.42f, 0.5f);

    // Partition 1: separates control room (front) from hallway
    // with door gap in center
    glPushMatrix();
    glTranslatef(-15.0f, 5.0f, -10.0f);
    DrawBox(12.0f, 10.0f, 0.3f);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(15.0f, 5.0f, -10.0f);
    DrawBox(12.0f, 10.0f, 0.3f);
    glPopMatrix();
    // Above door
    glPushMatrix();
    glTranslatef(0.0f, 8.0f, -10.0f);
    DrawBox(6.0f, 4.0f, 0.3f);
    glPopMatrix();

    // Partition 2: separates hallway from reactor chamber
    glPushMatrix();
    glTranslatef(-15.0f, 5.0f, -20.0f);
    DrawBox(12.0f, 10.0f, 0.3f);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(15.0f, 5.0f, -20.0f);
    DrawBox(12.0f, 10.0f, 0.3f);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.0f, 8.0f, -20.0f);
    DrawBox(6.0f, 4.0f, 0.3f);
    glPopMatrix();

    // Partition 3: lab on the right side
    glPushMatrix();
    glTranslatef(10.0f, 5.0f, 5.0f);
    glRotatef(90, 0, 1, 0);
    DrawBox(20.0f, 10.0f, 0.3f);
    glPopMatrix();

    // Floating wires scattered in the hallway ceiling
    DrawFloatingWires(-8.0f, 9.0f, -5.0f);
    DrawFloatingWires(5.0f, 9.5f, 0.0f);
    DrawFloatingWires(-2.0f, 8.5f, -15.0f);

    UnbindTex();
    SetMaterial_Reset();
}

// ─── Draw all station windows ────────────────────────────────────────────────
inline void Scene_DrawWindows() {
    // Front observation window
    DrawWindow(0, 4.5f, -29.9f, 0);
    // Left-wall window
    DrawWindow(-29.9f, 4.5f, 0, 90);
}

// ─── Draw the outer-space environment (seen through windows) ─────────────────
inline void Scene_DrawOuterSpace() {
    DrawStarfield();
    
    // Glowing Sun upper right (like the photo)
    DrawSun(2500.0f, 1200.0f, -3500.0f);
    
    // Moon upper left (like the photo)
    glPushMatrix();
    glRotatef(g_satelliteAngle * 0.1f, 0, 1, 0); // Slowly rotates
    DrawMoon(-1500.0f, 800.0f, -3000.0f, 80.0f); 
    glPopMatrix();
    
    // Massive Earth directly below the space station to create the horizon!
    glPushMatrix();
    glRotatef(g_satelliteAngle * 0.05f, 0, 1, 0); // Earth slowly rotates
    // Y is very far down (-3200), but radius is massive (3000), so the horizon sits right below us
    DrawEarth(0.0f, -3150.0f, -500.0f, 3000.0f); 
    glPopMatrix();
    
    DrawAsteroidBelt();
}

// ─── Draw station ring (external, visible through window) ────────────────────
inline void Scene_DrawStationRing() {
    glPushMatrix();
    glTranslatef(0, 5, -50);
    glRotatef(g_ringAngle, 0, 0, 1);
    glColor3f(0.5f, 0.5f, 0.55f);
    SetMaterial_Metal();
    BindTex(TEX_HULL);
    DrawTorus(18.0f, 1.2f, 36, 12);
    UnbindTex();
    SetMaterial_Reset();
    glPopMatrix();
}

// ─── Draw all control consoles ───────────────────────────────────────────────
inline void Scene_DrawConsoles() {
    // Main control room (front area, Z ~ -5..5)
    DrawConsole(-6, 0, -5, 0);
    DrawConsole( 6, 0, -5, 0);
    DrawConsole(-3, 0,  2, 180);
    DrawConsole( 3, 0,  2, 180);
    // Lab area consoles
    DrawConsole(18, 0, -5, -90);
    DrawConsole(18, 0,  5, -90);
}

// ─── Draw doors ──────────────────────────────────────────────────────────────
inline void Scene_DrawDoors() {
    DrawDoor(0,  0, 0, -10.0f);  // Control room → hallway
    DrawDoor(1,  0, 0, -20.0f);  // Hallway → reactor
    DrawDoor(2, 10, 0,   5.0f);  // Lab entrance (along partition)
    DrawDoor(3,  0, 0,  25.0f);  // Docking bay door
}

// ─── Draw the reactor chamber ────────────────────────────────────────────────
inline void Scene_DrawReactorChamber() {
    DrawReactorCore(0, 0, -25.0f);
    // Pipes from reactor to ceiling
    DrawPipe(2, 6, -25, 2, 10, -25, 0.2f);
    DrawPipe(-2, 6, -25, -2, 10, -25, 0.2f);
    DrawPipe(3, 3, -25, 8, 3, -25, 0.15f);
    DrawPipe(-3, 3, -25, -8, 3, -25, 0.15f);
    // Warning stripes (boxes with emissive red)
    if (g_reactorMode) {
        SetMaterial_GlowingRed();
        glColor3f(1.0f, 0.1f, 0.1f);
        glPushMatrix();
        glTranslatef(0, 0.05f, -25);
        DrawBox(8.0f, 0.05f, 8.0f);
        glPopMatrix();
        SetMaterial_Reset();
    }
}

// ─── Draw the docking bay (back area) ────────────────────────────────────────
inline void Scene_DrawDockingBay() {
    // Shuttle on landing pad
    DrawShuttle(g_shuttleX - 50.0f, 1.5f, 30.0f);
    // Escape pods along right wall
    DrawEscapePod(22, 1.5f, 25);
    DrawEscapePod(22, 1.5f, 20);
    DrawEscapePod(22, 1.5f, 15);
    // Landing pad markings
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 0.8f, 0.0f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(-5, 0.05f, 25); glVertex3f(5, 0.05f, 25);
    glVertex3f(5, 0.05f, 35);  glVertex3f(-5, 0.05f, 35);
    glEnd();
    glEnable(GL_LIGHTING);
}

// ─── Draw the laboratory ────────────────────────────────────────────────────
inline void Scene_DrawLab() {
    DrawRoboticArm(20, 0, 0);
    // Lab equipment - small boxes as equipment
    BindTex(TEX_PANEL);
    glColor3f(0.45f, 0.45f, 0.5f);
    glPushMatrix();
    glTranslatef(25, 1, -2);
    DrawBox(2, 2, 1.5f);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(25, 1, 3);
    DrawBox(1.5f, 2.5f, 1.5f);
    glPopMatrix();
    UnbindTex();
    
    // Draw NPCs (Meghla and Zafor) floating slightly in the lab
    float floatOffset = sinf(g_missionTimer * 2.0f) * 0.5f;
    DrawAstronaut(25, 2.0f + floatOffset, 0, -90, "meghla");
    DrawAstronaut(18, 2.0f - floatOffset, 4, 180, "zafor");
}

// ─── Draw external station features (ISS Style Exterior) ───────────────
inline void Scene_DrawExternalFeatures() {
    glPushMatrix();
    // Huge main truss structure (horizontal across X axis)
    glColor3f(0.7f, 0.7f, 0.75f);
    SetMaterial_Metal();
    
    // Main long truss
    glPushMatrix();
    glTranslatef(0, 15.0f, -10.0f); // Positioned above and slightly behind
    DrawBox(140.0f, 3.0f, 3.0f);    // Massive horizontal beam
    glPopMatrix();
    
    // Left solar array wings
    DrawSolarPanel(-60, 15, -10);
    DrawSolarPanel(-45, 15, -10);
    
    // Right solar array wings
    DrawSolarPanel(45, 15, -10);
    DrawSolarPanel(60, 15, -10);

    // Cylindrical modules extending from the station (Habitation/Lab nodes)
    glColor3f(0.8f, 0.8f, 0.85f);
    BindTex(TEX_HULL);
    
    // Node extending forward
    glPushMatrix();
    glTranslatef(0, 5, 15);
    glRotatef(90, 1, 0, 0); // lay flat
    DrawCylinder(5.0f, 20.0f, 24);
    glPopMatrix();
    
    // Node extending upwards
    glPushMatrix();
    glTranslatef(0, 20, -10);
    DrawCylinder(4.0f, 15.0f, 16);
    glPopMatrix();
    
    UnbindTex();

    // Radar on top of the vertical node
    DrawRadar(0, 28.0f, -10.0f);
    
    // Radiator panels (white vertical panels perpendicular to solar arrays)
    glColor3f(0.9f, 0.9f, 0.9f);
    glPushMatrix();
    glTranslatef(20, 10, -30);
    DrawBox(4.0f, 15.0f, 0.5f);
    glPopMatrix();
    
    glPushMatrix();
    glTranslatef(-20, 10, -30);
    DrawBox(4.0f, 15.0f, 0.5f);
    glPopMatrix();

    // Antennas
    DrawAntenna(25, 10, 25);
    DrawAntenna(-25, 10, 25);
    DrawAntenna(25, 10, -25);
    
    // Bangladesh Flag planted on the front of the massive vertical cylinder node!
    DrawBDFlag(0, 22.0f, -5.8f, 0); // Z=-5.8 is just outside the cylinder's curved hull

    glPopMatrix();
}

// ─── Emergency alarm lights (red pulsing beacons) ────────────────────────────
inline void Scene_DrawAlarmLights() {
    if (!g_alarmActive) return;
    float glow = g_alarmIntensity;
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glColor4f(1.0f, 0.05f, 0.05f, glow * 0.7f);
    // Place beacon spheres at corridor corners
    float positions[][3] = {
        {-10, 9.5f, -10}, {10, 9.5f, -10},
        {-10, 9.5f, -20}, {10, 9.5f, -20},
        {-10, 9.5f,   5}, {10, 9.5f,   5},
        {  0, 9.5f,  25}
    };
    for (int i = 0; i < 7; i++) {
        glPushMatrix();
        glTranslatef(positions[i][0], positions[i][1], positions[i][2]);
        glutSolidSphere(0.3f, 8, 6);
        glPopMatrix();
    }
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

// ─── MASTER SCENE RENDER ─────────────────────────────────────────────────────
inline void Scene_Render() {
    // 1. Outer space (behind everything, no depth write for skybox)
    glDepthMask(GL_FALSE);
    Scene_DrawOuterSpace();
    glDepthMask(GL_TRUE);

    // 2. External features
    Scene_DrawStationRing();
    Scene_DrawExternalFeatures();

    // 3. Station structure
    Scene_DrawStationShell();
    Scene_DrawPartitions();
    Scene_DrawWindows();
    Scene_DrawDoors();

    // 4. Interior details
    Scene_DrawConsoles();
    Scene_DrawReactorChamber();
    Scene_DrawDockingBay();
    Scene_DrawLab();

    // Player Model (only draw if not in first person, to avoid clipping)
    if (g_camera.mode == CAM_THIRD_PERSON || g_camera.mode == CAM_FREE) {
        DrawAstronaut(g_camera.posX, g_camera.posY - 1.5f, g_camera.posZ, g_camera.yaw - 90.0f, NULL);
    }

    // 5. Effects
    Scene_DrawAlarmLights();
    DrawParticles();
}

#endif // SCENE_H
