#include "MidiManager.h"

MidiManager::MidiManager(VoiceManager& vm)
    : voiceManager(vm) {
}

void MidiManager::processBlock(MidiBuffer& midiMessages) {
    int drumCh[4], melCh[3];
    for (int v = 0; v < 4; ++v) drumCh[v] = voiceManager.getDrumChannel(v);
    for (int i = 0; i < 3; ++i) melCh[i] = voiceManager.getMelodicChannel(i);

    for (const auto meta : midiMessages) {
        const auto msg = meta.getMessage();
        const int ch = msg.getChannel();
        const int note = msg.getNoteNumber();

        if (msg.isMidiClock()) {
            midiClockPulse.fetch_add(1, std::memory_order_relaxed);
            continue;
        }

        // Drum voices: each voice matches channel + fixed note.
        if (msg.isNoteOn()) {
            const int voice = VoiceManager::matchDrumVoice(ch, note, drumCh);
            if (voice >= 0) {
                ch10RawHitNote.store(note, std::memory_order_relaxed);
                ch10RawHitCount.fetch_add(1, std::memory_order_relaxed);
                drumVoiceHitCount[voice].fetch_add(1, std::memory_order_relaxed);
                continue;
            }
        }

        // Melodic tracks: index 0-2 → tracks 1-3.
        const int track = VoiceManager::matchMelodicVoice(ch, melCh);
        if (track < 0) continue;

        const int trackIdx = track + 1; // indices 1-3 used

        if (msg.isNoteOn()) {
            activeNotes[trackIdx].insert(note);
            channelNoteOnNote[trackIdx].store(note, std::memory_order_relaxed);
            channelNoteOnCount[trackIdx].fetch_add(1, std::memory_order_relaxed);
        }
        else if (msg.isNoteOff()) {
            activeNotes[trackIdx].erase(note);
        }

        channelHighestNote[trackIdx].store(
            activeNotes[trackIdx].empty() ? -1 : *activeNotes[trackIdx].rbegin(),
            std::memory_order_relaxed);
    }
}