#pragma once

#include "ZeroCrossingRate/PluginProcessor.h"

//==============================================================================
// Custom waveform component for visual feedback
class WaveformComponent : public juce::Component, private juce::Timer
{
public:
    WaveformComponent();
    ~WaveformComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void setThreshold(float threshold);

private:
    void timerCallback() override;
    void generateBars();

    std::vector<float>   barHeights;
    float                currentThreshold = 0.35f;
    static constexpr int numBars          = 60;
};

//==============================================================================
// Custom Look and Feel for modern styling
class ModernLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ModernLookAndFeel();

    void drawLinearSlider(juce::Graphics&                 g,
                          int                             x,
                          int                             y,
                          int                             width,
                          int                             height,
                          float                           sliderPos,
                          float                           minSliderPos,
                          float                           maxSliderPos,
                          const juce::Slider::SliderStyle style,
                          juce::Slider&                   slider) override;

    void drawLinearSliderThumb(juce::Graphics&                 g,
                               int                             x,
                               int                             y,
                               int                             width,
                               int                             height,
                               float                           sliderPos,
                               float                           minSliderPos,
                               float                           maxSliderPos,
                               const juce::Slider::SliderStyle style,
                               juce::Slider&                   slider) override;
};

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;
    void                       timerCallback() override;

    // UI Components
    juce::Slider thresholdSlider;
    juce::Label  thresholdLabel;
    juce::Label  thresholdValueLabel;
    juce::Label  titleLabel;
    juce::Label  subtitleLabel;
    juce::Label  zcrValueLabel;
    juce::Label  zcrUnitLabel;
    juce::Label  footerLabel;

    WaveformComponent waveformComponent;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> thresholdAttachment;

    // Custom styling
    ModernLookAndFeel modernLF;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessorEditor)
};
