/*
  ==============================================================================
    VoiceManager — channel assignment and MIDI note matching.
    Pure C++, no JUCE dependency, so it can be unit-tested independently.
  ==============================================================================
*/

#pragma once
#include <atomic>

class VoiceManager {
public:
    static constexpr int kNumDrumVoices = 4;
    static constexpr int kNumMelodicTracks = 3;

    // MIDI note numbers for each drum voice (C3, D3, E3, F#3 in JUCE octave numbering).
    static constexpr int kDrumNotes[4] = {48, 50, 52, 54};

    VoiceManager();

    void setDrumChannel(int voice, int ch);
    int getDrumChannel(int voice) const;
    void setMelodicChannel(int track, int ch);
    int getMelodicChannel(int track) const;

    // Returns voice index [0..3] if ch/note match a drum voice, or -1.
    static int matchDrumVoice(int ch, int note, const int drumCh[4]);
    // Returns melodic track index [0..2] if ch matches, or -1.
    static int matchMelodicVoice(int ch, const int melCh[3]);

    // Shared atomics — written from UI thread, read on audio thread.
    std::atomic<int> drumVoiceMidiChannel[4];
    std::atomic<int> melodicMidiChannel[3];
};