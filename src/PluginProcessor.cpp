/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
MidivisuAudioProcessor::MidivisuAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
          .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
          .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
      ),
      midiManager(voiceManager)
#else
     : midiManager(voiceManager)
#endif
{
}

MidivisuAudioProcessor::~MidivisuAudioProcessor() {
}

//==============================================================================
const String MidivisuAudioProcessor::getName() const {
    return JucePlugin_Name;
}

bool MidivisuAudioProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool MidivisuAudioProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool MidivisuAudioProcessor::isMidiEffect() const {
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double MidivisuAudioProcessor::getTailLengthSeconds() const {
    return 0.0;
}

int MidivisuAudioProcessor::getNumPrograms() {
    return 1;
}

int MidivisuAudioProcessor::getCurrentProgram() {
    return 0;
}

void MidivisuAudioProcessor::setCurrentProgram(int index) {
}

const String MidivisuAudioProcessor::getProgramName(int index) {
    return {};
}

void MidivisuAudioProcessor::changeProgramName(int index, const String& newName) {
}

//==============================================================================
void MidivisuAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
}

void MidivisuAudioProcessor::releaseResources() {
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MidivisuAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
#if JucePlugin_IsMidiEffect
    ignoreUnused(layouts);
    return true;
#else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void MidivisuAudioProcessor::processBlock(AudioBuffer<float>& buffer,
                                          MidiBuffer& midiMessages) {
    midiManager.processBlock(midiMessages);
}

//==============================================================================
bool MidivisuAudioProcessor::hasEditor() const {
    return true;
}

AudioProcessorEditor* MidivisuAudioProcessor::createEditor() {
    return new MidiVisuEditor(*this);
}

//==============================================================================
void MidivisuAudioProcessor::getStateInformation(MemoryBlock& destData) {
}

void MidivisuAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
}

//==============================================================================
AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new MidivisuAudioProcessor();
}