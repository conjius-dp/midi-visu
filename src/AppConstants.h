/*
  ==============================================================================
    AppConstants — shared display constants (colours, voice names).
    Included by PluginEditor.cpp, DrawManager.cpp, InteractionManager.cpp.
  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

// Drum voice colours: progressively darker lime green.
static const Colour drumColours[4] =
{
    Colour(0xff32cd32), // Voice 0 Kick  - limegreen
    Colour(0xff27a427), // Voice 1 Clap  - ~80%
    Colour(0xff1d7b1d), // Voice 2 Snare - ~60%
    Colour(0xff125212), // Voice 3 HH    - ~40%
};

// Per-voice colours for UI dots and log entries.
static const Colour kVoiceColours[7] =
{
    Colour(0xff32cd32), Colour(0xff27a427),
    Colour(0xff1d7b1d), Colour(0xff125212),
    Colours::orange, Colours::yellow, Colour(0xffb03060)
};

// Melodic channel colours (index 0 unused, 1-3 → ch2-4).
static const Colour kChColours[4] =
{
    Colours::limegreen, // index 0 (ch10, fallback)
    Colours::orange, // index 1 (ch2)
    Colours::yellow, // index 2 (ch3)
    Colour(0xffb03060), // index 3 (ch4)
};

// Display names for each voice.
static const char* kVoiceNames[7] = {
    "KICK", "NOISE", "SNARE", "HH", "K-2", "EREBUS", "HYDRA"
};

// Default MIDI channel assignments.
static constexpr int kDefaultDrumCh[4] = {10, 10, 10, 10};
static constexpr int kDefaultMelodicCh[3] = {2, 3, 4};