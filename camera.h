// =============================================================================
// camera.h - Camera System for 3D Space Station Survival
// Implements: First-Person, Third-Person, Free Cinematic cameras
// =============================================================================
#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glut.h>
#include <cmath>

// ─── Camera Mode Enum ────────────────────────────────────────────────────────
enum CameraMode {
    CAM_FIRST_PERSON = 1,
    CAM_THIRD_PERSON = 2,
    CAM_FREE         = 3
};

// ─── Camera State ────────────────────────────────────────────────────────────
struct Camera {
    // Position
    float posX, posY, posZ;

    // Euler angles (degrees)
    float yaw;    // horizontal rotation
    float pitch;  // vertical rotation

    // Derived direction vectors
    float dirX, dirY, dirZ;   // look direction
    float rightX, rightZ;     // strafe direction

    // Settings
    CameraMode mode;
    float moveSpeed;
    float sensitivity;
    float thirdPersonDist;  // distance behind for 3rd-person
    float thirdPersonHeight;

    // Target position (used for 3rd-person orbit)
    float targetX, targetY, targetZ;
};

// ─── Global camera instance ───────────────────────────────────────────────────
static Camera g_camera;

// ─── Initialize camera ───────────────────────────────────────────────────────
inline void Camera_Init() {
    g_camera.posX   =  0.0f;
    g_camera.posY   =  2.0f;
    g_camera.posZ   =  10.0f;
    g_camera.yaw    = -90.0f; // look down -Z
    g_camera.pitch  =  -5.0f;
    g_camera.mode   = CAM_THIRD_PERSON; // Default to 3rd person to see space suit!
    g_camera.moveSpeed        = 0.25f; // Faster spacewalk movement
    g_camera.sensitivity      = 0.25f;
    g_camera.thirdPersonDist  = 15.0f; // Pulled back to see more of the station!
    g_camera.thirdPersonHeight= 5.0f;
    g_camera.targetX = 0.0f;
    g_camera.targetY = 0.0f;
    g_camera.targetZ = 0.0f;
}

// ─── Update derived direction vectors ────────────────────────────────────────
inline void Camera_UpdateVectors() {
    float pitchRad = g_camera.pitch * (float)M_PI / 180.0f;
    float yawRad   = g_camera.yaw   * (float)M_PI / 180.0f;

    g_camera.dirX  = cosf(pitchRad) * cosf(yawRad);
    g_camera.dirY  = sinf(pitchRad);
    g_camera.dirZ  = cosf(pitchRad) * sinf(yawRad);

    // Right vector (cross of dir and world-up, projected onto XZ)
    g_camera.rightX = -sinf(yawRad);
    g_camera.rightZ =  cosf(yawRad);
}

// ─── Clamp pitch ─────────────────────────────────────────────────────────────
inline void Camera_ClampPitch() {
    if (g_camera.pitch >  89.0f) g_camera.pitch =  89.0f;
    if (g_camera.pitch < -89.0f) g_camera.pitch = -89.0f;
}

// ─── Apply the camera view transform ─────────────────────────────────────────
inline void Camera_Apply() {
    Camera_UpdateVectors();
    Camera& c = g_camera;

    if (c.mode == CAM_THIRD_PERSON) {
        // Orbit behind the target
        float ex = c.targetX - c.dirX * c.thirdPersonDist;
        float ey = c.targetY + c.thirdPersonHeight;
        float ez = c.targetZ - c.dirZ * c.thirdPersonDist;
        gluLookAt(
            ex, ey, ez,
            c.targetX, c.targetY + 1.0f, c.targetZ,
            0.0f, 1.0f, 0.0f
        );
    } else {
        // First-person or Free: look from pos in direction
        gluLookAt(
            c.posX, c.posY, c.posZ,
            c.posX + c.dirX,
            c.posY + c.dirY,
            c.posZ + c.dirZ,
            0.0f, 1.0f, 0.0f
        );
    }
}

// ─── Move forward / backward ─────────────────────────────────────────────────
inline void Camera_MoveForward(float amount) {
    Camera_UpdateVectors();
    g_camera.posX += g_camera.dirX * amount;
    g_camera.posZ += g_camera.dirZ * amount;
    if (g_camera.mode == CAM_FREE)
        g_camera.posY += g_camera.dirY * amount;
    // Sync target for 3rd-person
    g_camera.targetX = g_camera.posX;
    g_camera.targetY = g_camera.posY - 1.5f;
    g_camera.targetZ = g_camera.posZ;
}

inline void Camera_MoveBackward(float amount) { Camera_MoveForward(-amount); }

// ─── Strafe left / right ─────────────────────────────────────────────────────
inline void Camera_StrafeRight(float amount) {
    Camera_UpdateVectors();
    g_camera.posX += g_camera.rightX * amount;
    g_camera.posZ += g_camera.rightZ * amount;
    g_camera.targetX = g_camera.posX;
    g_camera.targetZ = g_camera.posZ;
}

inline void Camera_StrafeLeft(float amount) { Camera_StrafeRight(-amount); }

// ─── Rotate yaw / pitch (mouse look) ─────────────────────────────────────────
inline void Camera_RotateYaw(float degrees) {
    g_camera.yaw += degrees;
}

inline void Camera_RotatePitch(float degrees) {
    g_camera.pitch += degrees;
    Camera_ClampPitch();
}

// ─── Arrow-key rotation helpers ──────────────────────────────────────────────
inline void Camera_ArrowLeft()  { g_camera.yaw   -= 2.0f; }
inline void Camera_ArrowRight() { g_camera.yaw   += 2.0f; }
inline void Camera_ArrowUp()    { g_camera.pitch += 2.0f; Camera_ClampPitch(); }
inline void Camera_ArrowDown()  { g_camera.pitch -= 2.0f; Camera_ClampPitch(); }

// ─── Switch camera mode ───────────────────────────────────────────────────────
inline void Camera_SetMode(CameraMode m) {
    g_camera.mode = m;
}

// ─── Collision-check boundary ────────────────────────────────────────────────
// Simple AABB boundary for the space station interior
inline void Camera_ClampToBounds(float minX, float maxX,
                                  float minY, float maxY,
                                  float minZ, float maxZ) {
    if (g_camera.posX < minX) g_camera.posX = minX;
    if (g_camera.posX > maxX) g_camera.posX = maxX;
    if (g_camera.posY < minY) g_camera.posY = minY;
    if (g_camera.posY > maxY) g_camera.posY = maxY;
    if (g_camera.posZ < minZ) g_camera.posZ = minZ;
    if (g_camera.posZ > maxZ) g_camera.posZ = maxZ;
}

#endif // CAMERA_H
