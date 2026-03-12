#pragma once
#include <JuceHeader.h>

/**
 * SevenXSVGKnob - Versão Corrigida (Resolução de Erro de Compilação)
 * Corrigido o método de qualidade de imagem para o padrão JUCE atual.
 */
class SevenXSVGKnob : public juce::LookAndFeel_V4
{
public:
    SevenXSVGKnob()
    {
        auto svgData = BinaryData::knob_svg;
        auto svgDataSize = BinaryData::knob_svgSize;

        if (svgData != nullptr)
            drawable = juce::Drawable::createFromImageData(svgData, (size_t)svgDataSize);

        setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
        float sliderPos, const float startAngle, const float endAngle,
        juce::Slider& slider) override
    {
        if (drawable != nullptr)
        {
            // 1. Calcula o ângulo real
            auto rotation = startAngle + sliderPos * (endAngle - startAngle);

            // 2. Define a área do componente (Garantindo que é um quadrado perfeito)
            auto sliderSide = juce::jmin(width, height);
            auto bounds = juce::Rectangle<float>(x + (width - sliderSide) * 0.5f,
                y + (height - sliderSide) * 0.5f,
                (float)sliderSide, (float)sliderSide).reduced(15.0f);

            auto centerX = bounds.getCentreX();
            auto centerY = bounds.getCentreY();

            // 3. Obtém os limites originais do desenho SVG
            auto svgBounds = drawable->getDrawableBounds();

            // 4. MATEMÁTICA DA ROTAÇÃO:
            auto transform = juce::AffineTransform::translation(-svgBounds.getCentreX(), -svgBounds.getCentreY())
                .rotated(rotation)
                .scaled(juce::jmin(bounds.getWidth() / svgBounds.getWidth(),
                    bounds.getHeight() / svgBounds.getHeight()))
                .translated(centerX, centerY);

            // 5. Renderiza com alta qualidade - CORRIGIDO: setImageResamplingQuality
            g.setImageResamplingQuality(juce::Graphics::ResamplingQuality::highResamplingQuality);
            drawable->draw(g, 1.0f, transform);
        }
    }

    juce::Label* createSliderTextBox(juce::Slider& slider) override
    {
        auto* l = LookAndFeel_V4::createSliderTextBox(slider);
        l->setColour(juce::Label::outlineColourId, juce::Colours::transparentBlack);
        l->setColour(juce::Label::backgroundColourId, juce::Colours::black.withAlpha(0.4f));
        l->setColour(juce::Label::textColourId, juce::Colours::cyan);
        l->setFont(juce::FontOptions(15.0f).withStyle("Bold"));
        return l;
    }

private:
    std::unique_ptr<juce::Drawable> drawable;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SevenXSVGKnob)
};