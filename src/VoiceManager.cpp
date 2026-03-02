#include "VoiceManager.h"

VoiceManager::VoiceManager() {
    for (int v = 0; v < 4; ++v)
        drumVoiceMidiChannel[v].store(10, std::memory_order_relaxed);
    melodicMidiChannel[0].store(2, std::memory_order_relaxed);
    melodicMidiChannel[1].store(3, std::memory_order_relaxed);
    melodicMidiChannel[2].store(4, std::memory_order_relaxed);
}

void VoiceManager::setDrumChannel(int voice, int ch) {
    drumVoiceMidiChannel[voice].store(ch, std::memory_order_relaxed);
}

int VoiceManager::getDrumChannel(int voice) const {
    return drumVoiceMidiChannel[voice].load(std::memory_order_relaxed);
}

void VoiceManager::setMelodicChannel(int track, int ch) {
    melodicMidiChannel[track].store(ch, std::memory_order_relaxed);
}

int VoiceManager::getMelodicChannel(int track) const {
    return melodicMidiChannel[track].load(std::memory_order_relaxed);
}

int VoiceManager::matchDrumVoice(int ch, int note, const int drumCh[4]) {
    for (int v = 0; v < 4; ++v)
        if (ch == drumCh[v] && note == kDrumNotes[v])
            return v;
    return -1;
}

int VoiceManager::matchMelodicVoice(int ch, const int melCh[3]) {
    for (int i = 0; i < 3; ++i)
        if (ch == melCh[i])
            return i;
    return -1;
}