#pragma once
#include <string>
#include <vector>

// Pure C++ (no JUCE) — manages the video file list, selection, and play state.
// All methods are UI-thread-only; no synchronization needed.
class VideoListManager {
public:
    enum class PlayState { Stopped, Playing, Paused };

    struct FileEntry {
        std::string filename;
        double durationSec = 0.0;
    };

    // Replace the file list. Clears selectedIndex if it would be out of range.
    void setFiles(const std::vector<FileEntry>& entries);

    int fileCount() const;
    const std::string& filename(int i) const; // asserts valid index
    double duration(int i) const; // asserts valid index

    int selectedIndex() const;
    void setSelectedIndex(int idx); // negative → -1; ≥ count → clamped to last

    PlayState playState() const;
    void setPlayState(PlayState s);

    bool isPlaying() const; // true iff playState == Playing

private:
    std::vector<FileEntry> files_;
    int selectedIndex_ = -1;
    PlayState playState_ = PlayState::Stopped;
};