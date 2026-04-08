#include "FluidSimRenderer.h"
#include "AppConstants.h"

FluidSimRenderer::FluidSimRenderer() {
    buildColorLUT();
}

void FluidSimRenderer::buildColorLUT() {
    colorLUT_[0] = 0xFF000000; // dead = black
    for (int i = 0; i < 7; ++i)
        colorLUT_[i + 1] = kVoiceColours[i].getARGB();
}

void FluidSimRenderer::render(const FluidSimLogic& sim, Image& target) const {
    const int size = FluidSimLogic::kGridSize;

    if (!target.isValid() || target.getWidth() != size || target.getHeight() != size)
        target = Image(Image::ARGB, size, size, false);

    Image::BitmapData bmp(target, Image::BitmapData::writeOnly);

    for (int y = 0; y < size; ++y) {
        auto* row = bmp.getLinePointer(y);
        for (int x = 0; x < size; ++x) {
            const auto& cell = sim.cellAt(x, y);
            if (cell.colorIndex == 0 || !sim.isInsideMask(x, y)) {
                // Transparent black
                row[x * 4 + 0] = 0;
                row[x * 4 + 1] = 0;
                row[x * 4 + 2] = 0;
                row[x * 4 + 3] = 0;
            } else {
                const uint32 argb = colorLUT_[cell.colorIndex];
                // JUCE Image::ARGB on LE macOS: B G R A byte order
                row[x * 4 + 0] = (uint8)(argb & 0xFF);         // B
                row[x * 4 + 1] = (uint8)((argb >> 8) & 0xFF);  // G
                row[x * 4 + 2] = (uint8)((argb >> 16) & 0xFF); // R
                row[x * 4 + 3] = 255;                           // A
            }
        }
    }
}

void FluidSimRenderer::drawSyringes(Graphics& g, const FluidSimLogic& sim,
                                     Rectangle<int> bounds) const {
    const float scaleX = (float)bounds.getWidth() / FluidSimLogic::kGridSize;
    const float scaleY = (float)bounds.getHeight() / FluidSimLogic::kGridSize;
    const float nozzleLen = 12.0f;
    const float nozzleW = 4.0f;

    for (int v = 0; v < FluidSimLogic::kNumVoices; ++v) {
        const auto& syr = sim.syringePosition(v);
        const float sx = bounds.getX() + syr.x * scaleX;
        const float sy = bounds.getY() + syr.y * scaleY;

        g.setColour(Colour(colorLUT_[v + 1]));

        // Draw a small arrow/nozzle shape pointing inward
        Path nozzle;
        // Base perpendicular to direction
        const float perpX = -syr.dirY;
        const float perpY = syr.dirX;

        // Tip of nozzle (toward center)
        const float tipX = sx + syr.dirX * nozzleLen;
        const float tipY = sy + syr.dirY * nozzleLen;

        // Base corners
        const float bx1 = sx + perpX * nozzleW;
        const float by1 = sy + perpY * nozzleW;
        const float bx2 = sx - perpX * nozzleW;
        const float by2 = sy - perpY * nozzleW;

        nozzle.startNewSubPath(tipX, tipY);
        nozzle.lineTo(bx1, by1);
        nozzle.lineTo(bx2, by2);
        nozzle.closeSubPath();

        g.fillPath(nozzle);
    }
}
