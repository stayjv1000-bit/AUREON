#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class AureonAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AureonAudioProcessorEditor(AureonAudioProcessor&);
    ~AureonAudioProcessorEditor() override = default;
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    AureonAudioProcessor& audioProcessor;
    juce::Slider master, brightness, reverbAmount;
    juce::Label title, subtitle;
    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> masterAttachment, brightnessAttachment, reverbAttachment;
    void styleSlider(juce::Slider&, const juce::String&);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AureonAudioProcessorEditor)
};