#include "ZeroCrossingRate/PluginEditor.h"
#include <juce_core/juce_core.h>

//==============================================================================
// WaveformComponent Implementation
//==============================================================================
WaveformComponent::WaveformComponent()
{
    audioBuffer.reserve(maxBufferSize);
    startTimerHz(30); // Update display 30 times per second
}

WaveformComponent::~WaveformComponent()
{
    stopTimer();
}

void WaveformComponent::paint(juce::Graphics& g)
{
    // Dark background with subtle border
    g.setColour(juce::Colour(0xff1a202c));
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 8.0f);

    g.setColour(juce::Colour(0xff2d3748));
    g.drawRoundedRectangle(getLocalBounds().toFloat().reduced(1), 8.0f, 1.0f);

    const float width   = static_cast<float>(getWidth());
    const float height  = static_cast<float>(getHeight());
    const float centerY = height * 0.5f;
    const float margin  = 15.0f;

    // Draw waveform from audio buffer
    if (!audioBuffer.empty())
    {
        g.setColour(juce::Colour(0xff4299e1));

        juce::Path  waveformPath;
        const float samplesPerPixel = static_cast<float>(audioBuffer.size()) / (width - 2 * margin);
        bool        pathStarted     = false;

        for (float x = margin; x < width - margin; x += 1.0f)
        {
            const int sampleIndex = static_cast<int>((x - margin) * samplesPerPixel);
            if (sampleIndex >= 0 && sampleIndex < static_cast<int>(audioBuffer.size()))
            {
                // Scale audio sample (-1 to 1) to display coordinates
                const float sample = audioBuffer[static_cast<size_t>(sampleIndex)];
                const float y      = centerY - (sample * height * 0.3f); // Use 30% of height for waveform

                if (!pathStarted)
                {
                    waveformPath.startNewSubPath(x, y);
                    pathStarted = true;
                }
                else
                {
                    waveformPath.lineTo(x, y);
                }
            }
        }

        g.strokePath(waveformPath, juce::PathStrokeType(1.5f));
    }

    // Draw threshold lines (positive and negative)
    const float thresholdY_pos = centerY - (currentThreshold * height * 0.3f);
    const float thresholdY_neg = centerY + (currentThreshold * height * 0.3f);

    g.setColour(juce::Colour(0xfff56565));
    g.fillRect(margin, thresholdY_pos - 1.0f, width - 2 * margin, 2.0f);
    g.fillRect(margin, thresholdY_neg - 1.0f, width - 2 * margin, 2.0f);

    // Draw center line (0 amplitude)
    g.setColour(juce::Colour(0x404299e1));
    g.fillRect(margin, centerY - 0.5f, width - 2 * margin, 1.0f);
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

void WaveformComponent::pushAudioData(const float* audioData, int numSamples)
{
    // Add new samples to the buffer
    for (int i = 0; i < numSamples; ++i)
    {
        audioBuffer.push_back(audioData[i]);
    }

    // Keep buffer size under control
    while (static_cast<int>(audioBuffer.size()) > maxBufferSize)
    {
        audioBuffer.erase(audioBuffer.begin());
    }

    bufferNeedsUpdate = true;
}

void WaveformComponent::timerCallback()
{
    // Repaint if buffer has been updated
    if (bufferNeedsUpdate.exchange(false))
    {
        repaint();
    }
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

    // Note: footerLabel removed - replaced with poweredByLabel + essentia logo

    // Create the slider attachment
    thresholdAttachment.reset(new juce::AudioProcessorValueTreeState::SliderAttachment(processorRef.getAPVTS(),
                                                                                       "zeroCrossingThreshold",
                                                                                       thresholdSlider));

    // Load logos from binary data
    upfLogo      = juce::ImageCache::getFromMemory(BinaryData::upflogo_png, BinaryData::upflogo_pngSize);
    essentiaLogo = juce::ImageCache::getFromMemory(BinaryData::essentia_logo_png, BinaryData::essentia_logo_pngSize);

    // Setup "powered by" label
    poweredByLabel.setText("powered by", juce::dontSendNotification);
    poweredByLabel.setFont(juce::FontOptions(8.30437f));
    poweredByLabel.setColour(juce::Label::textColourId, juce::Colour(0xff718096));
    poweredByLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(poweredByLabel);

    // Set plugin size (fixed, not resizable)
    setSize(400, 500);
    setResizable(false, false);

    // Register with processor for audio data
    processorRef.setEditor(this);

    startTimerHz(30);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    // Unregister from processor
    processorRef.removeEditor();
    setLookAndFeel(nullptr);
}

void AudioPluginAudioProcessorEditor::pushAudioData(const float* audioData, int numSamples)
{
    waveformComponent.pushAudioData(audioData, numSamples);
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

    // Footer layout with consistent positioning
    // Define footer layout constants for easy maintenance
    const float footerBaselineFromBottom = 60.0f; // Distance from container bottom to logo baseline
    const float footerSideMargin         = 20.0f; // Margin from container edges
    const float standardLogoHeight       = 38.0f; // Standard height for visual consistency

    // Draw UPF logo (left side)
    if (upfLogo.isValid())
    {
        const float upfLogoWidth = upfLogo.getWidth() * (standardLogoHeight / upfLogo.getHeight());
        const int   upfLogoX     = static_cast<int>(containerBounds.getX() + footerSideMargin);
        const int   upfLogoY     = static_cast<int>(containerBounds.getBottom() - footerBaselineFromBottom);

        g.drawImage(upfLogo,
                    upfLogoX,
                    upfLogoY,
                    static_cast<int>(upfLogoWidth),
                    static_cast<int>(standardLogoHeight),
                    0,
                    0,
                    upfLogo.getWidth(),
                    upfLogo.getHeight());
    }

    // Draw Essentia logo (right side, aligned on same baseline)
    if (essentiaLogo.isValid())
    {
        const float essentiaLogoWidth = essentiaLogo.getWidth() * (standardLogoHeight / essentiaLogo.getHeight());
        const int   essentiaLogoX = static_cast<int>(containerBounds.getRight() - footerSideMargin - essentiaLogoWidth);
        const int   essentiaLogoY = static_cast<int>(containerBounds.getBottom() - footerBaselineFromBottom);

        g.drawImage(essentiaLogo,
                    essentiaLogoX,
                    essentiaLogoY,
                    static_cast<int>(essentiaLogoWidth),
                    static_cast<int>(standardLogoHeight),
                    0,
                    0,
                    essentiaLogo.getWidth(),
                    essentiaLogo.getHeight());
    }
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

    // Position "powered by" text above Essentia logo - simplified positioning
    const float logoHeight   = 38.0f;
    const float logoBaseline = 25.0f;
    const float textGap      = 15.0f;

    poweredByLabel.setBounds(
        getWidth() - 120,                                                         // 120px from right edge
        static_cast<int>(getHeight() - logoBaseline - logoHeight - textGap - 12), // Above logo with gap
        80,                                                                       // Text width
        12);                                                                      // Text height
}