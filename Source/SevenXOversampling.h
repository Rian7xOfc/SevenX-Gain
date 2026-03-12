#pragma once
#include <JuceHeader.h>

class SevenXOversampling
{
public:
    void prepare(double sampleRate, int samplesPerBlock, int channels)
    {
        juce::dsp::ProcessSpec spec;
        spec.sampleRate = sampleRate;
        spec.maximumBlockSize = samplesPerBlock;
        spec.numChannels = channels;

        oversampling = std::make_unique<juce::dsp::Oversampling<float>>(
            channels, 2, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR
        );

        oversampling->initProcessing(samplesPerBlock);
    }

    template<typename Process>
    void process(juce::AudioBuffer<float>& buffer, Process processSample)
    {
        juce::dsp::AudioBlock<float> block(buffer);
        auto upsampled = oversampling->processSamplesUp(block);

        for (size_t ch = 0; ch < upsampled.getNumChannels(); ++ch)
        {
            auto* data = upsampled.getChannelPointer(ch);
            for (size_t i = 0; i < upsampled.getNumSamples(); ++i)
                data[i] = processSample(data[i]);
        }

        oversampling->processSamplesDown(block);
    }

private:
    std::unique_ptr<juce::dsp::Oversampling<float>> oversampling;
};