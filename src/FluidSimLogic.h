/*
  ==============================================================================
    FluidSimLogic — pure C++ particle simulation on a circular grid.
    No JUCE dependency, so it can be unit-tested independently.
  ==============================================================================
*/

#pragma once
#include <cstdint>
#include <cmath>

class FluidSimLogic {
public:
    static constexpr int kGridSize = 200;
    static constexpr int kNumVoices = 7;
    static constexpr int kDefaultBurstSize = 8;
    static constexpr int kDefaultMaxAge = 300;
    static constexpr float kInjectionSpeed = 3.0f;
    static constexpr int kInjectionRadius = 10;
    static constexpr int kCohesionRadius = 4;
    static constexpr float kCohesionStrength = 0.03f;
    static constexpr float kDamping = 0.85f;
    static constexpr float kMaxSpeed = 0.6f;
    static constexpr float kRestThreshold = 0.01f;

    struct Particle {
        uint8_t colorIndex = 0; // 0 = empty, 1-7 = voice color
        float px = 0.f;        // sub-cell position (fractional part)
        float py = 0.f;
        float vx = 0.f;
        float vy = 0.f;
        float ax = 0.f;        // acceleration (accumulated forces)
        float ay = 0.f;
        int age = 0;
        int maxAge = kDefaultMaxAge;
    };

    struct SyringePos {
        int x;
        int y;
        float dirX; // unit vector pointing toward center
        float dirY;
    };

    FluidSimLogic();

    void init();
    void step();
    void inject(int voiceIndex, int burstSize, int maxAge);
    void clear();

    const Particle& cellAt(int x, int y) const;
    bool isInsideMask(int x, int y) const;
    const SyringePos& syringePosition(int voice) const;

    int particleCount() const;
    bool isFull() const;

private:
    Particle grid_[kGridSize][kGridSize];
    Particle gridNext_[kGridSize][kGridSize];
    bool circleMask_[kGridSize][kGridSize];
    SyringePos syringes_[kNumVoices];
    float syringePhase_[kNumVoices] = {};
    int particleCount_ = 0;
    int maskCellCount_ = 0;

    void computeMask();
    void computeSyringePositions();

    // Simple LCG random for deterministic injection spread
    uint32_t rngState_ = 12345;
    float nextRandom(); // returns [-1, 1]
};
