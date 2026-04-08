/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "VideoBackground.h"
#include "StyleManager.h"
#include "InteractionManager.h"
#include "UiManager.h"
#include "RangeSlider.h"
#include "VideoListManager.h"
#include "SeekBar.h"
#include "OptionsPanelLayout.h"
#include "SvgShapeManager.h"
#include "SvgWobbleLogic.h"
#include "VisuMode.h"
#include "CirclesMode.h"
#include "FluidSimMode.h"
#include <memory>

class PassthroughComboBox : public ComboBox {
public:
    using ComboBox::ComboBox;
    void mouseWheelMove(const MouseEvent& e,
                        const MouseWheelDetails& w) override {
        Component::mouseWheelMove(e, w);
    }
};

//==============================================================================
class MidiVisuEditor : public AudioProcessorEditor,
                       private Timer,
                       public SeekBar::Listener {
public:
    explicit MidiVisuEditor(MidivisuAudioProcessor&);
    ~MidiVisuEditor() override;

    //==============================================================================
    void paint(Graphics&) override;
    void paintOverChildren(Graphics&) override;
    void resized() override;
    bool keyPressed(const KeyPress& key) override;
    void mouseDown(const MouseEvent&) override;
    void mouseDrag(const MouseEvent&) override;
    void mouseUp(const MouseEvent&) override;
    void mouseWheelMove(const MouseEvent&, const MouseWheelDetails&) override;

private:
    void timerCallback() override;

    MidivisuAudioProcessor& audioProcessor;
    VideoBackground videoBackground;
    Image videoFrame;

    // Visualization mode
    std::unique_ptr<VisuMode> activeMode;
    int modeIndex = 0; // 0 = circles, 1 = fluid-sim
    void switchMode(int newMode);

    // MIDI event detection (shared across modes)
    int lastDrumHitCount[4]{0, 0, 0, 0};
    int melodicInjectTimer_[4]{0, 0, 0, 0};

    // Clock sync
    int lastClockPulse = 0;

    // Log panel
    bool logPanelOpen = false;
    StringArray logLines;
    Array<Colour> logColors;
    int logScrollOffset = 0;
    int lastCh10RawHitCount = 0;
    int lastChannelNoteOnCount[4] = {0, 0, 0, 0};
    static constexpr int maxLogLines = 500;
    static constexpr int logPanelWidth = 300;

    // Fluid-sim sliders (visible only in FluidSim mode)
    Slider simSpeedSlider;
    Slider burstSizeSlider;
    Slider particleLifetimeSlider;
    void writePositionsToFile(const File&) const;
    void readPositionsFromFile(const File&);
    void savePositions(); // opens "Save as…" dialog
    void loadPositions(); // opens "Load" dialog
    File getAutoSaveFile() const;
    File lastPositionsFile;
    std::unique_ptr<FileChooser> fileChooser;

    // Video file selection
    File lastVideoFile;
    File assetsVideoDir;
    VideoListManager videoListManager;
    int videoListScrollOffset = 0;

    void appendLog(const String& text, Colour color);

    // Options panel (O key)
    bool optionsPanelOpen = false;
    String voiceNames[7];
    Label voiceNameLabel[7];
    PassthroughComboBox voiceChannelBox[7];
    PassthroughComboBox clockDivisionBox;
    TextButton videoToggleButton{CharPointer_UTF8("\xf0\x9f\x91\x81")};   // 👁
    TextButton blurToggleButton{CharPointer_UTF8("\xf0\x9f\x92\xa7")};   // 💧
    ToggleButton floatToggle{"Floating"};
    ToggleButton collisionToggle{" Collisions"};
    ToggleButton clockKickToggle{"Clock Sync"};
    ToggleButton logMidiNotesToggle{"MIDI notes"};
    ToggleButton logMidiClockToggle{"MIDI clock"};
    TextButton clearLogButton{"Clear"};

    Slider blurSlider;
    Slider videoZoomSlider;
    Slider videoOpacitySlider;
    RangeSlider ballSizeSlider;
    Slider floatIntensitySlider;
    Slider floatSpeedSlider;
    Slider clockKickIntensitySlider;
    TextButton saveDefaultButton{"Save Settings"};
    TextButton savePositionsButton{"Save Settings to..."};
    TextButton loadPositionsButton{"Load"};
    TextButton videoPlayPauseButton{CharPointer_UTF8("\xe2\x96\xb6")};       // ▶
    TextButton videoStopButton{CharPointer_UTF8("\xe2\x96\xa0")};           // ■
    TextButton loopButton{CharPointer_UTF8("\xe2\x86\xbb")};               // ↻
    TextButton midiSettingsButton{"Audio / MIDI Settings"};
    Slider wobbleIntensitySlider;
    SeekBar seekBar;

    void seekBarLoopChanged(SeekBar* bar) override;
    void seekBarPlayheadDragged(SeekBar* bar) override;

    void showJuceAudioSettings();

    // Options panel layout — fold state and scroll offset.
    OptionsPanelLayout optionsLayout;

    // Style — declared before interaction/draw managers so they can access it.
    StyleManager styleManager;

    // Interaction and drawing managers — declared after all state they reference.
    InteractionManager interactionManager;
    UiManager uiManager;

    // (SVG shape rendering + wobble state moved to CirclesMode)

    // Pending loop values from settings, applied after maxValue becomes positive.
    double pendingLoopStart_ = -1.0;
    double pendingLoopEnd_ = -1.0;

    friend class InteractionManager;
    friend class UiManager;
    friend class CirclesMode;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiVisuEditor)
};