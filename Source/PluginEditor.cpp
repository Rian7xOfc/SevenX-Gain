#include "PluginProcessor.h"
#include "PluginEditor.h"

SevenXEditor::SevenXEditor (SevenXGainAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Knob de Ganho
    addAndMakeVisible (ganhoSlider);
    ganhoSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    ganhoSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
    ganhoSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::cyan);
    ganhoSlider.setRange (audioProcessor.gainParam->range.start, audioProcessor.gainParam->range.end);
    ganhoSlider.setValue (audioProcessor.gainParam->get());
    ganhoSlider.addListener (this);

    // Botão Bypass
    addAndMakeVisible (bypassButton);
    bypassButton.setButtonText ("BYPASS");
    bypassButton.setClickingTogglesState (true);
    bypassButton.setToggleState (audioProcessor.bypassParam->get(), juce::dontSendNotification);
    bypassButton.addListener (this);
    bypassButton.setColour (juce::TextButton::buttonOnColourId, juce::Colours::orange);

    // Botão Saturar
    addAndMakeVisible (satButton);
    satButton.setButtonText ("SATURAR");
    satButton.setClickingTogglesState (true);
    satButton.setToggleState (audioProcessor.satParam->get(), juce::dontSendNotification);
    satButton.addListener (this);
    satButton.setColour (juce::TextButton::buttonOnColourId, juce::Colours::red);

    // Botão Modo
    addAndMakeVisible (modeButton);
    modeButton.setClickingTogglesState (true);
    modeButton.setToggleState (audioProcessor.modeParam->get(), juce::dontSendNotification);
    modeButton.addListener (this);
    modeButton.setColour (juce::TextButton::buttonOnColourId, juce::Colours::purple);
    updateModeButtonText();

    startTimerHz (60);
    setSize (350, 520);
}

SevenXEditor::~SevenXEditor() {}

void SevenXEditor::updateModeButtonText() {
    modeButton.setButtonText (audioProcessor.modeParam->get() ? "MODO: MASTER" : "MODO: VOZ");
}

void SevenXEditor::timerCallback() { repaint(); }

void SevenXEditor::sliderValueChanged (juce::Slider* slider) {
    if (slider == &ganhoSlider) 
        audioProcessor.gainParam->setValueNotifyingHost ((float)ganhoSlider.getValue());
}

void SevenXEditor::buttonClicked (juce::Button* button) {
    if (button == &bypassButton) 
        audioProcessor.bypassParam->setValueNotifyingHost (bypassButton.getToggleState());
    
    if (button == &satButton) 
        audioProcessor.satParam->setValueNotifyingHost (satButton.getToggleState());
    
    if (button == &modeButton) {
        audioProcessor.modeParam->setValueNotifyingHost (modeButton.getToggleState());
        updateModeButtonText();
    }
}

void SevenXEditor::paint (juce::Graphics& g) {
    g.setGradientFill (juce::ColourGradient (juce::Colours::black, 0, 0, juce::Colour(0xff0f0f0f), 0, (float)getHeight(), false));
    g.fillAll();
    
    g.setColour (juce::Colours::cyan);
    g.setFont (juce::FontOptions(32.0f).withStyle("Bold"));
    g.drawText ("SevenX Gain", 0, 30, getWidth(), 40, juce::Justification::centred);
    
    // VU METER
    auto areaMedidor = juce::Rectangle<float> (285, 100, 30, 150);
    g.setColour (juce::Colours::darkgrey.withAlpha(0.2f));
    g.fillRoundedRectangle (areaMedidor, 5.0f);

    float nivelL = audioProcessor.getRMSLevel(0);
    float nivelR = audioProcessor.getRMSLevel(1);
    
    g.setColour (juce::Colours::cyan.withAlpha(0.8f));
    g.fillRoundedRectangle (290, 100 + (150 * (1.0f - nivelL)), 8, 150 * nivelL, 2.0f);
    g.fillRoundedRectangle (302, 100 + (150 * (1.0f - nivelR)), 8, 150 * nivelR, 2.0f);

    g.setColour (juce::Colours::grey.withAlpha(0.5f));
    g.setFont (12.0f);
    g.drawText ("SevenX Audio Labs v1.1 - PRO Separated", 0, getHeight() - 25, getWidth(), 20, juce::Justification::centred);
}

void SevenXEditor::resized() {
    ganhoSlider.setBounds (75, 100, 150, 150);
    bypassButton.setBounds (50, 310, 110, 40);
    satButton.setBounds (190, 310, 110, 40);
    modeButton.setBounds (50, 370, 250, 40);
}