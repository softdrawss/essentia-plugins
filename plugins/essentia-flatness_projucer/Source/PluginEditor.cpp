#include "PluginEditor.h"

ModernLookAndFeel::ModernLookAndFeel()
{
    setColour(juce::Label::textColourId, juce::Colours::white);
}

EssentiaPluginAudioProcessorEditor::EssentiaPluginAudioProcessorEditor(EssentiaPluginAudioProcessor& p)
    : AudioProcessorEditor(&p)
    , processorRef(p)
{
    setLookAndFeel(&modernLF);

    titleLabel.setText("Flatness Analyzer", juce::dontSendNotification);
    titleLabel.setFont(juce::FontOptions(24.0f));
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setInterceptsMouseClicks(false, false);
    addAndMakeVisible(titleLabel);

    subtitleLabel.setText("Real-time spectral flatness analysis", juce::dontSendNotification);
    subtitleLabel.setFont(juce::FontOptions(14.0f));
    subtitleLabel.setColour(juce::Label::textColourId, juce::Colour(0xffa0aec0));
    subtitleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(subtitleLabel);

    // Main flatness value
    rmsLinearValueLabel.setText("0.000", juce::dontSendNotification);
    rmsLinearValueLabel.setFont(juce::FontOptions(48.0f, juce::Font::bold));
    rmsLinearValueLabel.setColour(juce::Label::textColourId, juce::Colour(0xffed64a6));
    rmsLinearValueLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(rmsLinearValueLabel);

    rmsLinearUnitLabel.setText("Flatness (0=tonal, 1=noise)", juce::dontSendNotification);
    rmsLinearUnitLabel.setFont(juce::FontOptions(14.0f));
    rmsLinearUnitLabel.setColour(juce::Label::textColourId, juce::Colour(0xffa0aec0));
    rmsLinearUnitLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(rmsLinearUnitLabel);

    // Repurpose dB label to show noise/tonal character
    rmsDbValueLabel.setText("Tonal", juce::dontSendNotification);
    rmsDbValueLabel.setFont(juce::FontOptions(48.0f, juce::Font::bold));
    rmsDbValueLabel.setColour(juce::Label::textColourId, juce::Colour(0xffb794f6));
    rmsDbValueLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(rmsDbValueLabel);

    rmsDbUnitLabel.setText("Character", juce::dontSendNotification);
    rmsDbUnitLabel.setFont(juce::FontOptions(14.0f));
    rmsDbUnitLabel.setColour(juce::Label::textColourId, juce::Colour(0xffa0aec0));
    rmsDbUnitLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(rmsDbUnitLabel);

    upfLogo      = juce::ImageCache::getFromMemory(BinaryData::upflogo_png, BinaryData::upflogo_pngSize);
    essentiaLogo = juce::ImageCache::getFromMemory(BinaryData::essentia_logo_png, BinaryData::essentia_logo_pngSize);

    poweredByLabel.setText("powered by", juce::dontSendNotification);
    poweredByLabel.setFont(juce::FontOptions(8.30437f));
    poweredByLabel.setColour(juce::Label::textColourId, juce::Colour(0xff718096));
    poweredByLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(poweredByLabel);

    setSize(400, 350);
    setResizable(false, false);
    startTimerHz(30);
}

EssentiaPluginAudioProcessorEditor::~EssentiaPluginAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void EssentiaPluginAudioProcessorEditor::timerCallback()
{
    const float flatness = processorRef.getFlatness();

    rmsLinearValueLabel.setText(juce::String(flatness, 3), juce::dontSendNotification);

    // Show character based on flatness value
    juce::String character;
    if (flatness < 0.2f)
        character = "Tonal";
    else if (flatness < 0.5f)
        character = "Mixed";
    else
        character = "Noise";

    rmsDbValueLabel.setText(character, juce::dontSendNotification);

    repaint();
}

void EssentiaPluginAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff1a202c));
    
    // Draw a bar showing flatness level
    const float flatness = processorRef.getFlatness();
    const auto barArea = getLocalBounds().reduced(20).removeFromBottom(20);
    g.setColour(juce::Colour(0xff2d3748));
    g.fillRect(barArea);
    g.setColour(juce::Colour(0xffed64a6));
    g.fillRect(barArea.withWidth((int)(barArea.getWidth() * flatness)));

    // Draw logos
    if (upfLogo.isValid())
        g.drawImageWithin(upfLogo, 10, getHeight() - 40, 60, 30, juce::RectanglePlacement::centred);
    if (essentiaLogo.isValid())
        g.drawImageWithin(essentiaLogo, getWidth() - 80, getHeight() - 40, 70, 30, juce::RectanglePlacement::centred);
}

void EssentiaPluginAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(20);

    titleLabel.setBounds(area.removeFromTop(40));
    subtitleLabel.setBounds(area.removeFromTop(25));
    area.removeFromTop(10);

    auto row1 = area.removeFromTop(80);
    rmsLinearValueLabel.setBounds(row1.removeFromLeft(row1.getWidth() / 2));
    rmsDbValueLabel.setBounds(row1);

    auto row2 = area.removeFromTop(25);
    rmsLinearUnitLabel.setBounds(row2.removeFromLeft(row2.getWidth() / 2));
    rmsDbUnitLabel.setBounds(row2);

    area.removeFromTop(10);
    poweredByLabel.setBounds(area.removeFromTop(20));
}

void EssentiaPluginAudioProcessorEditor::mouseDown(const juce::MouseEvent& event)
{
    juce::ignoreUnused(event);
}

void EssentiaPluginAudioProcessorEditor::mouseMove(const juce::MouseEvent& event)
{
    juce::ignoreUnused(event);
}