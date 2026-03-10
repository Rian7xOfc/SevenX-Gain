#include "PluginProcessor.h"
#include "PluginEditor.h"

SevenXGainAudioProcessor::SevenXGainAudioProcessor()
     : AudioProcessor (BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                         .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    addParameter (gainParam = new juce::AudioParameterFloat ({ "gain", 1 }, "Ganho", 0.0f, 1.0f, 0.5f));
    addParameter (bypassParam = new juce::AudioParameterBool ({ "bypass", 1 }, "Bypass", false));
    addParameter (satParam = new juce::AudioParameterBool ({ "saturate", 1 }, "Saturar", false));
    addParameter (modeParam = new juce::AudioParameterBool ({ "mode", 1 }, "Modo Master", false));
}

SevenXGainAudioProcessor::~SevenXGainAudioProcessor() {}

void SevenXGainAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/)
{
    juce::ScopedNoDenormals noDenormals;

    // Medição VU
    rmsLevelLeft.store (buffer.getRMSLevel (0, 0, buffer.getNumSamples()));
    if (buffer.getNumChannels() > 1)
        rmsLevelRight.store (buffer.getRMSLevel (1, 0, buffer.getNumSamples()));

    if (*bypassParam) return;

    buffer.applyGain (*gainParam);

    if (*satParam)
    {
        for (int canal = 0; canal < buffer.getNumChannels(); ++canal)
        {
            float* dados = buffer.getWritePointer (canal);
            for (int i = 0; i < buffer.getNumSamples(); ++i)
            {
                dados[i] = SevenXSaturation::processSample (dados[i], true, *modeParam);
            }
        }
    }
}

// Implementações obrigatórias (Limpa de avisos)
const juce::String SevenXGainAudioProcessor::getName() const { return "SevenX Gain"; }
bool SevenXGainAudioProcessor::acceptsMidi() const { return false; }
bool SevenXGainAudioProcessor::producesMidi() const { return false; }
bool SevenXGainAudioProcessor::isMidiEffect() const { return false; }
double SevenXGainAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int SevenXGainAudioProcessor::getNumPrograms() { return 1; }
int SevenXGainAudioProcessor::getCurrentProgram() { return 0; }
void SevenXGainAudioProcessor::setCurrentProgram (int /*index*/) {}
const juce::String SevenXGainAudioProcessor::getProgramName (int /*index*/) { return {}; }
void SevenXGainAudioProcessor::changeProgramName (int /*index*/, const juce::String& /*newName*/) {}
void SevenXGainAudioProcessor::prepareToPlay (double /*sampleRate*/, int /*samplesPerBlock*/) {}
void SevenXGainAudioProcessor::releaseResources() {}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SevenXGainAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const {
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}
#endif

// Cria o Editor (Interface)
juce::AudioProcessorEditor* SevenXGainAudioProcessor::createEditor() { 
    return new SevenXEditor (*this); 
}

bool SevenXGainAudioProcessor::hasEditor() const { return true; }

void SevenXGainAudioProcessor::getStateInformation (juce::MemoryBlock& destData) {
    juce::MemoryOutputStream stream (destData, true);
    stream.writeFloat (*gainParam); stream.writeBool (*bypassParam); 
    stream.writeBool (*satParam); stream.writeBool (*modeParam);
}

void SevenXGainAudioProcessor::setStateInformation (const void* data, int sizeInBytes) {
    juce::MemoryInputStream stream (data, (size_t)sizeInBytes, false);
    gainParam->setValueNotifyingHost (stream.readFloat());
    bypassParam->setValueNotifyingHost (stream.readBool());
    satParam->setValueNotifyingHost (stream.readBool());
    modeParam->setValueNotifyingHost (stream.readBool());
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new SevenXGainAudioProcessor(); }