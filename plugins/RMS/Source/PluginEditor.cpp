/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
RMSAudioProcessorEditor::RMSAudioProcessorEditor (RMSAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    startTimerHz(30);
}

RMSAudioProcessorEditor::~RMSAudioProcessorEditor()
{
}

void RMSAudioProcessorEditor::timerCallback()
{
    repaint();
}

//==============================================================================
void RMSAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Essentia version:" + juce::String(essentia::version), getLocalBounds(), juce::Justification::centredTop, 1);

    g.setFont (35.0f);

    float rms = audioProcessor.getRMS();

    juce::String info;
    info << "RMS: " << juce::String(rms, 3) << "\n";

    g.drawFittedText(info, getLocalBounds().reduced(10), juce::Justification::centred, 2);
}

void RMSAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
