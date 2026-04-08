#include "CirclesMode.h"
#include "PluginEditor.h"

CirclesMode::CirclesMode(MidiVisuEditor& e)
    : editor_(e) {
    for (auto& p : circlePos) p = {};
    for (auto& p : floatOffset) p = {};
    for (auto& p : floatVel) p = {};
}

float CirclesMode::minRadius() const {
    return static_cast<float>(editor_.ballSizeSlider.getMinValue());
}

float CirclesMode::maxRadius() const {
    return static_cast<float>(editor_.ballSizeSlider.getMaxValue());
}

void CirclesMode::initDefaultPositions(int width, int height) {
    const float w = (float)width;
    const float h = (float)height;
    const float slotW = w / 4.0f;
    const float rowH = h / 4.0f;

    for (int v = 0; v < 4; ++v)
        circlePos[v] = {slotW * 0.5f, rowH * (3 - v) + rowH * 0.5f};

    for (int i = 1; i <= 3; ++i)
        circlePos[3 + i] = {slotW * i + slotW * 0.5f, h * 0.5f};
}

void CirclesMode::onDrumHit(int voiceIndex) {
    if (voiceIndex < 0 || voiceIndex >= 4) return;
    drumSmoothedRadius[voiceIndex] = maxRadius();
    SvgWobbleLogic::triggerDrumHit(wobbleState_[voiceIndex]);
}

void CirclesMode::onNoteOn(int melodicIndex, int note) {
    if (melodicIndex < 1 || melodicIndex > 3) return;
    const float target = minRadius() + (note / 127.0f) * (maxRadius() - minRadius());
    smoothedRadius[melodicIndex] = target;
    SvgWobbleLogic::updateMelodicWobble(wobbleState_[3 + melodicIndex], note);
}

void CirclesMode::onNoteOff(int melodicIndex) {
    if (melodicIndex < 1 || melodicIndex > 3) return;
    SvgWobbleLogic::updateMelodicWobble(wobbleState_[3 + melodicIndex], -1);
}

void CirclesMode::update(float dt) {
    const float minR = minRadius();
    const float maxR = maxRadius();

    // Drum decay
    for (int v = 0; v < 4; ++v) {
        drumSmoothedRadius[v] += (minR - drumSmoothedRadius[v]) * kDrumSmoothing;
        SvgWobbleLogic::decayAmplitude(wobbleState_[v], 0.08f);
        SvgWobbleLogic::advanceState(wobbleState_[v], dt);
    }

    // Melodic smoothing
    for (int i = 1; i < 4; ++i) {
        const int note = editor_.audioProcessor.midiManager.channelHighestNote[i].load();
        const float target = (note >= 0)
                                 ? (minR + (note / 127.0f) * (maxR - minR))
                                 : minR;
        smoothedRadius[i] += (target - smoothedRadius[i]) * kSmoothing;
        SvgWobbleLogic::updateMelodicWobble(wobbleState_[3 + i], note);
        SvgWobbleLogic::decayAmplitude(wobbleState_[3 + i], 0.08f);
        SvgWobbleLogic::advanceState(wobbleState_[3 + i], dt);
    }

    // Floating / collisions
    static Random rng;
    const bool floatActive = editor_.floatToggle.getToggleState();
    const bool collisionActive = editor_.collisionToggle.getToggleState();
    const bool kickActive = editor_.clockKickToggle.getToggleState();

    if (floatActive || kickActive || collisionActive) {
        const float intensity = floatActive
                                    ? static_cast<float>(editor_.floatIntensitySlider.getValue())
                                    : 0.0f;
        const float kForce = 5.4f * intensity;
        const float kDamping = 0.99f - static_cast<float>(editor_.floatSpeedSlider.getValue()) * 0.16f;

        for (int i = 0; i < 7; ++i) {
            if (intensity > 0.0f) {
                floatVel[i].x += (rng.nextFloat() * 2.0f - 1.0f) * kForce;
                floatVel[i].y += (rng.nextFloat() * 2.0f - 1.0f) * kForce;
            }
            floatVel[i] *= kDamping;
            floatOffset[i] += floatVel[i];
        }

        if (collisionActive) {
            float cr[7];
            for (int v = 0; v < 4; ++v) cr[v] = jmax(minR, drumSmoothedRadius[v]);
            for (int i = 1; i < 4; ++i) cr[3 + i] = jmax(minR, smoothedRadius[i]);

            const float W = static_cast<float>(editor_.getWidth());
            const float H = static_cast<float>(editor_.getHeight());

            // Ball-wall
            for (int i = 0; i < 7; ++i) {
                const float cx = circlePos[i].x + floatOffset[i].x;
                const float cy = circlePos[i].y + floatOffset[i].y;
                if (cx - cr[i] < 0.f) {
                    floatOffset[i].x += cr[i] - cx;
                    floatVel[i].x = std::abs(floatVel[i].x);
                } else if (cx + cr[i] > W) {
                    floatOffset[i].x -= cx + cr[i] - W;
                    floatVel[i].x = -std::abs(floatVel[i].x);
                }
                if (cy - cr[i] < 0.f) {
                    floatOffset[i].y += cr[i] - cy;
                    floatVel[i].y = std::abs(floatVel[i].y);
                } else if (cy + cr[i] > H) {
                    floatOffset[i].y -= cy + cr[i] - H;
                    floatVel[i].y = -std::abs(floatVel[i].y);
                }
            }

            // Ball-ball
            for (int i = 0; i < 7; ++i)
                for (int j = i + 1; j < 7; ++j) {
                    const float dx = (circlePos[j].x + floatOffset[j].x)
                        - (circlePos[i].x + floatOffset[i].x);
                    const float dy = (circlePos[j].y + floatOffset[j].y)
                        - (circlePos[i].y + floatOffset[i].y);
                    const float distSq = dx * dx + dy * dy;
                    const float minDist = cr[i] + cr[j];
                    if (distSq >= minDist * minDist || distSq < 0.0001f) continue;

                    const float dist = std::sqrt(distSq);
                    const float nx = dx / dist, ny = dy / dist;

                    const float dvn = (floatVel[i].x - floatVel[j].x) * nx
                        + (floatVel[i].y - floatVel[j].y) * ny;
                    if (dvn > 0.f) {
                        const float mi = ballMass[i], mj = ballMass[j];
                        const float imp = 2.f * dvn / (mi + mj);
                        floatVel[i].x -= imp * mj * nx;
                        floatVel[i].y -= imp * mj * ny;
                        floatVel[j].x += imp * mi * nx;
                        floatVel[j].y += imp * mi * ny;
                    }

                    const float half = (minDist - dist) * 0.5f;
                    floatOffset[i].x -= nx * half;
                    floatOffset[i].y -= ny * half;
                    floatOffset[j].x += nx * half;
                    floatOffset[j].y += ny * half;
                }
        }
    } else {
        for (int i = 0; i < 7; ++i)
            floatVel[i] = floatOffset[i] = {};
    }
}

void CirclesMode::paint(Graphics& g, Rectangle<int> /*bounds*/) const {
    const float wobbleAmt = static_cast<float>(
        editor_.wobbleIntensitySlider.getValue());

    // Need mutable copies of wobbleState for drawing (drawShape takes non-const ref)
    SvgWobbleLogic::VoiceState ws[7];
    for (int i = 0; i < 7; ++i) ws[i] = wobbleState_[i];

    for (int v = 0; v < 4; ++v) {
        const float r = drumSmoothedRadius[v];
        const float cx = circlePos[v].x + floatOffset[v].x;
        const float cy = circlePos[v].y + floatOffset[v].y;
        const_cast<CirclesMode*>(this)->svgShapeManager_.drawShape(
            g, v, cx, cy, r, drumColours[v], ws[v], wobbleAmt);
    }

    for (int i = 1; i < 4; ++i) {
        const float r = smoothedRadius[i];
        const float cx = circlePos[3 + i].x + floatOffset[3 + i].x;
        const float cy = circlePos[3 + i].y + floatOffset[3 + i].y;
        const_cast<CirclesMode*>(this)->svgShapeManager_.drawShape(
            g, 3 + i, cx, cy, r, kChColours[i], ws[3 + i], wobbleAmt);
    }
}

void CirclesMode::mouseDown(const MouseEvent& e) {
    draggedCircle_ = -1;
    const float minR = minRadius();

    for (int i = 6; i >= 0; --i) {
        const float r = (i < 4)
                            ? jmax(minR, drumSmoothedRadius[i])
                            : jmax(minR, smoothedRadius[i - 3]);
        const float hitR = jmax(r, 30.0f);
        if (e.position.getDistanceFrom(circlePos[i] + floatOffset[i]) <= hitR) {
            draggedCircle_ = i;
            dragOffset_ = circlePos[i] - e.position;
            break;
        }
    }
}

void CirclesMode::mouseDrag(const MouseEvent& e) {
    if (draggedCircle_ < 0) return;
    circlePos[draggedCircle_] = e.position + dragOffset_;
    editor_.repaint();
}

void CirclesMode::mouseUp(const MouseEvent& /*e*/) {
    if (draggedCircle_ >= 0)
        editor_.writePositionsToFile(editor_.getAutoSaveFile());
    draggedCircle_ = -1;
}
