#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
class BasicPianoVoice final : public juce::SynthesiserVoice
{
public:
    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<juce::SynthesiserSound*>(sound) != nullptr;
    }
    void startNote(int midiNote, float velocity, juce::SynthesiserSound*, int) override
    {
        frequency = juce::MidiMessage::getMidiNoteInHertz(midiNote);
        level = velocity * 0.12f;
        phase = 0.0;
        release = false;
        envelope = 0.0f;
    }
    void stopNote(float, bool allowTailOff) override
    {
        if (allowTailOff) release = true;
        else clearCurrentNote();
    }
    void pitchWheelMoved(int) override {}
    void controllerMoved(int, int) override {}
    void renderNextBlock(juce::AudioBuffer<float>& output, int start, int count) override
    {
        if (!isVoiceActive()) return;
        const auto sr = getSampleRate();
        if (sr <= 0.0) return;
        for (int i = 0; i < count; ++i)
        {
            envelope += release ? -0.0007f : 0.0035f;
            envelope = juce::jlimit(0.0f, 1.0f, envelope);
            if (release && envelope <= 0.0001f) { clearCurrentNote(); break; }
            const auto sample = static_cast<float>(std::sin(phase) * level * envelope);
            phase += juce::MathConstants<double>::twoPi * frequency / sr;
            if (phase >= juce::MathConstants<double>::twoPi) phase -= juce::MathConstants<double>::twoPi;
            for (int channel = 0; channel < output.getNumChannels(); ++channel)
                output.addSample(channel, start + i, sample);
        }
    }
private:
    double frequency = 440.0, phase = 0.0;
    float level = 0.0f, envelope = 0.0f;
    bool release = false;
};

class BasicPianoSound final : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};
}

juce::AudioProcessorValueTreeState::ParameterLayout AureonAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back(std::make_unique<juce::AudioParameterFloat>("master", "Master", juce::NormalisableRange<float>(0.0f, 1.0f), 0.8f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("brightness", "Brightness", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("reverb", "Reverb", juce::NormalisableRange<float>(0.0f, 1.0f), 0.18f));
    return { params.begin(), params.end() };
}

AureonAudioProcessor::AureonAudioProcessor()
 : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
   parameters(*this, nullptr, "AUREON_STATE", createParameterLayout())
{
    for (int i = 0; i < 16; ++i) synth.addVoice(new BasicPianoVoice());
    synth.addSound(new BasicPianoSound());
}

void AureonAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());
    reverb.prepare(spec);
    reverb.reset();
}

void AureonAudioProcessor::releaseResources()
{
    reverb.reset();
}

bool AureonAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto output = layouts.getMainOutputChannelSet();
    return output == juce::AudioChannelSet::mono() || output == juce::AudioChannelSet::stereo();
}

void AureonAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();
    synth.renderNextBlock(buffer, midi, 0, buffer.getNumSamples());

    const auto gain = parameters.getRawParameterValue("master")->load();
    buffer.applyGain(gain);

    juce::dsp::Reverb::Parameters rp;
    rp.roomSize = 0.55f;
    rp.damping = 0.45f;
    rp.wetLevel = parameters.getRawParameterValue("reverb")->load() * 0.5f;
    rp.dryLevel = 1.0f - rp.wetLevel * 0.35f;
    rp.width = 1.0f;
    reverb.setParameters(rp);

    juce::dsp::AudioBlock<float> audioBlock(buffer);
    juce::dsp::ProcessContextReplacing<float> context(audioBlock);
    reverb.process(context);
}

juce::AudioProcessorEditor* AureonAudioProcessor::createEditor()
{
    return new AureonAudioProcessorEditor(*this);
}

void AureonAudioProcessor::getStateInformation(juce::MemoryBlock& dest)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, dest);
}

void AureonAudioProcessor::setStateInformation(const void* data, int size)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, size));
    if (xml != nullptr && xml->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AureonAudioProcessor();
}
