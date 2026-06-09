#pragma once
#include "core/Common.h"

namespace ap3d {

/// Sound buffer ID (stub).
using SoundBuffer = unsigned int;

/// Sound source ID (stub).
using SoundSource = unsigned int;

/// Stub audio manager — audio disabled at build time.
/// All methods are no-ops that return safe defaults.
class AudioManager {
public:
    AudioManager() = default;
    ~AudioManager() = default;

    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    bool init() {
        std::cout << "[Audio] Audio disabled (AP3D_NO_AUDIO)\n";
        return true;
    }

    void shutdown() {}

    void setListenerPosition(const glm::vec3&, const glm::vec3&, const glm::vec3&) {}

    SoundBuffer createToneBuffer(float, float, float = 44100.0f) { return 0; }
    SoundBuffer createNoiseBuffer(float, float = 44100.0f) { return 0; }
    SoundBuffer createChimeBuffer(float, float) { return 0; }

    SoundSource createSource(bool = false, float = 1.0f) { return 0; }

    void playAtPosition(SoundSource, SoundBuffer, const glm::vec3&, float = 1.0f) {}
    void play(SoundSource, SoundBuffer, float = 1.0f) {}
    void stop(SoundSource) {}

    [[nodiscard]] bool isPlaying(SoundSource) const { return false; }

    void setSourcePosition(SoundSource, const glm::vec3&) {}
    void setSourceGain(SoundSource, float) {}
    void setSourceLooping(SoundSource, bool) {}

    void setMasterVolume(float) {}

    void deleteBuffer(SoundBuffer) {}
    void deleteSource(SoundSource) {}

    [[nodiscard]] bool isAvailable() const { return false; }
};

} // namespace ap3d
