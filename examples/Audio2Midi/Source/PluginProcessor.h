/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <essentia.h>
#include <stdio.h>
#include <vector>
#include <algorithmfactory.h>
#include <essentiamath.h>
#include <pool.h>

using namespace essentia;
using namespace essentia::standard;
using namespace std;

//==============================================================================
/**
*/
class RTAudio2MidiAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    RTAudio2MidiAudioProcessor();
    ~RTAudio2MidiAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    // custom functions
    void initialize_essentia_algorithms(int sampleRate, int frameSize);
    void configure_essentia_algorithms(int sampleRate, int frameSize);
    void connectBufferToAlgorithms();
    void load_audio_buffer(juce::AudioBuffer<float> buffer);
    
    void compute_algorithms();
    vector<float> get_features();
    void cleanupEssentia();

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RTAudio2MidiAudioProcessor)
    
    Algorithm* audio2Midi {nullptr};
    //Algorithm* energy;
    
    // audio2midi inputs and outputs
    vector<Real> inputFrame;
    Real rmsValue, pitch, rms;
    vector<string> messageType;
    vector<Real> midiNoteNumber;
    vector<Real> timeCompensation;
    
    // timestamp
    juce::AudioPlayHead::CurrentPositionInfo lastPosInfo;
    double currentTime, msPerSample, sampleRate;
};
