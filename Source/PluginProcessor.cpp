#include "PluginProcessor.h"
#include "PluginEditor.h"

// ==============================================================================
// INTERFACE VISUAL (EDITOR)
// ==============================================================================
class SevenXEditor : public juce::AudioProcessorEditor, public juce::Slider::Listener
{
public:
    SevenXEditor (juce::AudioProcessor& p, juce::AudioParameterFloat& volParam) 
        : AudioProcessorEditor (&p), volumeParam (volParam)
    {
        // Configuração do Knob
        addAndMakeVisible (ganhoSlider);
        ganhoSlider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        ganhoSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 60, 20);
        ganhoSlider.setNumDecimalPlacesToDisplay(2);
        
        // Cores Premium
        ganhoSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::cyan);
        ganhoSlider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
        ganhoSlider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);

        ganhoSlider.setRange (volumeParam.range.start, volumeParam.range.end);
        ganhoSlider.setValue (volumeParam.get());
        ganhoSlider.addListener (this);

        setSize (300, 400);
    }

    void sliderValueChanged (juce::Slider* slider) override
    {
        if (slider == &ganhoSlider)
            volumeParam.setValueNotifyingHost ((float)ganhoSlider.getValue());
    }

    void paint (juce::Graphics& g) override
    {
        // Fundo com Degradê
        juce::Colour corTopo = juce::Colours::black;
        juce::Colour corBaixo = juce::Colour(0xff0f0f0f);
        g.setGradientFill (juce::ColourGradient (corTopo, 0, 0, corBaixo, 0, (float)getHeight(), false));
        g.fillAll();
        
        // Título com a nova fonte do JUCE 8
        g.setColour (juce::Colours::cyan);
        g.setFont (juce::FontOptions(30.0f).withStyle("Bold"));
        g.drawText ("SevenX Gain", 0, 30, getWidth(), 40, juce::Justification::centred);
        
        g.setColour(juce::Colours::cyan.withAlpha(0.3f));
        g.drawHorizontalLine(75, 40.0f, (float)getWidth() - 40.0f);

        g.setColour (juce::Colours::grey);
        g.setFont (12.0f);
        g.drawText ("SevenX Audio Labs", 0, getHeight() - 30, getWidth(), 20, juce::Justification::centred);
    }

    void resized() override
    {
        ganhoSlider.setBounds (75, 110, 150, 150);
    }

private:
    juce::Slider ganhoSlider;
    juce::AudioParameterFloat& volumeParam;
};

// ==============================================================================
// LÓGICA DO PROCESSADOR
// ==============================================================================

SevenXGainAudioProcessor::SevenXGainAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true))
#endif
{
    // Criando o parâmetro
    addParameter (gainParam = new juce::AudioParameterFloat ({ "gain", 1 }, "Gain", 0.0f, 1.0f, 0.5f));
}

SevenXGainAudioProcessor::~SevenXGainAudioProcessor() {}

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
bool SevenXGainAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo()) return false;
    return true;
}
#endif

void SevenXGainAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.applyGain (*gainParam);
}

juce::AudioProcessorEditor* SevenXGainAudioProcessor::createEditor() 
{ 
    return new SevenXEditor (*this, *gainParam); 
}

bool SevenXGainAudioProcessor::hasEditor() const { return true; }

void SevenXGainAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream (destData, true).writeFloat (*gainParam);
}

void SevenXGainAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    gainParam->setValueNotifyingHost (juce::MemoryInputStream (data, (size_t)sizeInBytes, false).readFloat());
}

// ESTA FUNÇÃO RESOLVE O ERRO LNK2019
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SevenXGainAudioProcessor();
}