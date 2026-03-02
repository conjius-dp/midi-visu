#pragma once

#include <string>

class MultiHandleSliderLogic {
public:
    enum class HandleType { None, LoopStart, Playhead, LoopEnd, MiddleZone };

    explicit MultiHandleSliderLogic(double maxValue = 0.0);

    double loopStart() const { return loopStart_; }
    double playhead() const { return playhead_; }
    double loopEnd() const { return loopEnd_; }
    double maxValue() const { return maxValue_; }

    void setLoopStart(double v);
    void setPlayhead(double v);
    void setLoopEnd(double v);
    void setMaxValue(double v);

    HandleType hitTest(float mouseX,
                       float trackStart, float trackWidth,
                       float thumbRadius) const;

    void dragHandle(HandleType handle, float mouseX,
                    float trackStart, float trackWidth);

    void dragMiddleZone(double initialLoopStart, double initialLoopEnd,
                        float deltaPixels,
                        float trackStart, float trackWidth);

    static std::string formatTime(double seconds);

private:
    double loopStart_ = 0.0;
    double playhead_ = 0.0;
    double loopEnd_ = 0.0;
    double maxValue_ = 0.0;
};