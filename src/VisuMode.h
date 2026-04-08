/*
  ==============================================================================
    VisuMode — abstract base class for visualization modes.
    The rendering pipeline calls: video bg -> activeMode->paint() -> panels.
  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class VisuMode {
public:
    virtual ~VisuMode() = default;

    // MIDI event callbacks (voice indices 0-6)
    virtual void onDrumHit(int voiceIndex) = 0;
    virtual void onNoteOn(int melodicIndex, int note) = 0;
    virtual void onNoteOff(int melodicIndex) = 0;

    // Per-frame update (dt in seconds, typically 1/60)
    virtual void update(float dt) = 0;

    // Render the visualization layer into the given bounds.
    virtual void paint(Graphics& g, Rectangle<int> bounds) const = 0;

    // Mouse interaction (optional — default implementations do nothing)
    virtual void mouseDown(const MouseEvent& /*e*/) {}
    virtual void mouseDrag(const MouseEvent& /*e*/) {}
    virtual void mouseUp(const MouseEvent& /*e*/) {}
};
