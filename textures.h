// =============================================================================
// textures.h - Procedural Texture Generation for 3D Space Station Survival
// Generates all textures programmatically (no image files required)
// =============================================================================
#ifndef TEXTURES_H
#define TEXTURES_H

#include <GL/glut.h>
#include <cmath>
#include <cstdlib>

// ─── Texture ID enum ──────────────────────────────────────────────────────────
enum TextureID {
    TEX_METAL_WALL   = 0,
    TEX_METAL_FLOOR  = 1,
    TEX_CONSOLE      = 2,
    TEX_WINDOW_FRAME = 3,
    TEX_DOOR         = 4,
    TEX_PANEL        = 5,
    TEX_REACTOR      = 6,
    TEX_HULL         = 7,
    TEX_SPACE        = 8,   // star-field skybox face
    TEX_EARTH        = 9,
    TEX_COUNT        = 10
};

static GLuint g_texIDs[TEX_COUNT];

// ─── Noise helpers ────────────────────────────────────────────────────────────
static float hash2f(int x, int y) {
    int n = x + y * 57;
    n = (n << 13) ^ n;
    return 1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f;
}

static float smoothNoise(float x, float y) {
    int ix = (int)x; float fx = x - ix;
    int iy = (int)y; float fy = y - iy;
    float v1 = hash2f(ix,   iy);
    float v2 = hash2f(ix+1, iy);
    float v3 = hash2f(ix,   iy+1);
    float v4 = hash2f(ix+1, iy+1);
    float i1 = v1 + fx*(v2-v1);
    float i2 = v3 + fx*(v4-v3);
    return i1 + fy*(i2-i1);
}

// ─── Texture generators (128×128 RGBA) ───────────────────────────────────────
static void GenTex_MetalWall(unsigned char* px, int w, int h) {
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int idx = (y * w + x) * 4;
            float nx = x / (float)w * 4.0f;
            float ny = y / (float)h * 4.0f;
            float n  = (smoothNoise(nx, ny) + smoothNoise(nx*2,ny*2)*0.5f) / 1.5f;
            n = 0.5f + n * 0.5f; // remap to [0,1]

            // Steel-blue tint with panel lines
            unsigned char base = (unsigned char)(150 + n * 80);
            bool hLine = ((y % 32) < 2);
            bool vLine = ((x % 32) < 2);
            unsigned char line = (hLine || vLine) ? 80 : 0;
            px[idx+0] = (unsigned char)(base * 0.65f) - line;
            px[idx+1] = (unsigned char)(base * 0.70f) - line;
            px[idx+2] = (unsigned char)(base * 0.85f) - line;
            px[idx+3] = 255;
        }
    }
}

static void GenTex_MetalFloor(unsigned char* px, int w, int h) {
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int idx = (y * w + x) * 4;
            float nx = x / (float)w * 6.0f;
            float ny = y / (float)h * 6.0f;
            float n  = smoothNoise(nx, ny) * 0.3f + 0.7f;

            // Checker-grid floor tiles
            int tx = x / 16, ty = y / 16;
            bool checker = (tx + ty) % 2;
            float bright = checker ? 0.55f : 0.40f;
            unsigned char b = (unsigned char)(bright * n * 255);

            // Glow-line borders on tiles
            bool border = ((x % 16 == 0) || (y % 16 == 0));
            px[idx+0] = border ? 0   : (unsigned char)(b * 0.7f);
            px[idx+1] = border ? 180 : (unsigned char)(b * 0.8f);
            px[idx+2] = border ? 255 : b;
            px[idx+3] = 255;
        }
    }
}

static void GenTex_Console(unsigned char* px, int w, int h) {
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int idx = (y * w + x) * 4;
            // Dark base with green/blue LEDs pattern
            float nx = x / (float)w;
            float ny = y / (float)h;
            bool led = ((x % 8 < 3) && (y % 8 < 3));
            float glow = led ? 1.0f : 0.0f;
            // Vary LED color by position
            bool isGreen = ((x / 8 + y / 8) % 3 == 0);
            bool isBlue  = ((x / 8 + y / 8) % 3 == 1);
            px[idx+0] = (unsigned char)(10  + glow * (isGreen ? 30  : isBlue ? 10  : 180));
            px[idx+1] = (unsigned char)(15  + glow * (isGreen ? 200 : isBlue ? 80  : 20 ));
            px[idx+2] = (unsigned char)(25  + glow * (isGreen ? 50  : isBlue ? 255 : 20 ));
            px[idx+3] = 255;
            // Add scanline
            if (y % 4 == 0) { px[idx+0] /= 2; px[idx+1] /= 2; px[idx+2] /= 2; }
        }
    }
}

static void GenTex_Door(unsigned char* px, int w, int h) {
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int idx = (y * w + x) * 4;
            // Vertical steel bands with orange trim
            int band = x / (w / 6);
            bool trim = (x % (w/6) < 2) || (y < 2) || (y >= h-2);
            unsigned char steel = 100 + band * 15;
            px[idx+0] = trim ? 220 : (unsigned char)(steel * 0.75f);
            px[idx+1] = trim ? 100 : (unsigned char)(steel * 0.80f);
            px[idx+2] = trim ? 10  : (unsigned char)(steel * 0.90f);
            px[idx+3] = 255;
        }
    }
}

static void GenTex_Panel(unsigned char* px, int w, int h) {
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int idx = (y * w + x) * 4;
            bool hRule = (y % 16 < 2);
            bool vRule = (x % 16 < 2);
            unsigned char base = 60;
            px[idx+0] = (hRule||vRule) ? 40  : base;
            px[idx+1] = (hRule||vRule) ? 120 : (unsigned char)(base * 1.1f);
            px[idx+2] = (hRule||vRule) ? 200 : (unsigned char)(base * 1.3f);
            px[idx+3] = 255;
        }
    }
}

static void GenTex_Reactor(unsigned char* px, int w, int h) {
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int idx = (y * w + x) * 4;
            float cx = (x - w/2.0f) / (w/2.0f);
            float cy = (y - h/2.0f) / (h/2.0f);
            float dist = sqrtf(cx*cx + cy*cy);
            float ring = fmodf(dist * 8.0f, 1.0f);
            float t = 1.0f - dist;
            // Cyan-to-white radial glow
            px[idx+0] = (unsigned char)(ring * 80 * t);
            px[idx+1] = (unsigned char)(200 * t + ring * 55 * t);
            px[idx+2] = (unsigned char)(255 * t);
            px[idx+3] = 255;
        }
    }
}

static void GenTex_Hull(unsigned char* px, int w, int h) {
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int idx = (y * w + x) * 4;
            float n = smoothNoise(x/(float)w*5, y/(float)h*5);
            unsigned char v = (unsigned char)(80 + n * 60);
            bool rivet = ((x % 20 == 0 && y % 20 == 0));
            px[idx+0] = rivet ? 200 : (unsigned char)(v * 0.7f);
            px[idx+1] = rivet ? 200 : (unsigned char)(v * 0.8f);
            px[idx+2] = rivet ? 220 : v;
            px[idx+3] = 255;
        }
    }
}

static void GenTex_Space(unsigned char* px, int w, int h) {
    // Star-field: dark background + random white dots
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int idx = (y * w + x) * 4;
            float r = hash2f(x * 13 + 7, y * 7 + 3);
            bool star = (r > 0.96f);
            unsigned char brightness = star ? (unsigned char)(200 + r * 55) : 0;
            px[idx+0] = brightness;
            px[idx+1] = brightness;
            px[idx+2] = star ? 255 : 5;
            px[idx+3] = 255;
        }
    }
}

static void GenTex_Earth(unsigned char* px, int w, int h) {
    // Simplified Earth: blue ocean with green landmasses using noise
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int idx = (y * w + x) * 4;
            float u = x / (float)w;
            float v2 = y / (float)h;
            float n = smoothNoise(u * 5, v2 * 5) * 0.5f +
                      smoothNoise(u * 10, v2 * 10) * 0.3f +
                      smoothNoise(u * 20, v2 * 20) * 0.2f;
            if (n > 0.55f) {
                // Land
                px[idx+0] = 34; px[idx+1] = 139; px[idx+2] = 34;
            } else if (n > 0.40f) {
                // Shallow water
                px[idx+0] = 30; px[idx+1] = 100; px[idx+2] = 200;
            } else {
                // Deep ocean
                px[idx+0] = 10; px[idx+1] = 40; px[idx+2] = 180;
            }
            // Ice caps
            if (v2 < 0.08f || v2 > 0.92f) {
                px[idx+0] = 220; px[idx+1] = 240; px[idx+2] = 255;
            }
            px[idx+3] = 255;
        }
    }
}

// ─── Build and upload all textures ───────────────────────────────────────────
inline void Textures_Init() {
    const int W = 128, H = 128;
    unsigned char pixels[W * H * 4];

    glGenTextures(TEX_COUNT, g_texIDs);

    // Helper lambda to upload after generation
    auto upload = [&](int id) {
        glBindTexture(GL_TEXTURE_2D, g_texIDs[id]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT);
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, W, H, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    };

    GenTex_MetalWall  (pixels, W, H); upload(TEX_METAL_WALL);
    GenTex_MetalFloor (pixels, W, H); upload(TEX_METAL_FLOOR);
    GenTex_Console    (pixels, W, H); upload(TEX_CONSOLE);
    GenTex_Door       (pixels, W, H); upload(TEX_DOOR);
    GenTex_Panel      (pixels, W, H); upload(TEX_PANEL);
    GenTex_Reactor    (pixels, W, H); upload(TEX_REACTOR);
    GenTex_Hull       (pixels, W, H); upload(TEX_HULL);
    GenTex_Space      (pixels, W, H); upload(TEX_SPACE);
    GenTex_Earth      (pixels, W, H); upload(TEX_EARTH);

    // Window frame reuses panel texture
    glBindTexture(GL_TEXTURE_2D, g_texIDs[TEX_WINDOW_FRAME]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GenTex_Panel(pixels, W, H);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, W, H, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    glBindTexture(GL_TEXTURE_2D, 0);
}

// ─── Bind helpers ─────────────────────────────────────────────────────────────
inline void BindTex(TextureID id) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, g_texIDs[id]);
}

inline void UnbindTex() {
    glDisable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
}

#endif // TEXTURES_H
