/*
  ==============================================================================
    InteractionManager — handles all mouse and keyboard input for the editor.
    Friend of MidiVisuEditor so it can access private state directly.
  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class MidiVisuEditor; // forward declaration — full type in InteractionManager.cpp

class InteractionManager {
public:
    explicit InteractionManager(MidiVisuEditor& editor);

    bool keyPressed(const KeyPress& key) const;
    void mouseDown(const MouseEvent& e);
    void mouseDrag(const MouseEvent& e) const;
    void mouseUp(const MouseEvent& e);
    void mouseWheelMove(const MouseEvent& e, const MouseWheelDetails& w) const;

private:
    MidiVisuEditor& editor;
    int draggedCircle = -1;
    Point<float> dragOffset;
};