/*
  ==============================================================================
    CirclesMode — VisuMode implementation for the original wobbling circles.
    Owns all circle-specific state extracted from MidiVisuEditor.
  ==============================================================================
*/

#pragma once
#include "VisuMode.h"
#include "SvgShapeManager.h"
#include "SvgWobbleLogic.h"
#include "AppConstants.h"

class MidiVisuEditor; // forward declaration for accessing slider values

class CirclesMode : public VisuMode {
public:
    explicit CirclesMode(MidiVisuEditor& editor);

    void onDrumHit(int voiceIndex) override;
    void onNoteOn(int melodicIndex, int note) override;
    void onNoteOff(int melodicIndex) override;
    void update(float dt) override;
    void paint(Graphics& g, Rectangle<int> bounds) const override;
    void mouseDown(const MouseEvent& e) override;
    void mouseDrag(const MouseEvent& e) override;
    void mouseUp(const MouseEvent& e) override;

    // Position management (for save/load)
    Point<float> circlePos[7];
    void initDefaultPositions(int width, int height);

    // Floating physics state (needs to be accessible for clock kick)
    Point<float> floatOffset[7];
    Point<float> floatVel[7];
    float ballMass[7] = {1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f};

    // Radii (readable by interaction manager for hit-testing)
    float smoothedRadius[4]{0.f, 0.f, 0.f, 0.f};
    float drumSmoothedRadius[4]{0.f, 0.f, 0.f, 0.f};

    SvgShapeManager& shapes() { return svgShapeManager_; }

private:
    MidiVisuEditor& editor_;

    int lastDrumHitCount_[4]{0, 0, 0, 0};
    static constexpr float kSmoothing = 0.10f;
    static constexpr float kDrumSmoothing = 0.1f;

    SvgWobbleLogic::VoiceState wobbleState_[7];
    SvgShapeManager svgShapeManager_;

    // Drag state
    int draggedCircle_ = -1;
    Point<float> dragOffset_;

    // Helpers
    float minRadius() const;
    float maxRadius() const;
};
