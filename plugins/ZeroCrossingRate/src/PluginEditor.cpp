#include "ZeroCrossingRate/PluginEditor.h"
#include "ZeroCrossingRate/PluginProcessor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor& p)
    : AudioProcessorEditor(&p)
    , processorRef(p)
{
    // Set up the threshold slider
    thresholdSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    thresholdSlider.setRange(0.0, 1.0, 0.01);
    thresholdSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    addAndMakeVisible(thresholdSlider);

    // Set up the threshold label
    thresholdLabel.setText("Threshold", juce::dontSendNotification);
    thresholdLabel.attachToComponent(&thresholdSlider, true);
    addAndMakeVisible(thresholdLabel);

    // Create the slider attachment to connect it to the processor's parameter
    thresholdAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(processorRef.getAPVTS(),
                                                                                       "zeroCrossingThreshold",
                                                                                       thresholdSlider));

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(500, 350);
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
    const float  zcValue    = processorRef.getZeroCrossingRateValue();
    const double eFreqValue = processorRef.getEstimatedFrequency();

    juce::String info;
    info << "ZCR: " << juce::String(zcValue, 3) << "\n"
         << "EFreq: " << juce::String(eFreqValue, 1) << " Hz";

    // Draw in the upper half of the window
    g.drawFittedText(info,
                     getLocalBounds().withTrimmedBottom(getHeight() / 2).reduced(10),
                     juce::Justification::centred,
                     2);

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
    // Layout for the slider - place it centered in the bottom half of the window
    const int sliderY      = getHeight() / 2 + 50; // Position it further below the middle
    const int sliderHeight = 30;
    const int sliderWidth  = static_cast<int>(getWidth() * 0.7f); // Use 70% of the window width for the slider

    // Center the slider horizontally with enough space for label on left
    const int labelWidth = 80;
    const int sliderX    = (getWidth() - sliderWidth - labelWidth) / 2 + labelWidth;

    // Position slider centered horizontally
    thresholdSlider.setBounds(sliderX, sliderY, sliderWidth, sliderHeight);
}