#pragma once
#include <cstdint>
#include <memory>

// Decodes video frames via AVFoundation and exposes them as raw BGRA pixels so
// VideoBackground.mm never needs to include JuceHeader.h (which conflicts with
// AVFoundation headers due to 'Component' and OpenGL type clashes on macOS).
class VideoBackground
{
public:
    VideoBackground();
    ~VideoBackground();

    // Load and start looping the given file (pass absolute path as UTF-8 C string).
    // Immediately begins playback from t=20s.
    void loadFile (const char* absolutePath);

    // Playback control (no-op if no file is loaded).
    void play();
    void pause();
    void stop(); // pause + seek to t=0

    // Returns the current playback position in seconds (0.0 if no file loaded).
    double currentTime() const;

    // Returns total duration in seconds (0.0 if no file loaded).
    double duration() const;

    // Seek to the given time in seconds.
    void seek(double seconds);

    // Set loop region. When playback reaches endSec (or end of media),
    // seek back to startSec.
    void setLoopPoints(double startSec, double endSec);

    // Probe the duration of a video file without loading it for playback.
    // Returns 0.0 if the file cannot be read or has no valid duration.
    static double fileDuration(const char* absolutePath);

    // Returns true if a frame is available.
    // outPixels points into an internal buffer (BGRA, 4 bytes/pixel).
    // Valid only until the next call to getLatestFrame().
    bool getLatestFrame (int& outWidth, int& outHeight, int& outStride,
                         const uint8_t*& outPixels);

private:
    struct Pimpl;
    std::unique_ptr<Pimpl> pimpl;
};
