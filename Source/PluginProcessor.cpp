#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "BinaryData.h"

namespace
{
constexpr int firstMidiChannel = 0;
constexpr int lastMidiChannel = 15;
}

juce::AudioProcessorValueTreeState::ParameterLayout AureonAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "master", "Master", juce::NormalisableRange<float>(0.0f, 1.0f), 0.8f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "brightness", "Brightness", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "reverb", "Reverb", juce::NormalisableRange<float>(0.0f, 1.0f), 0.22f));
    return { params.begin(), params.end() };
}

AureonAudioProcessor::AureonAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, "AUREON_STATE", createParameterLayout())
{
}

AureonAudioProcessor::~AureonAudioProcessor()
{
    if (fluidSynth != nullptr)
        delete_fluid_synth(fluidSynth);
    if (fluidSettings != nullptr)
        delete_fluid_settings(fluidSettings);
}

bool AureonAudioProcessor::loadEmbeddedSoundFont()
{
    const auto appData = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
                             .getChildFile("AUREON");
    if (!appData.createDirectory())
        return false;

    const auto soundFontFile = appData.getChildFile("GeneralUser_GS.sf2");
    const auto licenseFile = appData.getChildFile("GeneralUser_GS_LICENSE.txt");

    if (!soundFontFile.existsAsFile()
        || soundFontFile.getSize() != static_cast<juce::int64>(BinaryData::GeneralUser_GS_sf2Size))
    {
        if (!soundFontFile.replaceWithData(BinaryData::GeneralUser_GS_sf2,
                                           static_cast<size_t>(BinaryData::GeneralUser_GS_sf2Size)))
            return false;
    }

    if (!licenseFile.existsAsFile()
        || licenseFile.getSize() != static_cast<juce::int64>(BinaryData::GeneralUser_GS_LICENSE_txtSize))
    {
        licenseFile.replaceWithData(BinaryData::GeneralUser_GS_LICENSE_txt,
                                    static_cast<size_t>(BinaryData::GeneralUser_GS_LICENSE_txtSize));
    }

    fluidSettings = new_fluid_settings();
    if (fluidSettings == nullptr)
        return false;

    fluid_settings_setnum(fluidSettings, "synth.sample-rate", getSampleRate());
    fluid_settings_setnum(fluidSettings, "synth.gain", 0.65);
    fluid_settings_setint(fluidSettings, "synth.polyphony", 256);
    fluid_settings_setint(fluidSettings, "synth.reverb.active", 1);
    fluid_settings_setint(fluidSettings, "synth.chorus.active", 1);

    fluidSynth = new_fluid_synth(fluidSettings);
    if (fluidSynth == nullptr)
        return false;

    soundFontId = fluid_synth_sfload(fluidSynth, soundFontFile.getFullPathName().toRawUTF8(), 1);
    if (soundFontId < 0)
        return false;

    // General MIDI bank 0, preset 0 is the acoustic Grand Piano.
    return fluid_synth_program_select(fluidSynth, 0, soundFontId, 0, 0) == FLUID_OK;
}

void AureonAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    if (fluidSynth != nullptr)
    {
        delete_fluid_synth(fluidSynth);
        fluidSynth = nullptr;
    }
    if (fluidSettings != nullptr)
    {
        delete_fluid_settings(fluidSettings);
        fluidSettings = nullptr;
    }

    leftScratch.assign(static_cast<size_t>(juce::jmax(1, samplesPerBlock)), 0.0f);
    rightScratch.assign(static_cast<size_t>(juce::jmax(1, samplesPerBlock)), 0.0f);

    loadEmbeddedSoundFont();
    juce::ignoreUnused(sampleRate);
}

void AureonAudioProcessor::releaseResources()
{
    if (fluidSynth != nullptr)
        fluid_synth_system_reset(fluidSynth);
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

    if (fluidSynth == nullptr)
        return;

    const auto master = parameters.getRawParameterValue("master")->load();
    const auto brightness = juce::roundToInt(parameters.getRawParameterValue("brightness")->load() * 127.0f);
    const auto reverbSend = juce::roundToInt(parameters.getRawParameterValue("reverb")->load() * 127.0f);

    for (int channel = firstMidiChannel; channel <= lastMidiChannel; ++channel)
    {
        fluid_synth_cc(fluidSynth, channel, 74, brightness);
        fluid_synth_cc(fluidSynth, channel, 91, reverbSend);
    }

    const auto numSamples = buffer.getNumSamples();
    const auto numChannels = buffer.getNumChannels();
    const auto scratchSize = static_cast<int>(leftScratch.size());

    auto renderSegment = [&](int start, int length)
    {
        int rendered = 0;
        while (rendered < length)
        {
            const auto chunk = juce::jmin(length - rendered, scratchSize);
            if (fluid_synth_write_float(fluidSynth, chunk,
                                        leftScratch.data(), 0, 1,
                                        rightScratch.data(), 0, 1) != FLUID_OK)
                break;

            for (int i = 0; i < chunk; ++i)
            {
                const auto destination = start + rendered + i;
                if (numChannels >= 2)
                {
                    buffer.addSample(0, destination, leftScratch[static_cast<size_t>(i)]);
                    buffer.addSample(1, destination, rightScratch[static_cast<size_t>(i)]);
                }
                else if (numChannels == 1)
                {
                    const auto mono = 0.5f * (leftScratch[static_cast<size_t>(i)]
                                            + rightScratch[static_cast<size_t>(i)]);
                    buffer.addSample(0, destination, mono);
                }
            }
            rendered += chunk;
        }
    };

    int cursor = 0;
    for (const auto metadata : midi)
    {
        const auto eventPosition = juce::jlimit(cursor, numSamples, metadata.samplePosition);
        renderSegment(cursor, eventPosition - cursor);
        cursor = eventPosition;

        const auto message = metadata.getMessage();
        const auto channel = juce::jlimit(0, 15, message.getChannel() - 1);

        if (message.isNoteOn())
            fluid_synth_noteon(fluidSynth, channel, message.getNoteNumber(), message.getVelocity());
        else if (message.isNoteOff())
            fluid_synth_noteoff(fluidSynth, channel, message.getNoteNumber());
        else if (message.isController())
            fluid_synth_cc(fluidSynth, channel, message.getControllerNumber(), message.getControllerValue());
        else if (message.isProgramChange())
            fluid_synth_program_change(fluidSynth, channel, message.getProgramChangeNumber());
        else if (message.isPitchWheel())
            fluid_synth_pitch_bend(fluidSynth, channel, message.getPitchWheelValue());
        else if (message.isAllNotesOff())
            fluid_synth_cc(fluidSynth, channel, 123, 0);
        else if (message.isAllSoundOff())
            fluid_synth_cc(fluidSynth, channel, 120, 0);
    }

    renderSegment(cursor, numSamples - cursor);
    buffer.applyGain(master);
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
