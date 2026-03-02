#include "RangeSliderLogic.h"

float RangeSliderLogic::valueToPixel(double value, double rangeMin, double rangeMax,
                                     float trackStart, float trackWidth) {
    if (rangeMax <= rangeMin) return trackStart;
    return trackStart + static_cast<float>((value - rangeMin) / (rangeMax - rangeMin)) *
        trackWidth;
}

double RangeSliderLogic::pixelToValue(float pixel, double rangeMin, double rangeMax,
                                      float trackStart, float trackWidth) {
    if (trackWidth <= 0.0f) return rangeMin;
    return rangeMin + static_cast<double>((pixel - trackStart) / trackWidth) * (rangeMax -
        rangeMin);
}

bool RangeSliderLogic::isInMiddleZone(float mouseX,
                                      double minVal, double maxVal,
                                      double rangeMin, double rangeMax,
                                      float trackStart, float trackWidth,
                                      float thumbRadius) {
    const float minPx = valueToPixel(minVal, rangeMin, rangeMax, trackStart, trackWidth);
    const float maxPx = valueToPixel(maxVal, rangeMin, rangeMax, trackStart, trackWidth);
    const float zoneLeft = minPx + thumbRadius;
    const float zoneRight = maxPx - thumbRadius;
    return zoneLeft < zoneRight && mouseX >= zoneLeft && mouseX <= zoneRight;
}

std::pair<double, double> RangeSliderLogic::applyDrag(
    double initialMin, double initialMax,
    double rangeMin, double rangeMax,
    float deltaPixels,
    float trackStart, float trackWidth) {
    if (trackWidth <= 0.0f) return {initialMin, initialMax};

    const double interval = initialMax - initialMin;
    const double deltaValue = static_cast<double>(deltaPixels) / static_cast<double>(
        trackWidth) * (rangeMax - rangeMin);

    double newMin = initialMin + deltaValue;
    double newMax = initialMax + deltaValue;

    if (newMin < rangeMin) {
        newMin = rangeMin;
        newMax = rangeMin + interval;
    }
    if (newMax > rangeMax) {
        newMax = rangeMax;
        newMin = rangeMax - interval;
    }

    return {newMin, newMax};
}