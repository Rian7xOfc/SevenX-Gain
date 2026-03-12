#include "PluginProcessor.h"
#include "PluginEditor.h"

SevenXGainAudioProcessor::SevenXGainAudioProcessor()
    : AudioProcessor(BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    // Adicionamos os dois controlos de volume
    addParameter(gainParam = new juce::AudioParameterFloat({ "gain", 1 }, "Input Gain", 0.0f, 2.0f, 1.0f));
    addParameter(outParam = new juce::AudioParameterFloat({ "out", 1 }, "Output Gain", 0.0f, 2.0f, 1.0f));

    addParameter(bypassParam = new juce::AudioParameterBool({ "bypass", 1 }, "Bypass", false));
    addParameter(satParam = new juce::AudioParameterBool({ "saturate", 1 }, "Saturação", true));
    addParameter(modeParam = new juce::AudioParameterBool({ "mode", 1 }, "Modo Master", false));
}

SevenXGainAudioProcessor::~SevenXGainAudioProcessor() {}

void SevenXGainAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    oversampling.prepare(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
}

void SevenXGainAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/)
{
    juce::ScopedNoDenormals noDenormals;

    rmsLevelLeft.store(buffer.getRMSLevel(0, 0, buffer.getNumSamples()));
    if (buffer.getNumChannels() > 1)
        rmsLevelRight.store(buffer.getRMSLevel(1, 0, buffer.getNumSamples()));

    if (*bypassParam) return;

    // 1. Aplica o Ganho de Entrada (Pre-Saturação)
    buffer.applyGain(*gainParam);

    // 2. Processa Saturação com Oversampling
    if (*satParam)
    {
        oversampling.process(buffer, [this](float sample) {
            return SevenXSaturation::processSample(sample, true, *modeParam);
            });
    }

    // 3. Aplica o Ganho de Saída (Post-Saturação / Maquilhagem)
    buffer.applyGain(*outParam);
}

// Métodos obrigatórios do JUCE
const juce::String SevenXGainAudioProcessor::getName() const { return "SevenX Gain"; }
bool SevenXGainAudioProcessor::acceptsMidi() const { return false; }
bool SevenXGainAudioProcessor::producesMidi() const { return false; }
bool SevenXGainAudioProcessor::isMidiEffect() const { return false; }
double SevenXGainAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int SevenXGainAudioProcessor::getNumPrograms() { return 1; }
int SevenXGainAudioProcessor::getCurrentProgram() { return 0; }
void SevenXGainAudioProcessor::setCurrentProgram(int) {}
const juce::String SevenXGainAudioProcessor::getProgramName(int) { return {}; }
void SevenXGainAudioProcessor::changeProgramName(int, const juce::String&) {}
void SevenXGainAudioProcessor::releaseResources() {}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SevenXGainAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}
#endif

juce::AudioProcessorEditor* SevenXGainAudioProcessor::createEditor() { return new SevenXEditor(*this); }
bool SevenXGainAudioProcessor::hasEditor() const { return true; }

void SevenXGainAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
    juce::MemoryOutputStream stream(destData, true);
    stream.writeFloat(*gainParam);
    stream.writeFloat(*outParam);
    stream.writeBool(*bypassParam);
    stream.writeBool(*satParam);
    stream.writeBool(*modeParam);
}

void SevenXGainAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
    juce::MemoryInputStream stream(data, (size_t)sizeInBytes, false);
    gainParam->setValueNotifyingHost(stream.readFloat());
    outParam->setValueNotifyingHost(stream.readFloat());
    bypassParam->setValueNotifyingHost(stream.readBool());
    satParam->setValueNotifyingHost(stream.readBool());
    modeParam->setValueNotifyingHost(stream.readBool());
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new SevenXGainAudioProcessor(); }