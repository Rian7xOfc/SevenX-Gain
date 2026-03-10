#include "PluginProcessor.h"
#include "PluginEditor.h"

// ==============================================================================
// SEVENX EDITOR - INTERFACE v1.1 COM MEDIDOR (VU METER)
// ==============================================================================
class SevenXEditor : public juce::AudioProcessorEditor, 
                     public juce::Slider::Listener, 
                     public juce::Button::Listener,
                     public juce::Timer // Adicionamos um Temporizador para atualizar o medidor
{
public:
    SevenXEditor (SevenXGainAudioProcessor& p, 
                  juce::AudioParameterFloat& volParam, 
                  juce::AudioParameterBool& bpParam,
                  juce::AudioParameterBool& sParam) 
        : AudioProcessorEditor (&p), audioProcessor (p), volumeParam (volParam), bypassParam (bpParam), satParam (sParam)
    {
        // Botão Rotativo de Ganho
        addAndMakeVisible (ganhoSlider);
        ganhoSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        ganhoSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
        ganhoSlider.setNumDecimalPlacesToDisplay(2);
        ganhoSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::cyan);
        ganhoSlider.setRange (volumeParam.range.start, volumeParam.range.end);
        ganhoSlider.setValue (volumeParam.get());
        ganhoSlider.addListener (this);

        // Botão de Bypass (Desvio)
        addAndMakeVisible (bypassButton);
        bypassButton.setButtonText ("BYPASS");
        bypassButton.setClickingTogglesState (true);
        bypassButton.setToggleState (bypassParam.get(), juce::dontSendNotification);
        bypassButton.addListener (this);
        bypassButton.setColour (juce::TextButton::buttonOnColourId, juce::Colours::orange);

        // Botão de Saturação
        addAndMakeVisible (satButton);
        satButton.setButtonText ("SATURAR");
        satButton.setClickingTogglesState (true);
        satButton.setToggleState (satParam.get(), juce::dontSendNotification);
        satButton.addListener (this);
        satButton.setColour (juce::TextButton::buttonOnColourId, juce::Colours::red);

        // Iniciamos o Temporizador (60 fotogramas por segundo para fluidez)
        startTimerHz (60);

        setSize (350, 480);
    }

    void timerCallback() override {
        repaint(); // Força a interface a redesenhar as barras do medidor
    }

    void sliderValueChanged (juce::Slider* slider) override {
        if (slider == &ganhoSlider) volumeParam.setValueNotifyingHost ((float)ganhoSlider.getValue());
    }

    void buttonClicked (juce::Button* button) override {
        if (button == &bypassButton) bypassParam.setValueNotifyingHost (bypassButton.getToggleState());
        if (button == &satButton) satParam.setValueNotifyingHost (satButton.getToggleState());
    }

    void paint (juce::Graphics& g) override {
        // Fundo
        g.setGradientFill (juce::ColourGradient (juce::Colours::black, 0, 0, juce::Colour(0xff0f0f0f), 0, (float)getHeight(), false));
        g.fillAll();
        
        // Título
        g.setColour (juce::Colours::cyan);
        g.setFont (juce::FontOptions(32.0f).withStyle("Bold"));
        g.drawText ("SevenX Gain", 0, 30, getWidth(), 40, juce::Justification::centred);
        
        // --- DESENHO DO MEDIDOR (VU METER) ---
        auto areaMedidor = juce::Rectangle<float> (280, 100, 30, 150);
        g.setColour (juce::Colours::darkgrey.withAlpha(0.3f));
        g.fillRoundedRectangle (areaMedidor, 5.0f); // Fundo do medidor

        // Calculamos a altura das barras baseada no som real
        float nivelL = audioProcessor.getRMSLevel(0);
        float nivelR = audioProcessor.getRMSLevel(1);
        
        g.setColour (juce::Colours::cyan);
        // Barra Esquerda
        g.fillRoundedRectangle (285, 100 + (150 * (1.0f - nivelL)), 8, 150 * nivelL, 2.0f);
        // Barra Direita
        g.fillRoundedRectangle (297, 100 + (150 * (1.0f - nivelR)), 8, 150 * nivelR, 2.0f);

        // Rodapé com versão atualizada para v1.1
        g.setColour (juce::Colours::grey.withAlpha(0.5f));
        g.setFont (12.0f);
        g.drawText ("SevenX Audio Labs v1.1", 0, getHeight() - 25, getWidth(), 20, juce::Justification::centred);
    }

    void resized() override {
        ganhoSlider.setBounds (75, 100, 150, 150);
        bypassButton.setBounds (50, 300, 110, 40);
        satButton.setBounds (190, 300, 110, 40);
    }

private:
    SevenXGainAudioProcessor& audioProcessor;
    juce::Slider ganhoSlider;
    juce::TextButton bypassButton;
    juce::TextButton satButton;
    juce::AudioParameterFloat& volumeParam;
    juce::AudioParameterBool& bypassParam;
    juce::AudioParameterBool& satParam;
};

// ==============================================================================
// LÓGICA DO PROCESSADOR
// ==============================================================================

SevenXGainAudioProcessor::SevenXGainAudioProcessor()
     : AudioProcessor (BusesProperties().withInput("Input", juce::AudioChannelSet::stereo(), true)
                                         .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
    addParameter (gainParam = new juce::AudioParameterFloat ({ "gain", 1 }, "Ganho", 0.0f, 1.0f, 0.5f));
    addParameter (bypassParam = new juce::AudioParameterBool ({ "bypass", 1 }, "Bypass", false));
    addParameter (satParam = new juce::AudioParameterBool ({ "saturate", 1 }, "Saturar", false));
}

SevenXGainAudioProcessor::~SevenXGainAudioProcessor() {}

void SevenXGainAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    // 1. Calculamos o nível RMS ANTES do bypass para o medidor mostrar sinal se houver entrada
    rmsLevelLeft.store (buffer.getRMSLevel (0, 0, buffer.getNumSamples()));
    if (buffer.getNumChannels() > 1)
        rmsLevelRight.store (buffer.getRMSLevel (1, 0, buffer.getNumSamples()));

    if (*bypassParam)
        return;

    // 2. Aplicamos o Ganho
    buffer.applyGain (*gainParam);

    // 3. Aplicamos a Saturação Vocal
    if (*satParam)
    {
        for (int canal = 0; canal < buffer.getNumChannels(); ++canal)
        {
            float* dadosDoCanal = buffer.getWritePointer (canal);
            for (int i = 0; i < buffer.getNumSamples(); ++i)
            {
                dadosDoCanal[i] = SevenXSaturation::processSample (dadosDoCanal[i], true);
            }
        }
    }
}

const juce::String SevenXGainAudioProcessor::getName() const { return "SevenX Gain"; }
bool SevenXGainAudioProcessor::acceptsMidi() const { return false; }
bool SevenXGainAudioProcessor::producesMidi() const { return false; }
bool SevenXGainAudioProcessor::isMidiEffect() const { return false; }
double SevenXGainAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int SevenXGainAudioProcessor::getNumPrograms() { return 1; }
int SevenXGainAudioProcessor::getCurrentProgram() { return 0; }
void SevenXGainAudioProcessor::setCurrentProgram (int index) {}
const juce::String SevenXGainAudioProcessor::getProgramName (int index) { return {}; }
void SevenXGainAudioProcessor::changeProgramName (int index, const juce::String& newName) {}
void SevenXGainAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock) {}
void SevenXGainAudioProcessor::releaseResources() {}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SevenXGainAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const {
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}
#endif

juce::AudioProcessorEditor* SevenXGainAudioProcessor::createEditor() { return new SevenXEditor (*this, *gainParam, *bypassParam, *satParam); }
bool SevenXGainAudioProcessor::hasEditor() const { return true; }

void SevenXGainAudioProcessor::getStateInformation (juce::MemoryBlock& destData) {
    juce::MemoryOutputStream stream (destData, true);
    stream.writeFloat (*gainParam);
    stream.writeBool (*bypassParam);
    stream.writeBool (*satParam);
}

void SevenXGainAudioProcessor::setStateInformation (const void* data, int sizeInBytes) {
    juce::MemoryInputStream stream (data, (size_t)sizeInBytes, false);
    gainParam->setValueNotifyingHost (stream.readFloat());
    bypassParam->setValueNotifyingHost (stream.readBool());
    satParam->setValueNotifyingHost (stream.readBool());
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new SevenXGainAudioProcessor(); }