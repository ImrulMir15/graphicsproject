#pragma once

/// Forward declarations and common includes for the entire engine.
/// Every source file should include this as the first project header.

// Standard library
#include <cstdint>
#include <cmath>
#include <memory>
#include <string>
#include <vector>
#include <array>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <chrono>
#include <optional>
#include <variant>

// OpenGL (GLAD2 must be included before GLFW)
#include <glad/gl.h>
#include <GLFW/glfw3.h>

// GLM
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/norm.hpp>

namespace ap3d {

// Type aliases
using uint = unsigned int;
using uchar = unsigned char;

// Constants
constexpr float PI = 3.14159265358979323846f;
constexpr float TWO_PI = 2.0f * PI;
constexpr float HALF_PI = PI / 2.0f;
constexpr float DEG_TO_RAD = PI / 180.0f;
constexpr float RAD_TO_DEG = 180.0f / PI;

// Forward declarations
class Engine;
class Window;
class Renderer;
class InputManager;
class Camera;
class PhysicsWorld;
class AudioManager;
class AssetManager;
class UISystem;
class Scene;
class SceneObject;

} // namespace ap3d
