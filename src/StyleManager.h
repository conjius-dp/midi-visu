/*
  ==============================================================================
    StyleManager — JUCE-aware wrapper around StyleTokens.
    Returns Colour/Font objects and applies dark styling to JUCE controls.
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "StyleTokens.h"

class StyleManager {
public:
    // Colour accessors
    juce::Colour panelBackground() const;
    juce::Colour logBackground() const;
    juce::Colour dialogBackground() const;
    juce::Colour border() const;
    juce::Colour divider() const;
    juce::Colour panelTitle() const;
    juce::Colour sectionHeading() const;
    juce::Colour voiceName() const;
    juce::Colour value() const;
    juce::Colour label() const;
    juce::Colour secondary() const;
    juce::Colour scrollTrack() const;
    juce::Colour logFallback() const;
    juce::Colour logInfo() const;
    juce::Colour logClock() const;
    juce::Colour listRowSelected() const;

    // Font accessors
    juce::Font headingFont() const; // non-monospace, kHeadingSize
    juce::Font monoFont() const; // monospace, kLabelSize
    juce::Font logFont() const; // monospace, kLogSize

    // Apply dark styling to JUCE controls
    void applyToSlider(juce::Slider&);
    void applyToToggleButton(juce::ToggleButton&);
    void applyToTextButton(juce::TextButton&);
    void applyToComboBox(juce::ComboBox&);
};