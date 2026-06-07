// =============================================================================
// models.h - Reusable 3D object drawing functions
// =============================================================================
#ifndef MODELS_H
#define MODELS_H

#include <GL/glut.h>
#include <cmath>
#include "textures.h"
#include "lighting.h"
#include "animation.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ─── Draw a textured box (centered at origin) ────────────────────────────────
inline void DrawBox(float w, float h, float d) {
    float hw=w/2, hh=h/2, hd=d/2;
    glBegin(GL_QUADS);
    // Front
    glNormal3f(0,0,1);
    glTexCoord2f(0,0); glVertex3f(-hw,-hh, hd);
    glTexCoord2f(1,0); glVertex3f( hw,-hh, hd);
    glTexCoord2f(1,1); glVertex3f( hw, hh, hd);
    glTexCoord2f(0,1); glVertex3f(-hw, hh, hd);
    // Back
    glNormal3f(0,0,-1);
    glTexCoord2f(0,0); glVertex3f( hw,-hh,-hd);
    glTexCoord2f(1,0); glVertex3f(-hw,-hh,-hd);
    glTexCoord2f(1,1); glVertex3f(-hw, hh,-hd);
    glTexCoord2f(0,1); glVertex3f( hw, hh,-hd);
    // Left
    glNormal3f(-1,0,0);
    glTexCoord2f(0,0); glVertex3f(-hw,-hh,-hd);
    glTexCoord2f(1,0); glVertex3f(-hw,-hh, hd);
    glTexCoord2f(1,1); glVertex3f(-hw, hh, hd);
    glTexCoord2f(0,1); glVertex3f(-hw, hh,-hd);
    // Right
    glNormal3f(1,0,0);
    glTexCoord2f(0,0); glVertex3f( hw,-hh, hd);
    glTexCoord2f(1,0); glVertex3f( hw,-hh,-hd);
    glTexCoord2f(1,1); glVertex3f( hw, hh,-hd);
    glTexCoord2f(0,1); glVertex3f( hw, hh, hd);
    // Top
    glNormal3f(0,1,0);
    glTexCoord2f(0,0); glVertex3f(-hw, hh, hd);
    glTexCoord2f(1,0); glVertex3f( hw, hh, hd);
    glTexCoord2f(1,1); glVertex3f( hw, hh,-hd);
    glTexCoord2f(0,1); glVertex3f(-hw, hh,-hd);
    // Bottom
    glNormal3f(0,-1,0);
    glTexCoord2f(0,0); glVertex3f(-hw,-hh,-hd);
    glTexCoord2f(1,0); glVertex3f( hw,-hh,-hd);
    glTexCoord2f(1,1); glVertex3f( hw,-hh, hd);
    glTexCoord2f(0,1); glVertex3f(-hw,-hh, hd);
    glEnd();
}

// ─── Draw a cylinder (along Y axis) ──────────────────────────────────────────
inline void DrawCylinder(float radius, float height, int slices) {
    float halfH = height / 2.0f;
    // Side
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= slices; i++) {
        float a = 2.0f * (float)M_PI * i / slices;
        float cs = cosf(a), sn = sinf(a);
        glNormal3f(cs, 0, sn);
        glTexCoord2f((float)i/slices, 0); glVertex3f(cs*radius, -halfH, sn*radius);
        glTexCoord2f((float)i/slices, 1); glVertex3f(cs*radius,  halfH, sn*radius);
    }
    glEnd();
    // Top cap
    glNormal3f(0,1,0);
    glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(0.5f,0.5f); glVertex3f(0, halfH, 0);
    for (int i = 0; i <= slices; i++) {
        float a = 2.0f*(float)M_PI*i/slices;
        glTexCoord2f(0.5f+0.5f*cosf(a),0.5f+0.5f*sinf(a));
        glVertex3f(cosf(a)*radius, halfH, sinf(a)*radius);
    }
    glEnd();
    // Bottom cap
    glNormal3f(0,-1,0);
    glBegin(GL_TRIANGLE_FAN);
    glTexCoord2f(0.5f,0.5f); glVertex3f(0,-halfH,0);
    for (int i = slices; i >= 0; i--) {
        float a = 2.0f*(float)M_PI*i/slices;
        glTexCoord2f(0.5f+0.5f*cosf(a),0.5f+0.5f*sinf(a));
        glVertex3f(cosf(a)*radius,-halfH, sinf(a)*radius);
    }
    glEnd();
}

// ─── Draw a torus (ring) ─────────────────────────────────────────────────────
inline void DrawTorus(float R, float r, int rings, int sides) {
    for (int i = 0; i < rings; i++) {
        float t0 = 2.0f*(float)M_PI*i/rings;
        float t1 = 2.0f*(float)M_PI*(i+1)/rings;
        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= sides; j++) {
            float s = 2.0f*(float)M_PI*j/sides;
            float cs = cosf(s), ss = sinf(s);
            for (int k = 1; k >= 0; k--) {
                float t = (k==1)?t1:t0;
                float ct = cosf(t), st = sinf(t);
                float x = (R + r*cs)*ct;
                float y = r*ss;
                float z = (R + r*cs)*st;
                glNormal3f(cs*ct, ss, cs*st);
                glTexCoord2f((float)(i+1-k)/rings, (float)j/sides);
                glVertex3f(x, y, z);
            }
        }
        glEnd();
    }
}

// ─── Draw a sphere ───────────────────────────────────────────────────────────
inline void DrawSphere(float radius, int slices, int stacks) {
    for (int i = 0; i < stacks; i++) {
        float lat0 = (float)M_PI*(-0.5f + (float)i/stacks);
        float lat1 = (float)M_PI*(-0.5f + (float)(i+1)/stacks);
        float y0 = sinf(lat0), y1 = sinf(lat1);
        float r0 = cosf(lat0), r1 = cosf(lat1);
        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= slices; j++) {
            float lng = 2.0f*(float)M_PI*j/slices;
            float x = cosf(lng), z = sinf(lng);
            glNormal3f(x*r0, y0, z*r0);
            glTexCoord2f((float)j/slices, (float)i/stacks);
            glVertex3f(x*r0*radius, y0*radius, z*r0*radius);
            glNormal3f(x*r1, y1, z*r1);
            glTexCoord2f((float)j/slices, (float)(i+1)/stacks);
            glVertex3f(x*r1*radius, y1*radius, z*r1*radius);
        }
        glEnd();
    }
}

// ─── Sliding Door ────────────────────────────────────────────────────────────
inline void DrawDoor(int doorIdx, float x, float y, float z) {
    float openAmt = g_doors[doorIdx].openAmount;
    float slideOffset = openAmt * 2.5f; // how far each half slides
    BindTex(TEX_DOOR);
    SetMaterial_Metal();
    // Left panel
    glPushMatrix();
    glTranslatef(x - slideOffset, y + 2.5f, z);
    DrawBox(2.5f, 5.0f, 0.3f);
    glPopMatrix();
    // Right panel
    glPushMatrix();
    glTranslatef(x + slideOffset, y + 2.5f, z);
    DrawBox(2.5f, 5.0f, 0.3f);
    glPopMatrix();
    // Door frame
    glColor3f(0.3f, 0.3f, 0.35f);
    glPushMatrix();
    glTranslatef(x, y + 5.3f, z);
    DrawBox(6.0f, 0.6f, 0.5f);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(x - 2.8f, y + 2.5f, z);
    DrawBox(0.4f, 5.6f, 0.5f);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(x + 2.8f, y + 2.5f, z);
    DrawBox(0.4f, 5.6f, 0.5f);
    glPopMatrix();
    SetMaterial_Reset();
    UnbindTex();
}

// ─── Control Console ─────────────────────────────────────────────────────────
inline void DrawConsole(float x, float y, float z, float rotY) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotY, 0, 1, 0);
    // Desk body
    SetMaterial_Metal();
    BindTex(TEX_PANEL);
    glColor3f(0.4f, 0.4f, 0.5f);
    DrawBox(3.0f, 1.0f, 1.5f);
    UnbindTex();
    // Screen
    BindTex(TEX_CONSOLE);
    glPushMatrix();
    glTranslatef(0, 1.2f, -0.5f);
    glRotatef(-15, 1, 0, 0);
    // Animate screen texture slightly
    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glTranslatef(0, g_alarmTimer * 0.1f, 0);
    DrawBox(2.5f, 1.8f, 0.1f);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    UnbindTex();
    // Keyboard area (small box)
    glColor3f(0.2f, 0.2f, 0.25f);
    glPushMatrix();
    glTranslatef(0, 0.55f, 0.3f);
    DrawBox(2.0f, 0.1f, 0.8f);
    // Add glowing keyboard keys
    SetMaterial_GlowingBlue();
    glColor3f(0.2f, 0.8f, 1.0f);
    glTranslatef(-0.8f, 0.05f, -0.2f);
    for(int row=0; row<3; row++) {
        for(int col=0; col<8; col++) {
            glPushMatrix();
            glTranslatef(col*0.22f, 0, row*0.22f);
            DrawBox(0.15f, 0.02f, 0.15f);
            glPopMatrix();
        }
    }
    glPopMatrix();
    SetMaterial_Reset();
    glPopMatrix();
}

// ─── Robotic Arm (4-joint) ───────────────────────────────────────────────────
inline void DrawRoboticArm(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glColor3f(0.6f, 0.6f, 0.65f);
    SetMaterial_Metal();
    // Base
    glRotatef(g_armBase, 0, 1, 0);
    DrawCylinder(0.5f, 0.4f, 16);
    // Shoulder
    glTranslatef(0, 0.4f, 0);
    glRotatef(g_armShoulder, 1, 0, 0);
    glColor3f(0.5f, 0.5f, 0.55f);
    DrawCylinder(0.25f, 2.5f, 12);
    // Elbow
    glTranslatef(0, 2.5f, 0);
    glRotatef(g_armElbow, 1, 0, 0);
    glColor3f(0.55f, 0.55f, 0.6f);
    DrawCylinder(0.2f, 2.0f, 12);
    // Wrist/gripper
    glTranslatef(0, 2.0f, 0);
    glRotatef(g_armWrist, 0, 0, 1);
    glColor3f(0.7f, 0.3f, 0.1f);
    // Two gripper fingers
    glPushMatrix();
    glTranslatef( 0.15f, 0.3f, 0); DrawBox(0.08f, 0.6f, 0.15f);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-0.15f, 0.3f, 0); DrawBox(0.08f, 0.6f, 0.15f);
    glPopMatrix();
    SetMaterial_Reset();
    glPopMatrix();
}

// ─── Radar Dish (continuously rotating) ──────────────────────────────────────
inline void DrawRadar(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(g_radarAngle, 0, 1, 0);
    glColor3f(0.6f, 0.6f, 0.65f);
    SetMaterial_Metal();
    // Mast
    DrawCylinder(0.15f, 2.0f, 10);
    // Dish (flattened sphere)
    glPushMatrix();
    glTranslatef(0, 1.2f, 0);
    glScalef(1.0f, 0.3f, 1.0f);
    glColor3f(0.7f, 0.75f, 0.8f);
    DrawSphere(1.0f, 16, 8);
    glPopMatrix();
    SetMaterial_Reset();
    glPopMatrix();
}

// ─── Solar Panel (rotating tilt) ─────────────────────────────────────────────
inline void DrawSolarPanel(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);
    float tilt = 20.0f * sinf(g_solarAngle * (float)M_PI / 180.0f);
    glRotatef(tilt, 1, 0, 0);
    glColor3f(0.1f, 0.1f, 0.4f);
    SetMaterial_Metal();
    // Support arm
    glPushMatrix();
    glColor3f(0.5f, 0.5f, 0.55f);
    DrawCylinder(0.08f, 3.0f, 8);
    glPopMatrix();
    // Panel (Massive ISS-style solar wings)
    glPushMatrix();
    glTranslatef(0, 1.8f, 0);
    BindTex(TEX_PANEL);
    // Draw base gold/blue solar material
    glColor3f(0.15f, 0.2f, 0.4f);
    DrawBox(6.0f, 0.1f, 30.0f);
    UnbindTex();
    glPopMatrix();
    SetMaterial_Reset();
    glPopMatrix();
}

// ─── Turbine Fan ─────────────────────────────────────────────────────────────
inline void DrawTurbine(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glColor3f(0.45f, 0.45f, 0.5f);
    SetMaterial_Metal();
    DrawCylinder(1.2f, 0.3f, 20); // housing
    // Blades
    glPushMatrix();
    glRotatef(g_turbineAngle, 0, 0, 1);
    for (int i = 0; i < 6; i++) {
        glPushMatrix();
        glRotatef(i * 60.0f, 0, 0, 1);
        glTranslatef(0.5f, 0, 0);
        glColor3f(0.55f, 0.55f, 0.6f);
        DrawBox(0.8f, 0.02f, 0.2f);
        glPopMatrix();
    }
    glPopMatrix();
    SetMaterial_Reset();
    glPopMatrix();
}

// ─── Reactor Core ────────────────────────────────────────────────────────────
inline void DrawReactorCore(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);
    // Outer containment cylinder
    glColor3f(0.35f, 0.35f, 0.4f);
    SetMaterial_Metal();
    BindTex(TEX_HULL);
    DrawCylinder(3.0f, 6.0f, 24);
    UnbindTex();
    // Inner glowing core sphere
    SetMaterial_GlowingBlue();
    float pulse = 0.8f + 0.2f * sinf(g_reactorPulse * 1.5f);
    BindTex(TEX_REACTOR);
    glPushMatrix();
    glScalef(pulse, pulse, pulse);
    glColor3f(0.1f, 0.8f, 1.0f);
    DrawSphere(1.5f, 20, 16);
    glPopMatrix();
    UnbindTex();
    // Ring around reactor
    glColor3f(0.4f, 0.6f, 0.8f);
    glPushMatrix();
    glRotatef(g_ringAngle * 3, 0, 1, 0);
    DrawTorus(2.5f, 0.15f, 24, 12);
    glPopMatrix();
    SetMaterial_Reset();
    glPopMatrix();
}

// ─── Space Shuttle ───────────────────────────────────────────────────────────
inline void DrawShuttle(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);
    SetMaterial_Metal();
    BindTex(TEX_HULL);
    // Fuselage
    glColor3f(0.8f, 0.8f, 0.85f);
    glPushMatrix();
    glScalef(1.0f, 0.5f, 3.0f);
    DrawSphere(1.5f, 16, 12);
    glPopMatrix();
    // Wings
    glColor3f(0.5f, 0.5f, 0.55f);
    glPushMatrix();
    glTranslatef(2.0f, -0.2f, 0.5f);
    DrawBox(2.5f, 0.1f, 2.0f);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-2.0f, -0.2f, 0.5f);
    DrawBox(2.5f, 0.1f, 2.0f);
    glPopMatrix();
    // Tail fin
    glPushMatrix();
    glTranslatef(0, 1.0f, 2.0f);
    DrawBox(0.1f, 1.5f, 1.0f);
    glPopMatrix();
    // Engine glow
    SetMaterial_GlowingBlue();
    glPushMatrix();
    glTranslatef(0, 0, 3.5f);
    glColor3f(0.2f, 0.5f, 1.0f);
    DrawSphere(0.4f, 10, 8);
    glPopMatrix();
    UnbindTex();
    SetMaterial_Reset();
    glPopMatrix();
}

// ─── Satellite ───────────────────────────────────────────────────────────────
inline void DrawSatellite(float orbitAngle) {
    glPushMatrix();
    glRotatef(orbitAngle, 0, 1, 0);
    glTranslatef(50.0f, 15.0f, 0);
    SetMaterial_Metal();
    glColor3f(0.7f, 0.7f, 0.75f);
    DrawBox(1.0f, 1.0f, 1.5f);
    // Solar panels
    glColor3f(0.1f, 0.1f, 0.5f);
    glPushMatrix();
    glTranslatef(2.0f, 0, 0);
    DrawBox(2.5f, 0.05f, 1.0f);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-2.0f, 0, 0);
    DrawBox(2.5f, 0.05f, 1.0f);
    glPopMatrix();
    // Antenna
    glColor3f(0.8f, 0.8f, 0.8f);
    glPushMatrix();
    glTranslatef(0, 0.8f, 0);
    DrawCylinder(0.03f, 1.0f, 6);
    glPopMatrix();
    SetMaterial_Reset();
    glPopMatrix();
}

// ─── Escape Pod ──────────────────────────────────────────────────────────────
inline void DrawEscapePod(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);
    SetMaterial_Metal();
    BindTex(TEX_HULL);
    glColor3f(0.9f, 0.5f, 0.1f);
    glPushMatrix();
    glScalef(1.0f, 0.8f, 1.5f);
    DrawSphere(1.2f, 14, 10);
    glPopMatrix();
    // Hatch
    glColor3f(0.3f, 0.3f, 0.35f);
    glPushMatrix();
    glTranslatef(0, 0, 1.5f);
    DrawBox(0.8f, 0.8f, 0.05f);
    glPopMatrix();
    UnbindTex();
    SetMaterial_Reset();
    glPopMatrix();
}

// ─── Station Window (with space view) ────────────────────────────────────────
inline void DrawWindow(float x, float y, float z, float rotY) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotY, 0, 1, 0);
    // Frame
    glColor3f(0.3f, 0.3f, 0.35f);
    SetMaterial_Metal();
    DrawBox(4.5f, 3.5f, 0.2f);
    // Glass (slightly transparent blue)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.1f, 0.15f, 0.3f, 0.3f);
    glPushMatrix();
    glTranslatef(0, 0, 0.15f);
    DrawBox(3.8f, 2.8f, 0.05f);
    glPopMatrix();
    glDisable(GL_BLEND);
    SetMaterial_Reset();
    glPopMatrix();
}

// ─── Pipe section ────────────────────────────────────────────────────────────
inline void DrawPipe(float x1,float y1,float z1, float x2,float y2,float z2, float r) {
    float dx=x2-x1, dy=y2-y1, dz=z2-z1;
    float len = sqrtf(dx*dx+dy*dy+dz*dz);
    if (len < 0.001f) return;
    glPushMatrix();
    glTranslatef(x1, y1, z1);
    // Align cylinder to direction
    float ax = -dz, az = dx; // cross with Y
    float angle = acosf(dy/len) * 180.0f/(float)M_PI;
    if (fabsf(dy/len) < 0.999f)
        glRotatef(angle, ax, 0, az);
    glColor3f(0.5f, 0.5f, 0.55f);
    SetMaterial_Metal();
    DrawCylinder(r, len, 8);
    SetMaterial_Reset();
    glPopMatrix();
}

// ─── Antenna mast ────────────────────────────────────────────────────────────
inline void DrawAntenna(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glColor3f(0.6f, 0.6f, 0.65f);
    SetMaterial_Metal();
    DrawCylinder(0.06f, 4.0f, 6);
    glPushMatrix();
    glTranslatef(0, 2.2f, 0);
    glColor3f(0.8f, 0.2f, 0.2f);
    DrawSphere(0.12f, 8, 6);
    glPopMatrix();
    SetMaterial_Reset();
    glPopMatrix();
}

// ─── Earth model ─────────────────────────────────────────────────────────────
inline void DrawEarth(float x, float y, float z, float radius) {
    glPushMatrix();
    glTranslatef(x, y, z);
    BindTex(TEX_EARTH);
    glColor3f(1, 1, 1);
    DrawSphere(radius, 32, 24);
    UnbindTex();
    glPopMatrix();
}

// ─── Moon model ──────────────────────────────────────────────────────────────
inline void DrawMoon(float x, float y, float z, float radius) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glColor3f(0.7f, 0.7f, 0.65f);
    DrawSphere(radius, 16, 12);
    glPopMatrix();
}

// ─── Starfield skybox (large inverted sphere) ────────────────────────────────
inline void DrawStarfield() {
    glPushMatrix();
    glDisable(GL_LIGHTING);
    BindTex(TEX_SPACE);
    glColor3f(1, 1, 1);
    // Add slow rotation to starfield to make it feel like orbiting
    glRotatef(g_satelliteAngle * 0.2f, 0, 1, 0);
    glRotatef(g_satelliteAngle * 0.1f, 1, 0, 0);
    // Draw inside of a massive sphere for the far-away universe
    GLUquadric* q = gluNewQuadric();
    gluQuadricOrientation(q, GLU_INSIDE);
    gluQuadricTexture(q, GL_TRUE);
    gluSphere(q, 7000.0f, 32, 32);
    gluDeleteQuadric(q);
    UnbindTex();
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

// ─── Bright Sun ─────────────────────────────────────────────────────────────
inline void DrawSun(float x, float y, float z) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glDisable(GL_LIGHTING);
    // Sun core
    glColor3f(1.0f, 0.95f, 0.8f);
    DrawSphere(40.0f, 32, 32);
    // Sun glow
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glColor4f(1.0f, 0.6f, 0.1f, 0.4f);
    DrawSphere(45.0f, 24, 24);
    glColor4f(1.0f, 0.3f, 0.0f, 0.2f);
    DrawSphere(55.0f, 24, 24);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

// ─── Asteroid belt ───────────────────────────────────────────────────────────
inline void DrawAsteroidBelt() {
    glColor3f(0.4f, 0.35f, 0.3f);
    for (int i = 0; i < 20; i++) {
        float angle = g_asteroidAngle + i * 18.0f;
        float r = 80.0f + (i % 5) * 5.0f;
        float px = cosf(angle * (float)M_PI/180.0f) * r;
        float py = sinf(i * 1.5f) * 3.0f;
        float pz = sinf(angle * (float)M_PI/180.0f) * r;
        float sz = 0.3f + (i % 3) * 0.4f;
        glPushMatrix();
        glTranslatef(px, py, pz);
        glRotatef(angle * 3, 1, 1, 0);
        glScalef(sz, sz * 0.7f, sz * 1.2f);
        glutSolidDodecahedron();
        glPopMatrix();
    }
}

// ─── Particles rendering ─────────────────────────────────────────────────────
inline void DrawParticles() {
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glPointSize(3.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < g_particleCount; i++) {
        Particle& p = g_particles[i];
        glColor4f(p.r, p.g, p.b, p.life);
        glVertex3f(p.x, p.y, p.z);
    }
    glEnd();
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

// ─── 3D Nameplate ────────────────────────────────────────────────────────────
inline void DrawNameplate(float x, float y, float z, const char* name) {
    glDisable(GL_LIGHTING);
    glColor3f(0.2f, 1.0f, 0.5f);
    glRasterPos3f(x, y, z);
    for (const char* c = name; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
    glEnable(GL_LIGHTING);
}

// ─── Astronaut (Space Suit) ──────────────────────────────────────────────────
inline void DrawAstronaut(float x, float y, float z, float yaw, const char* name) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(yaw, 0, 1, 0);
    
    // Draw Nameplate above head
    if (name) {
        DrawNameplate(-1.0f, 2.5f, 0, name);
    }

    SetMaterial_Metal();
    glColor3f(0.9f, 0.9f, 0.9f); // White suit

    // Body
    glPushMatrix();
    glTranslatef(0, 1.0f, 0);
    DrawBox(0.8f, 1.2f, 0.5f);
    glPopMatrix();

    // Head (Helmet)
    glPushMatrix();
    glTranslatef(0, 1.8f, 0);
    DrawSphere(0.4f, 16, 16);
    // Visor
    glColor3f(0.1f, 0.1f, 0.1f);
    glTranslatef(0, 0, 0.3f);
    DrawSphere(0.3f, 16, 16);
    glPopMatrix();

    // Left Arm
    glColor3f(0.9f, 0.9f, 0.9f);
    glPushMatrix();
    glTranslatef(-0.6f, 1.2f, 0);
    glRotatef(30, 0, 0, 1);
    DrawCylinder(0.15f, 1.0f, 8);
    glPopMatrix();

    // Right Arm
    glPushMatrix();
    glTranslatef(0.6f, 1.2f, 0);
    glRotatef(-30, 0, 0, 1);
    DrawCylinder(0.15f, 1.0f, 8);
    glPopMatrix();

    // Legs
    glPushMatrix();
    glTranslatef(-0.25f, 0.3f, 0);
    DrawCylinder(0.18f, 0.8f, 8);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(0.25f, 0.3f, 0);
    DrawCylinder(0.18f, 0.8f, 8);
    glPopMatrix();

    SetMaterial_Reset();
    glPopMatrix();
}

// ─── Floating Wires (ISS style) ──────────────────────────────────────────────
inline void DrawFloatingWires(float x, float y, float z) {
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glTranslatef(x, y, z);
    glLineWidth(3.0f);
    
    glBegin(GL_LINE_STRIP);
    glColor3f(0.1f, 0.3f, 0.8f); // Blue wire
    for(int i=0; i<15; i++) {
        glVertex3f(i*0.4f, sinf(i*0.8f)*0.5f, cosf(i)*0.2f);
    }
    glEnd();
    
    glBegin(GL_LINE_STRIP);
    glColor3f(0.8f, 0.8f, 0.1f); // Yellow wire
    for(int i=0; i<15; i++) {
        glVertex3f(i*0.3f, cosf(i*1.2f)*0.6f - 0.5f, sinf(i)*0.4f);
    }
    glEnd();
    
    glEnable(GL_LIGHTING);
    glLineWidth(1.0f);
    glPopMatrix();
}

// ─── Bangladesh Flag ─────────────────────────────────────────────────────────
inline void DrawBDFlag(float x, float y, float z, float rotY) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(rotY, 0, 1, 0);
    glDisable(GL_LIGHTING);
    
    // Green Background
    glColor3f(0.0f, 0.42f, 0.24f); // BD Green
    DrawBox(4.0f, 2.4f, 0.05f);
    
    // Red Circle
    glColor3f(0.86f, 0.11f, 0.18f); // BD Red
    glTranslatef(-0.2f, 0, 0.03f); // Slightly offset to the left and forward
    glScalef(1.0f, 1.0f, 0.1f);
    DrawSphere(0.8f, 16, 16);
    
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

#endif // MODELS_H
