#include "InteractionManager.h"
#include "PluginEditor.h"

#if JUCE_STANDALONE_APPLICATION
#include <juce_audio_plugin_client/Standalone/juce_StandaloneFilterWindow.h>
#endif

InteractionManager::InteractionManager(MidiVisuEditor& e)
    : editor(e) {
}

// ── Mouse ─────────────────────────────────────────────────────────────────────

void InteractionManager::mouseDown(const MouseEvent& e) {
    draggedCircle = -1;

    // Video list click — must be checked before the circle drag loop
    if (editor.optionsPanelOpen) {
        const int rowH_grid = 26, firstY = 88;
        const int videoY = firstY + 7 * rowH_grid + 20;
        const int ctrlY = videoY + 96;
        const int filesY = ctrlY + 210;
        const int listTop = filesY + 16;
        const int listH = 2 * 22;
        const int pxPanel = editor.getWidth() - editor.logPanelWidth;
        const int pad = 10;
        const int listL = pxPanel + pad;
        const int listW = editor.logPanelWidth - pad * 2;

        if (e.x >= listL && e.x < listL + listW
            && e.y >= listTop && e.y < listTop + listH) {
            const int row = (e.y - listTop) / 22;
            const int fileIdx = editor.videoListScrollOffset + row;
            if (fileIdx < editor.videoListManager.fileCount()) {
                editor.videoListManager.setSelectedIndex(fileIdx);
                const juce::File f = editor.assetsVideoDir.getChildFile(
                    juce::String(editor.videoListManager.filename(fileIdx)));
                if (f.existsAsFile()) {
                    editor.videoBackground.setLoopPoints(
                        editor.seekBar.getLoopStart(), editor.seekBar.getLoopEnd());
                    editor.videoBackground.loadFile(f.getFullPathName().toRawUTF8());
                    editor.lastVideoFile = f;
                    editor.videoListManager.setPlayState(
                        VideoListManager::PlayState::Playing);
                    editor.videoPlayPauseButton.setButtonText("Pause");
                    editor.writePositionsToFile(editor.getAutoSaveFile());
                }
                editor.repaint();
            }
            return;
        }
    }

    const float minR_ = static_cast<float>(editor.ballSizeSlider.getMinValue());

    for (int i = 6; i >= 0; --i) {
        const float r = (i < 4)
                            ? jmax(minR_, editor.drumSmoothedRadius[i])
                            : jmax(minR_, editor.smoothedRadius[i - 3]);
        const float hitR = jmax(r, 30.0f);
        if (e.position.getDistanceFrom(editor.circlePos[i] + editor.floatOffset[i]) <=
            hitR) {
            draggedCircle = i;
            dragOffset = editor.circlePos[i] - e.position;
            break;
        }
    }
}

void InteractionManager::mouseDrag(const MouseEvent& e) const {
    if (draggedCircle < 0) return;
    editor.circlePos[draggedCircle] = e.position + dragOffset;
    editor.repaint();
}

void InteractionManager::mouseUp(const MouseEvent& /*e*/) {
    if (draggedCircle >= 0)
        editor.writePositionsToFile(editor.getAutoSaveFile());
    draggedCircle = -1;
}

void InteractionManager::mouseWheelMove(const MouseEvent& e,
                                        const MouseWheelDetails& w) const {
    // Video list scroll
    if (editor.optionsPanelOpen) {
        const int rowH_grid = 26, firstY = 88;
        const int videoY = firstY + 7 * rowH_grid + 20;
        const int ctrlY = videoY + 96;
        const int filesY = ctrlY + 210;
        const int listTop = filesY + 16;
        const int listH = 2 * 22;
        const int pxPanel = editor.getWidth() - editor.logPanelWidth;
        const int pad = 10;
        const int listL = pxPanel + pad;
        const int listW = editor.logPanelWidth - pad * 2;

        if (e.x >= listL && e.x < listL + listW
            && e.y >= listTop && e.y < listTop + listH) {
            const int maxOff = jmax(0, editor.videoListManager.fileCount() - 2);
            editor.videoListScrollOffset -= roundToInt(w.deltaY * 3.0f);
            editor.videoListScrollOffset =
                jlimit(0, maxOff, editor.videoListScrollOffset);
            editor.repaint();
            return;
        }
    }

    if (!editor.logPanelOpen) return;
    if (e.x > editor.logPanelWidth) return;

    const int logAreaTop = 142;
    const int lineH = 18;
    const int visibleLines = jmax(1, (editor.getHeight() - logAreaTop) / lineH);
    const int maxOff = jmax(0, editor.logLines.size() - visibleLines);

    editor.logScrollOffset -= roundToInt(w.deltaY * 30.0f);
    editor.logScrollOffset = jlimit(0, maxOff, editor.logScrollOffset);
    editor.repaint();
}

// ── Keyboard ──────────────────────────────────────────────────────────────────

bool InteractionManager::keyPressed(const KeyPress& key) const {
    if (key == KeyPress::escapeKey) {
        // Exit fullscreen if active
        if (auto* peer = editor.getPeer())
            if (peer->isFullScreen()) {
                peer->setFullScreen(false);
                editor.setSize(1920, 1080);
                editor.repaint();
                return true;
            }

        const bool anyOpen = editor.optionsPanelOpen || editor.logPanelOpen;

        // Close all panels
        if (editor.optionsPanelOpen) {
            editor.optionsPanelOpen = false;
            for (auto& box : editor.voiceChannelBox) box.setVisible(false);
            for (auto& lbl : editor.voiceNameLabel) lbl.setVisible(false);
            editor.videoToggle.setVisible(false);
            editor.blurToggle.setVisible(false);
            editor.blurSlider.setVisible(false);
            editor.videoZoomSlider.setVisible(false);
            editor.videoOpacitySlider.setVisible(false);
            editor.saveDefaultButton.setVisible(false);
            editor.savePositionsButton.setVisible(false);
            editor.loadPositionsButton.setVisible(false);
            editor.videoPlayPauseButton.setVisible(false);
            editor.videoStopButton.setVisible(false);
            editor.midiSettingsButton.setVisible(false);
            editor.ballSizeSlider.setVisible(false);
            editor.floatToggle.setVisible(false);
            editor.collisionToggle.setVisible(false);
            editor.clockKickToggle.setVisible(false);
            editor.clockDivisionBox.setVisible(false);
            editor.clockKickIntensitySlider.setVisible(false);
            editor.floatIntensitySlider.setVisible(false);
            editor.floatSpeedSlider.setVisible(false);
            editor.seekBar.setVisible(false);
        }
        if (editor.logPanelOpen) {
            editor.logPanelOpen = false;
            editor.logMidiNotesToggle.setVisible(false);
            editor.logMidiClockToggle.setVisible(false);
            editor.clearLogButton.setVisible(false);
        }

        // If nothing was open, open both panels
        if (!anyOpen) {
            editor.optionsPanelOpen = true;
            for (auto& box : editor.voiceChannelBox) box.setVisible(true);
            for (auto& lbl : editor.voiceNameLabel) lbl.setVisible(true);
            editor.videoToggle.setVisible(true);
            editor.blurToggle.setVisible(true);
            editor.blurSlider.setVisible(true);
            editor.videoZoomSlider.setVisible(true);
            editor.videoOpacitySlider.setVisible(true);
            editor.saveDefaultButton.setVisible(true);
            editor.savePositionsButton.setVisible(true);
            editor.loadPositionsButton.setVisible(true);
            editor.videoPlayPauseButton.setVisible(true);
            editor.videoStopButton.setVisible(true);
            editor.midiSettingsButton.setVisible(true);
            editor.ballSizeSlider.setVisible(true);
            editor.floatToggle.setVisible(true);
            editor.collisionToggle.setVisible(true);
            editor.clockKickToggle.setVisible(true);
            editor.clockDivisionBox.setVisible(true);
            editor.clockKickIntensitySlider.setVisible(true);
            editor.floatIntensitySlider.setVisible(true);
            editor.floatSpeedSlider.setVisible(true);
            editor.seekBar.setVisible(true);

            editor.logPanelOpen = true;
            editor.logScrollOffset = 0;
            editor.logMidiNotesToggle.setVisible(true);
            editor.logMidiClockToggle.setVisible(true);
            editor.clearLogButton.setVisible(true);

            editor.resized();
        }

        editor.repaint();
        return true;
    }

    if (key == KeyPress('f', ModifierKeys::noModifiers, 0)) {
        if (auto* peer = editor.getPeer())
            peer->setFullScreen(!peer->isFullScreen());
        return true;
    }

    if (key == KeyPress('b', ModifierKeys::noModifiers, 0)) {
        editor.blurToggle.setToggleState(!editor.blurToggle.getToggleState(),
                                         dontSendNotification);
        editor.repaint();
        return true;
    }

    if (key == KeyPress('o', ModifierKeys::noModifiers, 0)) {
        editor.optionsPanelOpen = !editor.optionsPanelOpen;
        for (auto& box : editor.voiceChannelBox) box.setVisible(editor.optionsPanelOpen);
        editor.videoToggle.setVisible(editor.optionsPanelOpen);
        editor.blurToggle.setVisible(editor.optionsPanelOpen);
        editor.blurSlider.setVisible(editor.optionsPanelOpen);
        editor.videoZoomSlider.setVisible(editor.optionsPanelOpen);
        editor.videoOpacitySlider.setVisible(editor.optionsPanelOpen);
        editor.saveDefaultButton.setVisible(editor.optionsPanelOpen);
        editor.savePositionsButton.setVisible(editor.optionsPanelOpen);
        editor.loadPositionsButton.setVisible(editor.optionsPanelOpen);
        editor.videoPlayPauseButton.setVisible(editor.optionsPanelOpen);
        editor.videoStopButton.setVisible(editor.optionsPanelOpen);
        editor.midiSettingsButton.setVisible(editor.optionsPanelOpen);
        editor.ballSizeSlider.setVisible(editor.optionsPanelOpen);
        editor.floatToggle.setVisible(editor.optionsPanelOpen);
        editor.collisionToggle.setVisible(editor.optionsPanelOpen);
        editor.clockKickToggle.setVisible(editor.optionsPanelOpen);
        editor.clockDivisionBox.setVisible(editor.optionsPanelOpen);
        editor.clockKickIntensitySlider.setVisible(editor.optionsPanelOpen);
        editor.floatIntensitySlider.setVisible(editor.optionsPanelOpen);
        editor.floatSpeedSlider.setVisible(editor.optionsPanelOpen);
        editor.seekBar.setVisible(editor.optionsPanelOpen);
        for (int i = 0; i < 7; ++i)
            editor.voiceNameLabel[i].setVisible(editor.optionsPanelOpen);
        if (editor.optionsPanelOpen)
            editor.resized();
        editor.repaint();
        return true;
    }

    if (key == KeyPress('l', ModifierKeys::noModifiers, 0)) {
        editor.logPanelOpen = !editor.logPanelOpen;
        editor.logScrollOffset = 0;
        editor.logMidiNotesToggle.setVisible(editor.logPanelOpen);
        editor.logMidiClockToggle.setVisible(editor.logPanelOpen);
        editor.clearLogButton.setVisible(editor.logPanelOpen);
        if (editor.logPanelOpen) editor.resized();
        editor.repaint();
        return true;
    }

    return false;
}