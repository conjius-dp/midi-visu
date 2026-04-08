/*
  ==============================================================================
    FluidSimMode — VisuMode implementation for the particle-based fluid sim.
  ==============================================================================
*/

#pragma once
#include "VisuMode.h"
#include "FluidSimLogic.h"
#include "FluidSimRenderer.h"

class FluidSimMode : public VisuMode {
public:
    FluidSimMode();

    void onDrumHit(int voiceIndex) override;
    void onNoteOn(int melodicIndex, int note) override;
    void onNoteOff(int melodicIndex) override;
    void update(float dt) override;
    void paint(Graphics& g, Rectangle<int> bounds) const override;

    // Configurable parameters (set by editor from slider values)
    int stepsPerFrame = 1;
    int burstSize = FluidSimLogic::kDefaultBurstSize;
    int particleMaxAge = FluidSimLogic::kDefaultMaxAge;

    void debugInjectAll();
    FluidSimLogic& logic() { return sim_; }
    const FluidSimLogic& logic() const { return sim_; }

private:
    FluidSimLogic sim_;
    FluidSimRenderer renderer_;
    mutable Image image_;
};
