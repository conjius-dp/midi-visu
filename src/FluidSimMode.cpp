#include "FluidSimMode.h"

FluidSimMode::FluidSimMode() {
    sim_.init();
    // Inject a test burst from each syringe so the mode starts with visible content
    for (int v = 0; v < FluidSimLogic::kNumVoices; ++v)
        sim_.inject(v, 0, 300);
}

void FluidSimMode::debugInjectAll() {
    for (int v = 0; v < FluidSimLogic::kNumVoices; ++v)
        sim_.inject(v, 0, 300);
}

void FluidSimMode::onDrumHit(int voiceIndex) {
    sim_.inject(voiceIndex, burstSize, particleMaxAge);
}

void FluidSimMode::onNoteOn(int melodicIndex, int note) {
    (void)note;
    // melodicIndex 1-3 -> voice 4-6
    sim_.inject(3 + melodicIndex, burstSize, particleMaxAge);
}

void FluidSimMode::onNoteOff(int melodicIndex) {
    (void)melodicIndex;
    // No action on note-off for fluid sim
}

void FluidSimMode::update(float /*dt*/) {
    for (int i = 0; i < stepsPerFrame; ++i)
        sim_.step();
}

void FluidSimMode::paint(Graphics& g, Rectangle<int> bounds) const {
    // Render grid to image
    renderer_.render(sim_, image_);

    // Compute square arena centered in bounds
    const int side = jmin(bounds.getWidth(), bounds.getHeight());
    const int ax = bounds.getX() + (bounds.getWidth() - side) / 2;
    const int ay = bounds.getY() + (bounds.getHeight() - side) / 2;
    const Rectangle<int> arena(ax, ay, side, side);

    // Draw the fluid image scaled to arena
    g.drawImage(image_, arena.toFloat());

    // Draw syringe nozzles
    renderer_.drawSyringes(g, sim_, arena);

    // Debug: show particle count
    g.setColour(Colours::white);
    g.setFont(16.0f);
    g.drawText("Particles: " + String(sim_.particleCount()),
               10, 10, 200, 20, Justification::left);
}
