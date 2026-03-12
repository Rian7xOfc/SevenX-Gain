#pragma once

#include <JuceHeader.h>
#include "SevenXSaturation.h"
#include "SevenXOversampling.h"

/**
 * SevenXGainAudioProcessor
 * Adicionado o parâmetro de Output Gain para controlo total do volume.
 */
class SevenXGainAudioProcessor : public juce::AudioProcessor
{
public:
    SevenXGainAudioProcessor();
    ~SevenXGainAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    float getRMSLevel(int channel) const {
        return (channel == 0) ? rmsLevelLeft.load() : rmsLevelRight.load();
    }

    // Parâmetros de Áudio
    juce::AudioParameterFloat* gainParam;   // Input Gain
    juce::AudioParameterFloat* outParam;    // NOVO: Output Gain
    juce::AudioParameterBool* bypassParam;
    juce::AudioParameterBool* satParam;
    juce::AudioParameterBool* modeParam;

private:
    SevenXOversampling oversampling;

    std::atomic<float> rmsLevelLeft{ 0.0f };
    std::atomic<float> rmsLevelRight{ 0.0f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SevenXGainAudioProcessor)
};