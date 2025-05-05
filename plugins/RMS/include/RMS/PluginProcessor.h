#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <vector>
#include <essentia/algorithmfactory.h>
#include <essentia/essentiamath.h>

//==============================================================================
class AudioPluginAudioProcessor final : public juce::AudioProcessor
{
public:
    //==============================================================================
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

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

    // Essentia custom functions
    void initializeEssentiaAlgorithms(int sampleRate, int frameSize);
    void connectBufferToAlgorithms();
    std::vector<float> applyZeroPadding(juce::AudioBuffer<float>& buffer, int maxSampleSize);
    
    void loadEssentiaBuffer(std::vector<float> buffer);
    void computeEssentiaAlgorithms();
    void cleanupEssentia();
    
    float getRMS() const { return (float)essentia::amp2db(rmsValue); }

private:
    essentia::standard::Algorithm* rms;
    essentia::Real rmsValue;
    std::vector<essentia::Real> essentiaBuffer;
    
    int maxSampleSize {1024};
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};
