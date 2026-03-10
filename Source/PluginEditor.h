#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class SevenXEditor : public juce::AudioProcessorEditor, 
                     public juce::Slider::Listener, 
                     public juce::Button::Listener,
                     public juce::Timer
{
public:
    SevenXEditor (SevenXGainAudioProcessor&);
    ~SevenXEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    
    void sliderValueChanged (juce::Slider* slider) override;
    void buttonClicked (juce::Button* button) override;
    void timerCallback() override;

private:
    void updateModeButtonText();

    SevenXGainAudioProcessor& audioProcessor;

    juce::Slider ganhoSlider;
    juce::TextButton bypassButton;
    juce::TextButton satButton;
    juce::TextButton modeButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SevenXEditor)
};