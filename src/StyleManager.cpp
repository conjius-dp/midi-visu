#include "StyleManager.h"

//==============================================================================
// Colour accessors

juce::Colour StyleManager::panelBackground() const {
    return juce::Colour(StyleTokens::kPanelBg);
}

juce::Colour StyleManager::logBackground() const {
    return juce::Colour(StyleTokens::kLogBg);
}

juce::Colour StyleManager::dialogBackground() const {
    return juce::Colour(StyleTokens::kDialogBg);
}

juce::Colour StyleManager::border() const { return juce::Colour(StyleTokens::kBorder); }
juce::Colour StyleManager::divider() const { return juce::Colour(StyleTokens::kDivider); }

juce::Colour StyleManager::panelTitle() const {
    return juce::Colour(StyleTokens::kPanelTitle);
}

juce::Colour StyleManager::sectionHeading() const {
    return juce::Colour(StyleTokens::kSectionHead);
}

juce::Colour StyleManager::voiceName() const {
    return juce::Colour(StyleTokens::kVoiceName);
}

juce::Colour StyleManager::value() const { return juce::Colour(StyleTokens::kValue); }
juce::Colour StyleManager::label() const { return juce::Colour(StyleTokens::kLabel); }

juce::Colour StyleManager::secondary() const {
    return juce::Colour(StyleTokens::kSecondary);
}

juce::Colour StyleManager::scrollTrack() const {
    return juce::Colour(StyleTokens::kScrollTrack);
}

juce::Colour StyleManager::logFallback() const {
    return juce::Colour(StyleTokens::kLogFallback);
}

juce::Colour StyleManager::logInfo() const { return juce::Colour(StyleTokens::kLogInfo); }

juce::Colour StyleManager::logClock() const {
    return juce::Colour(StyleTokens::kLogClock);
}

juce::Colour StyleManager::listRowSelected() const {
    return juce::Colour(StyleTokens::kListRowSelected);
}

//==============================================================================
// Font accessors

juce::Font StyleManager::headingFont() const {
    return juce::Font(juce::FontOptions().withHeight(StyleTokens::kHeadingSize));
}

juce::Font StyleManager::monoFont() const {
    return juce::Font(juce::FontOptions()
                      .withName(juce::Font::getDefaultMonospacedFontName())
                      .withHeight(StyleTokens::kLabelSize));
}

juce::Font StyleManager::logFont() const {
    return juce::Font(juce::FontOptions()
                      .withName(juce::Font::getDefaultMonospacedFontName())
                      .withHeight(StyleTokens::kLogSize));
}

//==============================================================================
// Apply methods

void StyleManager::applyToSlider(juce::Slider& s) {
    s.setColour(juce::Slider::backgroundColourId, juce::Colour(StyleTokens::kSliderBg));
    s.setColour(juce::Slider::trackColourId, juce::Colour(StyleTokens::kSliderTrack));
    s.setColour(juce::Slider::thumbColourId, juce::Colour(StyleTokens::kSliderThumb));
    s.setColour(juce::Slider::textBoxTextColourId,
                juce::Colour(StyleTokens::kSliderText));
    s.setColour(juce::Slider::textBoxBackgroundColourId,
                juce::Colour(StyleTokens::kSliderBg));
    s.setColour(juce::Slider::textBoxOutlineColourId,
                juce::Colour(StyleTokens::kSliderOutline));
}

void StyleManager::applyToToggleButton(juce::ToggleButton& b) {
    b.setColour(juce::ToggleButton::textColourId, juce::Colour(StyleTokens::kButtonText));
    b.setColour(juce::ToggleButton::tickColourId, juce::Colour(StyleTokens::kTickOn));
    b.setColour(juce::ToggleButton::tickDisabledColourId,
                juce::Colour(StyleTokens::kTickOff));
}

void StyleManager::applyToTextButton(juce::TextButton& b) {
    b.setColour(juce::TextButton::buttonColourId, juce::Colour(StyleTokens::kButtonBg));
    b.setColour(juce::TextButton::buttonOnColourId,
                juce::Colour(StyleTokens::kButtonBgOn));
    b.setColour(juce::TextButton::textColourOffId,
                juce::Colour(StyleTokens::kButtonText));
    b.setColour(juce::TextButton::textColourOnId, juce::Colour(StyleTokens::kButtonText));
}

void StyleManager::applyToComboBox(juce::ComboBox& cb) {
    cb.setColour(juce::ComboBox::backgroundColourId,
                 juce::Colour(StyleTokens::kComboBoxBg));
    cb.setColour(juce::ComboBox::textColourId, juce::Colour(StyleTokens::kComboBoxText));
    cb.setColour(juce::ComboBox::outlineColourId,
                 juce::Colour(StyleTokens::kComboBoxOutline));
    cb.setColour(juce::ComboBox::arrowColourId,
                 juce::Colour(StyleTokens::kComboBoxArrow));
    cb.setColour(juce::ComboBox::focusedOutlineColourId,
                 juce::Colour(StyleTokens::kComboBoxOutline));
}