#include "RMS/PluginEditor.h"

#include "RMS/PluginProcessor.h"

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

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

void AudioPluginAudioProcessorEditor::timerCallback()
{
    repaint();
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(25.0f);
    g.drawFittedText("Essentia:" + juce::String(essentia::version),
                     getLocalBounds(),
                     juce::Justification::centredTop,
                     1);

    g.setFont(35.0f);

    const float rmsLin = processorRef.getRMSLinear();
    const float rmsdB  = processorRef.getRMSdB();

    juce::String info;
    info << "RMS (lin): " << juce::String(rmsLin, 3) << "\n"
         << "RMS (dB): " << juce::String(rmsdB, 3) << " dB";

    g.drawFittedText(info, getLocalBounds().reduced(10), juce::Justification::centred, 2);

    // footer
    g.setFont(15.0f);
    g.drawFittedText("plugins/RMS", getLocalBounds(), juce::Justification::centredBottom, 1);
}

void AudioPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
