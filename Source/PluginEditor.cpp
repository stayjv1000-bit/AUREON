#include "PluginEditor.h"

AureonAudioProcessorEditor::AureonAudioProcessorEditor(AureonAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(760, 470);
    title.setText("AUREON", juce::dontSendNotification);
    title.setFont(juce::Font(juce::FontOptions(34.0f, juce::Font::bold)));
    title.setColour(juce::Label::textColourId, juce::Colour(0xffe8c77a));
    addAndMakeVisible(title);

    subtitle.setText("SOUNDFONT GRAND PIANO  •  FLUIDSYNTH ENGINE", juce::dontSendNotification);
    subtitle.setFont(juce::Font(juce::FontOptions(12.0f)));
    subtitle.setColour(juce::Label::textColourId, juce::Colour(0xffaaa9a5));
    addAndMakeVisible(subtitle);

    styleSlider(master, "MASTER");
    styleSlider(brightness, "BRIGHTNESS");
    styleSlider(reverbAmount, "REVERB");

    masterAttachment = std::make_unique<Attachment>(audioProcessor.parameters, "master", master);
    brightnessAttachment = std::make_unique<Attachment>(audioProcessor.parameters, "brightness", brightness);
    reverbAttachment = std::make_unique<Attachment>(audioProcessor.parameters, "reverb", reverbAmount);
}

void AureonAudioProcessorEditor::styleSlider(juce::Slider& slider, const juce::String& name)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 22);
    slider.setName(name);
    slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffd6ad58));
    slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff41434a));
    slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xffffe0a0));
    slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xff45464b));
    addAndMakeVisible(slider);
}

void AureonAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff15161a));
    g.setColour(juce::Colour(0xff24262c));
    g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(18.0f), 14.0f);
    g.setColour(juce::Colour(0xffd6ad58));
    g.fillRect(36, 112, getWidth() - 72, 1);

    g.setColour(juce::Colour(0xffd7d7d7));
    g.setFont(juce::Font(juce::FontOptions(14.0f, juce::Font::bold)));
    g.drawText("TONE & SPACE", 48, 145, 220, 24, juce::Justification::left);

    g.setFont(juce::Font(juce::FontOptions(11.0f)));
    g.setColour(juce::Colour(0xffa5a5a5));
    g.drawText("GeneralUser GS SoundFont  •  Grand Piano preset  •  MIDI sustain support",
               48, 400, getWidth() - 96, 22, juce::Justification::left);
}

void AureonAudioProcessorEditor::resized()
{
    title.setBounds(42, 34, 300, 48);
    subtitle.setBounds(45, 78, 500, 22);
    master.setBounds(65, 190, 170, 175);
    brightness.setBounds(295, 190, 170, 175);
    reverbAmount.setBounds(525, 190, 170, 175);
}
