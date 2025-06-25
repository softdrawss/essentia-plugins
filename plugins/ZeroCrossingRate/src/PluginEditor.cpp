#include "ZeroCrossingRate/PluginEditor.h"
#include "ZeroCrossingRate/PluginProcessor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor& p)
    : AudioProcessorEditor(&p)
    , processorRef(p)
{
    juce::ignoreUnused(processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(400, 300);
    startTimerHz(30);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor() {}

void AudioPluginAudioProcessorEditor::timerCallback()
{
    repaint();
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Fill background
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    // Set common text color
    g.setColour(juce::Colours::white);

    // Draw header
    g.setFont(25.0f);
    g.drawFittedText("Essentia: " + juce::String(essentia::version),
                     getLocalBounds(),
                     juce::Justification::centredTop,
                     1);

    // Draw ZCR value
    g.setFont(35.0f);
    const float  zcValue = processorRef.getZeroCrossingRateValue();
    juce::String info    = "ZCR: " + juce::String(zcValue, 3);
    g.drawFittedText(info, getLocalBounds().reduced(10), juce::Justification::centred, 2);

    // Draw footer with build information
    g.setFont(15.0f);
    const juce::String lastBuildDate = __DATE__ " " __TIME__;
    g.drawFittedText("plugins/ZeroCrossingRate\nBuild: " + lastBuildDate,
                     getLocalBounds(),
                     juce::Justification::centredBottom,
                     1);
}

void AudioPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
