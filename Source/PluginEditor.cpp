#include "PluginProcessor.h"
#include "PluginEditor.h"

SevenXEditor::SevenXEditor(SevenXGainAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    auto setupKnob = [this](juce::Slider& s, juce::AudioParameterFloat* param, juce::String suffix) {
        addAndMakeVisible(s);
        s.setLookAndFeel(&customLookAndFeel);
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);

        // Caixa de texto com tamanho fixo e sem bordas
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 22);
        s.setTextValueSuffix(suffix);
        s.setRange(param->range.start, param->range.end);
        s.setValue(param->get());
        s.setNumDecimalPlacesToDisplay(1);
        s.addListener(this);
        };

    setupKnob(inputGainSlider, audioProcessor.gainParam, " dB");
    setupKnob(outputGainSlider, audioProcessor.outParam, " dB");

    auto setupButton = [this](juce::TextButton& b, juce::AudioParameterBool* param, juce::Colour color) {
        addAndMakeVisible(b);
        b.setClickingTogglesState(true);
        b.setToggleState(param->get(), juce::dontSendNotification);
        b.setColour(juce::TextButton::buttonOnColourId, color);
        b.setColour(juce::TextButton::buttonColourId, juce::Colour(0xff252525));
        b.addListener(this);
        };

    setupButton(bypassButton, audioProcessor.bypassParam, juce::Colours::orange);
    bypassButton.setButtonText("BYPASS");

    setupButton(satButton, audioProcessor.satParam, juce::Colours::red);
    satButton.setButtonText("SATURATE");

    setupButton(modeButton, audioProcessor.modeParam, juce::Colours::purple);
    updateModeButtonText();

    startTimerHz(60);
    setSize(480, 560);
}

SevenXEditor::~SevenXEditor()
{
    inputGainSlider.setLookAndFeel(nullptr);
    outputGainSlider.setLookAndFeel(nullptr);
}

void SevenXEditor::updateModeButtonText() {
    modeButton.setButtonText(audioProcessor.modeParam->get() ? "MODE: MASTER" : "MODE: VOCAL");
}

void SevenXEditor::timerCallback() { repaint(); }

void SevenXEditor::sliderValueChanged(juce::Slider* slider) {
    if (slider == &inputGainSlider) audioProcessor.gainParam->setValueNotifyingHost((float)inputGainSlider.getValue());
    if (slider == &outputGainSlider) audioProcessor.outParam->setValueNotifyingHost((float)outputGainSlider.getValue());
}

void SevenXEditor::buttonClicked(juce::Button* button) {
    if (button == &bypassButton) audioProcessor.bypassParam->setValueNotifyingHost(bypassButton.getToggleState());
    if (button == &satButton) audioProcessor.satParam->setValueNotifyingHost(satButton.getToggleState());
    if (button == &modeButton) {
        audioProcessor.modeParam->setValueNotifyingHost(modeButton.getToggleState());
        updateModeButtonText();
    }
}

void SevenXEditor::paint(juce::Graphics& g) {
    auto backgroundGradient = juce::ColourGradient(juce::Colour(0xff1a1a1a), (float)getWidth() / 2, 150.0f,
        juce::Colours::black, 0, (float)getHeight(), true);
    g.setGradientFill(backgroundGradient);
    g.fillAll();

    g.setColour(juce::Colours::cyan);
    g.setFont(juce::FontOptions(38.0f).withStyle("Bold"));
    g.drawText("SevenX Gain", 0, 35, getWidth(), 50, juce::Justification::centred);

    g.setColour(juce::Colours::grey.withAlpha(0.4f));
    g.setFont(11.0f);
    g.drawText("V1.1 PRO - HIGH FIDELITY PROCESSOR", 0, 80, getWidth(), 20, juce::Justification::centred);

    // Labels reposicionadas para não colidir com o valor numérico
    g.setColour(juce::Colours::white.withAlpha(0.6f));
    g.setFont(juce::FontOptions(13.0f).withStyle("Bold"));
    g.drawText("INPUT DRIVE", 40, 115, 180, 20, juce::Justification::centred);
    g.drawText("OUTPUT TRIM", 230, 115, 180, 20, juce::Justification::centred);

    // Medidor VU (Design Minimalista)
    auto areaVU = juce::Rectangle<float>(425, 140, 22, 155);
    g.setColour(juce::Colours::black.withAlpha(0.7f));
    g.fillRoundedRectangle(areaVU, 3.0f);

    float nivelL = audioProcessor.getRMSLevel(0);
    float nivelR = audioProcessor.getRMSLevel(1);

    auto meterGrad = juce::ColourGradient(juce::Colours::cyan, 0, 295, juce::Colours::red, 0, 140, false);
    g.setGradientFill(meterGrad);

    g.fillRoundedRectangle(428, 140 + (155 * (1.0f - nivelL)), 6, 155 * nivelL, 1.0f);
    g.fillRoundedRectangle(437, 140 + (155 * (1.0f - nivelR)), 6, 155 * nivelR, 1.0f);
}

void SevenXEditor::resized() {
    // Definimos áreas quadradas para garantir a rotação centralizada
    inputGainSlider.setBounds(40, 140, 180, 190);
    outputGainSlider.setBounds(230, 140, 180, 190);

    bypassButton.setBounds(60, 355, 170, 52);
    satButton.setBounds(250, 355, 170, 52);
    modeButton.setBounds(60, 425, 360, 52);
}