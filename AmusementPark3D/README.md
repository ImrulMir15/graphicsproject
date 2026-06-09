# AmusementPark3D

A complete 3D amusement park simulator written in C++20 with OpenGL 4.6 Core Profile.

## Features

- **Unified World**: Single continuous coordinate system — no loading zones or teleportation
- **5 Animated Rides**: Ferris Wheel, Roller Coaster, Carousel, Swing Ride, Drop Tower
- **3 Playable Minigames**: Shooting Gallery, Ring Toss, Basketball Challenge
- **FPS Camera**: Mouse look with raw input, smooth movement, pitch clamping
- **Collision System**: Sphere-AABB, Sphere-Sphere, Sphere-Cylinder with iterative resolution
- **Blinn-Phong Lighting**: Directional sun + 16 point lights + fog + tone mapping
- **Frustum Culling**: AABB-based culling for 60+ FPS performance
- **3D Positional Audio**: OpenAL Soft with procedural sound synthesis
- **Dear ImGui UI**: HUD, crosshair, notifications, minigame scoreboard, settings panel
- **Procedural Geometry**: All objects generated from primitives — no external model files required

## Park Contents

- Entrance Gate with arch
- Ticket Booth
- Food Court with tables and awning
- Decorative Fountain
- Benches along pathways
- 16 Lamp Posts
- ~30 Trees with varied scales
- Flower gardens
- Continuous boundary walls (no escape routes)
- Central + cross pathways

## Controls

| Input | Action |
|-------|--------|
| **Mouse** | Look around (primary input) |
| **W/A/S/D** | Move forward/left/backward/right |
| **Space** | Move up |
| **Left Shift** | Move down |
| **E** | Interact with rides/minigames |
| **Q** | Quit active minigame |
| **ESC** | Settings menu / unlock cursor |
| **TAB** | Toggle HUD |
| **Scroll** | Zoom (FOV adjust) |

## Prerequisites

1. **CMake 3.20+**: https://cmake.org/download/
2. **Visual Studio 2022** (Community Edition is free): https://visualstudio.microsoft.com/
   - Install "Desktop development with C++" workload
3. **Git**: https://git-scm.com/download/win

## Build Instructions

### Option 1: Using the build script

```batch
cd AmusementPark3D
build.bat
```

### Option 2: Manual CMake commands

```batch
cd AmusementPark3D

:: Configure (uses Visual Studio 2022 generator)
cmake -B build -G "Visual Studio 17 2022" -A x64

:: Build Release
cmake --build build --config Release -j 8
```

### Option 3: Using Ninja (faster builds)

Open "x64 Native Tools Command Prompt for VS 2022", then:

```batch
cd "g:\graphics lab\Project\AmusementPark3D"
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j 8
```

## Running

After building:

```batch
cd build\bin\Release
AmusementPark3D.exe
```

The executable will be at `build/bin/Release/AmusementPark3D.exe` (or `build/bin/AmusementPark3D.exe` with Ninja).

## Project Structure

```
AmusementPark3D/
├── CMakeLists.txt              # Build configuration
├── build.bat                   # One-click build script
├── setup.bat                   # Dependency setup script
├── README.md
├── assets/                     # Runtime assets (auto-copied)
│   ├── textures/
│   ├── models/
│   ├── audio/
│   └── shaders/
├── vendor/                     # Vendored dependencies
│   ├── glad/                   # OpenGL 4.6 loader (pre-generated)
│   └── stb/                    # stb_image (downloaded)
└── src/                        # Engine source code
    ├── main.cpp                # Entry point
    ├── core/
    │   ├── Common.h            # Shared includes and forward declarations
    │   ├── Engine.h/cpp        # Main engine orchestrator
    │   ├── Timer.h/cpp         # Frame timing
    │   └── Window.h/cpp        # GLFW window wrapper
    ├── input/
    │   └── InputManager.h/cpp  # Keyboard + mouse + raw input
    ├── camera/
    │   └── Camera.h/cpp        # FPS camera with collision support
    ├── renderer/
    │   ├── Shader.h/cpp        # GLSL shader program
    │   ├── Mesh.h/cpp          # VAO/VBO/EBO mesh wrapper
    │   ├── Texture.h/cpp       # Texture loading + procedural generation
    │   ├── ProceduralMeshes.h/cpp  # Geometry factory
    │   └── Renderer.h/cpp      # Blinn-Phong renderer with culling
    ├── physics/
    │   └── PhysicsWorld.h/cpp  # Collision detection and response
    ├── audio/
    │   └── AudioManager.h/cpp  # OpenAL 3D audio + synthesis
    ├── ui/
    │   └── UISystem.h/cpp      # Dear ImGui HUD + menus
    ├── world/
    │   └── ParkWorld.h/cpp     # Unified park environment
    ├── rides/
    │   └── Rides.h/cpp         # All 5 ride types
    └── minigames/
        └── Minigames.h/cpp     # All 3 minigame types
```

## Architecture

- **Modular Design**: Each system (renderer, physics, audio, input, UI) is fully isolated
- **RAII**: All GPU/audio resources use RAII via destructors and move semantics
- **FetchContent**: Dependencies (GLFW, GLM, ImGui, OpenAL) downloaded automatically by CMake
- **No External Assets Required**: All geometry is procedural, all textures are generated, all sounds are synthesized
- **Single Coordinate System**: Every object exists in one unified world space

## Technology Stack

| Component | Technology |
|-----------|-----------|
| Language | C++20 |
| Graphics | OpenGL 4.6 Core Profile |
| Window/Input | GLFW 3.3.8 |
| OpenGL Loader | GLAD2 |
| Math | GLM 0.9.9.8 |
| Texture Loading | stb_image |
| Audio | OpenAL Soft 1.23.1 |
| UI | Dear ImGui 1.89.9 |
| Build System | CMake 3.20+ |
| Target | Windows 10/11 x64 |
