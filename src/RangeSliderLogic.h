#pragma once

#include <utility>

class RangeSliderLogic {
public:
    // value → pixel X inside the slider component
    static float valueToPixel(double value, double rangeMin, double rangeMax,
                              float trackStart, float trackWidth);

    // pixel X → value (inverse of above)
    static double pixelToValue(float pixel, double rangeMin, double rangeMax,
                               float trackStart, float trackWidth);

    // true when mouseX is in the draggable middle zone:
    //   [minHandlePixel + thumbRadius , maxHandlePixel - thumbRadius]
    static bool isInMiddleZone(float mouseX,
                               double minVal, double maxVal,
                               double rangeMin, double rangeMax,
                               float trackStart, float trackWidth,
                               float thumbRadius);

    // Translate both handles by deltaPixels, clamping to [rangeMin, rangeMax]
    // while preserving the interval width.
    static std::pair<double, double> applyDrag(double initialMin, double initialMax,
                                               double rangeMin, double rangeMax,
                                               float deltaPixels,
                                               float trackStart, float trackWidth);
};