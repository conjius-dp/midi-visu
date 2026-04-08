/*
  ==============================================================================
    FluidSimRenderer — converts FluidSimLogic grid state into a JUCE Image.
    Draws syringe nozzles around the circle edge.
  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "FluidSimLogic.h"

class FluidSimRenderer {
public:
    FluidSimRenderer();

    void render(const FluidSimLogic& sim, Image& target) const;
    void drawSyringes(Graphics& g, const FluidSimLogic& sim,
                      Rectangle<int> bounds) const;

private:
    uint32 colorLUT_[8]; // index 0 = black, 1-7 = voice ARGB
    void buildColorLUT();
};
