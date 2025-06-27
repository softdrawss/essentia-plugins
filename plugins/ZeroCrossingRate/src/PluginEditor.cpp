#include "ZeroCrossingRate/PluginEditor.h"
#include "ZeroCrossingRate/PluginProcessor.h"

//==============================================================================
// WaveformComponent Implementation
//==============================================================================
WaveformComponent::WaveformComponent()
{
    generateBars();
    startTimerHz(5); // Update animation 5 times per second
}

WaveformComponent::~WaveformComponent()
{
    stopTimer();
}

void WaveformComponent::generateBars()
{
    barHeights.clear();
    barHeights.reserve(numBars);

    for (int i = 0; i < numBars; ++i)
    {
        barHeights.push_back(juce::Random::getSystemRandom().nextFloat() * 0.8f + 0.2f);
    }
}

void WaveformComponent::paint(juce::Graphics& g)
{
    // Dark background with subtle border
    g.setColour(juce::Colour(0xff1a202c));
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 8.0f);

    g.setColour(juce::Colour(0xff2d3748));
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(1), 8.0f, 1.0f);

    // Draw animated wave bars with solid color
    const float barWidth     = 3.0f;
    const float gap          = 2.0f;
    const float totalBarArea = (barWidth + gap) * numBars - gap;
    const float startX       = (getWidth() - totalBarArea) * 0.5f;
    const float maxBarHeight = getHeight() * 0.6f;
    const float centerY      = getHeight() * 0.5f;

    // Use solid blue color for bars
    g.setColour(juce::Colour(0xff4299e1));

    for (int i = 0; i < numBars; ++i)
    {
        const float x         = startX + i * (barWidth + gap);
        const float barHeight = maxBarHeight * barHeights[static_cast<size_t>(i)];
        const float y         = centerY - barHeight * 0.5f;

        juce::Rectangle<float> bar(x, y, barWidth, barHeight);
        g.fillRoundedRectangle(bar, 1.0f);
    }

    // Draw threshold line with solid color
    const float thresholdY = getHeight() * 0.2f + (1.0f - currentThreshold) * getHeight() * 0.6f;
    const float margin     = 15.0f;

    g.setColour(juce::Colour(0xfff56565));
    g.fillRect(margin, thresholdY - 1.0f, getWidth() - 2 * margin, 2.0f);
}

void WaveformComponent::resized()
{
    // Component automatically handles resizing
}

void WaveformComponent::setThreshold(float threshold)
{
    currentThreshold = threshold;
    repaint();
}

void WaveformComponent::timerCallback()
{
    // Animate the bars
    for (auto& height : barHeights)
    {
        height += (juce::Random::getSystemRandom().nextFloat() - 0.5f) * 0.1f;
        height = juce::jlimit(0.1f, 1.0f, height);
    }
    repaint();
}

//==============================================================================
// ModernLookAndFeel Implementation
//==============================================================================
ModernLookAndFeel::ModernLookAndFeel()
{
    // Set custom colors
    setColour(juce::Slider::trackColourId, juce::Colour(0xff2d3748));
    setColour(juce::Slider::thumbColourId, juce::Colour(0xff4299e1));
    setColour(juce::Label::textColourId, juce::Colours::white);
}

void ModernLookAndFeel::drawLinearSlider(juce::Graphics&                 g,
                                         int                             x,
                                         int                             y,
                                         int                             width,
                                         int                             height,
                                         float                           sliderPos,
                                         float                           minSliderPos,
                                         float                           maxSliderPos,
                                         const juce::Slider::SliderStyle style,
                                         juce::Slider&                   slider)
{
    juce::ignoreUnused(minSliderPos, maxSliderPos, style, slider);

    const float trackY      = y + height * 0.5f - 3.0f;
    const float trackHeight = 6.0f;

    // Draw track background with solid color
    g.setColour(juce::Colour(0xff2d3748));
    g.fillRoundedRectangle(x, trackY, width, trackHeight, 3.0f);

    // Draw filled portion with solid color
    const float filledWidth = sliderPos - x;
    if (filledWidth > 0)
    {
        g.setColour(juce::Colour(0xff4299e1));
        g.fillRoundedRectangle(x, trackY, filledWidth, trackHeight, 3.0f);
    }
}

void ModernLookAndFeel::drawLinearSliderThumb(juce::Graphics&                 g,
                                              int                             x,
                                              int                             y,
                                              int                             width,
                                              int                             height,
                                              float                           sliderPos,
                                              float                           minSliderPos,
                                              float                           maxSliderPos,
                                              const juce::Slider::SliderStyle style,
                                              juce::Slider&                   slider)
{
    juce::ignoreUnused(x, y, width, height, minSliderPos, maxSliderPos, style, slider);

    const float thumbSize = 20.0f;
    const float thumbX    = sliderPos - thumbSize * 0.5f;
    const float thumbY    = y + height * 0.5f - thumbSize * 0.5f;

    // Draw thumb with solid color
    g.setColour(juce::Colour(0xff4299e1));
    g.fillEllipse(thumbX, thumbY, thumbSize, thumbSize);

    // Add simple border
    g.setColour(juce::Colour(0xff3182ce));
    g.drawEllipse(thumbX, thumbY, thumbSize, thumbSize, 2.0f);
}

//==============================================================================
// AudioPluginAudioProcessorEditor Implementation
//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor& p)
    : AudioProcessorEditor(&p)
    , processorRef(p)
{
    // Set custom look and feel
    setLookAndFeel(&modernLF);

    // Setup title label
    titleLabel.setText("Zero Crossing Rate Analyzer", juce::dontSendNotification);
    titleLabel.setFont(juce::FontOptions(24.0f));
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    titleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(titleLabel);

    // Setup subtitle label
    subtitleLabel.setText("Real-time audio analysis tool", juce::dontSendNotification);
    subtitleLabel.setFont(juce::FontOptions(14.0f));
    subtitleLabel.setColour(juce::Label::textColourId, juce::Colour(0xffa0aec0));
    subtitleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(subtitleLabel);

    // Setup waveform component
    addAndMakeVisible(waveformComponent);

    // Setup ZCR value label
    zcrValueLabel.setText("847", juce::dontSendNotification);
    zcrValueLabel.setFont(juce::FontOptions(48.0f));
    zcrValueLabel.setColour(juce::Label::textColourId, juce::Colour(0xff4299e1));
    zcrValueLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(zcrValueLabel);

    // Setup ZCR unit label
    zcrUnitLabel.setText("ZERO CROSSINGS PER SECOND", juce::dontSendNotification);
    zcrUnitLabel.setFont(juce::FontOptions(14.0f));
    zcrUnitLabel.setColour(juce::Label::textColourId, juce::Colour(0xffa0aec0));
    zcrUnitLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(zcrUnitLabel);

    // Setup threshold label
    thresholdLabel.setText("Threshold", juce::dontSendNotification);
    thresholdLabel.setFont(juce::FontOptions(16.0f));
    thresholdLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    thresholdLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(thresholdLabel);

    // Setup threshold value label
    thresholdValueLabel.setText("0.35", juce::dontSendNotification);
    thresholdValueLabel.setFont(juce::FontOptions(14.0f));
    thresholdValueLabel.setColour(juce::Label::textColourId, juce::Colour(0xff4299e1));
    thresholdValueLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(thresholdValueLabel);

    // Setup threshold slider
    thresholdSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    thresholdSlider.setRange(0.0, 1.0, 0.01);
    thresholdSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    thresholdSlider.onValueChange = [this]() {
        const float value = static_cast<float>(thresholdSlider.getValue());
        waveformComponent.setThreshold(value);
        thresholdValueLabel.setText(juce::String(value, 2), juce::dontSendNotification);
    };
    addAndMakeVisible(thresholdSlider);

    // Setup footer label
    footerLabel.setText("powered by Essentia", juce::dontSendNotification);
    footerLabel.setFont(juce::FontOptions(12.0f));
    footerLabel.setColour(juce::Label::textColourId, juce::Colour(0xff718096));
    footerLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(footerLabel);

    // Create the slider attachment
    thresholdAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(processorRef.getAPVTS(),
                                                                                       "zeroCrossingThreshold",
                                                                                       thresholdSlider));

    // Set plugin size
    setSize(400, 500);
    setResizable(true, true);
    setResizeLimits(300, 400, 600, 750);

    startTimerHz(30);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void AudioPluginAudioProcessorEditor::timerCallback()
{
    // Update ZCR value display
    const float zcValue      = processorRef.getZeroCrossingRateValue();
    const float displayValue = static_cast<float>(zcValue); // Scale for display
    zcrValueLabel.setText(juce::String(displayValue), juce::dontSendNotification);

    repaint();
}

void AudioPluginAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Solid background
    g.setColour(juce::Colour(0xff1a1f2e));
    g.fillAll();

    // Draw main container with solid color and border
    const auto containerBounds = getLocalBounds().reduced(20).toFloat();
    g.setColour(juce::Colour(0xff2a3441));
    g.fillRoundedRectangle(containerBounds, 16.0f);

    g.setColour(juce::Colour(0xff3a4553));
    g.drawRoundedRectangle(containerBounds, 16.0f, 1.0f);

    // Simple accent line at top
    g.setColour(juce::Colour(0xff4299e1));
    g.fillRoundedRectangle(containerBounds.getX(), containerBounds.getY(), containerBounds.getWidth(), 2.0f, 1.0f);

    // Draw result display background with solid color
    const auto resultBounds = getLocalBounds()
                                  .reduced(50, 0)
                                  .withY(static_cast<int>(getHeight() * 0.4f))
                                  .withHeight(static_cast<int>(getHeight() * 0.15f))
                                  .toFloat();

    g.setColour(juce::Colour(0xff1a202c));
    g.fillRoundedRectangle(resultBounds, 12.0f);

    g.setColour(juce::Colour(0xff3a4553));
    g.drawRoundedRectangle(resultBounds, 12.0f, 1.0f);
}

void AudioPluginAudioProcessorEditor::resized()
{
    // Use setBoundsRelative for scalable layout

    // Header section (15% of height)
    titleLabel.setBoundsRelative(0.0f, 0.02f, 1.0f, 0.08f);
    subtitleLabel.setBoundsRelative(0.0f, 0.10f, 1.0f, 0.04f);

    // Waveform section (20% of height)
    waveformComponent.setBoundsRelative(0.05f, 0.18f, 0.9f, 0.18f);

    // Result display section (15% of height)
    zcrValueLabel.setBoundsRelative(0.1f, 0.42f, 0.8f, 0.08f);
    zcrUnitLabel.setBoundsRelative(0.1f, 0.50f, 0.8f, 0.03f);

    // Threshold control section (20% of height)
    thresholdLabel.setBoundsRelative(0.1f, 0.65f, 0.4f, 0.05f);
    thresholdValueLabel.setBoundsRelative(0.5f, 0.65f, 0.4f, 0.05f);
    thresholdSlider.setBoundsRelative(0.1f, 0.72f, 0.8f, 0.06f);

    // Footer section (10% of height)
    footerLabel.setBoundsRelative(0.1f, 0.88f, 0.8f, 0.04f);
}