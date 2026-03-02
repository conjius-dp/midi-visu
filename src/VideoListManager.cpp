#include "VideoListManager.h"
#include <cassert>

void VideoListManager::setFiles(const std::vector<FileEntry>& entries) {
    files_ = entries;
    if (selectedIndex_ >= static_cast<int>(files_.size()))
        selectedIndex_ = -1;
}

int VideoListManager::fileCount() const {
    return static_cast<int>(files_.size());
}

const std::string& VideoListManager::filename(int i) const {
    assert(i >= 0 && i < static_cast<int>(files_.size()));
    return files_[static_cast<std::size_t>(i)].filename;
}

double VideoListManager::duration(int i) const {
    assert(i >= 0 && i < static_cast<int>(files_.size()));
    return files_[static_cast<std::size_t>(i)].durationSec;
}

int VideoListManager::selectedIndex() const { return selectedIndex_; }

void VideoListManager::setSelectedIndex(int idx) {
    if (idx < 0 || files_.empty())
        selectedIndex_ = -1;
    else if (idx >= static_cast<int>(files_.size()))
        selectedIndex_ = static_cast<int>(files_.size()) - 1;
    else
        selectedIndex_ = idx;
}

VideoListManager::PlayState VideoListManager::playState() const { return playState_; }

void VideoListManager::setPlayState(PlayState s) { playState_ = s; }

bool VideoListManager::isPlaying() const { return playState_ == PlayState::Playing; }