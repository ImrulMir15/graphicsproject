# 3D Amusement Park Simulator

A high-performance, stand-alone Windows desktop 3D Amusement Park Simulator written in C++20 and OpenGL 4.6. This project features a unified world coordinate system, advanced input management, custom physics/collisions, dynamic lighting, 3D positional audio, interactive minigames, and a comprehensive build system.

## 🚀 Key Highlights & Realism Upgrades

- **Unified World Space**: The entire amusement park is loaded into a single coordinate system. No loading screens or zones.
- **Dynamic Sky & Sunset**: A custom sky dome with a multi-stop sunset gradient (indigo to magenta to golden orange) and an active glowing sun disc synced with directional lighting.
- **Cohesive Fog System**: Distance-based fog that perfectly matches the golden-orange sunset horizon, allowing elements to blend naturally.
- **Glitch-Free Roller Coaster**: 
  - **Closed-Loop Track**: The track is fully closed and periodic ($2\pi$-multiples) to prevent teleportation or discontinuities.
  - **Smooth Transitions**: Periodic height/radius formulas enforce matched derivatives (slopes) at all boundaries.
  - **Dynamic Carriage Orientation**: The coaster carriage computes a full 3D rotation matrix from the tangent and up vectors, pitching and rolling realistically along slopes and banks.
- **Blinn-Phong Lighting**: Directional sunlight combined with up to 16 point lights, specular mapping, and tone mapping/gamma correction.
- **Frustum Culling**: AABB-based view frustum culling for consistent 60+ FPS performance.

---

## 🎡 Park Attractions & Rides

1. **Ferris Wheel**: Rotating A-frame support structure with hanging gondolas and blinking neon lights.
2. **Roller Coaster**: A multi-rail coaster track with sleepers and a central spine. Includes a carriage following the track in 3D.
3. **Carousel**: Mahogany platform with rotating poles and bobbing horses.
4. **Swing Ride**: Rotating disc with swinging chains and seats.
5. **Drop Tower**: Steel tower structure with high-speed rising, pausing, and gravity-based dropping carriage.

---

## 🎯 Playable Minigames
Walk up to any minigame zone and press **E** to play:
- **Shooting Gallery**: Shoot moving targets with a crosshair.
- **Ring Toss**: Aim and throw rings to land them on pegs.
- **Basketball Challenge**: Shoot hoops with physics-based basketballs.

---

## ⌨️ Controls

| Key / Input | Action |
|-------------|--------|
| **W / A / S / D** | Move Forward / Left / Backward / Right |
| **Space** | Move Up / Jump |
| **Left Shift** | Move Down |
| **Mouse Move** | Look around (FPS Camera) |
| **Scroll Wheel** | Zoom (Adjust FOV) |
| **E** | Interact (Enter Ride / Start Minigame) |
| **Q** | Quit Active Ride or Minigame |
| **ESC** | Open Settings Panel / Unlock Cursor |
| **TAB** | Toggle HUD Display |

---

## 🛠️ Build and Run Instructions

### Prerequisites
Make sure you have the following installed:
1. **CMake 3.20+**
2. **Visual Studio 2022** (with "Desktop development with C++" workload) OR **MSYS2 (MinGW-w64)**
3. **Git**

### Automated Build (Recommended)
We have provided automated scripts inside the `AmusementPark3D` directory to handle dependency fetching, configuration, and building:

1. **Setup Dependencies**:
   Open a terminal and run:
   ```cmd
   cd AmusementPark3D
   setup.bat
   ```
2. **Compile and Run**:
   ```cmd
   build.bat
   ```

### Manual CMake Build
Alternatively, configure and compile using standard CMake:
```cmd
cd AmusementPark3D
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release -j 8
```
Run the executable from:
```cmd
build\bin\Release\AmusementPark3D.exe
```

---

## 📂 Project Structure

```
g:/graphics lab/Project/
├── AmusementPark3D/
│   ├── CMakeLists.txt        # CMake build configuration
│   ├── setup.bat             # Automates vendor and GLAD setup
│   ├── build.bat             # Automates compilation and launches the executable
│   ├── assets/               # Textures, audio, and shaders
│   ├── vendor/               # Third-party libraries (GLFW, ImGui, OpenAL, etc.)
│   └── src/                  # Source Code
│       ├── main.cpp          # Entry Point
│       ├── core/             # Engine, Window, and Timing loops
│       ├── camera/           # FPS camera with collision constraint
│       ├── input/            # Keyboard/Mouse input managers
│       ├── physics/          # Colliders and collision resolution
│       ├── renderer/         # Shader, Mesh, Texture, and Renderer systems
│       ├── world/            # Park objects, pathways, and environment
│       ├── rides/            # Animated rides logic (Rides.cpp)
│       └── minigames/        # Minigame states and logic
└── README.md                 # Project documentation (This file)
```
