#pragma once
#include "core/Common.h"

namespace ap3d {

/// High-resolution frame timer with delta time and FPS tracking.
class Timer {
public:
    Timer();

    /// Call at the start of each frame to update delta time.
    void tick();

    /// Get the time elapsed since last frame in seconds.
    [[nodiscard]] float deltaTime() const { return m_deltaTime; }

    /// Get smoothed FPS.
    [[nodiscard]] float fps() const { return m_fps; }

    /// Get total elapsed time since timer creation in seconds.
    [[nodiscard]] float totalTime() const { return m_totalTime; }

    /// Get the raw frame count.
    [[nodiscard]] uint64_t frameCount() const { return m_frameCount; }

private:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = Clock::time_point;

    TimePoint m_lastTime;
    float m_deltaTime = 0.0f;
    float m_totalTime = 0.0f;
    float m_fps = 0.0f;
    float m_fpsAccumulator = 0.0f;
    int m_fpsFrameCount = 0;
    uint64_t m_frameCount = 0;
};

} // namespace ap3d
