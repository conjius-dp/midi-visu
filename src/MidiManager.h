/*
  ==============================================================================
    MidiManager — owns all MIDI atomic state and processBlock logic.
  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <atomic>
#include <set>
#include "VoiceManager.h"

class MidiManager {
public:
    explicit MidiManager(VoiceManager& voiceManager);

    // Call from AudioProcessor::processBlock — audio thread only.
    void processBlock(MidiBuffer& midiMessages);

    // ── Shared atomic state — written on audio thread, read on UI thread ──────
    std::atomic<int> drumVoiceHitCount[4]{0, 0, 0, 0};

    // Raw drum tracking: fires on ANY matched drum note-on, for logging.
    std::atomic<int> ch10RawHitCount{0};
    std::atomic<int> ch10RawHitNote{-1};

    // Note-on event log for melodic tracks (indices 1-3).
    std::atomic<int> channelNoteOnCount[4]{0, 0, 0, 0};
    std::atomic<int> channelNoteOnNote[4]{-1, -1, -1, -1};

    // Highest active MIDI note per melodic track (indices 1-3), -1 = none.
    std::atomic<int> channelHighestNote[4]{-1, -1, -1, -1};

    // MIDI clock: counts every incoming clock pulse (24 per quarter note).
    std::atomic<int> midiClockPulse{0};

private:
    VoiceManager& voiceManager;
    std::set<int> activeNotes[4]; // audio-thread only; indices 1–3 used
};