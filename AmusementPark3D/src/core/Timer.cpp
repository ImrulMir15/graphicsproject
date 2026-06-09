#include "core/Timer.h"

namespace ap3d {

Timer::Timer()
    : m_lastTime(Clock::now())
{
}

void Timer::tick() {
    const auto now = Clock::now();
    const auto elapsed = std::chrono::duration<float>(now - m_lastTime);
    m_deltaTime = elapsed.count();
    m_lastTime = now;

    // Clamp delta time to prevent physics explosions after breakpoints/stalls
    if (m_deltaTime > 0.1f) {
        m_deltaTime = 0.016667f; // Cap at ~60fps equivalent
    }

    m_totalTime += m_deltaTime;
    m_frameCount++;

    // Smooth FPS calculation (update every 0.5 seconds)
    m_fpsAccumulator += m_deltaTime;
    m_fpsFrameCount++;
    if (m_fpsAccumulator >= 0.5f) {
        m_fps = static_cast<float>(m_fpsFrameCount) / m_fpsAccumulator;
        m_fpsAccumulator = 0.0f;
        m_fpsFrameCount = 0;
    }
}

} // namespace ap3d
