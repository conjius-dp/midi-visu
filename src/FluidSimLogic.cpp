#include "FluidSimLogic.h"
#include <cstring>

static constexpr float kPi = 3.14159265358979323846f;

FluidSimLogic::FluidSimLogic() {
    init();
}

void FluidSimLogic::init() {
    clear();
    computeMask();
    computeSyringePositions();
}

void FluidSimLogic::clear() {
    std::memset(grid_, 0, sizeof(grid_));
    std::memset(gridNext_, 0, sizeof(gridNext_));
    particleCount_ = 0;
}

void FluidSimLogic::computeMask() {
    const float cx = kGridSize * 0.5f;
    const float cy = kGridSize * 0.5f;
    const float r = kGridSize * 0.5f - 1.0f;
    const float r2 = r * r;

    maskCellCount_ = 0;
    for (int y = 0; y < kGridSize; ++y)
        for (int x = 0; x < kGridSize; ++x) {
            const float dx = (float)x - cx;
            const float dy = (float)y - cy;
            circleMask_[y][x] = (dx * dx + dy * dy) <= r2;
            if (circleMask_[y][x]) ++maskCellCount_;
        }
}

void FluidSimLogic::computeSyringePositions() {
    const float cx = kGridSize * 0.5f;
    const float cy = kGridSize * 0.5f;
    const float r = kGridSize * 0.5f - 1.0f;
    const float inset = (float)kInjectionRadius + 2.0f;

    for (int i = 0; i < kNumVoices; ++i) {
        const float angle = (float)i * 2.0f * kPi / (float)kNumVoices;
        const float cosA = std::cos(angle);
        const float sinA = std::sin(angle);

        syringes_[i].x = (int)(cx + (r - inset) * cosA);
        syringes_[i].y = (int)(cy + (r - inset) * sinA);
        syringes_[i].dirX = -cosA;
        syringes_[i].dirY = -sinA;
    }
}

float FluidSimLogic::nextRandom() {
    rngState_ = rngState_ * 1664525u + 1013904223u;
    return ((float)(rngState_ >> 16) / 32768.0f) - 1.0f;
}

static float clampF(float v, float lo, float hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

void FluidSimLogic::inject(int voiceIndex, int /*burstSize*/, int maxAge) {
    if (voiceIndex < 0 || voiceIndex >= kNumVoices) return;

    const auto& syr = syringes_[voiceIndex];
    const uint8_t color = (uint8_t)(voiceIndex + 1);
    const int r = kInjectionRadius;
    const int r2 = r * r;
    const int pushR = r + 4;
    const int pushR2 = pushR * pushR;

    const float sweep = std::sin(syringePhase_[voiceIndex]) * 0.6f;
    syringePhase_[voiceIndex] += 0.15f; // slow sweep

    const float cosS = std::cos(sweep);
    const float sinS = std::sin(sweep);
    const float dirX = syr.dirX * cosS - syr.dirY * sinS;
    const float dirY = syr.dirX * sinS + syr.dirY * cosS;

    // Push nearby particles
    for (int dy = -pushR; dy <= pushR; ++dy) {
        for (int dx = -pushR; dx <= pushR; ++dx) {
            const int d2 = dx * dx + dy * dy;
            if (d2 > pushR2 || d2 < 1) continue;

            const int gx = syr.x + dx;
            const int gy = syr.y + dy;
            if (gx < 0 || gx >= kGridSize || gy < 0 || gy >= kGridSize) continue;

            auto& existing = grid_[gy][gx];
            if (existing.colorIndex == 0) continue;

            const float dist = std::sqrt((float)d2);
            const float pushStrength = kInjectionSpeed * 2.0f * (1.0f - dist / (float)pushR);
            existing.vx += (float)dx / dist * pushStrength;
            existing.vy += (float)dy / dist * pushStrength;
        }
    }

    // Place new particles in empty cells only
    for (int dy = -r; dy <= r; ++dy) {
        for (int dx = -r; dx <= r; ++dx) {
            if (dx * dx + dy * dy > r2) continue;
            const int gx = syr.x + dx;
            const int gy = syr.y + dy;
            if (gx < 0 || gx >= kGridSize || gy < 0 || gy >= kGridSize) continue;
            if (!circleMask_[gy][gx]) continue;
            if (grid_[gy][gx].colorIndex != 0) continue;

            Particle p;
            p.colorIndex = color;
            p.px = (float)gx;
            p.py = (float)gy;
            p.vx = dirX * kInjectionSpeed;
            p.vy = dirY * kInjectionSpeed;
            p.ax = dirX * kInjectionSpeed;
            p.ay = dirY * kInjectionSpeed;
            p.age = 0;
            p.maxAge = maxAge;

            grid_[gy][gx] = p;
            ++particleCount_;
        }
    }
}

void FluidSimLogic::step() {
    // --- Forces pass (only on particles that need it) ---
    for (int y = 0; y < kGridSize; ++y) {
        for (int x = 0; x < kGridSize; ++x) {
            auto& p = grid_[y][x];
            if (p.colorIndex == 0) continue;

            const bool hasMotion = (p.vx != 0.f || p.vy != 0.f || p.ax != 0.f || p.ay != 0.f);

            // Still + interior = skip entirely
            if (!hasMotion) {
                if (y > 0 && grid_[y-1][x].colorIndex == p.colorIndex &&
                    y < kGridSize-1 && grid_[y+1][x].colorIndex == p.colorIndex &&
                    x > 0 && grid_[y][x-1].colorIndex == p.colorIndex &&
                    x < kGridSize-1 && grid_[y][x+1].colorIndex == p.colorIndex)
                    continue;
            }

            // Cohesion only for isolated particles (no adjacent same-color)
            bool hasAdjacentSame = false;
            if (y > 0 && grid_[y-1][x].colorIndex == p.colorIndex) hasAdjacentSame = true;
            if (!hasAdjacentSame && y < kGridSize-1 && grid_[y+1][x].colorIndex == p.colorIndex) hasAdjacentSame = true;
            if (!hasAdjacentSame && x > 0 && grid_[y][x-1].colorIndex == p.colorIndex) hasAdjacentSame = true;
            if (!hasAdjacentSame && x < kGridSize-1 && grid_[y][x+1].colorIndex == p.colorIndex) hasAdjacentSame = true;

            if (!hasAdjacentSame) {
                const int r0 = kCohesionRadius;
                const int yMin = (y - r0 < 0) ? 0 : y - r0;
                const int yMax = (y + r0 >= kGridSize) ? kGridSize - 1 : y + r0;
                const int xMin = (x - r0 < 0) ? 0 : x - r0;
                const int xMax = (x + r0 >= kGridSize) ? kGridSize - 1 : x + r0;

                for (int ny = yMin; ny <= yMax; ++ny)
                    for (int nx = xMin; nx <= xMax; ++nx) {
                        if (nx == x && ny == y) continue;
                        if (grid_[ny][nx].colorIndex != p.colorIndex) continue;
                        const float dx = (float)(nx - x);
                        const float dy = (float)(ny - y);
                        const float dist2 = dx * dx + dy * dy;
                        p.ax += dx * kCohesionStrength / dist2;
                        p.ay += dy * kCohesionStrength / dist2;
                    }
            }

            p.vx += p.ax;
            p.vy += p.ay;
            p.ax *= 0.85f;
            p.ay *= 0.85f;
            p.vx *= kDamping;
            p.vy *= kDamping;

            if (p.vx * p.vx + p.vy * p.vy + p.ax * p.ax + p.ay * p.ay
                < kRestThreshold * kRestThreshold) {
                p.vx = 0.f;  p.vy = 0.f;
                p.ax = 0.f;  p.ay = 0.f;
            }

            p.px = (float)x;
            p.py = (float)y;
            p.vx = clampF(p.vx, -kMaxSpeed, kMaxSpeed);
            p.vy = clampF(p.vy, -kMaxSpeed, kMaxSpeed);
        }
    }

    // --- Movement ---
    // Strategy: every particle RESERVES its original cell first.
    // Then moving particles try to move. If they succeed, they free the reservation.
    // This guarantees no particle is ever homeless.

    // Start by copying the entire grid — every particle keeps its cell.
    std::memcpy(gridNext_, grid_, sizeof(grid_));

    // Age all particles
    for (int y = 0; y < kGridSize; ++y)
        for (int x = 0; x < kGridSize; ++x)
            if (gridNext_[y][x].colorIndex != 0)
                gridNext_[y][x].age++;

    // Now process moving particles: try to relocate them.
    for (int y = 0; y < kGridSize; ++y) {
        for (int x = 0; x < kGridSize; ++x) {
            auto& p = gridNext_[y][x];
            if (p.colorIndex == 0) continue;
            if (p.vx == 0.f && p.vy == 0.f && p.ax == 0.f && p.ay == 0.f)
                continue; // stationary — already in place

            p.px += p.vx;
            p.py += p.vy;

            int tx = (int)std::round(p.px);
            int ty = (int)std::round(p.py);

            // Wall: stop, stay in place
            if (tx < 0 || tx >= kGridSize || ty < 0 || ty >= kGridSize
                || !circleMask_[ty][tx]) {
                p.vx = 0.f; p.vy = 0.f;
                p.ax = 0.f; p.ay = 0.f;
                p.px = (float)x; p.py = (float)y;
                continue; // stay at (x,y) — already there
            }

            // Same cell — no move needed
            if (tx == x && ty == y) continue;

            // Target free? Swap: move there, clear origin.
            if (gridNext_[ty][tx].colorIndex == 0) {
                gridNext_[ty][tx] = p;
                gridNext_[y][x].colorIndex = 0;
                std::memset(&gridNext_[y][x], 0, sizeof(Particle));
                continue;
            }

            // Target occupied — plastic collision, stay in place
            p.vx = 0.f; p.vy = 0.f;
            p.ax = 0.f; p.ay = 0.f;
            p.px = (float)x; p.py = (float)y;
            // Already at (x,y) in gridNext_, nothing to do
        }
    }

    // gridNext_ is already the final state — copy back and recount
    std::memcpy(grid_, gridNext_, sizeof(grid_));
    int count = 0;
    for (int y = 0; y < kGridSize; ++y)
        for (int x = 0; x < kGridSize; ++x)
            if (grid_[y][x].colorIndex != 0) ++count;
    particleCount_ = count;
}

const FluidSimLogic::Particle& FluidSimLogic::cellAt(int x, int y) const {
    static const Particle empty{};
    if (x < 0 || x >= kGridSize || y < 0 || y >= kGridSize) return empty;
    return grid_[y][x];
}

bool FluidSimLogic::isInsideMask(int x, int y) const {
    if (x < 0 || x >= kGridSize || y < 0 || y >= kGridSize) return false;
    return circleMask_[y][x];
}

const FluidSimLogic::SyringePos& FluidSimLogic::syringePosition(int voice) const {
    static const SyringePos dummy{0, 0, 0.f, 0.f};
    if (voice < 0 || voice >= kNumVoices) return dummy;
    return syringes_[voice];
}

int FluidSimLogic::particleCount() const {
    return particleCount_;
}

bool FluidSimLogic::isFull() const {
    return particleCount_ >= maskCellCount_;
}
